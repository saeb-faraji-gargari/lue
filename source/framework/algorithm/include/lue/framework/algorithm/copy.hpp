#pragma once
#include "lue/framework/core/type_traits.hpp"
#include <hpx/include/lcos.hpp>


namespace lue {
namespace detail {

/*!
    @brief      Return a new copy the array partition passed in
    @tparam     Partition Client class of partition component
    @param      partition Client of array partition component
    @return     A copy of the array partition passed in
*/
template<
    typename Partition>
Partition copy_partition(
    Partition const& partition)
{
    // Assert the locality of the partition is the same as the locality
    // this code runs on
    assert(
        hpx::get_colocation_id(partition.get_id()).get() ==
        hpx::find_here());

    // Implement copy constructor that does a deep copy?
    // return Partition{partition};

    // Copy the data and move it into a new partition
    return Partition{hpx::find_here(), partition.data(CopyMode::copy).get()};
}

}  // namespace detail


template<
    typename Partition>
struct CopyPartitionAction:
    hpx::actions::make_action<
        decltype(&detail::copy_partition<Partition>),
        &detail::copy_partition<Partition>,
        CopyPartitionAction<Partition>>
{};


/*!
    @brief      Return the result of copying a partitioned array
    @tparam     Element Type of elements in the arrays
    @tparam     rank Rank of the input array
    @tparam     Array Class template of the type of the arrays
    @param      array Partitioned array
    @return     New partitioned array
*/
template<
    typename Element,
    Rank rank,
    template<typename, Rank> typename Array>
Array<Element, rank> copy(
    Array<Element, rank> const& array)
{
    using Array_ = Array<Element, rank>;
    using Partition = PartitionT<Array_>;
    using Partitions = PartitionsT<Array_>;

    CopyPartitionAction<Partition> action;
    Partitions output_partitions{shape_in_partitions(array)};

    for(Index p = 0; p < nr_partitions(array); ++p) {

        output_partitions[p] = hpx::dataflow(
            hpx::launch::async,

            [action](
                Partition const& input_partition)
            {
                return action(
                    hpx::get_colocation_id(
                        hpx::launch::sync, input_partition.get_id()),
                    input_partition);
            },

            array.partitions()[p]);

    }

    return Array_{shape(array), std::move(output_partitions)};
}

}  // namespace lue
