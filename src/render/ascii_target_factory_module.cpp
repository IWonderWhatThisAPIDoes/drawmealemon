/**
 * @file ascii_target_factory_module.cpp
 * 
 * @ref target_factory_module that creates render targets
 * that render the parser's execution as ASCII art
 */

#include "ascii_target_factory_module.hpp"
#include "ascii_target.hpp"

namespace dmalem {

ascii_target_factory_module::ascii_target_factory_module(std::ostream& ostr) :
    ostr(&ostr)
{}

std::unique_ptr<render_target> ascii_target_factory_module::create_render_target() const {
    return std::make_unique<ascii_target>(*ostr);
}

void ascii_target_factory_module::set_options(const std::vector<std::string>& options) {
    // For now
    target_factory_module::set_options(options);
}

}
