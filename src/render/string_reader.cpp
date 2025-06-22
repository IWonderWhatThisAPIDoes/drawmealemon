/**
 * @file string_reader.cpp
 * 
 * Simple reader for parsing strings
 */

#include <charconv>
#include "string_reader.hpp"

namespace dmalem {

string_reader::string_reader(const std::string_view& buffer) noexcept : buffer(buffer) {}

bool string_reader::is_done() const noexcept {
    return buffer.empty();
}

void string_reader::take_ws() noexcept {
    while (!buffer.empty() && isspace(buffer.front()))
        buffer.remove_prefix(1);
}

bool string_reader::take_char(char c) noexcept {
    if (buffer.empty() || buffer.front() != c)
        return false;
    buffer.remove_prefix(1);
    return true;
}

bool string_reader::take_int(int& value) noexcept {
    auto result = std::from_chars(buffer.data(), buffer.data() + buffer.length(), value);
    if (result.ec != std::errc())
        return false;
    buffer.remove_prefix(result.ptr - buffer.data());
    return true;
}

bool string_reader::take_token(std::string_view& token) noexcept {
    auto it = buffer.begin();
    while (it != buffer.end() && (isalnum(*it) || *it == '_'))
        ++it;
    if (it == buffer.begin())
        return false;
    size_t length = it - buffer.begin();
    token = buffer.substr(0, length);
    buffer.remove_prefix(length);
    return true;
}

bool string_reader::take_until(char delim, std::string_view& span) noexcept {
    size_t length = buffer.find_first_of(delim);
    if (length == std::string_view::npos)
        return false;
    span = buffer.substr(0, length);
    buffer.remove_prefix(length);
    return true;
}

std::string_view string_reader::take_all() noexcept {
    std::string_view rest = buffer;
    buffer = std::string_view();
    return rest;
}

}
