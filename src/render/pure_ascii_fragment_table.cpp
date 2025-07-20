/**
 * @file pure_ascii_fragment_table.cpp
 * 
 * Default implementation of @ref ascii_fragment_table
 */

#include <iomanip>
#include "pure_ascii_fragment_table.hpp"

namespace dmalem {

static constexpr size_t max_nonterm_width = 10;
static constexpr size_t max_term_width = 10;
static constexpr char entry_arrow[] = "-> ";
static constexpr size_t entry_arrow_length = sizeof(entry_arrow) / sizeof(char) - 1;
static constexpr size_t input_column_width = entry_arrow_length + std::max(max_term_width + 1, max_nonterm_width + 3);

void pure_ascii_fragment_table::column_separator() const {
    stream() << "||";
}

void pure_ascii_fragment_table::left_column_head() const {
    stream() << std::right << std::setw(input_column_width) << "INPUT ";
}

void pure_ascii_fragment_table::right_column_head() const {
    stream() << " STACK";
}

void pure_ascii_fragment_table::state(const state_fragment_data& data) const {
    using enum state_fragment_data::row_kind;
    if (data.columnIndex >= data.columnCount - data.popCount) {
        // Special fragments for columns that are being removed
        if (data.rowKind == discard || data.rowKind == failure || data.rowKind == stack_overflow)
            stream() << "xx+";
        else if (data.columnIndex == data.columnCount - 1)
            stream() << "--`";
        else
            stream() << "--+";
    } else {
        // Use the normal fragment in all other cases
        switch (data.line) {
            case 0:
                stream() << "--,";
                break;
            case 1:
                if (data.state.has_value())
                    // Print the state number
                    stream() << std::right << std::setw(2) << data.state.value() << '|';
                else
                    // Pending reduce
                    stream() << " R|";
                break;
            default:
                stream() << "  |";
        }
    }
}

void pure_ascii_fragment_table::pull_nonterminal(size_t reduceCount) const {
    stream() << std::right << std::setw(input_column_width - entry_arrow_length - 1) <<
        (reduceCount == 0 ? ",-* " : ",---");
}

void pure_ascii_fragment_table::bring_token(const std::string_view& name) const {
    stream() << entry_arrow;
    if (name.length() >= input_column_width - entry_arrow_length)
        stream() << name.substr(0, input_column_width - entry_arrow_length - 3) << ".. ";
    else
        stream() << std::left << std::setw(input_column_width - entry_arrow_length) << name;
}

void pure_ascii_fragment_table::shift_token() const {
    stream()
        << std::setw(entry_arrow_length) << ' '
        << std::setw(input_column_width - entry_arrow_length) << std::setfill('-') << std::left
        << " `"
        << std::setfill(' ');
}

void pure_ascii_fragment_table::shift_nonterminal() const {
    stream() << std::right << std::setw(input_column_width - entry_arrow_length - 1) << "`---";
}

void pure_ascii_fragment_table::discard_token() const {
    stream() << std::right << std::setw(entry_arrow_length + 1) << 'X';
}

void pure_ascii_fragment_table::pending_token() const {
    stream() << std::right << std::setw(entry_arrow_length + 1) << '|';
}

void pure_ascii_fragment_table::empty_left_margin() const {
    stream() << std::right << std::setw(entry_arrow_length + 1) << ' ';
}

void pure_ascii_fragment_table::endl() const {
    stream() << '\n';
}

void pure_ascii_fragment_table::empty_left_column() const {
    stream() << std::setw(input_column_width - entry_arrow_length - 1) << ' ';
}

void pure_ascii_fragment_table::nonterminal_name(const std::string_view& name) const {
    if (name.length() >= input_column_width - entry_arrow_length - 2)
        stream() << name.substr(0, input_column_width - entry_arrow_length - 4) << ".. ";
    else
        stream() << std::right << std::setw(input_column_width - entry_arrow_length - 2) << name << ' ';
}

void pure_ascii_fragment_table::reduce_rule_label(const std::string_view& rule) const {
    stream() << ' ' << rule;
}

void pure_ascii_fragment_table::termination_label(parser_termination_cause cause) const {
    switch (cause) {
        case parser_termination_cause::accept:
            stream() << " Accept!";
            break;
        case parser_termination_cause::failure:
            stream() << " Failure!";
            break;
        case parser_termination_cause::stack_overflow:
            stream() << " Stack overflow!";
            break;
    }
}

void pure_ascii_fragment_table::syntax_error_label() const {
    stream() << " Syntax error";
}

}
