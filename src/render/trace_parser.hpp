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
     * Registers a string pattern and its associated handler
     * 
     * If more than one pattern matches, the first one in order of insertion is used
     * 
     * @param pattern Pattern to match against each line of input
     * @param action  Handler to call when an input line matches @p pattern
     * @return        @p this
     */
    trace_parser& add_pattern(string_pattern&& pattern, trace_action&& action) {
        patterns.emplace_back(std::move(pattern), std::move(action));
        return *this;
    }
    /**
     * Sets an output stream for logging error reports when a line fails to parse
     * 
     * @param ostr Stream that receives log messages. Must live for as long
     *             as any input is parsed using the parser
     * @return     @p this
     */
    trace_parser& log_to(std::ostream& ostr) noexcept {
        log = &ostr;
        return *this;
    }
    /**
     * Sets the target that will be forwarded to pattern handlers
     * 
     * @param target New target that will be forwarded to pattern handlers.
     *               Must live for as long as any input is parsed using the parser
     * @return       @p this
     */
    trace_parser& set_target(T& newTarget) noexcept {
        target = &newTarget;
        return *this;
    }
    /**
     * Reads an input stream to the end, and parses each line and calls
     * the handler associated with its pattern
     * 
     * If a line fails to parse, it is reported to @p log
     * 
     * @param input The input stream
     * @throw std::invalid_argument No target has been set to receive the input
     */
    void parse(std::istream& input) const {
        std::string line;
        while (std::getline(input, line))
            if (!parse_line(line) && log)
                *log << "Unexpected input (could not parse line): \"" << line << '"' << std::endl;
    }
    /**
     * Parses an input line and calls the handler associated with its pattern
     * 
     * @param line The input line
     * @return True on success, false if @p line does not match any registered pattern
     * @throw std::invalid_argument No target has been set to receive the input
     */
    bool parse_line(const std::string_view& line) const {
        if (!target)
            throw std::invalid_argument(__FUNCTION__);
        for (const auto& [pattern, action] : patterns) {
            std::vector<string_pattern::field> fields;
            if (pattern.match(line, fields)) {
                std::invoke(action, *target, fields);
                return true;
            }
        }
        return false;
    }
private:
    std::vector<std::pair<string_pattern, trace_action>> patterns;
    std::ostream* log = nullptr;
    T* target = nullptr;
};

}
