/**
 * @file failure.cpp
 * 
 * Error type for reporting test failures
 */

#include "failure.hpp"

namespace test {

test_failure::test_failure(
    const std::string_view& source,
    size_t line,
    const std::string_view& msg
) :
    source(source),
    line(line),
    msg(msg),
    whatText("Test failure at " + std::string(source) + ':' + std::to_string(line) + ": " + std::string(msg))
{}

const char* test_failure::message() const noexcept {
    return what();
}

const char* test_failure::what() const noexcept {
    return whatText.c_str();
}

}
