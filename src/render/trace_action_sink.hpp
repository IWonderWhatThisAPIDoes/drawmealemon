/**
 * @file trace_action_sink.hpp
 * 
 * Definition of trace notifications emited by Lemon
 * that are of interest
 */

#pragma once

#include <string_view>

namespace dmalem {

/**
 * Observer of trace notifications from Lemon
 */
class trace_action_sink {
public:
    virtual ~trace_action_sink() {}
    /**
     * Notifies the observer that a token has been read from the input
     * 
     * @param name Name of the input token
     */
    virtual void input_token(const std::string_view& name) = 0;
    /**
     * Notifies the observer that a token has been shifted
     * and the parser's state has changed
     * 
     * @param nextState New state of the parser
     */
    virtual void shift(int nextState) = 0;
    /**
     * Notifies the observer that a token has been shifted
     * with pending reduce
     */
    virtual void shift_reduce() = 0;
    /**
     * Notifies the observer that the last token cannot
     * be accepted in the current state
     */
    virtual void syntax_error() = 0;
    /**
     * Notifies the observer that the parser has reduced a rule
     * 
     * @param count How many tokens have been popped by the reduction
     * @param tokenName Name of the nonterminal that has been created by the reduction
     * @param ruleName Name of the rule that can be used to annotate the reduction
     */
    virtual void reduce(
        size_t count,
        const std::string_view& tokenName,
        const std::string_view& ruleName
    ) = 0;
    /**
     * Notifies the observer that a state has been popped outside of reduction
     */
    virtual void pop() = 0;
    /**
     * Notifies the observer that an input token has been discarded
     */
    virtual void discard() = 0;
    /**
     * Notifies the observer that the input has been successfully parsed
     */
    virtual void accept() = 0;
    /**
     * Notifies the observer that the parser has irrecoverably failed
     */
    virtual void failure() = 0;
    /**
     * Notifies the observer that the parser has overflown its stack limit
     */
    virtual void stack_overflow() = 0;
};

}
