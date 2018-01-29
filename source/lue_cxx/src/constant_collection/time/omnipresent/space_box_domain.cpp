#include "lue/constant_collection/time/omnipresent/space_box_domain.hpp"
#include "lue/tag.hpp"


namespace lue {
namespace constant_collection {
namespace time {
namespace omnipresent {

hdf5::Datatype SpaceBoxDomain::file_datatype(
    hdf5::Group const& parent)
{
    return hdf5::Dataset(parent, coordinates_tag).datatype();
}


SpaceBoxDomain::SpaceBoxDomain(
    SpaceDomain&& space_domain)

    : SpaceBoxDomain{
        std::forward<SpaceDomain>(space_domain),
        hdf5::memory_datatype(file_datatype(space_domain))}

{
}


SpaceBoxDomain::SpaceBoxDomain(
    SpaceDomain&& space_domain,
    hdf5::Datatype const& memory_datatype)

    : SpaceDomain{std::forward<SpaceDomain>(space_domain)},
      _items{*this, coordinates_tag, memory_datatype}

{
}


SpaceBoxDomain::SpaceBoxes const& SpaceBoxDomain::items() const
{
    return _items;
}


SpaceBoxDomain::SpaceBoxes& SpaceBoxDomain::items()
{
    return _items;
}


SpaceBoxDomain::SpaceBoxes& SpaceBoxDomain::reserve(
    hsize_t const nr_items)
{
    _items.reserve(nr_items);

    return _items;
}


// bool space_box_domain_exists(
//     hdf5::Group const& group)
//     // PropertySet const& property_set)
// {
//     auto const& space = property_set.domain().space();
// 
//     return space_box_exists(space);
// }


// void create_space_box_domain(
//     hdf5::Identifier const& location,
//     hid_t const file_type_id,
//     hid_t const memory_type_id,
//     size_t rank)
// {
//     create_space_box(location, file_type_id, memory_type_id, rank);
// }


SpaceBoxDomain create_space_box_domain(
    PropertySet& property_set,
    hdf5::Datatype const file_datatype,
    hdf5::Datatype const memory_datatype,
    size_t const rank)
{
    auto& domain = property_set.domain();

    auto space = omnipresent::create_space_domain(domain,
        SpaceDomain::Configuration(
            SpaceDomain::Configuration::DomainType::located,
            SpaceDomain::Configuration::ItemType::box)
    );

    // auto& space = domain.space();

    // create_space_box(space, file_datatype, memory_datatype, rank);

    // A box is defined by the coordinates of two opposite points
    // (diagonally). Two of them is enough.
    hdf5::Shape value_shape = {2 * rank };

    constant_shape::same_shape::create_constant(
        space, coordinates_tag, file_datatype, memory_datatype, value_shape);

    return SpaceBoxDomain{std::move(space), memory_datatype};
}

}  // namespace omnipresent
}  // namespace time
}  // namespace constant_collection
}  // namespace lue