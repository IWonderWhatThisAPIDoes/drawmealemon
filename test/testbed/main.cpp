/**
 * @file main.cpp
 * 
 * Entry point of the test runner
 */

#include <iostream>
#include <cstdlib>
#include "suite.hpp"

int main() {
    if (test::has_tests()) {
        return test::run_tests() ? EXIT_SUCCESS : EXIT_FAILURE;
    } else {
        std::cout << "\33[0;31mNo test cases found\33[0m" << std::endl;
        return EXIT_FAILURE;
    }
}
