/**
 * @file failure.hpp
 * 
 * Error type for reporting test failures
 */

#pragma once

#include <exception>
#include <string>

namespace test {

/**
 * Exception that indicates a failed test case
 */
class test_failure : public std::exception {
public:
    /**
     * Constructor
     * 
     * @param source Name of the source file where the failure occurred
     * @param line Number of the source line where the failure occurred
     * @param msg Explanation of the failure
     */
    test_failure(
        const std::string_view& source,
        size_t line,
        const std::string_view& msg = "No description provided"
    );
    /**
     * Description of the failure for the needs of test reports
     * 
     * @return Description of the failure
     */
    const char* message() const noexcept;
    /**
     * Internal description of the exception object
     * 
     * @return Description of the exception
     */
    const char* what() const noexcept override;
private:
    std::string source;
    size_t line;
    std::string msg;
    std::string whatText;
};

}
