/**
 * @file target_factory.hpp
 * 
 * Construction of @ref render_target implementations
 */

#pragma once

#include <memory>
#include <string_view>
#include "render_target.hpp"

namespace dmalem {

/**
 * Constructs implementations of @ref render_target
 */
class target_factory {
public:
    /**
     * Constructs a @ref render_target by the name of its implementation
     * 
     * @param name Name of the render target implementation
     */
    static std::unique_ptr<render_target> create_by_name(const std::string_view& name);
};

}
