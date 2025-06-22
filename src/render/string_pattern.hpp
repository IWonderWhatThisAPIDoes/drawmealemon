/**
 * @file string_pattern.hpp
 * 
 * Pattern-based parsing
 */

#pragma once

#include <string>
#include <vector>
#include <variant>
#include <iostream>

namespace dmalem {

/**
 * Represents a pattern that can be matched against a string
 * 
 * A pattern is a literal string with the following formatting matchers:
 * - `%%` - Literal `%`
 * - `%d` - Decimal integer
 * - `%s` - Token - a string that consists of alphanumeric characters and underscores
 * - `%S` - Span - all characters until the following end delimiter
 *   (or the end of the input) is encountered
 */
class string_pattern {
public:
    /**
     * Constructor
     * 
     * @param pattern String that describes the pattern
     * @throw std::invalid_argument @p pattern is invalid
     */
    string_pattern(const char* pattern);
    /**
     * Constructor
     * 
     * @param pattern String that describes the pattern
     * @throw std::invalid_argument @p pattern is invalid
     */
    string_pattern(const std::string& pattern);
    /**
     * Constructor
     * 
     * @param pattern String that describes the pattern
     * @throw std::invalid_argument @p pattern is invalid
     */
    string_pattern(std::string&& pattern);
    /**
     * Validates a pattern string
     * 
     * @param pattern String that describes the pattern
     * @return True if the string is a valid pattern, false otherwise
     */
    static bool is_valid_pattern(const std::string_view& pattern) noexcept;
    /**
     * Checks if a character may appear after a `%` in a pattern string
     * 
     * @param c The character to test
     * @return True if the character may appear after a `%`, false otherwise
     */
    static bool is_pattern_character(char c) noexcept;
    /**
     * Type of the value that is a result of a pattern match
     */
    using field = std::variant<std::string_view, int>;
    /**
     * Matches a string against the pattern
     * 
     * @param      target The input string to be matched against the pattern
     * @param[out] fields Values that have been captured by the pattern's matchers
     * @return            True if the pattern successfully matched, false othwerise
     */
    bool match(const std::string_view& target, std::vector<field>& fields) const noexcept {
        return match(target, &fields);
    }
    /**
     * Matches a string against the pattern
     * 
     * @param      target The input string to be matched against the pattern
     * @param[out] fields Values that have been captured by the pattern's matchers
     * @return            True if the pattern successfully matched, false othwerise
     */
    bool match(const std::string_view& target, std::vector<field>* fields = nullptr) const;
private:
    std::string pattern;
};

}
