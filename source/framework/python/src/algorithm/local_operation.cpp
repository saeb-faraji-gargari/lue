#include <pybind11/pybind11.h>


// TODO These are not all local operations. Split them over multiple
//     modules, or rename this module.
namespace lue::framework {

    void bind_add(pybind11::module& module);
    void bind_all(pybind11::module& module);
    void bind_divide(pybind11::module& module);
    void bind_equal_to(pybind11::module& module);
    void bind_greater_than(pybind11::module& module);
    void bind_greater_than_equal_to(pybind11::module& module);
    void bind_less_than(pybind11::module& module);
    void bind_less_than_equal_to(pybind11::module& module);
    void bind_maximum(pybind11::module& module);
    void bind_minimum(pybind11::module& module);
    void bind_multiply(pybind11::module& module);
    void bind_not_equal_to(pybind11::module& module);
    void bind_sqrt(pybind11::module& module);
    void bind_subtract(pybind11::module& module);
    void bind_uniform(pybind11::module& module);
    void bind_where(pybind11::module& module);


    void bind_local_operations(
        pybind11::module& module)
    {
        bind_add(module);
        bind_all(module);
        bind_divide(module);
        bind_equal_to(module);
        bind_greater_than(module);
        bind_greater_than_equal_to(module);
        bind_less_than(module);
        bind_less_than_equal_to(module);
        bind_maximum(module);
        bind_minimum(module);
        bind_not_equal_to(module);
        bind_sqrt(module);
        bind_subtract(module);
        bind_multiply(module);
        bind_uniform(module);
        bind_where(module);
    }

}  // namespace lue::framework
