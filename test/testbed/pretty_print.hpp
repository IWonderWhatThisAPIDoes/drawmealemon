/**
 * @file pretty_print.hpp
 * 
 * Pretty-printing of values in test failure reports
 */

#pragma once

#include <concepts>
#include <sstream>
#include <iomanip>

namespace test {

/**
 * Wrapper for a value that can be inserted into a stream
 * to print the value in a test failure report
 */
template<class T>
class pretty_printer {
public:
    explicit pretty_printer(const T& t) noexcept : ref(t) {}
    const T& ref;
};

/**
 * Formats a value using the @ref pretty_printer wrapper
 * 
 * @param t The value to pretty-print
 * @return String that contains a pretty-print of the value
 * 
 * @example
 * ```
 * std::cout << pretty_print(42) << pretty_print("hello world");
 * ```
 */
template<class T>
std::string pretty_print(const T& t) noexcept {
    std::ostringstream ostr;
    ostr << pretty_printer<std::remove_reference_t<T>>(t);
    return ostr.str();
}

/**
 * Wrapper for a character value that prints it to a stream
 * in escaped form
 */
class esc {
public:
    esc(char c) : c(c) {}
    char c;
};

/**
 * Writes a character to a stream in escaped form
 * 
 * @param ostr The target stream
 * @param c The character to print
 * @return Reference to the target stream
 */
std::ostream& operator<<(std::ostream& ostr, esc c);

/**
 * Wrapper for a byte sequence that prints it to a stream
 */
class bytes {
public:
    bytes(const unsigned char* ptr, size_t length) : ptr(ptr), length(length) {}
    const unsigned char* ptr;
    size_t length;
};

/**
 * Pretty-prints a byte array to a stream
 * 
 * @param ostr The target stream
 * @param b The byte array to print
 * @return Reference to the target stream
 */
std::ostream& operator<<(std::ostream& ostr, bytes b);

/**
 * Types that can be inserted into a stream
 */
template<class T>
concept streamable = requires(std::ostream& s, const T& t) {
    { s << t } -> std::same_as<std::ostream&>;
};

/**
 * Types that can be viewed as a string
 */
template<class T>
concept stringlike = std::convertible_to<const T&, std::string_view>;

/**
 * Pretty-prints a string view into a stream in escaped form
 * 
 * @param ostr The target stream
 * @param s The string to print
 * @return Reference to the target stream
 */
std::ostream& operator<<(std::ostream& ostr, const pretty_printer<std::string_view>& s);

/**
 * Pretty-prints a character into a stream in escaped form
 * 
 * @param ostr The target stream
 * @param c The character to print
 * @return Reference to the target stream
 */
std::ostream& operator<<(std::ostream& ostr, const pretty_printer<char>& c);

/**
 * Pretty-prints a character into a stream in escaped form
 * 
 * @param ostr The target stream
 * @param c The character to print
 * @return Reference to the target stream
 */
std::ostream& operator<<(std::ostream& ostr, const pretty_printer<unsigned char>& c);

// If no better option is available, print the object's raw bytes
template<class T>
std::ostream& operator<<(std::ostream& ostr, const pretty_printer<T>& t) {
    return ostr << bytes(reinterpret_cast<const unsigned char*>(&t.ref), sizeof(T));
}

// Everything that converts to a string must be escaped
template<stringlike T>
std::ostream& operator<<(std::ostream& ostr, const pretty_printer<T>& t) {
    return ostr << pretty_print(std::string_view(t.ref));
}

// Everything that can be inserted into a stream
// can be pretty-printed without change (except the strings, see above)
template<streamable T>
requires(!stringlike<T>)
std::ostream& operator<<(std::ostream& ostr, const pretty_printer<T>& t) {
    return ostr << t.ref;
}

// Enumerations can be pretty-printed as their underlying numbers
template<class T>
requires(!streamable<T> && std::is_enum_v<T>)
std::ostream& operator<<(std::ostream& ostr, const pretty_printer<T>& t) {
    return ostr << static_cast<std::underlying_type_t<T>>(t.ref);
}

}
