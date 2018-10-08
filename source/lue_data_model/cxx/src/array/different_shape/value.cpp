#include "lue/array/different_shape/value.hpp"
#include "lue/core/tag.hpp"


namespace lue {
namespace different_shape {

/*!
    @brief      Open value @a name in @a parent
*/
Value::Value(
    hdf5::Group& parent,
    std::string const& name)
:
    ValueGroup{parent, name},
    _nr_objects{attributes().read<Count>(nr_objects_tag)}

{
}


/*!
    @brief      Open value @a name in @a parent
*/
Value::Value(
    hdf5::Group& parent,
    std::string const& name,
    hdf5::Datatype const& memory_datatype)
:
    ValueGroup{parent, name, memory_datatype},
    _nr_objects{attributes().read<Count>(nr_objects_tag)}

{
}


Value::Value(
    ValueGroup&& group)
:
    ValueGroup{std::forward<ValueGroup>(group)},
    _nr_objects{attributes().read<Count>(nr_objects_tag)}

{
}


Count Value::nr_objects() const
{
    return _nr_objects;
}


/*!
    @brief      Make space for additional object arrays of objects
    @param      nr_objects Number of objects (equals number of object
                arrays and number of value arrays)
    @param      ids For each object, the object ID
    @param      shapes For each object, the shape of the object array
*/
void Value::expand(
    Count const nr_objects,
    ID const* ids,
    hdf5::Shape const* shapes)
{
    for(std::size_t o = 0; o < nr_objects; ++o) {
        expand(ids[o], shapes[o]);
    }

    _nr_objects += nr_objects;

    attributes().write<Count>(nr_objects_tag, _nr_objects);
}


/*!
    @brief      Make space for additional object arrays of objects
    @param      nr_objects Number of objects (equals number of object
                arrays and number of value arrays)
    @param      ids For each object, the object ID
    @param      shapes For each object, the shape of the object array. For
                each object, @a shapes must contain rank sizes.
*/
void Value::expand(
    Count const nr_objects,
    ID const* ids,
    hdf5::Shape::value_type const* shapes)
{
    auto rank = this->rank();

    for(std::size_t o = 0, s = 0; o < nr_objects; ++o, s +=rank) {
        expand(ids[o], hdf5::Shape{shapes[s], shapes[s + 1]});
    }

    _nr_objects += nr_objects;

    attributes().write<Count>(nr_objects_tag, _nr_objects);
}


/*!
    @brief      Make space for an additional object array
    @param      id The object ID
    @param      shape The shape of the object array
*/
void Value::expand(
    ID const id,
    hdf5::Shape const& shape)
{
    std::string const name = std::to_string(id);
    hdf5::Shape max_dimension_sizes{shape};
    auto dataspace = hdf5::create_dataspace(shape, max_dimension_sizes);

    hdf5::Dataset::CreationPropertyList creation_property_list;
    // TODO
    // auto chunk_dimension_sizes =
    //     hdf5::chunk_shape(array_shape, file_datatype.size());
    // creation_property_list.set_chunk(chunk_dimension_sizes);

    /* auto dataset = */ hdf5::create_dataset(
        this->id(), name, file_datatype(), dataspace, creation_property_list);
}


Array Value::operator[](
    ID const id)
{
    std::string const name = std::to_string(id);

    return Array{*this, name, memory_datatype()};
}


/*!
    @brief      Create value @a name in @a parent
*/
Value create_value(
    hdf5::Group& parent,
    std::string const& name,
    hdf5::Datatype const& memory_datatype,
    Rank const rank)
{
    return create_value(
        parent, name,
        file_datatype(memory_datatype), memory_datatype,
        rank);
}


/*!
    @brief      Create value @a name in @a parent
*/
Value create_value(
    hdf5::Group& parent,
    std::string const& name,
    hdf5::Datatype const& file_datatype,
    hdf5::Datatype const& memory_datatype,
    Rank const rank)
{
    auto group = create_value_group(
        parent, name, file_datatype, memory_datatype, rank);

    group.attributes().write<Count>(nr_objects_tag, 0);

    return Value{std::move(group)};
}

}  // namespace different_shape
}  // namespace lue