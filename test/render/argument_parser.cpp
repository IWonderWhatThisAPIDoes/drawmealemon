/**
 * @file argument_parser.cpp
 * 
 * Tests for the @ref argument_parser class
 */

#include "../testbed/test.hpp"
#include "../../src/render/argument_parser.hpp"

using dmalem::argument_parser;

TEST(no_arguments) {
    const auto output = argument_parser::parse(0, nullptr);
    TEST_ASSERT_EQ(output.targetName, "");
    TEST_ASSERT(output.targetOptions.empty());
}

TEST(only_first_argument) {
    const char* argv[] = {"a.out"};
    const auto output = argument_parser::parse(1, argv);
    TEST_ASSERT_EQ(output.targetName, "");
    TEST_ASSERT(output.targetOptions.empty());
}

TEST(split_target_name) {
    const char* argv[] = {"a.out", "-t", "name"};
    const auto output = argument_parser::parse(3, argv);
    TEST_ASSERT_EQ(output.targetName, "name");
    TEST_ASSERT(output.targetOptions.empty());
}

TEST(joined_target_name) {
    const char* argv[] = {"a.out", "-tname"};
    const auto output = argument_parser::parse(2, argv);
    TEST_ASSERT_EQ(output.targetName, "name");
    TEST_ASSERT(output.targetOptions.empty());
}

TEST(empty_target_name) {
    const char* argv[] = {"a.out", "-t", ""};
    const auto output = argument_parser::parse(3, argv);
    TEST_ASSERT_EQ(output.targetName, "");
    TEST_ASSERT(output.targetOptions.empty());
}

TEST(split_option) {
    const char* argv[] = {"a.out", "-o", "opt"};
    const auto output = argument_parser::parse(3, argv);
    TEST_ASSERT_EQ(output.targetName, "");
    TEST_ASSERT_EQ(output.targetOptions.size(), 1);
    TEST_ASSERT_EQ(output.targetOptions[0], "opt");
}

TEST(joined_option) {
    const char* argv[] = {"a.out", "-oopt"};
    const auto output = argument_parser::parse(2, argv);
    TEST_ASSERT_EQ(output.targetName, "");
    TEST_ASSERT_EQ(output.targetOptions.size(), 1);
    TEST_ASSERT_EQ(output.targetOptions[0], "opt");
}

TEST(empty_option) {
    const char* argv[] = {"a.out", "-o", ""};
    const auto output = argument_parser::parse(3, argv);
    TEST_ASSERT_EQ(output.targetName, "");
    TEST_ASSERT_EQ(output.targetOptions.size(), 1);
    TEST_ASSERT_EQ(output.targetOptions[0], "");
}

TEST(multiple_split_options) {
    const char* argv[] = {"a.out", "-o", "opta", "-o", "optb"};
    const auto output = argument_parser::parse(5, argv);
    TEST_ASSERT_EQ(output.targetName, "");
    TEST_ASSERT_EQ(output.targetOptions.size(), 2);
    TEST_ASSERT_EQ(output.targetOptions[0], "opta");
    TEST_ASSERT_EQ(output.targetOptions[1], "optb");
}

TEST(multiple_joined_options) {
    const char* argv[] = {"a.out", "-oopta", "-ooptb"};
    const auto output = argument_parser::parse(3, argv);
    TEST_ASSERT_EQ(output.targetName, "");
    TEST_ASSERT_EQ(output.targetOptions.size(), 2);
    TEST_ASSERT_EQ(output.targetOptions[0], "opta");
    TEST_ASSERT_EQ(output.targetOptions[1], "optb");
}

TEST(joined_and_split_option) {
    const char* argv[] = {"a.out", "-oopta", "-o", "optb"};
    const auto output = argument_parser::parse(4, argv);
    TEST_ASSERT_EQ(output.targetName, "");
    TEST_ASSERT_EQ(output.targetOptions.size(), 2);
    TEST_ASSERT_EQ(output.targetOptions[0], "opta");
    TEST_ASSERT_EQ(output.targetOptions[1], "optb");
}

TEST(split_and_joined_option) {
    const char* argv[] = {"a.out", "-o", "opta", "-ooptb"};
    const auto output = argument_parser::parse(4, argv);
    TEST_ASSERT_EQ(output.targetName, "");
    TEST_ASSERT_EQ(output.targetOptions.size(), 2);
    TEST_ASSERT_EQ(output.targetOptions[0], "opta");
    TEST_ASSERT_EQ(output.targetOptions[1], "optb");
}

TEST(duplicate_options) {
    const char* argv[] = {"a.out", "-o", "opt", "-oopt"};
    const auto output = argument_parser::parse(4, argv);
    TEST_ASSERT_EQ(output.targetName, "");
    TEST_ASSERT_EQ(output.targetOptions.size(), 2);
    TEST_ASSERT_EQ(output.targetOptions[0], "opt");
    TEST_ASSERT_EQ(output.targetOptions[1], "opt");
}

TEST(target_name_before_option) {
    const char* argv[] = {"a.out", "-t", "target", "-o", "opt"};
    const auto output = argument_parser::parse(5, argv);
    TEST_ASSERT_EQ(output.targetName, "target");
    TEST_ASSERT_EQ(output.targetOptions.size(), 1);
    TEST_ASSERT_EQ(output.targetOptions[0], "opt");
}

TEST(target_name_after_option) {
    const char* argv[] = {"a.out", "-o", "opt", "-t", "target"};
    const auto output = argument_parser::parse(5, argv);
    TEST_ASSERT_EQ(output.targetName, "target");
    TEST_ASSERT_EQ(output.targetOptions.size(), 1);
    TEST_ASSERT_EQ(output.targetOptions[0], "opt");
}

TEST(target_name_between_options) {
    const char* argv[] = {"a.out", "-o", "opta", "-t", "target", "-o", "optb"};
    const auto output = argument_parser::parse(7, argv);
    TEST_ASSERT_EQ(output.targetName, "target");
    TEST_ASSERT_EQ(output.targetOptions.size(), 2);
    TEST_ASSERT_EQ(output.targetOptions[0], "opta");
    TEST_ASSERT_EQ(output.targetOptions[1], "optb");
}

TEST(duplicate_target_name) {
    const char* argv[] = {"a.out", "-tname", "-tname"};
    const auto e = TEST_ASSERT_THROW(argument_parser::parse(3, argv), argument_parser::error);
    TEST_ASSERT_EQ(e.code(), argument_parser::error_code::duplicate_flag);
}

TEST(duplicate_target_name_after_empty) {
    const char* argv[] = {"a.out", "-t", "", "-t", ""};
    const auto e = TEST_ASSERT_THROW(argument_parser::parse(5, argv), argument_parser::error);
    TEST_ASSERT_EQ(e.code(), argument_parser::error_code::duplicate_flag);
}

TEST(missing_target_name) {
    const char* argv[] = {"a.out", "-t"};
    const auto e = TEST_ASSERT_THROW(argument_parser::parse(2, argv), argument_parser::error);
    TEST_ASSERT_EQ(e.code(), argument_parser::error_code::missing_argument);
}

TEST(missing_target_name_before_next_flag) {
    const char* argv[] = {"a.out", "-t", "-oa"};
    const auto e = TEST_ASSERT_THROW(argument_parser::parse(3, argv), argument_parser::error);
    TEST_ASSERT_EQ(e.code(), argument_parser::error_code::missing_argument);
}

TEST(missing_option) {
    const char* argv[] = {"a.out", "-o"};
    const auto e = TEST_ASSERT_THROW(argument_parser::parse(2, argv), argument_parser::error);
    TEST_ASSERT_EQ(e.code(), argument_parser::error_code::missing_argument);
}

TEST(missing_option_before_next_flag) {
    const char* argv[] = {"a.out", "-o", "-oa"};
    const auto e = TEST_ASSERT_THROW(argument_parser::parse(3, argv), argument_parser::error);
    TEST_ASSERT_EQ(e.code(), argument_parser::error_code::missing_argument);
}

TEST(empty_flag) {
    const char* argv[] = {"a.out", "-"};
    const auto e = TEST_ASSERT_THROW(argument_parser::parse(2, argv), argument_parser::error);
    TEST_ASSERT_EQ(e.code(), argument_parser::error_code::unknown_flag);
}

TEST(unknown_flag) {
    const char* argv[] = {"a.out", "-a"};
    const auto e = TEST_ASSERT_THROW(argument_parser::parse(2, argv), argument_parser::error);
    TEST_ASSERT_EQ(e.code(), argument_parser::error_code::unknown_flag);
}

TEST(non_flag_argument) {
    const char* argv[] = {"a.out", "hello", "world"};
    const auto e = TEST_ASSERT_THROW(argument_parser::parse(3, argv), argument_parser::error);
    TEST_ASSERT_EQ(e.code(), argument_parser::error_code::missing_flag);
}

TEST(non_flag_argument_after_flag) {
    const char* argv[] = {"a.out", "-t", "target" "hello", "world"};
    const auto e = TEST_ASSERT_THROW(argument_parser::parse(5, argv), argument_parser::error);
    TEST_ASSERT_EQ(e.code(), argument_parser::error_code::missing_flag);
}
