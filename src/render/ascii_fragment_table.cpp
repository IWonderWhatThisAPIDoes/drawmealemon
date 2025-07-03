/**
 * @file ascii_fragment_table.cpp
 * 
 * Definitions of specific character sequences used
 * to pretty-print visualizations
 */

#include <stdexcept>
#include "ascii_fragment_table.hpp"

namespace dmalem {

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
