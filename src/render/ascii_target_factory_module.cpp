/**
 * @file ascii_target_factory_module.cpp
 * 
 * @ref target_factory_module that creates render targets
 * that render the parser's execution as ASCII art
 */

#include <charconv>
#include "ascii_target_factory_module.hpp"
#include "ascii_target.hpp"
#include "pure_ascii_fragment_table.hpp"

namespace dmalem {

ascii_target_factory_module::ascii_target_factory_module(std::ostream& ostr) :
    ostr(&ostr)
{}

std::unique_ptr<render_target> ascii_target_factory_module::create_render_target() const {
    auto fragments = std::make_unique<pure_ascii_fragment_table>();
    fragments->input_column_width(inputColumnWidth);
    auto target = std::make_unique<ascii_target>(*ostr, std::move(fragments));
    return target;
}

void ascii_target_factory_module::set_options(const std::vector<std::string>& options) {
    size_t newInputColumnWidth = 0;

    for (const auto& option : options) {
        if (option.starts_with("iw=")) {
            if (newInputColumnWidth)
                throw bad_render_target_options(option);
            auto result = std::from_chars(&option[3], &option.back() + 1, newInputColumnWidth);
            if (result.ec != std::errc() || result.ptr != &option.back() + 1 || newInputColumnWidth == 0)
                throw bad_render_target_options(option);
        } else {
            throw bad_render_target_options(option);
        }
    }

    if (newInputColumnWidth)
        inputColumnWidth = newInputColumnWidth;
}

}
