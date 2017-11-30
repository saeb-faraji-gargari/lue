#include "lue/item/constant_size/same_shape.hpp"
#include "lue/hdf5/chunk.hpp"
#include <cassert>


namespace lue {
namespace constant_size {

/*!
    @brief      Open value @a name in @a parent
*/
SameShape::SameShape(
    hdf5::Group const& parent,
    std::string const& name,
    hdf5::Datatype const& memory_datatype)

    : Array{parent, name, memory_datatype}

{
}


/*!
    @brief      Move in @a dataset
*/
SameShape::SameShape(
    hdf5::Dataset&& dataset,
    hdf5::Datatype const& memory_datatype)

    : Array{std::forward<hdf5::Dataset>(dataset), memory_datatype}

{
}


/*!
    @brief      Reserve space for @a nr_items item values
*/
void SameShape::reserve(
    hsize_t const nr_items)
{
    auto shape = this->shape();
    shape[0] = nr_items;

    resize(shape);
}


/*!
    @brief      Return number of items for which values are stored
*/
hsize_t SameShape::nr_items() const
{
    return shape()[0];
}


/*!
    @brief      Return shape of an item value

    The shape returned is not the shape of the underlying HDF5 dataset. It
    is the shape of each of the item values.
*/
hdf5::Shape SameShape::value_shape() const
{
    auto const shape = this->shape();

    assert(shape.begin() != shape.end());

    return hdf5::Shape{shape.begin() + 1, shape.end()};
}


hdf5::Hyperslab SameShape::hyperslab(
    hsize_t const idx) const
{
    auto shape = this->shape();

    hdf5::Offset offset(shape.size(), 0);
    offset[0] = idx;

    hdf5::Count count(shape.begin(), shape.end());
    count[0] = 1;

    return hdf5::Hyperslab{offset, count};
}


void SameShape::read(
    hsize_t const idx,
    void* buffer)
{
    Array::read(hyperslab(idx), buffer);
}


void SameShape::write(
    hsize_t const idx,
    void const* buffer)
{
    Array::write(hyperslab(idx), buffer);
}


/*!
    @brief      Create value @a name in @a parent
*/
SameShape create_same_shape(
    hdf5::Group const& parent,
    std::string const& name,
    hdf5::Datatype const& memory_datatype)
{
    return create_same_shape(
        parent, name, file_datatype(memory_datatype), memory_datatype,
        hdf5::Shape{});
}


/*!
    @brief      Create value @a name in @a parent
*/
SameShape create_same_shape(
    hdf5::Group const& parent,
    std::string const& name,
    hdf5::Datatype const& file_datatype,
    hdf5::Datatype const& memory_datatype)
{
    return create_same_shape(
        parent, name, file_datatype, memory_datatype, hdf5::Shape{});
}


/*!
    @brief      Create value @a name in @a parent

    The @a value_shape passed in is the shape of each of the individual
    item values.

    The underlying HDF5 dataset is chunked according to hdf5::chunk_shape().
*/
SameShape create_same_shape(
    hdf5::Group const& parent,
    std::string const& name,
    hdf5::Datatype const& file_datatype,
    hdf5::Datatype const& memory_datatype,
    hdf5::Shape const& value_shape)
{
    hdf5::Shape dimension_sizes{value_shape};
    dimension_sizes.insert(dimension_sizes.begin(), 0);

    hdf5::Shape max_dimension_sizes{value_shape};
    max_dimension_sizes.insert(max_dimension_sizes.begin(), H5S_UNLIMITED);

    auto dataspace = hdf5::create_dataspace(
        dimension_sizes, max_dimension_sizes);

    hdf5::Dataset::CreationPropertyList creation_property_list;
    auto chunk_dimension_sizes =
        hdf5::chunk_shape(value_shape, file_datatype.size());
    creation_property_list.set_chunk(chunk_dimension_sizes);

    auto dataset = hdf5::create_dataset(
        parent.id(), name, file_datatype, dataspace, creation_property_list);

    return SameShape{std::move(dataset), memory_datatype};
}

}  // namespace constant_size
}  // namespace lue
