/**
 * @file ascii_target_factory_module.hpp
 * 
 * @ref target_factory_module that creates render targets
 * that render the parser's execution as ASCII art
 */

#pragma once

#include <iostream>
#include "target_factory_module.hpp"

namespace dmalem {

/**
 * @ref target_factory_module that creates ASCII render targets
 * 
 * ## Render Target Options
 * | Option   | Description                                       |
 * |----------|---------------------------------------------------|
 * | `iw=<n>` | Sets the width of the input column, in characters |
 */
class ascii_target_factory_module : public target_factory_module {
public:
    /**
     * Constructs a new ASCII render target factory
     * 
     * @param ostr Stream that all render targets created by the factory
     * will render to. Must outlive all render targets created by the factory
     */
    ascii_target_factory_module(std::ostream& ostr);

    std::unique_ptr<render_target> create_render_target() const override;
    void set_options(const std::vector<std::string>& options) override;
private:
    std::ostream* ostr;
    size_t inputColumnWidth = 0;
};

}
