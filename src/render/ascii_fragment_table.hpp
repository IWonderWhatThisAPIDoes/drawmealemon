/**
 * @file ascii_fragment_table.hpp
 * 
 * Definitions of specific character sequences used
 * to pretty-print visualizations
 */

#pragma once

#include <iostream>
#include <optional>
#include <variant>

namespace dmalem {

/**
 * Enumerates the possible end states of the parser
 */
enum class parser_termination_cause {
    /**
     * Input has been parsed successfully
     */
    accept,
    /**
     * Failure due to an irrecoverable syntax error
     */
    failure,
    /**
     * Failure due to parser stack overflow
     */
    stack_overflow,
};

/**
 * Describes a fragment of a state column and the context
 * in which it appears
 * 
 * Can be mapped to an ASCII visualization fragment
 */
struct state_fragment_data {
    /**
     * Enumerates the kinds of rows of the visualization
     * that are relevant to presentation of the state columns
     */
    enum class row_kind {
        /**
         * Row where no updates take place
         */
        neutral,
        /**
         * Row where a new state or pending reduce is shifted
         */
        shift,
        /**
         * Row where a rule is reduces
         */
        reduce,
        /**
         * Row where one or more states are discarded
         */
        discard,
        /**
         * Row where the stack is cleared out because the parser
         * has terminated successfully
         */
        accept,
        /**
         * Row where the stack is cleared out because the parser has failed
         */
        failure,
        /**
         * Row where the stack is cleared out because it has overflown
         */
        stack_overflow,
    };
    /**
     * Index of the state represented by the particular column.
     * Empty if the column corresponds to a pending reduce instaed
     */
    std::optional<int> state;
    /**
     * Index of the current line, measured from the start of the state column
     */
    size_t line;
    /**
     * How many state columns there are in total
     */
    size_t columnCount;
    /**
     * Zero-based index of the current state column
     */
    size_t columnIndex;
    /**
     * What kind of change is made to the stack in the current row
     */
    row_kind rowKind;
    /**
     * If @ref rowKind is either @ref row_kind::neutral @ref row_kind::shift,
     * this is zero, otherwise it is how many states will be removed by the update
     */
    size_t popCount;
    /**
     * Constructs a @ref row_kind corresponding to a terminating row
     * with termination due to a given cause
     * 
     * @param cause The cause for termination
     * @return Row kind of the termination row
     * @throw std::logic_error @p cause is a trap value
     */
    static row_kind termination_row_kind(parser_termination_cause cause);
};

/**
 * Provides methods for pasting template character sequences
 * that comprise an ASCII visualization
 * 
 * The ASCII visualization can look (roughly, depending
 * on the implementation of this class) as follows.
 * The template consists of several columns that contain
 * specific aspects of the visualization.
 * 
 * ```text
 *       INPUT  |  STACK
 *              |
 * [token]      | #
 *  |           | # #
 *  | [nonterm] | # #
 *  |           | #
 * 
 * |-|<------->|-|<--->|
 *  ^     |     ^   |
 *  |     |     |   +------ Right column (parser stack), split into fixed-width states
 *  |     |     +---------- Column separator
 *  |     +---------------- Left column (input buffer)
 *  +---------------------- Left margin (decoration for input buffer)
 * ```
 */
class ascii_fragment_table {
public:
    /**
     * Constructs a fragment table with no target
     */
    ascii_fragment_table();
    /**
     * Constructs a fragment table that renders into a stream
     * 
     * @param ostr The target stream. Must live for as long as this
     *             object receives any input
     */
    explicit ascii_fragment_table(std::ostream& ostr);
    virtual ~ascii_fragment_table() {}

    /**
     * Sets width of the input column of the visualization
     * 
     * Defaults to the minimum value supported by the implementation
     * 
     * @param width Width of the input column. Must be at least
     *              @ref min_input_column_width. Smaller values will be
     *              clamped up
     */
    void input_column_width(size_t width) noexcept;
    /**
     * Retrieves the width of the input column
     * 
     * Caller must clamp the value in case it is too small
     * 
     * @return Width of the input column
     */
    size_t input_column_width() const noexcept;
    /**
     * Sets the target stream that the outputs will be rendered into
     * 
     * @param ostr The target stream. Must live for as long as this
     *             object receives any input
     */
    void stream(std::ostream& ostr) noexcept;
    /**
     * Retrieves the target stream
     * 
     * @return The target stream
     * @throw std::logic_error The target stream has not been set
     */
    std::ostream& stream() const;
    /**
     * Prints the column separator into a stream
     */
    virtual void column_separator() const = 0;
    /**
     * Prints the header of the left column, padded
     * to the width of the left column with margin
     */
    virtual void left_column_head() const = 0;
    /**
     * Prints the header of the right column
     */
    virtual void right_column_head() const = 0;
    /**
     * Prints a row of a state column, at fixed state width
     * 
     * @param data Description of the particular state fragment to render
     */
    virtual void state(const state_fragment_data& data) const = 0;
    /**
     * Prints the indicator of a reduce,
     * padded to the width of the left column (without margin)
     * 
     * @param reduceCount How many states have been reduced from the stack
     */
    virtual void pull_nonterminal(size_t reduceCount) const = 0;
    /**
     * Prints the row where a new input token is read,
     * padded to the width of the left column with margin
     * 
     * @param name Name of the token
     */
    virtual void bring_token(const std::string_view& name) const = 0;
    /**
     * Prints the indicator of an input token being shifted,
     * padded to the width of the left column with margin
     */
    virtual void shift_token() const = 0;
    /**
     * Prints the indicator of a pending nonterminal being shifted,
     * padded to the width of the left column (without margin)
     */
    virtual void shift_nonterminal() const = 0;
    /**
     * Prints the indicator of an input token being discarded,
     * padded to the width of the left margin
     */
    virtual void discard_token() const = 0;
    /**
     * Prints the indicator of an input token is pending,
     * padded to the width of the left margin
     */
    virtual void pending_token() const = 0;
    /**
     * Prints whitespace that fills the width of the left margin
     */
    virtual void empty_left_margin() const = 0;
    /**
     * Prints a line terminator to the output
     */
    virtual void endl() const = 0;
    /**
     * Prints whitespace that fills the width of the left column (without margin)
     */
    virtual void empty_left_column() const = 0;
    /**
     * Prints the row where a new nonterminal is created,
     * padded to the width of the left column (without margin)
     * 
     * @param name Name of the nonterminal
     */
    virtual void nonterminal_name(const std::string_view& name) const = 0;
    /**
     * Prints an indicator of a rule that has been reduced
     * 
     * @param name Content of the rule
     */
    virtual void reduce_rule_label(const std::string_view& rule) const = 0;
    /**
     * Prints a label that indicates the parser has exited
     * 
     * @param cause Indicates the reason for the parser exiting
     */
    virtual void termination_label(parser_termination_cause cause) const = 0;
    /**
     * Prints the label that indicates a syntax error
     */
    virtual void syntax_error_label() const = 0;
private:
    std::ostream* ostr;
    size_t inputWidth = 0;
};

}
