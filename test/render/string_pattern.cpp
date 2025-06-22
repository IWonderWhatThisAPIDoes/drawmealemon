/**
 * @file string_pattern.cpp
 * 
 * Tests for the @ref string_pattern class
 */

#include "../testbed/test.hpp"
#include "../../src/render/string_pattern.hpp"

using dmalem::string_pattern;

TEST(valid_matchers_are_valid) {
    TEST_ASSERT(string_pattern::is_valid_pattern("hello world %%%d%s%S"));
}

TEST(unknown_matchers_are_invalid) {
    TEST_ASSERT(!string_pattern::is_valid_pattern("% "));
}

TEST(percent_at_end_is_invalid) {
    TEST_ASSERT(!string_pattern::is_valid_pattern("%"));
}

TEST(construct_from_string) {
    string_pattern pat = std::string("hello");
}

TEST(construct_from_string_ref) {
    const std::string p = "hello";
    string_pattern pat = p;
}

TEST(literal_pattern_matches_string_exactly) {
    string_pattern pat("hello world 123!");
    std::vector<string_pattern::field> fields;
    TEST_ASSERT(pat.match("hello world 123!", fields));
    TEST_ASSERT(fields.empty());
}

TEST(literal_pattern_rejects_shorter_string) {
    string_pattern pat("hello world 123!");
    TEST_ASSERT(!pat.match("hello world 123"));
}

TEST(literal_pattern_rejects_longer_string) {
    string_pattern pat("hello world 123!");
    TEST_ASSERT(!pat.match("hello world 123!!"));
}

TEST(whitespace_is_matched_flexibly) {
    string_pattern pat("hello world !");
    std::vector<string_pattern::field> fields;
    TEST_ASSERT(pat.match("hello  \n\tworld!", fields));
    TEST_ASSERT(fields.empty());
}

TEST(leading_and_trailing_whitespace_is_accepted) {
    string_pattern pat("abc");
    std::vector<string_pattern::field> fields;
    TEST_ASSERT(pat.match("  \nabc\n  \t ", fields));
    TEST_ASSERT(fields.empty());
}

TEST(fails_on_unexpected_whitespace) {
    string_pattern pat("abc");
    TEST_ASSERT(!pat.match("a bc"));
}

TEST(construction_fails_with_invalid_pattern) {
    TEST_ASSERT_THROW(string_pattern("% "), std::invalid_argument);
}

TEST(construction_fails_with_incomplete_pattern) {
    TEST_ASSERT_THROW(string_pattern("%"), std::invalid_argument);
}

TEST(d_matcher_parses_integer_input) {
    string_pattern pat("%d %d %d");
    std::vector<string_pattern::field> fields;
    TEST_ASSERT(pat.match(" 42 -150 000 ", fields));
    TEST_ASSERT_EQ(fields.size(), 3);
    TEST_ASSERT_EQ(fields[0], string_pattern::field(42));
    TEST_ASSERT_EQ(fields[1], string_pattern::field(-150));
    TEST_ASSERT_EQ(fields[2], string_pattern::field(0));
}

TEST(s_matcher_parses_token) {
    string_pattern pat("%s");
    std::vector<string_pattern::field> fields;
    TEST_ASSERT(pat.match(" Hello_world_42 ", fields));
    TEST_ASSERT_EQ(fields.size(), 1);
    TEST_ASSERT_EQ(fields[0], string_pattern::field("Hello_world_42"));
}

TEST(s_matcher_takes_characters_until_delimiter) {
    string_pattern pat("a [%S] b");
    std::vector<string_pattern::field> fields;
    TEST_ASSERT(pat.match("a[b a]b", fields));
    TEST_ASSERT_EQ(fields.size(), 1);
    TEST_ASSERT_EQ(fields[0], string_pattern::field("b a"));
}

TEST(s_matcher_at_end_of_pattern_takes_any_suffix) {
    string_pattern pat("a %S");
    std::vector<string_pattern::field> fields;
    TEST_ASSERT(pat.match("a b c d", fields));
    TEST_ASSERT_EQ(fields.size(), 1);
    TEST_ASSERT_EQ(fields[0], string_pattern::field("b c d"));
}

TEST(s_matcher_takes_first_occurrence) {
    string_pattern pat("[%S]%S");
    std::vector<string_pattern::field> fields;
    TEST_ASSERT(pat.match("[a [b] c] d", fields));
    TEST_ASSERT_EQ(fields.size(), 2);
    TEST_ASSERT_EQ(fields[0], string_pattern::field("a [b"));
    TEST_ASSERT_EQ(fields[1], string_pattern::field(" c] d"));
}

TEST(empty_pattern) {
    string_pattern pat("");
    std::vector<string_pattern::field> fields;
    TEST_ASSERT(pat.match("  \n\n\t ", fields));
    TEST_ASSERT(fields.empty());
}
