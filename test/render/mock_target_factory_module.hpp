/**
 * @file mock_target_factory_module.hpp
 * 
 * Mock implementation of @ref target_factory_module for testing
 */

#pragma once

#include "../../src/render/target_factory_module.hpp"
#include "../testbed/mock.hpp"

namespace dmalem {

/**
 * Mock implementation of @ref target_factory_module
 */
class mock_target_factory_module : public target_factory_module {
public:
    mock_target_factory_module() = default;
    mock_target_factory_module(test::mock_context& context) : context(context) {}
    std::unique_ptr<render_target> create_render_target() const override { return createRenderTarget(); }
    void set_options(const std::vector<std::string>& options) override { return setOptions(options); }
    test::mock_context context;
    test::mock<std::unique_ptr<render_target>()> createRenderTarget{context};
    test::mock<void(const test::mkeep<std::vector<std::string>>&)> setOptions{context};
};

}
