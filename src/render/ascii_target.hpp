/**
 * @file ascii_target.hpp
 * 
 * Implementation of @ref render_target that renders
 * the parser's execution as ASCII art
 */

#pragma once

#include <iostream>
#include "render_target.hpp"

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
    void blank_left_column();
    void blank_right_column();
    void endl();
    void shift_first_row();
    void flush_error_recovery();

    std::ostream* ostr;
    size_t stateCount = 0;
    size_t pendingTokens = 0;
    bool pendingSyntaxError = false;
    size_t errorRecoveryPopped = 0;
};

}
