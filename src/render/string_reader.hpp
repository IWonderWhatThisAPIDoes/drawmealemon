/**
 * @file string_reader.hpp
 * 
 * Simple reader for parsing strings
 */

#pragma once

#include <string>

namespace dmalem {

/**
 * Wraps a string view and provides stream-like operations
 * that help with parsing the string
 */
class string_reader {
public:
    /**
     * Wraps a string view in a line reader
     * 
     * All strings emited by the reader live for as long as the provided view
     * 
     * @param buffer The string view that will be read
     */
    string_reader(const std::string_view& buffer) noexcept;
    /**
     * Checks whether the reader is at the end of the input string
     * 
     * @return True if the reader is at the end, false otherwise
     */
    bool is_done() const noexcept;
    /**
     * Discards leading whitespace
     */
    void take_ws() noexcept;
    /**
     * Reads a single character and asserts that it is the one provided
     * 
     * On failure, the input string remains unchanged
     * 
     * @param c The expected character
     * @return  True if the character was read, false otherwise
     */
    bool take_char(char c) noexcept;
    /**
     * Reads an integer value
     * 
     * On failure, the input string remains unchanged
     * 
     * @param[out] value On success, contains the value that was read
     * @return           True on success, false if the input does not start with a digit
     *                   or the number is too large to store in a variable
     */
    bool take_int(int& value) noexcept;
    /**
     * Reads a contiguous sequence of alphanumeric characters or underscores
     * 
     * On failure, the input string remains unchanged
     * 
     * @param[out] token On success, contains the token that was read.
     *                   Lives for as long as the input string
     * @return           True on success, false if the input does not start with a alphanumeric
     *                   character or underscore
     */
    bool take_token(std::string_view& token) noexcept;
    /**
     * Reads characters until a specific character is encountered
     * 
     * @param     delim The character that terminates the input.
     *                  It is not discarded from the input nor present in the output
     * @param[out] span On success, contains all characters until
     *                  the first occurrence of @p delim.
     *                  Lives for as long as the input string
     * @return          True on success, false if input does not contain @p delim
     */
    bool take_until(char delim, std::string_view& span) noexcept;
    /**
     * Empties the input into a string view
     * 
     * @return All characters left in the input.
     *         Lives for as long as the input string
     */
    std::string_view take_all() noexcept;
private:
    std::string_view buffer;
};

}
