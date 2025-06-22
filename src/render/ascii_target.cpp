/**
 * @file ascii_target.cpp
 * 
 * Implementation of @ref render_target that renders
 * the parser's execution as ASCII art
 */

#include <iomanip>
#include "ascii_target.hpp"

namespace dmalem {

static constexpr size_t max_nonterm_width = 10;
static constexpr size_t max_term_width = 10;
static constexpr char entry_arrow[] = "-> ";
static constexpr size_t entry_arrow_length = sizeof(entry_arrow) / sizeof(char) - 1;
static constexpr size_t input_column_width = entry_arrow_length + std::max(max_term_width + 1, max_nonterm_width + 3);

ascii_target::ascii_target(std::ostream& ostr) : ostr(&ostr) {
    ostr << std::setw(input_column_width) << "INPUT " << "|| STACK\n";
    blank_left_column();
    ostr << "--,\n";
    blank_left_column();
    ostr << " 0|\n";
    stateCount = 1;
}

void ascii_target::blank_left_column() {
    *ostr <<
        std::setw(entry_arrow_length) << ' ' <<
        (pendingTokens > 0 ? '|' : ' ') <<
        std::setw(input_column_width - entry_arrow_length - 1) << ' ' <<
        "||";
}

void ascii_target::blank_right_column() {
    for (size_t i = 0; i < stateCount; ++i)
        *ostr << "  |";
}

void ascii_target::endl() {
    *ostr << '\n';
}

void ascii_target::shift_first_row() {
    if (pendingTokens > 1) {
        *ostr
            << std::setw(entry_arrow_length) << ' ' << '|'
            << std::right << std::setw(input_column_width - entry_arrow_length - 1)
            << "`---"
            << "||";
    } else {
        *ostr
            << std::setw(entry_arrow_length) << ' '
            << std::setw(input_column_width - entry_arrow_length) << std::setfill('-') << std::left
            << " `"
            << std::setfill(' ')
            << "||";
    }
    --pendingTokens;
    blank_right_column();
    *ostr << "--,\n";
}

void ascii_target::flush_error_recovery() {
    if (!pendingSyntaxError)
        return;
    pendingSyntaxError = false;
    stateCount -= errorRecoveryPopped;
    *ostr
        << std::setw(entry_arrow_length) << ' ' << '|'
        << std::right << std::setw(input_column_width - entry_arrow_length - 1)
        << ",-* " << "||";
    blank_right_column();
    for (size_t i = 0; i < errorRecoveryPopped; ++i)
        *ostr << "xx+";
    *ostr << " Syntax error\n";
    *ostr
        << std::setw(entry_arrow_length) << ' ' << '|'
        << std::right << std::setw(input_column_width - entry_arrow_length - 2)
        << "error"
        << " ||";
    blank_right_column();
    endl();
    ++pendingTokens;
}

void ascii_target::input_token(const std::string_view& name) {
    *ostr << entry_arrow << std::left << std::setw(input_column_width - entry_arrow_length) << name << "||";
    blank_right_column();
    endl();
    ++pendingTokens;
}

void ascii_target::shift(int nextState) {
    flush_error_recovery();
    shift_first_row();
    blank_left_column();
    blank_right_column();
    *ostr << std::right << std::setw(2) << nextState << "|\n";
    ++stateCount;
}

void ascii_target::shift_reduce() {
    flush_error_recovery();
    shift_first_row();
    blank_left_column();
    blank_right_column();
    *ostr << " R|\n";
    ++stateCount;
}

void ascii_target::syntax_error() {
    pendingSyntaxError = true;
    errorRecoveryPopped = 0;
}

void ascii_target::reduce(size_t count, const std::string_view& tokenName, const std::string_view& ruleName) {
    stateCount -= count;
    *ostr
        << std::setw(entry_arrow_length) << ' ' << '|'
        << std::right << std::setw(input_column_width - entry_arrow_length - 1)
        << (count == 0 ? ",-* " : ",---")
        << "||";
    blank_right_column();
    for (size_t i = 1; i < count; ++i)
        *ostr << "--+";
    if (count != 0)
        *ostr << "--`";
    *ostr
        << ' ' << ruleName << '\n'
        << std::setw(entry_arrow_length) << ' ' << '|'
        << std::right << std::setw(input_column_width - entry_arrow_length - 2)
        << tokenName
        << " ||";
    blank_right_column();
    endl();
    ++pendingTokens;
}

void ascii_target::pop() {
    ++errorRecoveryPopped;
}

void ascii_target::discard() {
    *ostr <<
        std::setw(entry_arrow_length) << ' ' << 'X' <<
        std::setw(input_column_width - entry_arrow_length - 1) << ' ' <<
        "||";
    blank_right_column();
    endl();
    pendingSyntaxError = false;
    pendingTokens = 0;
    blank_left_column();
    blank_right_column();
    endl();
}

void ascii_target::accept() {
    *ostr <<
        std::setw(entry_arrow_length) << ' ' << 'X' <<
        std::setw(input_column_width - entry_arrow_length - 1) << ' ' <<
        "||";
    for (size_t i = 1; i < stateCount; ++i)
        *ostr << "--+";
    *ostr << "--`\n";
    pendingTokens = 0;
    blank_left_column();
    *ostr << " Accept" << std::endl;
}

void ascii_target::failure() {
    *ostr
        << std::setw(entry_arrow_length) << ' ' << 'X'
        << std::right << std::setw(input_column_width - entry_arrow_length - 1)
        << ' ' << "||";
    for (size_t i = 0; i < stateCount; ++i)
        *ostr << "xx+";
    endl();
    stateCount = 0;
    pendingTokens = 0;
    pendingSyntaxError = false;
    blank_left_column();
    *ostr << " Failure!" << std::endl;
}

void ascii_target::stack_overflow() {
    *ostr
        << std::setw(entry_arrow_length) << ' ' << 'X'
        << std::right << std::setw(input_column_width - entry_arrow_length - 1)
        << ' ' << "||";
    for (size_t i = 0; i < stateCount; ++i)
        *ostr << "xx+";
    endl();
    stateCount = 0;
    pendingTokens = 0;
    pendingSyntaxError = false;
    blank_left_column();
    *ostr << " Stack overflow!" << std::endl;
}

void ascii_target::set_options(const std::vector<std::string>& options) {

}

void ascii_target::finalize() {
    ostr->flush();
}

}
