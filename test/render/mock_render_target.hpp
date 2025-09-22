/**
 * @file mock_render_target.hpp
 * 
 * Mock implementation of @ref render_target for testing
 */

#pragma once

#include "../../src/render/render_target.hpp"
#include "../testbed/mock.hpp"

namespace dmalem {

/**
 * Mock implementation of @ref render_target
 */
class mock_render_target : public render_target {
public:
    void finalize() override {}
    void input_token(const std::string_view& name) override {}
    void shift(int nextState) override {}
    void shift_reduce() override {}
    void syntax_error() override {}
    void reduce(
        size_t count,
        const std::string_view& tokenName,
        const std::string_view& ruleName
    ) override {}
    void pop() override {}
    void discard() override {}
    void accept() override {}
    void failure() override {}
    void stack_overflow() override {}
};

}
