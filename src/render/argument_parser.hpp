/**
 * @file argument_parser.hpp
 * 
 * Parsing of the command line
 */

#pragma once

#include <string>
#include <vector>
#include <stdexcept>
#include "render_target.hpp"

namespace dmalem {

/**
 * Handles the parsing of the command line
 */
class argument_parser {
public:
    /**
     * Output of the command line parser
     */
    struct output {
        /**
         * Identifier of the selected render target
         * 
         * Empty if no render target is set
         */
        std::string targetName;
        /**
         * Options that should be passed to the selected render target
         */
        std::vector<std::string> targetOptions;
    };
    /**
     * Identifiers of error conditions in the command line
     */
    enum class error_code {
        /**
         * A flag was expected, but something else was found
         */
        missing_flag,
        /**
         * A flag that is not recognized by the parser was found
         */
        unknown_flag,
        /**
         * A plain argument was expected, but a flag was found
         */
        missing_argument,
        /**
         * A flag was used more than once in a contradictory way
         */
        duplicate_flag,
    };
    /**
     * Exception thrown by the parser when invalid input is received
     */
    class error : public std::logic_error {
    public:
        /**
         * Constructor
         * 
         * @param code Error code that identifies the condition that caused the error
         * @param argument The offending argument that caused the error
         */
        explicit error(error_code code, const char* argument);
        /**
         * Retrieves the error code that identifies the condition that caused the error
         * 
         * @return The error code
         */
        error_code code() const noexcept;
        /**
         * Retrieves the offending argument that caused the error
         * 
         * @return The offending argument, as a null-terminated string,
         *         that lives for as long as the error object
         */
        const char* argument() const noexcept;
    private:
        error_code errcode;
        std::string offendingArgument;
        /**
         * Constructs the message that describes the error
         * 
         * This is the same message returned by @ref std:exception::what
         * 
         * @param code Error code that identifies the condition that caused the error
         * @param argument The offending argument that caused the error
         * @return Message that describes the error
         */
        static std::string make_message(error_code code, const char* argument);
    };
    /**
     * Parses a command line
     * 
     * @param argc How many arguments are present
     * @param argv Array that contains the arguments, as null-terminated strings
     * @return Options extracted from the command line
     * @throw argument_parser::error The arguments are not valid
     */
    static output parse(size_t argc, const char* const * argv);
};

}
