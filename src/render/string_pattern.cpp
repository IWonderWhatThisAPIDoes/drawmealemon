/**
 * @file string_pattern.cpp
 * 
 * Pattern-based parsing
 */

#include <stdexcept>
#include "string_pattern.hpp"
#include "string_reader.hpp"

namespace dmalem {

string_pattern::string_pattern(const char* pattern) : string_pattern(std::string(pattern)) {}

string_pattern::string_pattern(const std::string& pattern) : string_pattern(std::string(pattern)) {}

string_pattern::string_pattern(std::string&& pattern) : pattern(std::move(pattern)) {
    if (!is_valid_pattern(this->pattern))
        throw std::invalid_argument(__FUNCTION__);
}

bool string_pattern::is_valid_pattern(const std::string_view& pattern) noexcept {
    for (auto it = pattern.begin(); it != pattern.end(); ++it)
        if (*it == '%' && (++it == pattern.end() || !is_pattern_character(*it)))
            return false;
    return true;
}

bool string_pattern::is_pattern_character(char c) noexcept {
    return c == '%' || c == 'd' || c == 's' || c == 'S';
}

bool string_pattern::match(const std::string_view& target, std::vector<field>* fields) const {
    string_reader t(target);
    t.take_ws();
    std::vector<field> parsedFields;
    for (auto it = pattern.begin(); it != pattern.end(); ++it) {
        switch (*it) {
            case ' ': {
                t.take_ws();
                break;
            }
            case '%': {
                if (++it == pattern.end()) {
                    // This should be unreachable because the constructor
                    // should already ensure no invalid patterns make it through
                    throw std::logic_error(__FUNCTION__);
                }
                switch (*it) {
                    case '%': {
                        if (!t.take_char('%'))
                            return false;
                        break;
                    }
                    case 'd': {
                        int value;
                        if (!t.take_int(value))
                            return false;
                        parsedFields.push_back(value);
                        break;
                    }
                    case 's': {
                        std::string_view token;
                        if (!t.take_token(token))
                            return false;
                        parsedFields.push_back(token);
                        break;
                    }
                    case 'S': {
                        std::string_view span;
                        if (std::next(it) == pattern.end())
                            span = t.take_all();
                        else if (!t.take_until(*std::next(it), span))
                            return false;
                        parsedFields.push_back(span);
                        break;
                    }
                    default: {
                        // This should be unreachable because the constructor
                        // should already ensure no invalid patterns make it through
                        throw std::logic_error(__FUNCTION__);
                    }
                }
                break;
            }
            default: {
                if (!t.take_char(*it))
                    return false;
                break;
            }
        }
    }
    t.take_ws();
    if (!t.is_done())
        return false;
    if (fields)
        *fields = std::move(parsedFields);
    return true;
}

}
