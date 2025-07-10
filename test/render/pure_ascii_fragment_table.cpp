/**
 * @file pure_ascii_fragment_table.cpp
 * 
 * Tests for the @ref pure_ascii_fragment_table class
 */

#include <sstream>
#include "../testbed/test.hpp"
#include "../../src/render/pure_ascii_fragment_table.hpp"

using dmalem::pure_ascii_fragment_table;

TEST(column_separator) {
    std::ostringstream ostr;
    pure_ascii_fragment_table(ostr).column_separator();
    TEST_ASSERT_EQ(ostr.str(), "||");
}

TEST(endl) {
    std::ostringstream ostr;
    pure_ascii_fragment_table(ostr).endl();
    TEST_ASSERT_EQ(ostr.str(), "\n");
}

TEST(input_token_is_printed) {
    std::ostringstream ostr;
    pure_ascii_fragment_table table(ostr);
    table.bring_token("Hello");
    table.bring_token("World");
    TEST_ASSERT_NE(ostr.str().find("Hello"), std::string::npos);
    TEST_ASSERT_NE(ostr.str().find("World"), std::string::npos);
}

TEST(nonterminal_is_printed) {
    std::ostringstream ostr;
    pure_ascii_fragment_table table(ostr);
    table.nonterminal_name("hello");
    table.nonterminal_name("world");
    TEST_ASSERT_NE(ostr.str().find("hello"), std::string::npos);
    TEST_ASSERT_NE(ostr.str().find("world"), std::string::npos);
}

TEST(reduce_rule_is_printed) {
    std::ostringstream ostr;
    pure_ascii_fragment_table(ostr).reduce_rule_label("block ::= begin end");
    TEST_ASSERT_NE(ostr.str().find("block ::= begin end"), std::string::npos);
}

TEST(left_margins_have_equal_length) {
    std::ostringstream ostr;
    pure_ascii_fragment_table table(ostr);
    // Empty margin
    table.empty_left_margin();
    const auto empty = ostr.str();
    // Pending token margin
    ostr.str("");
    table.pending_token();
    const auto token = ostr.str();
    // Discard token margin
    ostr.str("");
    table.discard_token();
    const auto discard = ostr.str();
    // Verify that all margin widths are equal
    TEST_ASSERT_EQ(empty.length(), token.length());
    TEST_ASSERT_EQ(empty.length(), discard.length());
}

TEST(left_columns_have_equal_length) {
    std::ostringstream ostr;
    pure_ascii_fragment_table table(ostr);
    // Empty left column
    table.empty_left_column();
    const auto empty = ostr.str();
    // Empty reduce
    ostr.str("");
    table.pull_nonterminal(0);
    const auto conjure = ostr.str();
    // Non-empty reduce
    ostr.str("");
    table.pull_nonterminal(1);
    const auto pull = ostr.str();
    // Nonterminal name
    ostr.str("");
    table.nonterminal_name("hello");
    const auto nonterm = ostr.str();
    // Very long nonterminal name
    ostr.str("");
    table.nonterminal_name("hello_world_and_everyone_in_it");
    const auto longn = ostr.str();
    // Shift nonterminal
    ostr.str("");
    table.shift_nonterminal();
    const auto shift = ostr.str();
    // Verify that all column widths are equal
    TEST_ASSERT_EQ(empty.length(), conjure.length());
    TEST_ASSERT_EQ(empty.length(), pull.length());
    TEST_ASSERT_EQ(empty.length(), nonterm.length());
    TEST_ASSERT_EQ(empty.length(), longn.length());
    TEST_ASSERT_EQ(empty.length(), shift.length());
}

TEST(left_double_columns_have_equal_length) {
    std::ostringstream ostr;
    pure_ascii_fragment_table table(ostr);
    // Empty margin and column for reference
    table.empty_left_margin();
    table.empty_left_column();
    const auto empty = ostr.str();
    // Column header
    ostr.str("");
    table.left_column_head();
    const auto head = ostr.str();
    // Token name
    ostr.str("");
    table.bring_token("Hello");
    const auto token = ostr.str();
    // Very long token name
    ostr.str("");
    table.bring_token("HelloWorldAndEveryoneInIt");
    const auto longn = ostr.str();
    // Shift token
    ostr.str("");
    table.shift_token();
    const auto shift = ostr.str();
    // Verify that all column widths are equal
    TEST_ASSERT_EQ(empty.length(), token.length());
    TEST_ASSERT_EQ(empty.length(), head.length());
    TEST_ASSERT_EQ(empty.length(), longn.length());
    TEST_ASSERT_EQ(empty.length(), shift.length());
}

TEST(states_have_equal_length) {
    using enum dmalem::state_fragment_data::row_kind;
    std::ostringstream ostr;
    pure_ascii_fragment_table table(ostr);
    // State column
    table.state({.state = 1, .line = 42, .columnCount = 1});
    const auto state = ostr.str();
    // Start of state column
    ostr.str("");
    table.state({.state = 1, .line = 0, .columnCount = 1, .rowKind = shift});
    const auto state1 = ostr.str();
    // Start of state column
    ostr.str("");
    table.state({.state = 1, .line = 1, .columnCount = 1});
    const auto state2 = ostr.str();
    // Pending reduce column
    ostr.str("");
    table.state({.line = 42, .columnCount = 1});
    const auto pending = ostr.str();
    // Start of pending reduce column
    ostr.str("");
    table.state({.line = 0, .columnCount = 1, .rowKind = shift});
    const auto pending1 = ostr.str();
    // Start of pending reduce column
    ostr.str("");
    table.state({.line = 1, .columnCount = 1});
    const auto pending2 = ostr.str();
    // Reduce
    ostr.str("");
    table.state({.line = 42, .columnCount = 2, .columnIndex = 0, .rowKind = reduce, .popCount = 2});
    const auto reduce1 = ostr.str();
    // Reduce last
    ostr.str("");
    table.state({.line = 42, .columnCount = 2, .columnIndex = 1, .rowKind = reduce, .popCount = 2});
    const auto reduce2 = ostr.str();
    // discard
    ostr.str("");
    table.state({.line = 42, .columnCount = 2, .columnIndex = 1, .rowKind = discard, .popCount = 1});
    const auto discard1 = ostr.str();
    // Verify that all column widths are equal
    TEST_ASSERT_EQ(state.length(), state1.length());
    TEST_ASSERT_EQ(state.length(), state2.length());
    TEST_ASSERT_EQ(state.length(), pending.length());
    TEST_ASSERT_EQ(state.length(), pending1.length());
    TEST_ASSERT_EQ(state.length(), pending2.length());
    TEST_ASSERT_EQ(state.length(), reduce1.length());
    TEST_ASSERT_EQ(state.length(), reduce2.length());
    TEST_ASSERT_EQ(state.length(), discard1.length());
}
