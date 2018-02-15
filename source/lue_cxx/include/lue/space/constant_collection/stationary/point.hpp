#pragma once
#include "lue/item/constant_shape/same_shape/continuous_value.hpp"


namespace lue {
namespace constant_collection {
namespace stationary {

class Point:
    public lue::constant_shape::same_shape::ContinuousValue
{

public:

                   Point               (hdf5::Group const& parent,
                                        hdf5::Datatype const& memory_datatype);

                   Point               (Point const&)=delete;

                   Point               (Point&&)=default;

                   ~Point              ()=default;

    Point&         operator=           (Point const&)=delete;

    Point&         operator=           (Point&&)=default;

private:

};


Point              create_point        (hdf5::Group& parent,
                                        hdf5::Datatype const& memory_datatype,
                                        std::size_t rank);

Point              create_point        (hdf5::Group& parent,
                                        hdf5::Datatype const& file_datatype,
                                        hdf5::Datatype const& memory_datatype,
                                        std::size_t rank);

}  // namespace stationary
}  // namespace constant_collection
}  // namespace lue
