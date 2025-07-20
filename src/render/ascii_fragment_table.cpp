/**
 * @file ascii_fragment_table.cpp
 * 
 * Definitions of specific character sequences used
 * to pretty-print visualizations
 */

#include <stdexcept>
#include "ascii_fragment_table.hpp"

namespace dmalem {

state_fragment_data::row_kind state_fragment_data::termination_row_kind(parser_termination_cause cause) {
    switch (cause) {
        case parser_termination_cause::accept:
            return row_kind::accept;
        case parser_termination_cause::failure:
            return row_kind::failure;
        case parser_termination_cause::stack_overflow:
            return row_kind::stack_overflow;
        default:
            throw std::logic_error(__FUNCTION__);
    }
}

ascii_fragment_table::ascii_fragment_table() : ostr(nullptr) {}

ascii_fragment_table::ascii_fragment_table(std::ostream& ostr) : ostr(&ostr) {}

void ascii_fragment_table::stream(std::ostream& ostr) noexcept {
    this->ostr = &ostr;
}

std::ostream& ascii_fragment_table::stream() const {
    if (!ostr)
        throw std::logic_error(__FUNCTION__);
    return *ostr;
}

}
