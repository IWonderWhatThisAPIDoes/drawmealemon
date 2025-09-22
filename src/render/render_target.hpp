/**
 * @file render_target.hpp
 * 
 * Definition of the generic render target
 */

#pragma once

#include <vector>
#include <string>
#include "trace_action_sink.hpp"

namespace dmalem {

/**
 * Represents an output for rendering the parser state
 */
class render_target : public trace_action_sink {
public:
    /**
     * Notifies the render target that its whole input has been read
     * and that it should finish the in-progress rendering
     * 
     * No other calls may be issued to this interface after calling this
     */
    virtual void finalize() = 0;
};

}
