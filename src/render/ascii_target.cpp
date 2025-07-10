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
    return stack_frame {line, std::nullopt};
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
    if (pendingToken)
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
    right_column();
    endl();
}

void ascii_target::right_column(state_fragment_data::row_kind rowKind, size_t popCount) {
    for (size_t i = 0; i < stackContents.size(); ++i) {
        fragments->state({
            .state       = stackContents[i].stateId,
            .line        = lineIndex - stackContents[i].firstLine,
            .columnCount = stackContents.size(),
            .columnIndex = i,
            .rowKind     = rowKind,
            .popCount    = popCount,
        });
    }
}

void ascii_target::shift_first_row() {
    if (pendingNonterminal) {
        left_margin();
        fragments->shift_nonterminal();
        pendingNonterminal = false;
    } else if (pendingToken) {
        fragments->shift_token();
        pendingToken = false;
    } else {
        throw std::logic_error(__FUNCTION__);
    }
    fragments->column_separator();
    right_column(state_fragment_data::row_kind::shift);
    endl();
}

void ascii_target::flush_error_recovery() {
    if (!pendingSyntaxError)
        return;
    pendingSyntaxError = false;
    left_margin();
    fragments->pull_nonterminal(0); // Error nonterminal is always conjured with no state being popped
    fragments->column_separator();
    right_column(state_fragment_data::row_kind::discard, errorRecoveryPopped);
    for (size_t i = 0; i < errorRecoveryPopped; ++i)
        stackContents.pop_back();
    fragments->syntax_error_label();
    endl();
    left_margin();
    fragments->nonterminal_name("error");
    fragments->column_separator();
    right_column();
    endl();
    pendingNonterminal = true;
}

void ascii_target::endl() {
    ++lineIndex;
    fragments->endl();
}

void ascii_target::input_token(const std::string_view& name) {
    fragments->bring_token(name);
    fragments->column_separator();
    right_column();
    endl();
    pendingToken = true;
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
    left_margin();
    fragments->pull_nonterminal(count);
    fragments->column_separator();
    right_column(state_fragment_data::row_kind::reduce, count);
    for (size_t i = 0; i < count; ++i)
        stackContents.pop_back();
    fragments->reduce_rule_label(ruleName);
    endl();
    left_margin();
    fragments->nonterminal_name(tokenName);
    fragments->column_separator();
    right_column();
    endl();
    pendingNonterminal = true;
}

void ascii_target::pop() {
    ++errorRecoveryPopped;
}

void ascii_target::discard() {
    fragments->discard_token();
    fragments->empty_left_column();
    fragments->column_separator();
    right_column();
    endl();
    pendingSyntaxError = false;
    pendingToken = false;
    blank_line();
}

void ascii_target::accept() {
    fragments->discard_token();
    fragments->empty_left_column();
    fragments->column_separator();
    right_column(state_fragment_data::row_kind::accept, stackContents.size());
    endl();
    pendingToken = false;
    pendingNonterminal = false;
    blank_left_column();
    fragments->accept_label();
    endl();
}

void ascii_target::failure() {
    fragments->discard_token();
    fragments->empty_left_column();
    fragments->column_separator();
    right_column(state_fragment_data::row_kind::failure, stackContents.size());
    endl();
    stackContents.clear();
    pendingToken = false;
    pendingNonterminal = false;
    blank_left_column();
    fragments->failure_label();
    endl();
}

void ascii_target::stack_overflow() {
    fragments->discard_token();
    fragments->empty_left_column();
    fragments->column_separator();
    right_column(state_fragment_data::row_kind::stack_overflow, stackContents.size());
    endl();
    stackContents.clear();
    pendingToken = false;
    pendingNonterminal = false;
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
