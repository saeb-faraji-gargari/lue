#pragma once
#include "lue/framework/core/component/array_partition.hpp"
#include <hpx/include/lcos.hpp>


namespace lue {
namespace detail {

template<
    typename Partition>
PartitionT<Partition, ElementT<Partition>, 1> unique_partition(
    Partition const& partition)
{
    assert(
        hpx::get_colocation_id(partition.get_id()).get() ==
        hpx::find_here());

    using InputData = DataT<Partition>;
    using Element = ElementT<Partition>;

    using OutputPartition = PartitionT<Partition, Element, 1>;
    using OutputData = DataT<OutputPartition>;
    using OutputShape = ShapeT<OutputPartition>;

    hpx::id_type partition_id = partition.get_id();

    return partition.data(CopyMode::share).then(
        hpx::util::unwrapping(
            [partition_id](
                InputData const& partition_data)
            {
                // Copy values from input array into a set
                std::set<Element> unique_values(
                    partition_data.begin(), partition_data.end());
                assert(unique_values.size() <= partition_data.size());

                // Copy unique values from set into output array
                OutputData output_data{OutputShape{{unique_values.size()}}};
                assert(output_data.size() == unique_values.size());
                std::copy(
                    unique_values.begin(), unique_values.end(),
                    output_data.begin());

                return OutputPartition{partition_id, output_data};
            }
        )
    );
}

}  // namespace detail


template<
    typename Partition>
struct UniquePartitionAction:
    hpx::actions::make_action<
        decltype(&detail::unique_partition<Partition>),
        &detail::unique_partition<Partition>,
        UniquePartitionAction<Partition>>
{};


template<
    typename Element,
    std::size_t rank,
    template<typename, std::size_t> typename Array>
hpx::future<Array<Element, 1>> unique(
    Array<Element, rank> const& array)
{

    // - Determine unique values per partition
    // - Collect these values and determine overall collection of
    //     unique values
    // - The result is a 1-dimensional array with a single partition,
    //     located on the current locality

    using InputArray = Array<Element, rank>;
    using InputPartition = PartitionT<InputArray>;

    using OutputArray = Array<Element, 1>;
    using OutputPartitions = PartitionsT<OutputArray>;
    using OutputPartition = PartitionT<OutputArray>;
    using OutputData = DataT<OutputPartition>;
    using OutputShape = ShapeT<OutputArray>;

    OutputPartitions output_partitions{OutputShape{{nr_partitions(array)}}};
    UniquePartitionAction<InputPartition> action;

    for(std::size_t p = 0; p < nr_partitions(array); ++p) {

        InputPartition const& input_partition = array.partitions()[p];

        output_partitions[p] =
            hpx::get_colocation_id(input_partition.get_id()).then(
                hpx::util::unwrapping(
                    [=](
                        hpx::naming::id_type const locality_id)
                    {
                        return hpx::dataflow(
                            hpx::launch::async,
                            action,
                            locality_id,
                            input_partition);
                    }
                )
            );

    }

    // Collect all unique values into a single array, on the current locality

    // The unique values in each partition are being determined on
    // their respective localities. Attach a continuation that determines
    // the unique values in the whole array once the partitions are
    // ready. This continuation runs on our own locality. This implies
    // data is transported from remote localities to us.
    return hpx::when_all(
            output_partitions.begin(), output_partitions.end()).then(
        hpx::util::unwrapping(
            [](auto const& partitions) {

                // Collection of unique values of all partition results
                std::set<Element> unique_values;

                for(auto const& partition: partitions) {

                    auto const data = partition.data(CopyMode::share).get();

                    unique_values.insert(
                        data.begin(),
                        data.end());

                }

                // Shape in elements of resulting partitioned array
                OutputShape shape{{unique_values.size()}};

                // Shape in partitions of resulting partitioned array
                OutputShape shape_in_partitions{{1}};

                // Copy unique values into an array-data collection
                OutputData result_values{shape};
                std::copy(
                    unique_values.begin(), unique_values.end(),
                    result_values.begin());

                // Store array data in partition component
                OutputPartition result_partition{
                    // TODO Does this work? colocated is called on the
                    //     result of find_here. What does that mean?
                    hpx::find_here(), result_values};

                // Store partition component in a collection
                OutputPartitions result_partitions{
                    shape_in_partitions, std::move(result_partition)};

                // Store collection of partitions in a partitioned array
                OutputArray result_array{shape, std::move(result_partitions)};

                return result_array;
            }
        )
    );
}

}  // namespace lue
