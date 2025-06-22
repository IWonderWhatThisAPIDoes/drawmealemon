/**
 * @file trace_parser.cpp
 * 
 * Tests for the @ref trace_parser class
 */

#include <sstream>
#include "../testbed/test.hpp"
#include "../../src/render/trace_parser.hpp"

using dmalem::trace_parser;
using dmalem::string_pattern;

/**
 * @ref string_pattern::field with value semantics for caching
 */
using owned_pattern_field = std::variant<std::string, int>;

/**
 * Converts a list of string pattern fields to the owned variant
 */
std::vector<owned_pattern_field> convert_log_vector(const std::vector<string_pattern::field>& v) {
    std::vector<owned_pattern_field> log;
    log.reserve(v.size());
    for (const auto& field: v) {
        switch (field.index()) {
            case 0:
                log.emplace_back(std::string(std::get<0>(field)));
                break;
            case 1:
                log.emplace_back(std::get<1>(field));
                break;
        }
    }
    return log;
}

/**
 * Shorthand for a mock input pattern handler
 */
using mock_handler = test::mock<void(
    test::mkeep<int&>,
    const test::mkeep<
        std::vector<string_pattern::field>,
        std::vector<owned_pattern_field>,
        convert_log_vector
    >&
)>;

TEST(no_logs_on_empty_input) {
    std::ostringstream log;
    std::istringstream input("");
    int dummy = 0;
    trace_parser<int>::parse(input, {}, log, dummy);
    TEST_ASSERT_EQ_(log.str(), "", "Parser should not log anything when input is empty");
}

TEST(match_first_pattern) {
    std::ostringstream log;
    std::istringstream input("abc");
    int dummy = 0;
    mock_handler realMock;
    mock_handler unusedMock;
    trace_parser<int>::pattern_list patterns = {
        {"abc", realMock},
        {"def", unusedMock},
    };
    trace_parser<int>::parse(input, patterns, log, dummy);
    TEST_ASSERT_EQ_(log.str(), "", "Parser should not log anything when all input parses successfully");
    TEST_ASSERT_EQ_(realMock.call_count(), 1, "First pattern should have matched and its handler should have been called");
    TEST_ASSERT_EQ_(unusedMock.call_count(), 0, "Second pattern's handler should not have been called");
    TEST_ASSERT_EQ_(&std::get<0>(realMock.args()), &dummy, "Target passed to parser should be forwarded to handler");
    TEST_ASSERT(std::get<1>(realMock.args()).empty());
}

TEST(match_second_pattern) {
    std::ostringstream log;
    std::istringstream input("def");
    int dummy = 0;
    mock_handler realMock;
    mock_handler unusedMock;
    trace_parser<int>::pattern_list patterns = {
        {"abc", unusedMock},
        {"def", realMock},
    };
    trace_parser<int>::parse(input, patterns, log, dummy);
    TEST_ASSERT_EQ_(log.str(), "", "Parser should not log anything when all input parses successfully");
    TEST_ASSERT_EQ_(realMock.call_count(), 1, "Second pattern should have matched and its handler should have been called");
    TEST_ASSERT_EQ_(unusedMock.call_count(), 0, "First pattern's handler should not have been called");
    TEST_ASSERT_EQ_(&std::get<0>(realMock.args()), &dummy, "Target passed to parser should be forwarded to handler");
    TEST_ASSERT(std::get<1>(realMock.args()).empty());
}

TEST(match_nothing) {
    std::ostringstream log;
    std::istringstream input("not-a-pattern");
    int dummy = 0;
    mock_handler unusedMock;
    trace_parser<int>::pattern_list patterns = {{"abc", unusedMock}};
    trace_parser<int>::parse(input, patterns, log, dummy);
    TEST_ASSERT_EQ_(unusedMock.call_count(), 0, "Handler of non-matching pattern should not have been called");
    TEST_ASSERT_NE_(log.str().find("not-a-pattern"), std::string::npos, "Log should contain the discarded input line at some point");
}

TEST(ignores_trailing_newline) {
    std::ostringstream log;
    std::istringstream input("abc\n");
    int dummy = 0;
    mock_handler mock;
    trace_parser<int>::pattern_list patterns = {{"abc", mock}};
    trace_parser<int>::parse(input, patterns, log, dummy);
    TEST_ASSERT_EQ_(log.str(), "", "Parser should not log anything when all input parses successfully");
    TEST_ASSERT_EQ_(mock.call_count(), 1, "Handler should have been called once");
    TEST_ASSERT_EQ_(&std::get<0>(mock.args()), &dummy, "Target passed to parser should be forwarded to handler");
    TEST_ASSERT(std::get<1>(mock.args()).empty());
}

TEST(match_multiple_patterns) {
    std::ostringstream log;
    std::istringstream input("one\nthree\ntwo\none");
    int dummy = 0;
    test::mock_context ctx;
    mock_handler mockOne(ctx);
    mock_handler mockTwo(ctx);
    mock_handler mockThree(ctx);
    trace_parser<int>::pattern_list patterns = {
        {"one", mockOne},
        {"two", mockTwo},
        {"three", mockThree},
    };
    trace_parser<int>::parse(input, patterns, log, dummy);
    TEST_ASSERT_EQ_(log.str(), "", "Parser should not log anything when all input parses successfully");
    TEST_ASSERT_EQ(mockOne.call_count(), 2);
    TEST_ASSERT_EQ(mockTwo.call_count(), 1);
    TEST_ASSERT_EQ(mockThree.call_count(), 1);
    TEST_ASSERT_LT(mockOne.global_call_order(), mockThree.global_call_order());
    TEST_ASSERT_LT(mockThree.global_call_order(), mockTwo.global_call_order());
    TEST_ASSERT_LT(mockTwo.global_call_order(), mockOne.global_call_order(1));
}

TEST(matchers_in_pattern_are_forwarded_to_handler) {
    std::ostringstream log;
    std::istringstream input("hello, 42");
    int dummy = 0;
    mock_handler mock;
    trace_parser<int>::pattern_list patterns = {{"%s , %d", mock}};
    trace_parser<int>::parse(input, patterns, log, dummy);
    TEST_ASSERT_EQ_(log.str(), "", "Parser should not log anything when all input parses successfully");
    TEST_ASSERT_EQ_(mock.call_count(), 1, "Handler should have been called once");
    TEST_ASSERT_EQ(std::get<1>(mock.args()).size(), 2);
    TEST_ASSERT_EQ(std::get<1>(mock.args())[0], owned_pattern_field("hello"));
    TEST_ASSERT_EQ(std::get<1>(mock.args())[1], owned_pattern_field(42));
}

TEST(wildcard_pattern_matches_anything) {
    std::ostringstream log;
    std::istringstream input("abc\n\n42");
    int dummy = 0;
    mock_handler mock;
    trace_parser<int>::pattern_list patterns = {{"%S", mock}};
    trace_parser<int>::parse(input, patterns, log, dummy);
    TEST_ASSERT_EQ_(log.str(), "", "Parser should not log anything when all input parses successfully");
    TEST_ASSERT_EQ(mock.call_count(), 3);
    TEST_ASSERT_EQ(std::get<1>(mock.args(0)).size(), 1);
    TEST_ASSERT_EQ(std::get<1>(mock.args(0))[0], owned_pattern_field("abc"));
    TEST_ASSERT_EQ(std::get<1>(mock.args(1)).size(), 1);
    TEST_ASSERT_EQ(std::get<1>(mock.args(1))[0], owned_pattern_field(""));
    TEST_ASSERT_EQ(std::get<1>(mock.args(2)).size(), 1);
    TEST_ASSERT_EQ(std::get<1>(mock.args(2))[0], owned_pattern_field("42"));
}

TEST(first_matching_pattern_is_used) {
    std::ostringstream log;
    std::istringstream input("1 2 3 4 5");
    int dummy = 0;
    mock_handler realMock;
    mock_handler unusedMock;
    trace_parser<int>::pattern_list patterns = {
        {"something else", unusedMock},
        {"1 %s 3 %d 5", realMock},
        {"1 2 %d %d 5", unusedMock},
        {"1 2 3 4 5", unusedMock},
        {"1 %s 3 %d 5", unusedMock},
    };
    trace_parser<int>::parse(input, patterns, log, dummy);
    TEST_ASSERT_EQ_(log.str(), "", "Parser should not log anything when all input parses successfully");
    TEST_ASSERT_EQ_(realMock.call_count(), 1, "Handler of first matching pattern should have been called");
    TEST_ASSERT_EQ_(unusedMock.call_count(), 0, "Handlers of other patterns should not have been called");
}
