/**
 * @file ascii_target_factory_module.cpp
 * 
 * Tests for the @ref ascii_target_factory_module class
 */

#include <sstream>
#include "../testbed/test.hpp"
#include "../../src/render/ascii_target_factory_module.hpp"
#include "../../src/render/ascii_target.hpp"

using dmalem::target_factory_module;
using dmalem::ascii_target_factory_module;
using dmalem::ascii_target;

TEST(target_is_ascii) {
    std::ostringstream ostr;
    ascii_target_factory_module factory(ostr);
    auto target = factory.create_render_target();
    dynamic_cast<ascii_target&>(*target);
}

TEST(target_renders_to_provided_stream) {
    std::ostringstream ostr;
    ascii_target_factory_module factory(ostr);
    auto target = factory.create_render_target();
    target->input_token("Hello");
    target->shift(4);
    target->accept();
    target->finalize();
    TEST_ASSERT_NE(ostr.str().find("Hello"), std::string::npos);
    TEST_ASSERT_NE(ostr.str().find("4"), std::string::npos);
    TEST_ASSERT_NE(ostr.str().find("Accept"), std::string::npos);
}

TEST(input_column_width_is_forwarded_to_target) {
    std::ostringstream ostr;
    ascii_target_factory_module factory(ostr);
    factory.set_options({"iw=42"});
    auto target = factory.create_render_target();
    target->input_token("Hello");
    target->finalize();
    const size_t firstEndl = ostr.str().find('\n');
    TEST_ASSERT_NE_(firstEndl, std::string::npos, "At least one full line should be in the output");
    TEST_ASSERT_GT_(firstEndl, 42, "Output should be at least as wide as its left column");
}

TEST(input_width_needs_value) {
    std::ostringstream ostr;
    ascii_target_factory_module factory(ostr);
    TEST_ASSERT_THROW(factory.set_options({"iw="}), target_factory_module::bad_render_target_options);
}

TEST(cannot_set_input_column_width_to_non_numeric) {
    std::ostringstream ostr;
    ascii_target_factory_module factory(ostr);
    TEST_ASSERT_THROW(factory.set_options({"iw=hello"}), target_factory_module::bad_render_target_options);
}

TEST(cannot_set_input_column_width_to_zero) {
    std::ostringstream ostr;
    ascii_target_factory_module factory(ostr);
    TEST_ASSERT_THROW(factory.set_options({"iw=0"}), target_factory_module::bad_render_target_options);
}

TEST(cannot_set_input_column_width_twice) {
    std::ostringstream ostr;
    ascii_target_factory_module factory(ostr);
    TEST_ASSERT_THROW(factory.set_options({"iw=10", "iw=12"}), target_factory_module::bad_render_target_options);
}

TEST(throws_on_unknown_option) {
    std::ostringstream ostr;
    ascii_target_factory_module factory(ostr);
    TEST_ASSERT_THROW(factory.set_options({"not-an-option"}), target_factory_module::bad_render_target_options);
}
