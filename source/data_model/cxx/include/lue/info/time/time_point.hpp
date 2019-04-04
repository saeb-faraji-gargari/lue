#pragma once
#include "lue/info/time/location_in_time.hpp"


namespace lue {

/*!
    - Zero or more time points per item
    - Each time point has a unique location in time
*/
class TimePoint:
    public LocationInTime
{

public:

                   TimePoint           (hdf5::Group& parent);

                   TimePoint           (TimePoint const&)=delete;

                   TimePoint           (TimePoint&&)=default;

                   TimePoint           (LocationInTime&& value);

                   ~TimePoint          ()=default;

    TimePoint&     operator=           (TimePoint const&)=delete;

    TimePoint&     operator=           (TimePoint&&)=default;

    Count          nr_points           () const;

private:

};


TimePoint          create_time_point   (hdf5::Group& parent);

}  // namespace lue
