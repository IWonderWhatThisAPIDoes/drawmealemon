/**
 * @file default_target_factory.hpp
 * 
 * Definition of the @ref target_factory that can construct
 * the default set of render targets
 */

#pragma once

#include <iostream>
#include "target_factory.hpp"

namespace dmalem {

/**
 * Constructs the target factory with default modules
 * 
 * @param ostr Output stream that stream-based render targets will render to.
 * Must outlive the factory and all render targets created by it
 * @return Target factory with default modules
 */
target_factory default_target_factory(std::ostream& ostr);

}
