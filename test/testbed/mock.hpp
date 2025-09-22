/**
 * @file mock.hpp
 * 
 * Mocking function implementations
 */

#pragma once

#include <functional>
#include <tuple>
#include <memory>

namespace test {

/**
 * Placeholder that replaces a discarded argument in a mock call log
 */
struct mock_discarded_argument {};

/**
 * Helper for @ref mkeep template construction.
 * Performs an implicit cast between types, as a function
 * 
 * @tparam T Source type
 * @tparam R Target type
 * @param t Value to cast to the target type
 */
template<class R, class T>
R default_mkeep_cast(T t) {
    return t;
}

/**
 * Marker that tells a mock to copy its argument and store its value for later
 * 
 * @tparam T Type of the actual argument
 * @tparam R Type of the value logged for later retrieval
 * @tparam F Conversion from @p T to @p R
 */
template<class T, class R = std::remove_const_t<T>, auto F = default_mkeep_cast<T, R>>
struct mkeep;

template<class T>
struct mock_arg {
    using log_type = mock_discarded_argument;
    using arg_type = T;
    static constexpr log_type forward(const arg_type& t) noexcept {
        return {};
    }
};

template<class T, class R, auto F>
struct mock_arg<mkeep<T, R, F>> {
    using log_type = R;
    using arg_type = T;
    static constexpr log_type forward(const arg_type& t) noexcept {
        return F(t);
    }
};

template<class T, class R, auto F>
struct mock_arg<mkeep<T, R, F>&> {
    using log_type = R;
    using arg_type = T&;
    static constexpr log_type forward(const arg_type& t) noexcept {
        return F(t);
    }
};

template<class T, class R, auto F>
struct mock_arg<const mkeep<T, R, F>&> {
    using log_type = R;
    using arg_type = const T&;
    static constexpr log_type forward(const arg_type& t) noexcept {
        return F(t);
    }
};

/**
 * Maps types of mock function arguments to types
 * that their values should be converted to before logging
 */
template<class T>
using mock_arg_log_t = typename mock_arg<T>::log_type;

/**
 * Removes marker types from a mock argument type
 * to get the actual type of the argument accepted by the mock
 */
template<class T>
using mock_arg_t = typename mock_arg<T>::arg_type;

/**
 * Context object shared between multiple mock objects
 * 
 * Use this class to enable call order assertions across different mocks
 * 
 * Mock contexts have reference semantics:
 * a copy of the context will share its call order.
 */
class mock_context {
public:
    /**
     * Constructs a new mock context
     */
    mock_context() : ctl(std::make_shared<control_block>()) {}
private:
    /**
     * Shared state of the mock contexts
     */
    struct control_block {
        size_t callOrder = 0;
    };
    std::shared_ptr<control_block> ctl;
    template<class>
    friend class mock;
};

/**
 * Basic mock wrapper for a function
 * 
 * Mock wrappers have reference semantics:
 * a copy of the mock will share its call log
 * and mock implementation. This makes it easier
 * to evaluate the outcome, even if the tested API
 * needs to own its callbacks
 * 
 * @tparam F Type of the mocked function
 */
template<class F>
class mock {
    static_assert(false, "Only function types can be mocked");
};

template<class R, class...Args>
class mock<R(Args...)> {
public:
    /**
     * Constructs an empty mock wrapper
     * 
     * If the return type is `void`, the mock will do nothing when called.
     * Otherwise, a `std::bad_function_call` is thrown
     */
    mock() : mock(nullptr) {}
    /**
     * Constructs a mock wrapper with the provided implementation
     * 
     * @param impl Implementation called through the mock
     */
    mock(std::function<R(mock_arg_t<Args>...)>&& impl) :
        ctl(std::make_shared<control_block>(std::move(impl)))
    {}
    /**
     * Constructs a mock wrapper with the provided implementation and context
     * 
     * @param impl Implementation called through the mock
     * @param context Context that binds the mock to other mocks created with it
     */
    mock(std::function<R(mock_arg_t<Args>...)>&& impl, mock_context& context) :
        ctl(std::make_shared<control_block>(std::move(impl))),
        ctx(context.ctl)
    {}
    /**
     * Constructs a mock wrapper with the provided context
     * 
     * @param context Context that binds the mock to other mocks created with it
     */
    mock(mock_context& context) : mock(nullptr, context) {}
    /**
     * Call the mocked function
     * 
     * If an implementation is provided for the mock, the call is forwarded to it
     * 
     * @param args Arguments forwarded to the mock implementation, if any
     */
    R operator()(mock_arg_t<Args>...args) const {
        // Take note that the call was made
        make_call_record(mock_arg<Args>::forward(args)...);
        // If the function returns void, forgive missing implementation
        if constexpr (std::is_same_v<R, void>) {
            if (!ctl->implementation) {
                return;
            }
        }
        // Forward the call to implementation
        return ctl->implementation(std::forward<mock_arg_t<Args>>(args)...);
    }
    /**
     * Queries the total number of calls made to the mock
     * 
     * @return Number of times the mock was called
     */
    size_t call_count() const noexcept {
        return ctl->calls.size();
    }
    /**
     * Retrieves the arguments that have been passed to a call
     * 
     * @param callIndex Index of the queried call in call order
     * @return Tuple of arguments passed to the specified call
     * @throw std::out_of_range @p callIndex is out of range
     */
    const std::tuple<mock_arg_log_t<Args>...>& args(size_t callIndex = 0) const {
        return ctl->calls.at(callIndex).args;
    }
    /**
     * Retrieves the call order of a call, relative to all calls
     * made to mocks created with the same context
     * 
     * @param callIndex Index of the queried call in call order
     * @return Index of the call in global order
     * @throw std::out_of_range @p callIndex is out of range
     */
    size_t global_call_order(size_t callIndex = 0) const {
        return ctl->calls.at(callIndex).callOrder;
    }
private:
    void make_call_record(mock_arg_log_t<Args>...args) const {
        call_record record = {
            { std::forward<mock_arg_log_t<Args>>(args)... },
            next_global_call_order(),
        };
        ctl->calls.push_back(std::move(record));
    }
    size_t next_global_call_order() const {
        return ctx ? ctx->callOrder++ : ctl->calls.size();
    }
    /**
     * Information about a call made to the mock
     */
    struct call_record {
        std::tuple<mock_arg_log_t<Args>...> args;
        size_t callOrder;
    };
    /**
     * Shared state of the mock wrappers
     */
    struct control_block {
        std::function<R(mock_arg_t<Args>...)> implementation;
        std::vector<call_record> calls;
    };
    std::shared_ptr<control_block> ctl;
    std::shared_ptr<mock_context::control_block> ctx;
};

}
