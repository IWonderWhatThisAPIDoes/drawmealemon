/**
 * @file main.cpp
 * 
 * Entry point of the renderer module
 */

#include <iostream>
#include "argument_parser.hpp"
#include "default_target_factory.hpp"
#include "default_trace_parser.hpp"

using namespace dmalem;

int main(int argc, const char* const* argv) {
    auto args = argument_parser::parse(argc, argv);
    auto targetFactory = default_target_factory(std::cout);
    auto target = targetFactory.create_by_name(args.targetName, args.targetOptions);
    default_trace_parser().log_to(std::cerr).set_target(*target).parse(std::cin);
    target->finalize();
}
