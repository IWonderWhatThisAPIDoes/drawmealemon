/**
 * @file ascii_target.cpp
 * 
 * Implementation of @ref render_target that renders
 * the parser's execution as ASCII art
 */

#include "ascii_target.hpp"
#include "pure_ascii_fragment_table.hpp"

namespace dmalem {

ascii_target::stack_frame ascii_target::stack_frame::state(int state, size_t line) noexcept {
    return stack_frame {line, state};
}

ascii_target::stack_frame ascii_target::stack_frame::pending_reduce(size_t line) noexcept {
    return stack_frame {line, 0, true};
}

ascii_target::ascii_target(std::ostream& ostr) :
    ostr(&ostr),
    fragments(std::make_unique<pure_ascii_fragment_table>())
{
    fragments->stream(ostr);
    header();
    stackContents.push_back(stack_frame::state(0, lineIndex));
    blank_line();
    blank_line();
}

void ascii_target::header() {
    fragments->left_column_head();
    fragments->column_separator();
    fragments->right_column_head();
    endl();
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

void ascii_target::blank_line() {
    blank_left_column();
    blank_right_column();
    endl();
}

void ascii_target::blank_right_column() {
    for (const stack_frame& frame : stackContents) {
        if (frame.isPendingReduce)
            fragments->pending_reduce(lineIndex - frame.firstLine);
        else
            fragments->state(frame.stateId, lineIndex - frame.firstLine);
    }
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
    endl();
}

void ascii_target::flush_error_recovery() {
    if (!pendingSyntaxError)
        return;
    pendingSyntaxError = false;
    for (size_t i = 0; i < errorRecoveryPopped; ++i)
        stackContents.pop_back();
    left_margin();
    fragments->conjure_nonterminal();
    fragments->column_separator();
    blank_right_column();
    for (size_t i = 0; i < errorRecoveryPopped; ++i)
        fragments->discard_state();
    fragments->syntax_error_label();
    endl();
    left_margin();
    fragments->nonterminal_name("error");
    fragments->column_separator();
    blank_right_column();
    endl();
    ++pendingTokens;
}

void ascii_target::endl() {
    ++lineIndex;
    fragments->endl();
}

void ascii_target::input_token(const std::string_view& name) {
    fragments->bring_token(name);
    fragments->column_separator();
    blank_right_column();
    endl();
    ++pendingTokens;
}

void ascii_target::shift(int nextState) {
    flush_error_recovery();
    stackContents.push_back(stack_frame::state(nextState, lineIndex));
    shift_first_row();
    blank_line();
}

void ascii_target::shift_reduce() {
    flush_error_recovery();
    stackContents.push_back(stack_frame::pending_reduce(lineIndex));
    shift_first_row();
    blank_line();
}

void ascii_target::syntax_error() {
    pendingSyntaxError = true;
    errorRecoveryPopped = 0;
}

void ascii_target::reduce(size_t count, const std::string_view& tokenName, const std::string_view& ruleName) {
    for (size_t i = 0; i < count; ++i)
        stackContents.pop_back();
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
    endl();
    left_margin();
    fragments->nonterminal_name(tokenName);
    fragments->column_separator();
    blank_right_column();
    endl();
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
    endl();
    pendingSyntaxError = false;
    pendingTokens = 0;
    blank_line();
}

void ascii_target::accept() {
    fragments->discard_token();
    fragments->empty_left_column();
    fragments->column_separator();
    for (size_t i = 1; i < stackContents.size(); ++i)
        fragments->reduce_state();
    fragments->reduce_last_state();
    endl();
    pendingTokens = 0;
    blank_left_column();
    fragments->accept_label();
    endl();
}

void ascii_target::failure() {
    fragments->discard_token();
    fragments->empty_left_column();
    fragments->column_separator();
    for (size_t i = 0; i < stackContents.size(); ++i)
        fragments->discard_state();
    endl();
    stackContents.clear();
    pendingTokens = 0;
    pendingSyntaxError = false;
    blank_left_column();
    fragments->failure_label();
    endl();
}

void ascii_target::stack_overflow() {
    fragments->discard_token();
    fragments->empty_left_column();
    fragments->column_separator();
    for (size_t i = 0; i < stackContents.size(); ++i)
        fragments->discard_state();
    endl();
    stackContents.clear();
    pendingTokens = 0;
    pendingSyntaxError = false;
    blank_left_column();
    fragments->overflow_label();
    endl();
}

void ascii_target::set_options(const std::vector<std::string>& options) {

}

void ascii_target::finalize() {
    ostr->flush();
}

}
