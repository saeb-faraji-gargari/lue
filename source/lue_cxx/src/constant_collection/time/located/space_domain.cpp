#include "lue/constant_collection/time/located/space_domain.hpp"


namespace lue {
namespace constant_collection {
namespace time {
namespace located {

SpaceDomain::SpaceDomain(
    hdf5::Group const& group)

    : constant_collection::SpaceDomain(group)

{
}


SpaceDomain::SpaceDomain(
    constant_collection::SpaceDomain&& domain)

    : constant_collection::SpaceDomain{
          std::forward<constant_collection::SpaceDomain>(domain)}

{
}


SpaceDomain create_space_domain(
    hdf5::Group const& group,
    SpaceDomain::Configuration const& configuration)
{
    auto domain = constant_collection::create_space_domain(group, configuration);

    return std::move(domain);
}

}  // namespace located
}  // namespace time
}  // namespace constant_collection
}  // namespace lue