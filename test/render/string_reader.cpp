/**
 * @file string_reader.cpp
 * 
 * Tests for the @ref string_reader class
 */

#include "../testbed/test.hpp"
#include "../../src/render/string_reader.hpp"

using dmalem::string_reader;

TEST(empty_reader_is_at_end) {
    const string_reader r("");
    TEST_ASSERT(r.is_done());
}

TEST(non_empty_reader_is_not_at_end) {
    const string_reader r("a");
    TEST_ASSERT(!r.is_done());
}

TEST(whitespace_only_input) {
    string_reader r("   \n\n\t \n");
    r.take_ws();
    TEST_ASSERT(r.is_done());
}

TEST(take_all_returns_input) {
    string_reader r(" hello world\n");
    TEST_ASSERT_EQ(r.take_all(), " hello world\n");
    TEST_ASSERT(r.is_done());
}

TEST(take_all_returns_only_remaining_input) {
    string_reader r("  \n\n\t hello world \n");
    r.take_ws();
    TEST_ASSERT_EQ(r.take_all(), "hello world \n");
    TEST_ASSERT(r.is_done());
}

TEST(take_all_again_returns_empty) {
    string_reader r("hello world");
    TEST_ASSERT_EQ(r.take_all(), "hello world");
    TEST_ASSERT_EQ(r.take_all(), "");
}

TEST(no_whitespace) {
    string_reader r("hello world");
    r.take_ws();
    TEST_ASSERT_EQ(r.take_all(), "hello world");
}

TEST(take_whitespace_from_empty_input) {
    string_reader r("");
    r.take_ws();
}

TEST(take_char_takes_single_char) {
    string_reader r("aa");
    TEST_ASSERT(r.take_char('a'));
    TEST_ASSERT_EQ(r.take_all(), "a");
}

TEST(take_char_takes_last_char) {
    string_reader r("a");
    TEST_ASSERT(r.take_char('a'));
    TEST_ASSERT(r.is_done());
}

TEST(take_char_fails_on_incorrect_char) {
    string_reader r("ab");
    TEST_ASSERT(!r.take_char('b'));
    TEST_ASSERT_EQ(r.take_all(), "ab");
}

TEST(take_int_takes_int_value) {
    string_reader r("42abc");
    int value;
    TEST_ASSERT(r.take_int(value));
    TEST_ASSERT_EQ(value, 42);
    TEST_ASSERT_EQ(r.take_all(), "abc");
}

TEST(take_int_takes_negative_value) {
    string_reader r("-42abc");
    int value;
    TEST_ASSERT(r.take_int(value));
    TEST_ASSERT_EQ(value, -42);
    TEST_ASSERT_EQ(r.take_all(), "abc");
}

TEST(take_int_fails_on_overflow) {
    string_reader r("9999999999999999abc");
    int value;
    TEST_ASSERT(!r.take_int(value));
    TEST_ASSERT_EQ(r.take_all(), "9999999999999999abc");
}

TEST(take_int_takes_last_character) {
    string_reader r("42");
    int value;
    TEST_ASSERT(r.take_int(value));
    TEST_ASSERT_EQ(value, 42);
    TEST_ASSERT(r.is_done());
}

TEST(take_int_fails_on_non_digit) {
    string_reader r("abc");
    int value;
    TEST_ASSERT(!r.take_int(value));
    TEST_ASSERT_EQ(r.take_all(), "abc");
}

TEST(take_int_fails_on_end) {
    string_reader r("");
    int value;
    TEST_ASSERT(!r.take_int(value));
}

TEST(take_token_takes_single_token) {
    string_reader r("Hello_world_1-1");
    std::string_view token;
    TEST_ASSERT(r.take_token(token));
    TEST_ASSERT_EQ(token, "Hello_world_1");
    TEST_ASSERT_EQ(r.take_all(), "-1");
}

TEST(take_token_takes_last_character) {
    string_reader r("Hello_world");
    std::string_view token;
    TEST_ASSERT(r.take_token(token));
    TEST_ASSERT_EQ(token, "Hello_world");
    TEST_ASSERT(r.is_done());
}

TEST(take_token_fails_on_non_token_character) {
    string_reader r("-abc");
    std::string_view token;
    TEST_ASSERT(!r.take_token(token));
    TEST_ASSERT_EQ(r.take_all(), "-abc");
}

TEST(take_token_fails_on_end) {
    string_reader r("");
    std::string_view token;
    TEST_ASSERT(!r.take_token(token));
}

TEST(take_until_takes_everything_until_delimiter) {
    string_reader r("Hello world! \n\tabc42");
    std::string_view span;
    TEST_ASSERT(r.take_until('4', span));
    TEST_ASSERT_EQ(span, "Hello world! \n\tabc");
    TEST_ASSERT_EQ(r.take_all(), "42");
}

TEST(take_until_takes_empty_span) {
    string_reader r("abc");
    std::string_view span;
    TEST_ASSERT(r.take_until('a', span));
    TEST_ASSERT_EQ(span, "");
    TEST_ASSERT_EQ(r.take_all(), "abc");
}

TEST(take_until_fails_if_delimiter_never_appears) {
    string_reader r("Hello world! \n\tabc");
    std::string_view span;
    TEST_ASSERT(!r.take_until('4', span));
    TEST_ASSERT_EQ(r.take_all(), "Hello world! \n\tabc");
}

TEST(take_until_fails_on_empty_input) {
    string_reader r("");
    std::string_view span;
    TEST_ASSERT(!r.take_until('4', span));
}

TEST(multiple_takes) {
    string_reader r("hello [world] 42");
    std::string_view token, span;
    int value;
    TEST_ASSERT(r.take_token(token));
    r.take_ws();
    TEST_ASSERT(r.take_char('['));
    TEST_ASSERT(r.take_until(']', span));
    TEST_ASSERT(r.take_char(']'));
    r.take_ws();
    TEST_ASSERT(r.take_int(value));
    TEST_ASSERT(r.is_done());
    TEST_ASSERT_EQ(token, "hello");
    TEST_ASSERT_EQ(span, "world");
    TEST_ASSERT_EQ(value, 42);
}
