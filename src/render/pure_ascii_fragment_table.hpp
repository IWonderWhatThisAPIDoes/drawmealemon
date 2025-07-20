/**
 * @file pure_ascii_fragment_table.hpp
 * 
 * Default implementation of @ref ascii_fragment_table
 */

#pragma once

#include "ascii_fragment_table.hpp"

namespace dmalem {

/**
 * Default implementation of @ref ascii_fragment_table
 */
class pure_ascii_fragment_table : public ascii_fragment_table {
public:
    using ascii_fragment_table::ascii_fragment_table;

    void column_separator() const override;
    void left_column_head() const override;
    void right_column_head() const override;
    void state(const state_fragment_data& data) const override;
    void pull_nonterminal(size_t reduceCount) const override;
    void bring_token(const std::string_view& name) const override;
    void shift_token() const override;
    void shift_nonterminal() const override;
    void discard_token() const override;
    void pending_token() const override;
    void empty_left_margin() const override;
    void endl() const override;
    void empty_left_column() const override;
    void nonterminal_name(const std::string_view& name) const override;
    void reduce_rule_label(const std::string_view& rule) const override;
    void termination_label(parser_termination_cause cause) const override;
    void syntax_error_label() const override;
};

}
