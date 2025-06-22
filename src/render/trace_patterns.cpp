/**
 * @file trace_patterns.cpp
 * 
 * Pattern table of outputs produced by Lemon
 * that can be used with a @ref trace_parser
 */

#include <algorithm>
#include "trace_patterns.hpp"

namespace dmalem {

/**
 * Pattern handler that does nothing
 */
static void nop(trace_action_sink&, const std::vector<string_pattern::field>&) {}

/**
 * Pattern handler for notifications of new tokens being read
 */
static void input_token(trace_action_sink& sink, const std::vector<string_pattern::field>& fields) {
    const std::string_view tokenName = std::get<std::string_view>(fields.at(0));
    sink.input_token(tokenName);
}

/**
 * Pattern handler for notifications of tokens being shifted
 */
static void shift_state(trace_action_sink& sink, const std::vector<string_pattern::field>& fields) {
    const int nextState = std::get<int>(fields.at(1));
    sink.shift(nextState);
}

/**
 * Pattern handler for notifications of rule reduction
 */
static void reduce(trace_action_sink& sink, const std::vector<string_pattern::field>& fields) {
    const std::string_view ruleText = std::get<std::string_view>(fields.at(1));
    const size_t separatorCount = std::ranges::count(ruleText, ' ');
    // The rule name should be in the form [nonterm ::= token ...]
    // At least that one space should be present
    if (separatorCount == 0)
        throw std::invalid_argument(__FUNCTION__);
    // Get the target nonterminal as well
    const size_t firstSeparator = ruleText.find_first_of(' ');
    const std::string_view targetName = ruleText.substr(0, firstSeparator);
    // To count the number of tokens on the right side,
    // we count the number of spaces, leaving out the first one
    sink.reduce(separatorCount - 1, targetName, ruleText);
}

/**
 * Constructs a pattern handler that calls a method of the target
 * without any arguments
 */
static auto method(void (trace_action_sink::* fun)()) {
    return [fun](trace_action_sink& sink, const auto&) {
        std::invoke(fun, sink);
    };
}

const trace_parser<trace_action_sink>::pattern_list trace_patterns::TABLE = {
    {"Stack grows from %d to %d entries.",                            nop},
    {"Popping %s",                                                    method(trace_action_sink::pop)},
    {"FALLBACK %s => %s",                                             nop},
    {"WILDCARD %s => %s",                                             nop},
    {"Stack Overflow!",                                               method(trace_action_sink::stack_overflow)},
    {"Shift '%S', go to state %d",                                    shift_state},
    {"... then shift '%S', go to state %d",                           shift_state},
    {"Shift '%S', pending reduce %d",                                 method(trace_action_sink::shift_reduce)},
    {"... then shift '%S', pending reduce %d",                        method(trace_action_sink::shift_reduce)},
    {"Fail!",                                                         method(trace_action_sink::failure)},
    {"Accept!",                                                       method(trace_action_sink::accept)},
    {"Input '%S' in state %d",                                        input_token},
    {"Input '%S' with pending reduce %d",                             input_token},
    {"Reduce %d [%S], pop back to state %d.",                         reduce},
    {"Reduce %d [%S] without external action, pop back to state %d.", reduce},
    {"Reduce %d [%S].",                                               reduce},
    {"Reduce %d [%S] without external action.",                       reduce},
    {"Syntax Error!",                                                 method(trace_action_sink::syntax_error)},
    {"Discard input token %s",                                        method(trace_action_sink::discard)},
    {"Return. Stack=%S]",                                             nop},
};

}
