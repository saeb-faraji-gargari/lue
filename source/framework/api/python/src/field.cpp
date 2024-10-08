#include "lue/framework/api/cxx/operator.hpp"
#include <pybind11/operators.h>


namespace lue::api {

    void bind_field(pybind11::module& module)
    {
        pybind11::class_<Field>(module, "Field")

            // bool(a), not a, if a, while a, ...
            .def(
                "__bool__",
                []([[maybe_unused]] Field const& field)
                {
                    // ValueError
                    throw std::invalid_argument("The truth value of a field is ambiguous");
                })

            // a + b, a += b
            // .def(pybind11::self + pybind11::self)
            .def("__add__", add, pybind11::is_operator())
            .def("__radd__", add, pybind11::is_operator())


            // TODO
            // https://pybind11.readthedocs.io/en/stable/advanced/classes.html#operator-overloading

            ;
    }

}  // namespace lue::api
