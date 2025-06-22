/**
 * @file suite.cpp
 * 
 * Test suite organization
 */

#include <iostream>
#include <string>
#include <typeinfo>
#include "suite.hpp"
#include "failure.hpp"

using namespace std::string_literals;

namespace test {

bool has_tests() {
    return !testSuites.empty();
}

bool run_tests() {
    int suitesRun = 0, suitesPassed = 0, casesRun = 0, casesPassed = 0;
    std::cout << "Running all tests...\n";

    for (const auto& [suiteName, cases] : testSuites) {
        int currentCasesRun = 0, currentCasesPassed = 0;
        std::cout << "\n===[ " << suiteName << " (" << cases.size()
            << " test" << (cases.size() == 1 ? "" : "s") << ") ]===\n";

        for (const auto& [caseName, fun] : cases) {
            std::cout << caseName << ": ";
            bool succeeded = false;
            std::string note;

            try {
                fun();
                succeeded = true;
            } catch (const test_failure& e) {
                note = e.message();
            } catch (const std::exception& e) {
                note = "Test threw an exception of type "s + typeid(e).name() + " [what: " + e.what() + ']';
            } catch (...) {
                note = "Test threw a non-exception object";
            }

            if (succeeded) {
                std::cout << "\33[0;32mPASS\33[0m" << std::endl;
                ++currentCasesPassed;
            } else {
                std::cout << "\33[0;31mFAIL\33[0m\n" << note << std::endl;
            }
            ++currentCasesRun;
        }

        casesRun += currentCasesRun;
        casesPassed += currentCasesPassed;
        ++suitesRun;
        if (currentCasesRun == currentCasesPassed)
            ++suitesPassed;
    }

    const bool testSuccess = casesRun == casesPassed;

    std::cout << "\n===[ RESULTS ]===\nTest result: "
        << (testSuccess ? "\33[0;32mSUCCESS\33[0m\n" : "\33[0;31mFAILURE\33[0m\n")
        << "Passed " << suitesPassed << '/' << suitesRun << " test suites, "
        << casesPassed << '/' << casesRun << " test cases" << std::endl;

    return testSuccess;
}

}
