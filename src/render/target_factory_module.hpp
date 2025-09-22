/**
 * @file target_factory_module.hpp
 *
 * Generic module of a factory that constructs one type
 * of @ref render_target
 */

#pragma once

#include <memory>
#include <vector>
#include <stdexcept>
#include "render_target.hpp"

namespace dmalem {

/**
 * Factory that creates one implementation of @ref render_target
 */
class target_factory_module {
public:
    /**
     * Exception that signals that @ref target_factory_module::set_options
     * has received an option that it could not process
     */
    class bad_render_target_options : public std::invalid_argument {
    public:
        /**
         * Constructs a bad render target options exception
         *
         * @param badOptions Option that was not recognized by the target type
         */
        bad_render_target_options(const std::string_view& badOption);
        /**
         * Get the option that caused this exception
         * 
         * @return Option that was not recognized by the target type
         */
        const std::string& bad_option() const noexcept;
    private:
        std::string badOption;
    };

    virtual ~target_factory_module() {}
    /**
     * Constructs a new instance of the module's @ref render_target implementation
     *
     * @return The constructed render target
     */
    virtual std::unique_ptr<render_target> create_render_target() const = 0;
    /**
     * Sets the options that can define specific output settings for the render target
     * 
     * Default implementation expects no options and fails if the argument is non-empty
     *
     * @param options The options that define requested settings for the target
     * @throw bad_render_target_options @p options is not valid for this type of render target
     */
    virtual void set_options(const std::vector<std::string>& options);
};

}
