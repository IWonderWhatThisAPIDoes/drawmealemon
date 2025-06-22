/**
 * @file assert.hpp
 * 
 * Macros for failing test cases
 */

#pragma once

#include "failure.hpp"
#include "pretty_print.hpp"

/**
 * Fails a test with a generic message
 */
#define FAIL() FAIL_()

/**
 * Fails a test case with a provided message
 * 
 * @param msg Explanation of the failure
 */
#define FAIL_(msg) (throw ::test::test_failure(__FILE__, __LINE__, (msg)))

/**
 * Fails a test case if a condition evaluates to false
 * 
 * @param cond The condition to verify
 */
#define TEST_ASSERT(cond) TEST_ASSERT_(cond, "")

/**
 * Fails a test case if a condition evaluates to false
 * 
 * @param cond The condition to verify
 * @param msg Explanation of the assertion
 */
#define TEST_ASSERT_(cond, msg) \
    ([&] { \
        if (!(cond)) \
            FAIL_("Assertion failed: " msg "\nCondition: " #cond); \
    }())

/**
 * Fails a test case if a binary operator called on them returns false
 * 
 * @param left Left-hand side of the expression
 * @param right Right-hand side of the expression
 * @param op The operator to include in the assertion
 * @param msg Explanation of the assertion
 */
#define TEST_ASSERT_BOP_(left, right, op, msg) \
    ([&] { \
        const auto& l = (left); \
        const auto& r = (right); \
        if (!(l op r)) \
            FAIL_( \
                "Assertion failed: " msg "\nCondition: " #left " " #op " " #right \
                "\n     Left: " + ::test::pretty_print(l) + \
                "\n    Right: " + ::test::pretty_print(r) \
            ); \
    }())

/**
 * Fails a test case if two expressions are not equal
 * 
 * @param left Left-hand side of the equality
 * @param right Right-hand side of the equality
 */
#define TEST_ASSERT_EQ(left, right) TEST_ASSERT_EQ_(left, right, "")

/**
 * Fails a test case if two expressions are not equal
 * 
 * @param left Left-hand side of the equality
 * @param right Right-hand side of the equality
 * @param msg Explanation of the assertion
 */
#define TEST_ASSERT_EQ_(left, right, msg) TEST_ASSERT_BOP_(left, right, ==, msg)

/**
 * Fails a test case if two expressions are equal
 * 
 * @param left Left-hand side of the inequality
 * @param right Right-hand side of the inequality
 */
#define TEST_ASSERT_NE(left, right) TEST_ASSERT_NE_(left, right, "")

/**
 * Fails a test case if two expressions are equal
 * 
 * @param left Left-hand side of the inequality
 * @param right Right-hand side of the inequality
 * @param msg Explanation of the assertion
 */
#define TEST_ASSERT_NE_(left, right, msg) TEST_ASSERT_BOP_(left, right, !=, msg)

/**
 * Fails a test case unless an expression compares greater than another
 * 
 * @param left Left-hand side of the inequality
 * @param right Right-hand side of the inequality
 */
#define TEST_ASSERT_GT(left, right) TEST_ASSERT_GT_(left, right, "")

/**
 * Fails a test case unless an expression compares greater than another
 * 
 * @param left Left-hand side of the inequality
 * @param right Right-hand side of the inequality
 * @param msg Explanation of the assertion
 */
#define TEST_ASSERT_GT_(left, right, msg) TEST_ASSERT_BOP_(left, right, >, msg)

/**
 * Fails a test case unless an expression compares less than another
 * 
 * @param left Left-hand side of the inequality
 * @param right Right-hand side of the inequality
 */
#define TEST_ASSERT_LT(left, right) TEST_ASSERT_LT_(left, right, "")

/**
 * Fails a test case unless an expression compares less than another
 * 
 * @param left Left-hand side of the inequality
 * @param right Right-hand side of the inequality
 * @param msg Explanation of the assertion
 */
#define TEST_ASSERT_LT_(left, right, msg) TEST_ASSERT_BOP_(left, right, <, msg)

/**
 * Fails a test case unless an expression compares greater than or equal to another
 * 
 * @param left Left-hand side of the inequality
 * @param right Right-hand side of the inequality
 */
#define TEST_ASSERT_GE(left, right) TEST_ASSERT_GE_(left, right, "")

/**
 * Fails a test case unless an expression compares greater than or equal to another
 * 
 * @param left Left-hand side of the inequality
 * @param right Right-hand side of the inequality
 * @param msg Explanation of the assertion
 */
#define TEST_ASSERT_GE_(left, right, msg) TEST_ASSERT_BOP_(left, right, >=, msg)

/**
 * Fails a test case unless an expression compares less than or equal to another
 * 
 * @param left Left-hand side of the inequality
 * @param right Right-hand side of the inequality
 */
#define TEST_ASSERT_LE(left, right) TEST_ASSERT_LE_(left, right, "")

/**
 * Fails a test case unless an expression compares less than or equal to another
 * 
 * @param left Left-hand side of the inequality
 * @param right Right-hand side of the inequality
 * @param msg Explanation of the assertion
 */
#define TEST_ASSERT_LE_(left, right, msg) TEST_ASSERT_BOP_(left, right, <=, msg)

/**
 * Fails a test case if a statement does not throw an exception
 * 
 * @param expr The statement that is expected to throw
 * @param except Type of the expected exception
 * @return The expected exception that was caught
 */
#define TEST_ASSERT_THROW(expr, except) TEST_ASSERT_THROW_(expr, except, "")

/**
 * Fails a test case if a statement does not throw an exception
 * 
 * @param expr The statement that is expected to throw
 * @param except Type of the expected exception
 * @param msg Explanation of the assertion
 * @return The expected exception that was caught
 */
#define TEST_ASSERT_THROW_(expr, except, msg) \
    ([&]() -> except { \
        using ety = except; \
        try { \
            expr; \
        } catch (const ety& e) { \
            return e; \
        } catch (const std::exception& e) { \
            FAIL_( \
                "Assertion failed: incorrect exception was thrown" \
                "\nExpected: " #except \
                "\n     Got: " + std::string(typeid(e).name()) + " [what: " + e.what() + ']' \
            ); \
        } catch (...) { \
            FAIL_( \
                "Assertion failed: incorrect exception was thrown" \
                "\nExpected: " #except \
                "\n     Got: <non-exception type>" \
            ); \
        } \
        FAIL_("Assertion failed: exception was not thrown\nExpected: " #except); \
    }())
