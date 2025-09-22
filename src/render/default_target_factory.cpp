/**
 * @file default_target_factory.hpp
 * 
 * Definition of the @ref target_factory that can construct
 * the default set of render targets
 */

#include "default_target_factory.hpp"
#include "ascii_target_factory_module.hpp"

namespace dmalem {

target_factory default_target_factory(std::ostream& ostr) {
    target_factory factory;
    factory.add_module("", std::make_unique<ascii_target_factory_module>(ostr));
    factory.add_module("ascii", std::make_unique<ascii_target_factory_module>(ostr));
    return factory;
}

}
