#include "../python_extension.hpp"
#include "lue/object/universes.hpp"
#include "../core/collection.hpp"
#include <pybind11/pybind11.h>
#include <boost/algorithm/string/join.hpp>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {
namespace data_model {
namespace {

static std::string formal_string_representation(
    Universe const& universe)
{
    return fmt::format(
        "Universe(pathname='{}')",
        universe.id().pathname());
}


static std::string informal_string_representation(
    Universe const& universe)
{
    return fmt::format(
        "{}\n"
        "    phenomena: [{}]",
        formal_string_representation(universe),
        boost::algorithm::join(universe.phenomena().names(), ", "));
}

}  // Anonymous namespace


void init_universe(
    py::module& module)
{

    BASE_COLLECTION(Universes, Universe)


    py::class_<Universes, UniverseCollection>(
        module,
        "Universes",
        R"(
    Collection of universes

    Universe collections can be obtained from :class:`Dataset` instances.
)")

        .def(
            "add",
            &Universes::add,
            "name"_a,
            R"(
    Add new universe to collection

    :param str name: Name of universe to create
    :return: Universe created
    :rtype: lue.Universe
    :raises RuntimeError: In case the universe cannot be created
)",
            py::return_value_policy::reference_internal)

        ;


    py::class_<Universe, hdf5::Group>(
        module,
        "Universe",
        R"(
    Class for representing a system's state

    A universe contains :class:`phenomena <Phenomena>` that have something
    in common, for example:

    - They are all generated by the same model
    - They are all measured in the same field campaign
    - They are all created by the same organization

    Universes can be used to organize alternative versions of the same
    state, for example to manage state generated by different models.
)")

        .def(
            "__repr__",
            [](Universe const& universe) {
                return formal_string_representation(universe);
            }
        )

        .def(
            "__str__",
            [](Universe const& universe) {
                return informal_string_representation(universe);
            }
        )

        .def(
            "add_phenomenon",
            &Universe::add_phenomenon,
            "name"_a,
            "description"_a="",
            R"(
    Add new phenomenon to dataset

    :param str name: Name of phenomenon to create
    :param str description: Description
    :return: Phenomenon created
    :rtype: lue.Phenomenon
    :raises RuntimeError: In case the phenomenon cannot be created
)",
            py::return_value_policy::reference_internal)

        .def_property_readonly(
            "phenomena",
            py::overload_cast<>(&Universe::phenomena),
            R"(
    Return phenomena collection

    :rtype: lue.Phenomena
)",
            py::return_value_policy::reference_internal)

        .def(
            "__getattr__",
            [](
                Universe& universe,
                std::string const& phenomenon_name)
            {
                if(!universe.phenomena().contains(phenomenon_name)) {
                    // TODO We are throwing a KeyError here. Should be
                    // an AttributeError, but pybind11 does not seem to
                    // support that yet.
                    //
                    // Python message:
                    // AttributeError: 'x' object has no attribute 'y'
                    // Ours is a little bit different:
                    throw pybind11::key_error(fmt::format(
                        "Universe does not contain phenomenon '{}'",
                        phenomenon_name));
                }

                return py::cast(universe.phenomena()[phenomenon_name]);
            },
            py::return_value_policy::reference_internal)

        ;

}

}  // namespace data_model
}  // namespace lue
