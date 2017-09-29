#pragma once
#include "lue/utility/application.hpp"
#include "lue/validate/issues.hpp"


namespace lue {
namespace utility {

class Validate:
    public Application

{

public:

                   Validate            (std::vector<std::string> const&
                                            arguments);

    int            run_implementation  () final;

private:

    void           print_issues        (Issues const& issues) const;

};

}  // namespace utility
}  // namespace lue