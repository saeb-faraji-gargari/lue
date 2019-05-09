#include "submodule.hpp"
#include <pybind11/pybind11.h>


namespace py = pybind11;


namespace lue {
namespace different_shape {
namespace constant_shape {

void init_submodule(
    py::module& module)
{
    py::module submodule = module.def_submodule(
        "constant_shape",
        R"(
    :mod:`lue.different_shape.constant_shape` --- Object arrays
    ===========================================================

    The :mod:`lue.different_shape.constant_shape` package contains
    functionality for manipulating object arrays with different shapes
    that (the shapes) are constant through time.
)");

    init_properties(submodule);
    init_property(submodule);
    init_value(submodule);
}

}  // namespace constant_shape
}  // namespace different_shape
}  // namespace lue