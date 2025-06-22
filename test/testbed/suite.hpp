/**
 * @file suite.hpp
 * 
 * Test suite organization
 */

#pragma once

#include <map>
#include <string>

namespace test {

/**
 * Runs and evaluates all tests
 * 
 * @return True if all tests succeeded, false otherwise
 */
bool run_tests();

/**
 * Checks whether any tests have been registered
 * 
 * @return True if there are tests to run, false otherwise
 */
bool has_tests();

/**
 * Tests that have been registered, organized into test suites
 */
inline std::map<std::string, std::map<std::string, void(*)()>> testSuites;

}

/**
 * Declares a test case
 * 
 * @param suite Name of the test suite
 * @param case Name of the test case
 * 
 * @example
 * ```cpp
 * TEST(foo_suite, bar_case) {
 *     // assertions go here
 * }
 * ```
 */
#define TEST(case) \
    TEST_FUN_DECL(case); \
    ADD_TEST_CASE(case); \
    TEST_FUN_DECL(case)

#define TEST_INSERT_DUMMY(case) test_insert_ ## case

#define ADD_TEST_CASE(case) \
    static void (* TEST_INSERT_DUMMY(case))() = \
    ::test::testSuites[__BASE_FILE__][#case] = &TEST_FUN_NAME(case)

#define TEST_FUN_NAME(case) test_case_ ## case

#define TEST_FUN_DECL(case) static void TEST_FUN_NAME(case)()
