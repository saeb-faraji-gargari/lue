#include "lue/framework/api/c/io.h"
#include "lue/framework/api/cxx/io.hpp"
#include "field.hpp"


auto from_gdal(char const* name) -> Field*
{
    lue::api::Field result = lue::api::from_gdal(name);

    return new Field{.instance = new lue::api::Field{std::move(result)}};
}


void to_gdal(Field* field, char const* name)
{
    lue::api::to_gdal(as_cxx_field(field), name);
}
