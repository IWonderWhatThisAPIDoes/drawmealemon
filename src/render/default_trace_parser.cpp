/**
 * @file default_trace_parser.cpp
 * 
 * @ref trace_parser preset with all patterns of outputs
 * produced by Lemon
 */

#include <algorithm>
#include "default_trace_parser.hpp"

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

trace_parser<trace_action_sink> default_trace_parser() {
    trace_parser<trace_action_sink> parser;
    parser
        .add_pattern("Stack grows from %d to %d entries.",                            nop)
        .add_pattern("Popping %s",                                                    method(trace_action_sink::pop))
        .add_pattern("FALLBACK %s => %s",                                             nop)
        .add_pattern("WILDCARD %s => %s",                                             nop)
        .add_pattern("Stack Overflow!",                                               method(trace_action_sink::stack_overflow))
        .add_pattern("Shift '%S', go to state %d",                                    shift_state)
        .add_pattern("... then shift '%S', go to state %d",                           shift_state)
        .add_pattern("Shift '%S', pending reduce %d",                                 method(trace_action_sink::shift_reduce))
        .add_pattern("... then shift '%S', pending reduce %d",                        method(trace_action_sink::shift_reduce))
        .add_pattern("Fail!",                                                         method(trace_action_sink::failure))
        .add_pattern("Accept!",                                                       method(trace_action_sink::accept))
        .add_pattern("Input '%S' in state %d",                                        input_token)
        .add_pattern("Input '%S' with pending reduce %d",                             input_token)
        .add_pattern("Reduce %d [%S], pop back to state %d.",                         reduce)
        .add_pattern("Reduce %d [%S] without external action, pop back to state %d.", reduce)
        .add_pattern("Reduce %d [%S].",                                               reduce)
        .add_pattern("Reduce %d [%S] without external action.",                       reduce)
        .add_pattern("Syntax Error!",                                                 method(trace_action_sink::syntax_error))
        .add_pattern("Discard input token %s",                                        method(trace_action_sink::discard))
        .add_pattern("Return. Stack=%S]",                                             nop);
    return parser;
}

}
