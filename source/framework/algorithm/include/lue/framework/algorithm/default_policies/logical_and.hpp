#pragma once
#include "lue/framework/algorithm/logical_and.hpp"
#include "lue/framework/algorithm/operator.hpp"


namespace lue {
    namespace policy::logical_and {

        template<
            typename OutputElement,
            typename InputElement>
        using DefaultPolicies = policy::DefaultPolicies<
            AllValuesWithinDomain<InputElement, InputElement>,
            OutputElements<OutputElement>,
            InputElements<InputElement, InputElement>>;

    }  // namespace policy::logical_and


    namespace default_policies {

        LUE_BINARY_LOCAL_OPERATION_OVERLOADS2(
            logical_and, detail::LogicalAnd, policy::logical_and::DefaultPolicies)
        LUE_BINARY_LOGICAL_OPERATOR(&&, logical_and)

    }  // namespace default_policies
}  // namespace lue
