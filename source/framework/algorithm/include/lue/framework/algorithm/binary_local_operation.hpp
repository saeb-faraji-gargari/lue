#pragma once
#include "lue/framework/algorithm/functor_traits.hpp"
#include "lue/framework/algorithm/scalar.hpp"
#include "lue/framework/partitioned_array.hpp"


namespace lue {

    // local_operation(partition, scalar)
    template<typename Policies, typename InputElement1, typename InputElement2, Rank rank, typename Functor>
    auto binary_local_operation(
        hpx::id_type locality_id,
        Policies const& policies,
        ArrayPartition<InputElement1, rank> const& input_partition,
        hpx::shared_future<InputElement2> const& input_scalar,
        Functor const& functor) -> ArrayPartition<OutputElementT<Functor>, rank>;

    // local_operation(array, array)
    template<typename Policies, typename InputElement1, typename InputElement2, Rank rank, typename Functor>
    auto binary_local_operation(
        Policies const& policies,
        PartitionedArray<InputElement1, rank> const& input_array1,
        PartitionedArray<InputElement2, rank> const& input_array2,
        Functor const& functor) -> PartitionedArray<OutputElementT<Functor>, rank>;

    // local_operation(array, scalar)
    template<typename Policies, typename InputElement1, typename InputElement2, Rank rank, typename Functor>
    auto binary_local_operation(
        Policies const& policies,
        PartitionedArray<InputElement1, rank> const& input_array,
        hpx::shared_future<InputElement2> const& input_scalar,
        Functor const& functor) -> PartitionedArray<OutputElementT<Functor>, rank>;

    // local_operation(scalar, array)
    template<typename Policies, typename InputElement1, typename InputElement2, Rank rank, typename Functor>
    auto binary_local_operation(
        Policies const& policies,
        hpx::shared_future<InputElement1> const& input_scalar,
        PartitionedArray<InputElement2, rank> const& input_array,
        Functor const& functor) -> PartitionedArray<OutputElementT<Functor>, rank>;

    // local_operation(scalar, scalar)
    template<typename Policies, typename InputElement1, typename InputElement2, typename Functor>
    auto binary_local_operation(
        Policies const& policies,
        hpx::shared_future<InputElement1> const& input_scalar1,
        hpx::shared_future<InputElement2> const& input_scalar2,
        Functor const& functor) -> hpx::future<OutputElementT<Functor>>;

}  // namespace lue


// All overloads *with* a Policies template parameter
#define LUE_BINARY_LOCAL_OPERATION_OVERLOADS_WITH_POLICIES(name, Functor)                                    \
                                                                                                             \
    /* f(policies, array, array) */                                                                          \
    template<typename Policies, typename InputElement, Rank rank>                                            \
    auto name(                                                                                               \
        Policies const& policies,                                                                            \
        PartitionedArray<InputElement, rank> const& array1,                                                  \
        PartitionedArray<InputElement, rank> const& array2)                                                  \
    {                                                                                                        \
        using OutputElement = OutputElementT<Functor<InputElement>>;                                         \
                                                                                                             \
        return binary_local_operation(policies, array1, array2, Functor<InputElement, OutputElement>{});     \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    /* f(policies, array, scalar) */                                                                         \
    template<typename Policies, typename OutputElement, typename InputElement, Rank rank>                    \
    auto name(                                                                                               \
        Policies const& policies,                                                                            \
        PartitionedArray<InputElement, rank> const& array,                                                   \
        Scalar<InputElement> const& scalar)                                                                  \
        ->PartitionedArray<OutputElement, rank>                                                              \
    {                                                                                                        \
        return binary_local_operation(                                                                       \
            policies, array, scalar.future(), Functor<InputElement, OutputElement>{});                       \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    /* f(policies, array, value_f) */                                                                        \
    template<typename Policies, typename OutputElement, typename InputElement, Rank rank>                    \
    auto name(                                                                                               \
        Policies const& policies,                                                                            \
        PartitionedArray<InputElement, rank> const& array,                                                   \
        hpx::shared_future<InputElement> const& value_f)                                                     \
        ->PartitionedArray<OutputElement, rank>                                                              \
    {                                                                                                        \
        return name<Policies, OutputElement, InputElement, rank>(                                            \
            policies, array, Scalar<InputElement>{value_f});                                                 \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    /* f(policies, array, value) */                                                                          \
    template<typename Policies, typename OutputElement, typename InputElement, Rank rank>                    \
    auto name(                                                                                               \
        Policies const& policies,                                                                            \
        PartitionedArray<InputElement, rank> const& array,                                                   \
        InputElement const& value)                                                                           \
        ->PartitionedArray<OutputElement, rank>                                                              \
    {                                                                                                        \
        return name<Policies, OutputElement, InputElement, rank>(                                            \
            policies, array, Scalar<InputElement>{value});                                                   \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    /* f(policies, array, scalar) */                                                                         \
    template<typename Policies, typename InputElement, Rank rank>                                            \
    auto name(                                                                                               \
        Policies const& policies,                                                                            \
        PartitionedArray<InputElement, rank> const& array,                                                   \
        Scalar<InputElement> const& scalar)                                                                  \
    {                                                                                                        \
        using OutputElement = OutputElementT<Functor<InputElement>>;                                         \
                                                                                                             \
        return name<Policies, OutputElement, InputElement, rank>(policies, array, scalar);                   \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    /* f(policies, array, value_f) */                                                                        \
    template<typename Policies, typename InputElement, Rank rank>                                            \
    auto name(                                                                                               \
        Policies const& policies,                                                                            \
        PartitionedArray<InputElement, rank> const& array,                                                   \
        hpx::shared_future<InputElement> const& value_f)                                                     \
    {                                                                                                        \
        using OutputElement = OutputElementT<Functor<InputElement>>;                                         \
                                                                                                             \
        return name<Policies, OutputElement, InputElement, rank>(                                            \
            policies, array, Scalar<InputElement>{value_f});                                                 \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    /* f(policies, array, value) */                                                                          \
    template<typename Policies, typename InputElement, Rank rank>                                            \
    auto name(                                                                                               \
        Policies const& policies,                                                                            \
        PartitionedArray<InputElement, rank> const& array,                                                   \
        InputElement const& value)                                                                           \
    {                                                                                                        \
        using OutputElement = OutputElementT<Functor<InputElement>>;                                         \
                                                                                                             \
        return name<Policies, OutputElement, InputElement, rank>(                                            \
            policies, array, Scalar<InputElement>{value});                                                   \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    /* f(policies, scalar, array) */                                                                         \
    template<typename Policies, typename OutputElement, typename InputElement, Rank rank>                    \
    auto name(                                                                                               \
        Policies const& policies,                                                                            \
        Scalar<InputElement> const& scalar,                                                                  \
        PartitionedArray<InputElement, rank> const& array)                                                   \
        ->PartitionedArray<OutputElement, rank>                                                              \
    {                                                                                                        \
        return binary_local_operation(                                                                       \
            policies, scalar.future(), array, Functor<InputElement, OutputElement>{});                       \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    /* f(policies, value_f, array) */                                                                        \
    template<typename Policies, typename OutputElement, typename InputElement, Rank rank>                    \
    auto name(                                                                                               \
        Policies const& policies,                                                                            \
        hpx::shared_future<InputElement> const& value_f,                                                     \
        PartitionedArray<InputElement, rank> const& array)                                                   \
        ->PartitionedArray<OutputElement, rank>                                                              \
    {                                                                                                        \
        return name<Policies, OutputElement, InputElement, rank>(                                            \
            policies, Scalar<InputElement>{value_f}, array);                                                 \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    /* f(policies, value, array) */                                                                          \
    template<typename Policies, typename OutputElement, typename InputElement, Rank rank>                    \
    auto name(                                                                                               \
        Policies const& policies,                                                                            \
        InputElement const& value,                                                                           \
        PartitionedArray<InputElement, rank> const& array)                                                   \
        ->PartitionedArray<OutputElement, rank>                                                              \
    {                                                                                                        \
        return name<Policies, OutputElement, InputElement, rank>(                                            \
            policies, Scalar<InputElement>{value}, array);                                                   \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    /* f(policies, scalar, array) */                                                                         \
    template<typename Policies, typename InputElement, Rank rank>                                            \
    auto name(                                                                                               \
        Policies const& policies,                                                                            \
        Scalar<InputElement> const& scalar,                                                                  \
        PartitionedArray<InputElement, rank> const& array)                                                   \
    {                                                                                                        \
        using OutputElement = OutputElementT<Functor<InputElement>>;                                         \
                                                                                                             \
        return name<Policies, OutputElement, InputElement, rank>(policies, scalar, array);                   \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    /* f(policies, value_f, array) */                                                                        \
    template<typename Policies, typename InputElement, Rank rank>                                            \
    auto name(                                                                                               \
        Policies const& policies,                                                                            \
        hpx::shared_future<InputElement> const& value_f,                                                     \
        PartitionedArray<InputElement, rank> const& array)                                                   \
    {                                                                                                        \
        using OutputElement = OutputElementT<Functor<InputElement>>;                                         \
                                                                                                             \
        return name<Policies, OutputElement, InputElement, rank>(                                            \
            policies, Scalar<InputElement>{value_f}, array);                                                 \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    /* f(policies, value, array) */                                                                          \
    template<typename Policies, typename InputElement, Rank rank>                                            \
    auto name(                                                                                               \
        Policies const& policies,                                                                            \
        InputElement const& value,                                                                           \
        PartitionedArray<InputElement, rank> const& array)                                                   \
    {                                                                                                        \
        using OutputElement = OutputElementT<Functor<InputElement>>;                                         \
                                                                                                             \
        return name<Policies, OutputElement, InputElement, rank>(                                            \
            policies, Scalar<InputElement>{value}, array);                                                   \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    /* f(policies, scalar, scalar) */                                                                        \
    template<typename Policies, typename OutputElement, typename InputElement>                               \
    auto name(                                                                                               \
        Policies const& policies, Scalar<InputElement> const& scalar1, Scalar<InputElement> const& scalar2)  \
        ->Scalar<OutputElement>                                                                              \
    {                                                                                                        \
        return binary_local_operation(                                                                       \
            policies, scalar1.future(), scalar2.future(), Functor<InputElement, OutputElement>{});           \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    /* f(policies, scalar, scalar) */                                                                        \
    template<typename Policies, typename InputElement>                                                       \
    auto name(                                                                                               \
        Policies const& policies, Scalar<InputElement> const& scalar1, Scalar<InputElement> const& scalar2)  \
    {                                                                                                        \
        using OutputElement = OutputElementT<Functor<InputElement>>;                                         \
                                                                                                             \
        return name<Policies, OutputElement, InputElement>(policies, scalar1, scalar2);                      \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    /* f(policies, scalar, value) */                                                                         \
    template<typename Policies, typename OutputElement, typename InputElement>                               \
    auto name(Policies const& policies, Scalar<InputElement> const& scalar, InputElement const& value)       \
        ->Scalar<OutputElement>                                                                              \
    {                                                                                                        \
        return name<Policies, OutputElement, InputElement>(                                                  \
            policies, scalar.future(), Scalar<InputElement>{value});                                         \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    /* f(policies, scalar, value) */                                                                         \
    template<typename Policies, typename InputElement>                                                       \
    auto name(Policies const& policies, Scalar<InputElement> const& scalar, InputElement const& value)       \
    {                                                                                                        \
        using OutputElement = OutputElementT<Functor<InputElement>>;                                         \
                                                                                                             \
        return name<Policies, OutputElement, InputElement>(policies, scalar, Scalar<InputElement>{value});   \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    /* f(policies, value, scalar) */                                                                         \
    template<typename Policies, typename OutputElement, typename InputElement>                               \
    auto name(Policies const& policies, InputElement const& value, Scalar<InputElement> const& scalar)       \
        ->Scalar<OutputElement>                                                                              \
    {                                                                                                        \
        return name<Policies, OutputElement, InputElement>(                                                  \
            policies, Scalar<InputElement>{value}, scalar.future());                                         \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    /* f(policies, value, scalar) */                                                                         \
    template<typename Policies, typename InputElement>                                                       \
    auto name(Policies const& policies, InputElement const& value, Scalar<InputElement> const& scalar)       \
    {                                                                                                        \
        using OutputElement = OutputElementT<Functor<InputElement>>;                                         \
                                                                                                             \
        return name<Policies, OutputElement, InputElement>(policies, Scalar<InputElement>{value}, scalar);   \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    /* f(policies, value, value) */                                                                          \
    template<typename Policies, typename OutputElement, typename InputElement>                               \
    auto name(Policies const& policies, InputElement const& value1, InputElement const& value2)              \
        ->Scalar<OutputElement>                                                                              \
    {                                                                                                        \
        return name<Policies, OutputElement, InputElement>(                                                  \
            policies,                                                                                        \
            Scalar<InputElement>{value1},                                                                    \
            Scalar<InputElement>{value2},                                                                    \
            Functor<InputElement, OutputElement>{});                                                         \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    /* f(policies, value, value) */                                                                          \
    template<typename Policies, typename InputElement>                                                       \
    auto name(Policies const& policies, InputElement const& value1, InputElement const& value2)              \
    {                                                                                                        \
        using OutputElement = OutputElementT<Functor<InputElement>>;                                         \
                                                                                                             \
        return name<Policies, OutputElement, InputElement>(                                                  \
            policies, Scalar<InputElement>{value1}, Scalar<InputElement>{value2});                           \
    }


// All overloads *without* a Policies template parameter
#define LUE_BINARY_LOCAL_OPERATION_OVERLOADS_WITHOUT_POLICIES(name, Functor, Policies)                       \
                                                                                                             \
    /* f(array, array) */                                                                                    \
    template<typename InputElement, Rank rank>                                                               \
    auto name(                                                                                               \
        PartitionedArray<InputElement, rank> const& array1,                                                  \
        PartitionedArray<InputElement, rank> const& array2)                                                  \
    {                                                                                                        \
        using OutputElement = OutputElementT<Functor<InputElement>>;                                         \
        using Policies_ = Policies<OutputElement, InputElement>;                                             \
                                                                                                             \
        return name(Policies_{}, array1, array2);                                                            \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    /* f(array, scalar) */                                                                                   \
    template<typename OutputElement, typename InputElement, Rank rank>                                       \
    PartitionedArray<OutputElement, rank> name(                                                              \
        PartitionedArray<InputElement, rank> const& array, Scalar<InputElement> const& scalar)               \
    {                                                                                                        \
        using Policies_ = Policies<OutputElement, InputElement>;                                             \
                                                                                                             \
        return name<Policies_, OutputElement, InputElement, rank>(Policies_{}, array, scalar);               \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    /* f(array, value_f) */                                                                                  \
    template<typename OutputElement, typename InputElement, Rank rank>                                       \
    PartitionedArray<OutputElement, rank> name(                                                              \
        PartitionedArray<InputElement, rank> const& array, hpx::shared_future<InputElement> const& value_f)  \
    {                                                                                                        \
        using Policies_ = Policies<OutputElement, InputElement>;                                             \
                                                                                                             \
        return name<Policies_, OutputElement, InputElement, rank>(                                           \
            Policies_{}, array, Scalar<InputElement>{value_f});                                              \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    /* f(array, value) */                                                                                    \
    template<typename OutputElement, typename InputElement, Rank rank>                                       \
    PartitionedArray<OutputElement, rank> name(                                                              \
        PartitionedArray<InputElement, rank> const& array, InputElement const& value)                        \
    {                                                                                                        \
        using Policies_ = Policies<OutputElement, InputElement>;                                             \
                                                                                                             \
        return name<Policies_, OutputElement, InputElement, rank>(                                           \
            Policies_{}, array, Scalar<InputElement>{value});                                                \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    /* f(array, scalar) */                                                                                   \
    template<typename InputElement, Rank rank>                                                               \
    auto name(PartitionedArray<InputElement, rank> const& array, Scalar<InputElement> const& scalar)         \
    {                                                                                                        \
        using OutputElement = OutputElementT<Functor<InputElement>>;                                         \
                                                                                                             \
        return name<OutputElement, InputElement, rank>(array, scalar);                                       \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    /* f(array, value_f) */                                                                                  \
    template<typename InputElement, Rank rank>                                                               \
    auto name(                                                                                               \
        PartitionedArray<InputElement, rank> const& array, hpx::shared_future<InputElement> const& value_f)  \
    {                                                                                                        \
        using OutputElement = OutputElementT<Functor<InputElement>>;                                         \
                                                                                                             \
        return name<OutputElement, InputElement, rank>(array, Scalar<InputElement>{value_f});                \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    /* f(array, value) */                                                                                    \
    template<typename InputElement, Rank rank>                                                               \
    auto name(PartitionedArray<InputElement, rank> const& array, InputElement const& value)                  \
    {                                                                                                        \
        using OutputElement = OutputElementT<Functor<InputElement>>;                                         \
                                                                                                             \
        return name<OutputElement, InputElement, rank>(array, Scalar<InputElement>{value});                  \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    /* f(scalar, array) */                                                                                   \
    template<typename OutputElement, typename InputElement, Rank rank>                                       \
    PartitionedArray<OutputElement, rank> name(                                                              \
        Scalar<InputElement> const& scalar, PartitionedArray<InputElement, rank> const& array)               \
    {                                                                                                        \
        using Policies_ = Policies<OutputElement, InputElement>;                                             \
                                                                                                             \
        return name<Policies_, OutputElement, InputElement, rank>(Policies_{}, scalar, array);               \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    /* f(value_f, array) */                                                                                  \
    template<typename OutputElement, typename InputElement, Rank rank>                                       \
    PartitionedArray<OutputElement, rank> name(                                                              \
        hpx::shared_future<InputElement> const& value_f, PartitionedArray<InputElement, rank> const& array)  \
    {                                                                                                        \
        using Policies_ = Policies<OutputElement, InputElement>;                                             \
                                                                                                             \
        return name<Policies_, OutputElement, InputElement, rank>(                                           \
            Policies_{}, Scalar<InputElement>{value_f}, array);                                              \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    /* f(value, array) */                                                                                    \
    template<typename OutputElement, typename InputElement, Rank rank>                                       \
    PartitionedArray<OutputElement, rank> name(                                                              \
        InputElement const& value, PartitionedArray<InputElement, rank> const& array)                        \
    {                                                                                                        \
        using Policies_ = Policies<OutputElement, InputElement>;                                             \
                                                                                                             \
        return name<Policies_, OutputElement, InputElement, rank>(                                           \
            Policies_{}, Scalar<InputElement>{value}, array);                                                \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    /* f(scalar, array) */                                                                                   \
    template<typename InputElement, Rank rank>                                                               \
    auto name(Scalar<InputElement> const& scalar, PartitionedArray<InputElement, rank> const& array)         \
    {                                                                                                        \
        using OutputElement = OutputElementT<Functor<InputElement>>;                                         \
        using Policies_ = Policies<OutputElement, InputElement>;                                             \
                                                                                                             \
        return name<Policies_, OutputElement, InputElement, rank>(Policies_{}, scalar, array);               \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    /* f(value_f, array) */                                                                                  \
    template<typename InputElement, Rank rank>                                                               \
    auto name(                                                                                               \
        hpx::shared_future<InputElement> const& value_f, PartitionedArray<InputElement, rank> const& array)  \
    {                                                                                                        \
        using OutputElement = OutputElementT<Functor<InputElement>>;                                         \
        using Policies_ = Policies<OutputElement, InputElement>;                                             \
                                                                                                             \
        return name<Policies_, OutputElement, InputElement, rank>(                                           \
            Policies_{}, Scalar<InputElement>{value_f}, array);                                              \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    /* f(value, array) */                                                                                    \
    template<typename InputElement, Rank rank>                                                               \
    auto name(InputElement const& value, PartitionedArray<InputElement, rank> const& array)                  \
    {                                                                                                        \
        using OutputElement = OutputElementT<Functor<InputElement>>;                                         \
        using Policies_ = Policies<OutputElement, InputElement>;                                             \
                                                                                                             \
        return name<Policies_, OutputElement, InputElement, rank>(                                           \
            Policies_{}, Scalar<InputElement>{value}, array);                                                \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    /* f(scalar, scalar) */                                                                                  \
    template<typename InputElement>                                                                          \
    auto name(Scalar<InputElement> const& scalar1, Scalar<InputElement> const& scalar2)                      \
    {                                                                                                        \
        using OutputElement = OutputElementT<Functor<InputElement>>;                                         \
        using Policies_ = Policies<OutputElement, InputElement>;                                             \
                                                                                                             \
        return name<Policies_, OutputElement, InputElement>(Policies_{}, scalar1, scalar2);                  \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    /* f(scalar, value) */                                                                                   \
    template<typename OutputElement, typename InputElement>                                                  \
    auto name(Scalar<InputElement> const& scalar, InputElement const& value)->Scalar<OutputElement>          \
    {                                                                                                        \
        using Policies_ = Policies<OutputElement, InputElement>;                                             \
                                                                                                             \
        return name<Policies_, OutputElement, InputElement>(                                                 \
            Policies_{}, scalar.future(), Scalar<InputElement>{value});                                      \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    /* f(scalar, value) */                                                                                   \
    template<typename InputElement>                                                                          \
    auto name(Scalar<InputElement> const& scalar, InputElement const& value)                                 \
    {                                                                                                        \
        using OutputElement = OutputElementT<Functor<InputElement>>;                                         \
        using Policies_ = Policies<OutputElement, InputElement>;                                             \
                                                                                                             \
        return name<Policies_, OutputElement, InputElement>(                                                 \
            Policies_{}, scalar, Scalar<InputElement>{value});                                               \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    /* f(value, scalar) */                                                                                   \
    template<typename OutputElement, typename InputElement>                                                  \
    auto name(InputElement const& value, Scalar<InputElement> const& scalar)->Scalar<OutputElement>          \
    {                                                                                                        \
        using Policies_ = Policies<OutputElement, InputElement>;                                             \
                                                                                                             \
        return name<Policies_, OutputElement, InputElement>(                                                 \
            Policies_{}, Scalar<InputElement>{value}, scalar.future());                                      \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    /* f(value, scalar) */                                                                                   \
    template<typename InputElement>                                                                          \
    auto name(InputElement const& value, Scalar<InputElement> const& scalar)                                 \
    {                                                                                                        \
        using OutputElement = OutputElementT<Functor<InputElement>>;                                         \
        using Policies_ = Policies<OutputElement, InputElement>;                                             \
                                                                                                             \
        return name<Policies_, OutputElement, InputElement>(                                                 \
            Policies_{}, Scalar<InputElement>{value}, scalar);                                               \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    /* f(value, value) */                                                                                    \
    template<typename OutputElement, typename InputElement>                                                  \
    auto name(InputElement const& value1, InputElement const& value2)                                        \
    {                                                                                                        \
        using Policies_ = Policies<OutputElement, InputElement>;                                             \
                                                                                                             \
        return name<Policies_, OutputElement, InputElement>(                                                 \
            Policies_{}, Scalar<InputElement>{value1}, Scalar<InputElement>{value2});                        \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    /* f(value, value) */                                                                                    \
    template<typename InputElement>                                                                          \
    auto name(InputElement const& value1, InputElement const& value2)                                        \
    {                                                                                                        \
        using OutputElement = OutputElementT<Functor<InputElement>>;                                         \
        using Policies_ = Policies<OutputElement, InputElement>;                                             \
                                                                                                             \
        return name<Policies_, OutputElement, InputElement>(                                                 \
            Policies_{}, Scalar<InputElement>{value1}, Scalar<InputElement>{value2});                        \
    }
