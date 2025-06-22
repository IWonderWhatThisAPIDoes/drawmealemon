/**
 * @file argument_parser.cpp
 * 
 * Parsing of the command line
 */

#include "argument_parser.hpp"

using namespace std::string_literals;

namespace dmalem {

argument_parser::error::error(error_code code, const char* argument) :
    logic_error("Invalid command line arguments: " + make_message(code, argument)),
    errcode(code),
    offendingArgument(argument)
{}

argument_parser::error_code argument_parser::error::code() const noexcept {
    return errcode;
}

const char* argument_parser::error::argument() const noexcept {
    return offendingArgument.c_str();
}

std::string argument_parser::error::make_message(error_code code, const char* argument) {
    switch (code) {
        case error_code::unknown_flag:
            return "Unknown flag: "s + argument;
        case error_code::missing_argument:
            return "Expected argument after "s + argument;
        case error_code::missing_flag:
            return "Expected flag, got "s + argument;
        case error_code::duplicate_flag:
            return "Duplicate or contradicting arguments: "s + argument;
        default:
            throw std::invalid_argument(__FUNCTION__);
    }
}

argument_parser::output argument_parser::parse(size_t argc, const char* const * argv) {
    output o;
    bool gotTarget = false;

    for (size_t i = 1; i < argc; ++i) {
        // This argument must be a flag
        if (argv[i][0] != '-')
            throw error(error_code::missing_flag, argv[i]);
        
        // Match the argument against flags we know
        switch (argv[i][1]) {
            // -t: target name
            case 't': {
                // This can only be set once, fail if the flag shows up again
                if (gotTarget)
                    throw error(error_code::duplicate_flag, argv[i]);
                gotTarget = true;
                // The actual value can be in this argument or in the next one
                // '-t target' and '-ttarget' are both valid
                if (argv[i][2] == '\0') {
                    if (++i >= argc || argv[i][0] == '-')
                        throw error(error_code::missing_argument, argv[i - 1]);
                    o.targetName = argv[i];
                } else {
                    o.targetName = argv[i] + 2;
                }
                break;
            }
            // -o: target option
            case 'o': {
                // The actual value can be in this argument or in the next one
                // '-o option' and '-ooption' are both valid
                if (argv[i][2] == '\0') {
                    if (++i >= argc || argv[i][0] == '-')
                        throw error(error_code::missing_argument, argv[i - 1]);
                    o.targetOptions.push_back(argv[i]);
                } else {
                    o.targetOptions.push_back(argv[i] + 2);
                }
                break;
            }
            // Everything else is an invalid flag
            default: throw error(error_code::unknown_flag, argv[i]);
        }
    }

    return o;
}

}
