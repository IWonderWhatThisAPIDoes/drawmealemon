/**
 * @file ascii_target.cpp
 * 
 * Implementation of @ref render_target that renders
 * the parser's execution as ASCII art
 */

#include "ascii_target.hpp"
#include "pure_ascii_fragment_table.hpp"

namespace dmalem {

ascii_target::ascii_target(std::ostream& ostr) :
    ostr(&ostr),
    fragments(std::make_unique<pure_ascii_fragment_table>())
{
    fragments->stream(ostr);
    fragments->left_column_head();
    fragments->column_separator();
    fragments->right_column_head();
    fragments->endl();
    blank_left_column();
    fragments->shift_state_first_row(0);
    fragments->endl();
    blank_left_column();
    fragments->shift_state_second_row(0);
    fragments->endl();
    stateCount = 1;
}

void ascii_target::left_margin() {
    if (pendingTokens > 0)
        fragments->pending_token();
    else
        fragments->empty_left_margin();
}

void ascii_target::blank_left_column() {
    left_margin();
    fragments->empty_left_column();
    fragments->column_separator();
}

void ascii_target::blank_right_column() {
    for (size_t i = 0; i < stateCount; ++i)
        fragments->state();
}

void ascii_target::shift_first_row() {
    if (pendingTokens > 1) {
        fragments->pending_token();
        fragments->shift_nonterminal();
        fragments->column_separator();
    } else {
        fragments->shift_token();
        fragments->column_separator();
    }
    --pendingTokens;
    blank_right_column();
    fragments->shift_state_first_row(0);
    fragments->endl();
}

void ascii_target::flush_error_recovery() {
    if (!pendingSyntaxError)
        return;
    pendingSyntaxError = false;
    stateCount -= errorRecoveryPopped;
    left_margin();
    fragments->conjure_nonterminal();
    fragments->column_separator();
    blank_right_column();
    for (size_t i = 0; i < errorRecoveryPopped; ++i)
        fragments->discard_state();
    fragments->syntax_error_label();
    fragments->endl();
    left_margin();
    fragments->nonterminal_name("error");
    fragments->column_separator();
    blank_right_column();
    fragments->endl();
    ++pendingTokens;
}

void ascii_target::input_token(const std::string_view& name) {
    fragments->bring_token(name);
    fragments->column_separator();
    blank_right_column();
    fragments->endl();
    ++pendingTokens;
}

void ascii_target::shift(int nextState) {
    flush_error_recovery();
    shift_first_row();
    blank_left_column();
    blank_right_column();
    fragments->shift_state_second_row(nextState);
    fragments->endl();
    ++stateCount;
}

void ascii_target::shift_reduce() {
    flush_error_recovery();
    shift_first_row();
    blank_left_column();
    blank_right_column();
    fragments->shift_reduce_second_row();
    fragments->endl();
    ++stateCount;
}

void ascii_target::syntax_error() {
    pendingSyntaxError = true;
    errorRecoveryPopped = 0;
}

void ascii_target::reduce(size_t count, const std::string_view& tokenName, const std::string_view& ruleName) {
    stateCount -= count;
    left_margin();
    if (count == 0)
        fragments->conjure_nonterminal();
    else
        fragments->pull_nonterminal();
    fragments->column_separator();
    blank_right_column();
    for (size_t i = 1; i < count; ++i)
        fragments->reduce_state();
    if (count != 0)
        fragments->reduce_last_state();
    fragments->reduce_rule_label(ruleName);
    fragments->endl();
    left_margin();
    fragments->nonterminal_name(tokenName);
    fragments->column_separator();
    blank_right_column();
    fragments->endl();
    ++pendingTokens;
}

void ascii_target::pop() {
    ++errorRecoveryPopped;
}

void ascii_target::discard() {
    fragments->discard_token();
    fragments->empty_left_column();
    fragments->column_separator();
    blank_right_column();
    fragments->endl();
    pendingSyntaxError = false;
    pendingTokens = 0;
    blank_left_column();
    blank_right_column();
    fragments->endl();
}

void ascii_target::accept() {
    fragments->discard_token();
    fragments->empty_left_column();
    fragments->column_separator();
    for (size_t i = 1; i < stateCount; ++i)
        fragments->reduce_state();
    fragments->reduce_last_state();
    fragments->endl();
    pendingTokens = 0;
    blank_left_column();
    fragments->accept_label();
    fragments->endl();
}

void ascii_target::failure() {
    fragments->discard_token();
    fragments->empty_left_column();
    fragments->column_separator();
    for (size_t i = 0; i < stateCount; ++i)
        fragments->discard_state();
    fragments->endl();
    stateCount = 0;
    pendingTokens = 0;
    pendingSyntaxError = false;
    blank_left_column();
    fragments->failure_label();
    fragments->endl();
}

void ascii_target::stack_overflow() {
    fragments->discard_token();
    fragments->empty_left_column();
    fragments->column_separator();
    for (size_t i = 0; i < stateCount; ++i)
        fragments->discard_state();
    fragments->endl();
    stateCount = 0;
    pendingTokens = 0;
    pendingSyntaxError = false;
    blank_left_column();
    fragments->overflow_label();
    fragments->endl();
}

void ascii_target::set_options(const std::vector<std::string>& options) {

}

void ascii_target::finalize() {
    ostr->flush();
}

}
