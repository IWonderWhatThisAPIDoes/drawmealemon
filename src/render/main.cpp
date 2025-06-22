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
#include "target_factory.hpp"
#include "trace_action_sink.hpp"

using namespace dmalem;

int main(int argc, const char* const* argv) {
    auto args = argument_parser::parse(argc, argv);
    auto target = target_factory::create_by_name(args.targetName.c_str());
    target->set_options(args.targetOptions);
    trace_parser<trace_action_sink>::parse(std::cin, trace_patterns::TABLE, std::cerr, *target);
    target->finalize();
}
