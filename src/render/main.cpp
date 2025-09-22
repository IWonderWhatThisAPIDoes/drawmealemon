/**
 * @file main.cpp
 * 
 * Entry point of the renderer module
 */

#include <memory>
#include <iostream>
#include "argument_parser.hpp"
#include "trace_parser.hpp"
#include "trace_patterns.hpp"
#include "default_target_factory.hpp"
#include "trace_action_sink.hpp"

using namespace dmalem;

int main(int argc, const char* const* argv) {
    auto args = argument_parser::parse(argc, argv);
    auto targetFactory = default_target_factory(std::cout);
    auto target = targetFactory.create_by_name(args.targetName, args.targetOptions);
    trace_parser<trace_action_sink>::parse(std::cin, trace_patterns::TABLE, std::cerr, *target);
    target->finalize();
}
