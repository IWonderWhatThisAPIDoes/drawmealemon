/**
 * @file target_factory.cpp
 * 
 * Construction of @ref render_target implementations
 */

#include <iostream>
#include "target_factory.hpp"
#include "ascii_target.hpp"

namespace dmalem {

std::unique_ptr<render_target> target_factory::create_by_name(const std::string_view& name) {
    if (name == "ascii" || name == "")
        return std::make_unique<ascii_target>(std::cout);
    else
        return nullptr;
}

}
