/**
 * @file trace_patterns.hpp
 * 
 * Pattern table of outputs produced by Lemon
 * that can be used with a @ref trace_parser
 */

#pragma once

#include <vector>
#include "trace_parser.hpp"
#include "trace_action_sink.hpp"

namespace dmalem {

/**
 * Pattern table that maps actions to outputs that can be produced by Lemon
 */
class trace_patterns {
public:
    /**
     * The table that maps Lemon's output line patterns to trace actions
     */
    static const trace_parser<trace_action_sink>::pattern_list TABLE;
};

}
