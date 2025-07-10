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
    void set_options(const std::vector<std::string>& options) override;
    void finalize() override;

private:
    void header();
    void left_margin();
    void blank_left_column();
    void right_column(
        state_fragment_data::row_kind rowKind = state_fragment_data::row_kind::neutral,
        size_t popCount = 0
    );
    void blank_line();
    void shift_first_row();
    void flush_error_recovery();
    void endl();
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
        /**
         * Constructs a stack frame that corresponds to a state
         * 
         * @param state Index of the state associated with the frame
         * @param line Index of the output line where the frame starts
         * @return The new stack frame
         */
        static stack_frame state(int state, size_t line) noexcept;
        /**
         * Constructs a stack frame that corresponds to a pending reduce
         * 
         * @param line Index of the output line where the frame starts
         * @return The new stack frame
         */
        static stack_frame pending_reduce(size_t line) noexcept;
    };

    std::ostream* ostr;
    std::unique_ptr<ascii_fragment_table> fragments;
    std::vector<stack_frame> stackContents;
    bool pendingToken = false;
    bool pendingNonterminal = false;
    bool pendingSyntaxError = false;
    size_t errorRecoveryPopped = 0;
    size_t lineIndex = 0;
};

}
