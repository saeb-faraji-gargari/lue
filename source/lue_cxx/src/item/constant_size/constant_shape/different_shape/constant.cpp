#include "lue/item/constant_size/constant_shape/different_shape/constant.hpp"
#include "lue/item/constant_size/constant_shape/different_shape/collection.hpp"
#include "lue/hdf5/chunk.hpp"
#include "lue/tag.hpp"


namespace lue {
namespace constant_size {
namespace constant_shape {
namespace different_shape {

/*!
    @brief      Open value @a name in @a parent
*/
Constant::Constant(
    hdf5::Group const& parent,
    std::string const& name)

    : Group{parent, name},
      _nr_items{attributes().read<hsize_t>(nr_items_tag)},
      _rank{attributes().read<int>(rank_tag)},
      _file_datatype{hdf5::decode_datatype(
          attributes().read<std::vector<unsigned char>>(datatype_tag))},
      _memory_datatype{hdf5::memory_datatype(_file_datatype)}

{
}


/*!
    @brief      Open value @a name in @a parent
*/
Constant::Constant(
    hdf5::Group const& parent,
    std::string const& name,
    hdf5::Datatype const& memory_datatype)

    : Group{parent, name},
      _nr_items{attributes().read<hsize_t>(nr_items_tag)},
      _rank{attributes().read<int>(rank_tag)},
      _file_datatype{hdf5::decode_datatype(
          attributes().read<std::vector<unsigned char>>(datatype_tag))},
      _memory_datatype{memory_datatype}

{
}


/*!
    @brief      Move in @a group
*/
Constant::Constant(
    hdf5::Group&& group,
    hdf5::Datatype const& memory_datatype)

    : Group{std::forward<hdf5::Group>(group)},
      _nr_items{attributes().read<hsize_t>(nr_items_tag)},
      _rank{attributes().read<int>(rank_tag)},
      _file_datatype{hdf5::decode_datatype(
          attributes().read<std::vector<unsigned char>>(datatype_tag))},
      _memory_datatype{memory_datatype}

{
}


/*!
    @brief      Return number of items for which values are stored
*/
hsize_t Constant::nr_items() const
{
    return _nr_items;
}


/*!
    @brief      Return rank of each of the item's value
*/
int Constant::rank() const
{
    return _rank;
}


/*!
    @brief      Return in-file datatype
*/
hdf5::Datatype const& Constant::file_datatype() const
{
    return _file_datatype;
}


/*!
    @brief      Return in-memory datatype
*/
hdf5::Datatype const& Constant::memory_datatype() const
{
    return _memory_datatype;
}


/*!
    @brief      Reserve space for a single item's value
    @param      idx Index of item
    @param      value_shape Shape of the item's value

    The underlying HDF5 dataset is chunked according to hdf5::chunk_shape().
*/
void Constant::reserve(
    hsize_t const idx,
    hdf5::Shape const& value_shape)
{
    std::string name = std::to_string(idx);

    auto dataspace = hdf5::create_dataspace(value_shape);

    hdf5::Dataset::CreationPropertyList creation_property_list;
    // TODO Handle chunking. This doesn't work.
    // auto chunk_dimension_sizes =
    //     hdf5::chunk_shape(value_shape, _file_datatype.size());
    // creation_property_list.set_chunk(chunk_dimension_sizes);

    hdf5::create_dataset(
        id(), name, _file_datatype, dataspace, creation_property_list);
}


/*!
    @brief      Reserve space for @a nr_items item values shaped as
                @a value_shapes
*/
void Constant::reserve(
    hsize_t const nr_items,
    hsize_t const* value_shapes)
{
    for(hsize_t i = 0; i < nr_items; ++i) {
        hsize_t const* shape = &(value_shapes[i * _rank]);
        reserve(i, hdf5::Shape(shape, shape + _rank));
    }

    _nr_items = nr_items;

    attributes().write<hsize_t>(nr_items_tag, nr_items);
}


/*!
    @brief      Return dataset corresponding to item @a idx
*/
Array Constant::operator[](
    hsize_t const idx) const
{
    return Array{*this, std::to_string(idx), _memory_datatype};
}


hdf5::Shape Constant::value_shape(
    hsize_t idx)
{
    return operator[](idx).shape();
}


void Constant::read(
    hsize_t const idx,
    void* buffer)
{
    operator[](idx).read(buffer);
}


void Constant::write(
    hsize_t const idx,
    void const* buffer)
{
    operator[](idx).write(buffer);
}


/*!
    @brief      Create collection @a name in @a parent

    The datatype is of the individual values. The @a rank passed in
    defines the dimensionality of the underlying datasets.
*/
Constant create_constant(
    hdf5::Group& parent,
    std::string const& name,
    hdf5::Datatype const& memory_datatype,
    int const rank)
{
    return create_constant(
        parent, name, hdf5::file_datatype(memory_datatype), memory_datatype,
        rank);
}


/*!
    @brief      Create value @a name in @a parent

    The datatypes are of the individual values. The @a rank passed in
    defines the dimensionality of the underlying datasets.
*/
Constant create_constant(
    hdf5::Group& parent,
    std::string const& name,
    hdf5::Datatype const& file_datatype,
    hdf5::Datatype const& memory_datatype,
    int const rank)
{
    auto group = create_group(parent, name, file_datatype, rank);

    group.attributes().write<hsize_t>(nr_time_domain_items_tag, 0);

    return Constant{std::move(group), memory_datatype};
}

}  // namespace different_shape
}  // namespace constant_shape
}  // namespace constant_size
}  // namespace lue
