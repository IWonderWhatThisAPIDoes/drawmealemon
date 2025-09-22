/**
 * @file target_factory.cpp
 * 
 * Construction of @ref render_target implementations
 */

#include <iostream>
#include "target_factory.hpp"

namespace dmalem {

target_factory::bad_render_target_name::bad_render_target_name(const std::string_view& name) :
    invalid_argument("No render target type registered for requested name"),
    name(name)
{}

const std::string& target_factory::bad_render_target_name::the_name() const noexcept {
    return name;
}

void target_factory::add_module(
    const std::string_view& name,
    std::unique_ptr<target_factory_module>&& mod
) {
    std::string nameStr(name);
    if (modules.contains(nameStr))
        throw std::invalid_argument(__FUNCTION__);
    modules.emplace(std::move(nameStr), std::move(mod));
}

std::unique_ptr<render_target> target_factory::create_by_name(
    const std::string_view& name,
    const std::vector<std::string>& options
) const {
    const auto it = modules.find(std::string(name));
    if (it == modules.end())
        throw bad_render_target_name(name);
    auto& target_module = *it->second;
    target_module.set_options(options);
    return target_module.create_render_target();
}

}
