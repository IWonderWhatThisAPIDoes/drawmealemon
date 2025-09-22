/**
 * @file ascii_target.hpp
 * 
 * Implementation of @ref render_target that renders
 * the parser's execution as ASCII art
 */

#pragma once

#include <iostream>
#include <memory>
#include <vector>
#include <optional>
#include "render_target.hpp"
#include "ascii_fragment_table.hpp"

namespace dmalem {

/**
 * Implementation of @ref render_target that renders
 * the parser's execution as ASCII art into a stream
 */
class ascii_target : public render_target {
public:
    /**
     * Constructs an ASCII render target that renders to a stream
     * 
     * @param ostr The stream for the target to render to.
     *             Must outlive the target
     */
    ascii_target(std::ostream& ostr);

    void input_token(const std::string_view& name) override;
    void shift(int nextState) override;
    void shift_reduce() override;
    void syntax_error() override;
    void reduce(size_t count, const std::string_view& tokenName, const std::string_view& ruleName) override;
    void pop() override;
    void discard() override;
    void accept() override;
    void failure() override;
    void stack_overflow() override;
    void finalize() override;

private:
    /**
     * Prints the header row of the output
     */
    void header();
    /**
     * Prints the left margin of the visualization that may or may not
     * contain a pending token indicator
     */
    void left_margin();
    /**
     * Prints a whole empty left column,
     * including the margin and the column separator
     */
    void blank_left_column();
    /**
     * Prints a row of the stack trace in a given context
     * 
     * @param rowKind Optionally indicate special context of the row
     * @param popCount For relevant values of the @p rowKind parameter,
     *                 indicates how many states are being removed in this row
     */
    void right_column(
        state_fragment_data::row_kind rowKind = state_fragment_data::row_kind::neutral,
        size_t popCount = 0
    );
    /**
     * Prints a full row with no special content
     */
    void blank_line();
    /**
     * Shifts a token to a new state or pending reduce
     * 
     * @param nextState Index of the state to switch to,
     *                  or empty if the frame corresponds to a pending reduce
     */
    void shift_frame(std::optional<int> nextState);
    /**
     * If error recovery is in progress, ends it and prints
     * the row that describes how the error was handled
     */
    void flush_error_recovery();
    /**
     * If a new input token has not yet been printed,
     * prints it now
     */
    void flush_input_token();
    /**
     * Prints a line terminator. Also increments row counter
     */
    void endl();
    /**
     * Prints the footer of the visualization
     * 
     * @param cause Indicates how the parser exited
     */
    void footer(parser_termination_cause cause);
    /**
     * Information about a single stack frame
     */
    struct stack_frame {
        /**
         * Index of the line where the frame started
         */
        size_t firstLine;
        /**
         * Index of the state represented by the frame.
         * Empty if the frame corresponds to a pending reduce instead
         */
        std::optional<int> stateId;
    };

    std::ostream* ostr;
    std::unique_ptr<ascii_fragment_table> fragments;
    std::vector<stack_frame> stackContents;
    bool pendingToken = false;
    bool pendingNonterminal = false;
    bool pendingSyntaxError = false;
    size_t errorRecoveryPopped = 0;
    size_t lineIndex = 0;
    std::string pendingInput;
};

}
