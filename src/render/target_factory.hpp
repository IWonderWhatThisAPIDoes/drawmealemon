/**
 * @file target_factory.hpp
 * 
 * Construction of @ref render_target implementations
 */

#pragma once

#include <map>
#include <memory>
#include <string_view>
#include "render_target.hpp"
#include "target_factory_module.hpp"

namespace dmalem {

/**
 * Constructs implementations of @ref render_target
 */
class target_factory {
public:
    /**
     * Exception that signals that @ref create_by_name
     * was passed a render target name that does not correspond
     * to any render target type
     */
    class bad_render_target_name : public std::invalid_argument {
    public:
        /**
         * Constructs a bad render target name exception
         *
         * @param name The requested render target name
         */
        bad_render_target_name(const std::string_view& name);
        /**
         * Get the requested render target name that caused the exception
         * 
         * @return Render target name requested by the caller
         */
        const std::string& the_name() const noexcept;
    private:
        std::string name;
    };

    /**
     * Registers a new implementation under a given name
     * 
     * @param name Name of the render target implementation
     * @param mod Factory module that constructs the new implementation
     * @throw std::invalid_argument @p name is already in use
     */
    void add_module(const std::string_view& name, std::unique_ptr<target_factory_module>&& mod);
    /**
     * Constructs a @ref render_target by the name of its implementation
     * 
     * @param name Name of the render target implementation
     * @param options Options that affect behavior of the render target
     * @throw target_factory_module::bad_render_target_options @p options are not valid
     * @throw target_factory::bad_render_target_name @p name is not associated with a render target
     */
    std::unique_ptr<render_target> create_by_name(
        const std::string_view& name,
        const std::vector<std::string>& options
    ) const;

private:
    std::map<std::string, std::unique_ptr<target_factory_module>> modules;
};

}
