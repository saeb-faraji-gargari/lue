#include "submodule.hpp"
#include <pybind11/pybind11.h>


namespace py = pybind11;


namespace lue {
namespace different_shape {
namespace variable_shape {

void init_submodule(
    py::module& module)
{
    py::module submodule = module.def_submodule(
        "variable_shape",
        R"(
    :mod:`lue.different_shape.variable_shape` --- Object arrays
    ===========================================================

    The :mod:`lue.different_shape.variable_shape` package contains
    functionality for manipulating object arrays with different shapes
    that (the shapes) are variable through time.
)");

    init_properties(submodule);
    init_property(submodule);
    init_value(submodule);
}

}  // namespace variable_shape
}  // namespace different_shape
}  // namespace lue