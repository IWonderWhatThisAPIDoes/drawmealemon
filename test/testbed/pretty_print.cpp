/**
 * @file pretty_print.cpp
 * 
 * Pretty-printing of values in test failure reports
 */

#include "pretty_print.hpp"

namespace test {

std::ostream& operator<<(std::ostream& ostr, esc c) {
    switch (c.c) {
        case '\\': {
            ostr << "\\\\";
            break;
        }
        case '"': {
            ostr << "\\\"";
            break;
        }
        case '\'': {
            ostr << "\\'";
            break;
        }
        case '\n': {
            ostr << "\\n";
            break;
        }
        case '\t': {
            ostr << "\\t";
            break;
        }
        case '\r': {
            ostr << "\\r";
            break;
        }
        case '\b': {
            ostr << "\\b";
            break;
        }
        case '\a': {
            ostr << "\\a";
            break;
        }
        case '\v': {
            ostr << "\\v";
            break;
        }
        case '\f': {
            ostr << "\\f";
            break;
        }
        default: {
            if (isprint(c.c) && isascii(c.c)) {
                ostr << c.c;
            } else {
                auto flags = ostr.flags();
                ostr << std::oct << '\\' << static_cast<unsigned>(static_cast<unsigned char>(c.c));
                ostr.flags(flags);
            }
            break;
        }
    }

    return ostr;
}

std::ostream& operator<<(std::ostream& ostr, bytes b) {
    // Save the stream's format
    auto flags = ostr.flags();
    auto fill = ostr.fill();

    ostr << '<' << std::hex << std::setfill('0');
    for (size_t i = 0; i < b.length; ++i) {
        if (i != 0)
            ostr << ' ';
        ostr << std::setw(2) << static_cast<unsigned>(b.ptr[i]);
    }
    ostr << '>';

    // Restore the stream's format
    ostr.flags(flags);
    ostr.fill(fill);

    return ostr;
}

std::ostream& operator<<(std::ostream& ostr, const pretty_printer<std::string_view>& s) {
    ostr << '"';
    for (char c : s.ref) {
        ostr << esc(c);
    }
    return ostr << '"';
}

std::ostream& operator<<(std::ostream& ostr, const pretty_printer<char>& c) {
    return ostr << '\'' << esc(c.ref) << "' (" << static_cast<int>(c.ref) << ')';
}

std::ostream& operator<<(std::ostream& ostr, const pretty_printer<unsigned char>& c) {
    return ostr << '\'' << esc(static_cast<char>(c.ref)) << "' (" << static_cast<unsigned>(c.ref) << ')';
}

}
