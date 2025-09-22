/**
 * @file target_factory_module.cpp
 *
 * Generic module of a factory that constructs one type
 * of @ref render_target
 */

#include "target_factory_module.hpp"

namespace dmalem {

target_factory_module::bad_render_target_options::bad_render_target_options(
    const std::string_view& badOption
) :
    invalid_argument("Incorrect options passed to render target factory"),
    badOption(badOption)
{}

const std::string& target_factory_module::bad_render_target_options::bad_option() const noexcept {
    return badOption;
}

void target_factory_module::set_options(const std::vector<std::string>& options) {
    if (!options.empty())
        throw bad_render_target_options(options.front());
}

}
