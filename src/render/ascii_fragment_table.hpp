/**
 * @file ascii_fragment_table.hpp
 * 
 * Definitions of specific character sequences used
 * to pretty-print visualizations
 */

#pragma once

#include <iostream>

namespace dmalem {

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
     * Prints the first row of a state column, at the fixed state width
     * 
     * @param state Index of the state
     */
    virtual void shift_state_first_row(int state) const = 0;
    /**
     * Prints the second row of a state column, at the fixed state width
     * 
     * @param state Index of the state
     */
    virtual void shift_state_second_row(int state) const = 0;
    /**
     * Prints the first row of a pending reduce column, at the fixed state width
     */
    virtual void shift_reduce_first_row() const = 0;
    /**
     * Prints the second row of a pending reduce column, at the fixed state width
     */
    virtual void shift_reduce_second_row() const = 0;
    /**
     * Prints a row of a state column, at fixed state width
     */
    virtual void state() const = 0;
    /**
     * Prints a row of a pending reduce column, at fixed state width
     */
    virtual void pending_reduce() const = 0;
    /**
     * Prints the indicator of a non-empty reduce,
     * padded to the width of the left column (without margin)
     */
    virtual void pull_nonterminal() const = 0;
    /**
     * Prints the indicator of an empty reduce,
     * padded to the width of the left column (without margin)
     */
    virtual void conjure_nonterminal() const = 0;
    /**
     * Prints the last row of a state or pending reduce column
     * as it is being reduced
     */
    virtual void reduce_state() const = 0;
    /**
     * Prints the last row of a state or pending reduce column
     * as it is being reduced, and is the topmost state column
     */
    virtual void reduce_last_state() const = 0;
    /**
     * Prints the last row of a state or pending reduce column
     * as it is being discarded
     */
    virtual void discard_state() const = 0;
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
     * Prints the label that indicates the parser has exited successfully
     */
    virtual void accept_label() const = 0;
    /**
     * Prints the label that indicates the parser has exited with a failure
     */
    virtual void failure_label() const = 0;
    /**
     * Prints the label that indicates the parser has overflown its stack
     */
    virtual void overflow_label() const = 0;
    /**
     * Prints the label that indicates a syntax error
     */
    virtual void syntax_error_label() const = 0;
private:
    std::ostream* ostr;
};

}
