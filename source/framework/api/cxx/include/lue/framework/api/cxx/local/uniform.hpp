#pragma once
#include "lue/framework/api/cxx/export.hpp"
#include "lue/framework/api/cxx/field.hpp"


namespace lue::api {

    auto LUE_FAPI_EXPORT uniform(
        Shape<Count, 2> const& array_shape,
        Shape<Count, 2> const& partition_shape,
        Field const& min_value,
        Field const& max_value) -> Field;

    auto LUE_FAPI_EXPORT
    uniform(Shape<Count, 2> const& array_shape, Field const& min_value, Field const& max_value) -> Field;

}  // namespace lue::api
