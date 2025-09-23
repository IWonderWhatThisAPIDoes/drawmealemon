/**
 * @file default_trace_parser.hpp
 * 
 * @ref trace_parser preset with all patterns of outputs
 * produced by Lemon
 */

#pragma once

#include <vector>
#include "trace_parser.hpp"
#include "trace_action_sink.hpp"

namespace dmalem {

/**
 * Trace parser that can handle outputs that can be produced by Lemon
 */
trace_parser<trace_action_sink> default_trace_parser();

}
