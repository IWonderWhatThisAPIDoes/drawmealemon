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
    header();
    stackContents.push_back({.firstLine = lineIndex, .stateId = 0});
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

void ascii_target::shift_frame(std::optional<int> nextState) {
    // If a nonterminal is being shifted as a result of a pending reduce
    // from previous token, do not indicate the new token just yet
    if (!pendingNonterminal)
        flush_input_token();
    // If it's the error nonterminal being shifted, indicate the error recovery now
    flush_error_recovery();
    stackContents.push_back({.firstLine = lineIndex, .stateId = nextState});
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
    blank_line();
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

void ascii_target::flush_input_token() {
    if (pendingInput.empty())
        return;
    fragments->bring_token(pendingInput);
    fragments->column_separator();
    right_column();
    endl();
    pendingInput.clear();
    pendingToken = true;
}

void ascii_target::endl() {
    ++lineIndex;
    fragments->endl();
}

void ascii_target::footer(parser_termination_cause cause) {
    fragments->discard_token();
    fragments->empty_left_column();
    fragments->column_separator();
    right_column(state_fragment_data::termination_row_kind(cause), stackContents.size());
    // If the parser exits due to an immediate syntax error, print the notice as well
    if (pendingSyntaxError)
        fragments->syntax_error_label();
    endl();
    // Clear all state
    stackContents.clear();
    pendingToken = false;
    pendingNonterminal = false;
    pendingSyntaxError = false;
    // Print the final line
    blank_left_column();
    fragments->termination_label(cause);
    endl();
}

void ascii_target::input_token(const std::string_view& name) {
    // Postpone printing of the token until after pending reduce is handled
    pendingInput = name;
}

void ascii_target::shift(int nextState) {
    shift_frame(nextState);
}

void ascii_target::shift_reduce() {
    shift_frame(std::nullopt);
}

void ascii_target::syntax_error() {
    // Syntax error means the new token was not expected in this state,
    // so it should be printed by now
    flush_input_token();
    // Enter error recovery mode, do not print anything yet
    // The error will be printed once its handling is completed
    pendingSyntaxError = true;
    errorRecoveryPopped = 0;
}

void ascii_target::reduce(size_t count, const std::string_view& tokenName, const std::string_view& ruleName) {
    // Indicate that a token has been read from the input,
    // unless the topmost frame is a pending reduce,
    // in which case the rule can be reduced before seeing that token
    if (stackContents.back().stateId.has_value())
        flush_input_token();
    // The row where the reduction takes place
    left_margin();
    fragments->pull_nonterminal(count);
    fragments->column_separator();
    right_column(state_fragment_data::row_kind::reduce, count);
    fragments->reduce_rule_label(ruleName);
    endl();
    // Pop the reduced states off the stack
    for (size_t i = 0; i < count; ++i)
        stackContents.pop_back();
    // The row where the nonterminal name appears
    left_margin();
    fragments->nonterminal_name(tokenName);
    fragments->column_separator();
    right_column();
    endl();
    // Set the nonterminal flag so that the next shift affects the nonterminal
    pendingNonterminal = true;
}

void ascii_target::pop() {
    // Popping a token only makes sense in the context of error recovery
    if (!pendingSyntaxError)
        throw std::logic_error(__FUNCTION__);
    // Remember that the token was popped, but do not print anything yet
    // This will be printed all at once when the error recovery is flushed
    ++errorRecoveryPopped;
}

void ascii_target::discard() {
    // Cannot discard a token when there are not any
    if (!pendingToken)
        throw std::logic_error(__FUNCTION__);
    // Print the row
    fragments->discard_token();
    fragments->empty_left_column();
    fragments->column_separator();
    right_column();
    // If this is a second error that does not shift an error nonterminal,
    // print the error notice here instead
    if (pendingSyntaxError)
        fragments->syntax_error_label();
    endl();
    // Reset flags
    pendingSyntaxError = false;
    pendingToken = false;
    // Add a line of spacing before bringing the next token
    blank_line();
}

void ascii_target::accept() {
    footer(parser_termination_cause::accept);
}

void ascii_target::failure() {
    footer(parser_termination_cause::failure);
}

void ascii_target::stack_overflow() {
    // Overflow may occurr by attempting to shift a new token
    // or the error nonterminal (or a different nonterminal,
    // but those have already been printed)
    flush_input_token();
    flush_error_recovery();
    footer(parser_termination_cause::stack_overflow);
}

void ascii_target::finalize() {
    ostr->flush();
}

}
