/**
 * @file trace_parser.hpp
 * 
 * Parsing of trace output from Lemon
 */

#pragma once

#include <iostream>
#include <vector>
#include <functional>
#include "string_pattern.hpp"

namespace dmalem {

/**
 * Parses trace output from Lemon, line by line,
 * by matching against string patterns,
 * and calls the handlers associated with the matching pattern
 * 
 * @tparam T Type of the target object on which handlers operate
 */
template<class T>
class trace_parser {
public:
    /**
     * Type of a callback that handles a trace line pattern
     * 
     * @param target Target object passed to the handler
     * @param fields Fields matched by the string pattern associated with the callback
     */
    using trace_action = std::function<void(T& target, const std::vector<string_pattern::field>& fields)>;
    /**
     * Type of the list of patterns and their handlers sent to the parser
     */
    using pattern_list = std::vector<std::pair<string_pattern, trace_action>>;
    /**
     * Reads an input stream to the end, and parses each line and calls
     * the handler associated with its pattern
     * 
     * If a line fails to parse, it is reported to @p log
     * 
     * @param    input The input stream
     * @param patterns String patterns to match and their associated handlers
     * @param      log Receives error reports when a line fails to parse
     * @param   target Target object that will be passed to handlers
     */
    static void parse(
        std::istream& input,
        const pattern_list& patterns,
        std::ostream& log,
        T& target
    ) {
        std::string line;
        while (std::getline(input, line))
            if (!parse_line(line, patterns, target))
                log << "Unexpected input (could not parse line): \"" << line << '"' << std::endl;
    }
    /**
     * Parses an input line and calls the handler associated with its pattern
     * 
     * @param     line The input line
     * @param patterns String patterns to match and their associated handlers
     * @param   target Target object that will be passed to handlers
     * @return         True on success, false if @p line does not match any
     *                 pattern in @p patterns
     */
    static bool parse_line(
        const std::string_view& line,
        const pattern_list& patterns,
        T& target
    ) {
        for (const auto& [pattern, action] : patterns) {
            std::vector<string_pattern::field> fields;
            if (pattern.match(line, fields)) {
                std::invoke(action, target, fields);
                return true;
            }
        }
        return false;
    }
};

}
