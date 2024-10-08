#pragma once
#include "lue/framework/algorithm/functor_traits.hpp"
#include "lue/framework/algorithm/scalar.hpp"
#include "lue/framework/core/annotate.hpp"
#include "lue/framework/core/component.hpp"
#include "lue/framework/partitioned_array.hpp"


namespace lue {
    namespace detail {

        template<typename Policies, typename InputPartition, typename OutputPartition, typename Functor>
        OutputPartition unary_local_operation_partition(
            Policies const& policies, InputPartition const& input_partition, Functor const& functor)
        {
            using Offset = OffsetT<InputPartition>;
            using InputData = DataT<InputPartition>;
            using OutputData = DataT<OutputPartition>;

            lue_hpx_assert(input_partition.is_ready());

            return hpx::dataflow(
                hpx::launch::async,
                hpx::unwrapping(

                    [input_partition, policies, functor](
                        Offset const& offset, InputData const& input_partition_data)
                    {
                        AnnotateFunction annotation{"unary_local_operation_partition"};

                        HPX_UNUSED(input_partition);

                        OutputData output_partition_data{input_partition_data.shape()};

                        auto const& dp = policies.domain_policy();
                        auto const& indp = std::get<0>(policies.inputs_policies()).input_no_data_policy();
                        auto const& ondp = std::get<0>(policies.outputs_policies()).output_no_data_policy();
                        auto const& rp = std::get<0>(policies.outputs_policies()).range_policy();

                        Count const nr_elements{lue::nr_elements(input_partition_data)};

                        for (Index i = 0; i < nr_elements; ++i)
                        {
                            if (indp.is_no_data(input_partition_data, i))
                            {
                                ondp.mark_no_data(output_partition_data, i);
                            }
                            else if (!dp.within_domain(input_partition_data[i]))
                            {
                                ondp.mark_no_data(output_partition_data, i);
                            }
                            else
                            {
                                output_partition_data[i] = functor(input_partition_data[i]);

                                if (!rp.within_range(input_partition_data[i], output_partition_data[i]))
                                {
                                    ondp.mark_no_data(output_partition_data, i);
                                }
                            }
                        }

                        return OutputPartition{hpx::find_here(), offset, std::move(output_partition_data)};
                    }

                    ),
                input_partition.offset(),
                input_partition.data());
        }


        template<typename Policies, typename InputPartition, typename OutputPartition, typename Functor>
        struct UnaryLocalOperationPartitionAction:
            hpx::actions::make_action<
                decltype(&unary_local_operation_partition<
                         Policies,
                         InputPartition,
                         OutputPartition,
                         Functor>),
                &unary_local_operation_partition<Policies, InputPartition, OutputPartition, Functor>,
                UnaryLocalOperationPartitionAction<Policies, InputPartition, OutputPartition, Functor>>::type
        {
        };

    }  // namespace detail


    template<typename Policies, typename InputElement, Rank rank, typename Functor>
    PartitionedArray<OutputElementT<Functor>, rank> unary_local_operation(
        Policies const& policies,
        PartitionedArray<InputElement, rank> const& input_array,
        Functor const& functor)
    {
        using InputArray = PartitionedArray<InputElement, rank>;
        using InputPartitions = PartitionsT<InputArray>;
        using InputPartition = PartitionT<InputArray>;

        using OutputArray = PartitionedArray<OutputElementT<Functor>, rank>;
        using OutputPartitions = PartitionsT<OutputArray>;
        using OutputPartition = PartitionT<OutputArray>;

        lue_hpx_assert(all_are_valid(input_array.partitions()));

        detail::UnaryLocalOperationPartitionAction<Policies, InputPartition, OutputPartition, Functor> action;

        Localities<rank> const& localities{input_array.localities()};
        InputPartitions const& input_partitions{input_array.partitions()};
        OutputPartitions output_partitions{shape_in_partitions(input_array)};

        for (Index p = 0; p < nr_partitions(input_array); ++p)
        {
            output_partitions[p] = hpx::dataflow(
                hpx::launch::async,

                [locality_id = localities[p], action, policies, functor](
                    InputPartition const& input_partition)
                {
                    AnnotateFunction annotation{"unary_local_operation"};

                    return action(locality_id, policies, input_partition, functor);
                },

                input_partitions[p]);
        }

        return OutputArray{shape(input_array), localities, std::move(output_partitions)};
    }


    template<typename Policies, typename InputElement, typename Functor>
    auto unary_local_operation(
        Policies const& policies, Scalar<InputElement> const& input_scalar, Functor const& functor)
        -> Scalar<OutputElementT<Functor>>
    {
        using OutputElement = OutputElementT<Functor>;

        return hpx::dataflow(
            hpx::launch::async,
            hpx::unwrapping(
                [policies, functor](auto const& input_value) -> OutputElement
                {
                    auto const& dp = policies.domain_policy();
                    auto const& indp = std::get<0>(policies.inputs_policies()).input_no_data_policy();
                    auto const& ondp = std::get<0>(policies.outputs_policies()).output_no_data_policy();
                    auto const& rp = std::get<0>(policies.outputs_policies()).range_policy();

                    OutputElement output_value;

                    if (indp.is_no_data(input_value) || !dp.within_domain(input_value))
                    {
                        ondp.mark_no_data(output_value);
                    }
                    else
                    {
                        output_value = functor(input_value);

                        if (!rp.within_range(input_value, output_value))
                        {
                            ondp.mark_no_data(output_value);
                        }
                    }

                    return output_value;
                }),
            input_scalar.future());
    }

}  // namespace lue
