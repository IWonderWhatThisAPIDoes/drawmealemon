/**
 * @file target_factory.cpp
 * 
 * Tests for the @ref target_factory class
 */

#include "../testbed/test.hpp"
#include "../../src/render/target_factory.hpp"
#include "mock_target_factory_module.hpp"
#include "mock_render_target.hpp"

using dmalem::target_factory;
using dmalem::mock_target_factory_module;
using dmalem::mock_render_target;
using dmalem::render_target;

TEST(empty_factory_always_throws) {
    const target_factory factory;
    TEST_ASSERT_THROW(factory.create_by_name("", {}), target_factory::bad_render_target_name);
}

TEST(adding_name_twice_fails) {
    target_factory factory;
    auto mod1 = std::make_unique<mock_target_factory_module>();
    auto mod2 = std::make_unique<mock_target_factory_module>();
    factory.add_module("name", std::move(mod1));
    TEST_ASSERT_THROW(factory.add_module("name", std::move(mod2)), std::invalid_argument);
}

TEST(unknown_name_fails) {
    target_factory factory;
    auto mod = new mock_target_factory_module();
    factory.add_module("name", std::unique_ptr<mock_target_factory_module>(mod));
    TEST_ASSERT_THROW(factory.create_by_name("not-a-module", {}), target_factory::bad_render_target_name);
    TEST_ASSERT_EQ(mod->createRenderTarget.call_count(), 0);
    TEST_ASSERT_EQ(mod->setOptions.call_count(), 0);
}

TEST(registered_module_is_called_by_name) {
    target_factory factory;
    auto mod = new mock_target_factory_module();
    auto target = new mock_render_target();
    mod->createRenderTarget = test::mock<std::unique_ptr<render_target>()>(
        [=] { return std::unique_ptr<render_target>(target); },
        mod->context
    );
    factory.add_module("name", std::unique_ptr<mock_target_factory_module>(mod));
    auto returnedTarget = factory.create_by_name("name", {});
    TEST_ASSERT_EQ(mod->setOptions.call_count(), 1);
    TEST_ASSERT_EQ(mod->createRenderTarget.call_count(), 1);
    TEST_ASSERT_EQ(mod->setOptions.args(), std::make_tuple(std::vector<std::string>()));
    TEST_ASSERT_EQ(returnedTarget.get(), target);
}

TEST(arguments_are_passed_to_module) {
    target_factory factory;
    auto mod = new mock_target_factory_module();
    auto target = new mock_render_target();
    mod->createRenderTarget = test::mock<std::unique_ptr<render_target>()>(
        [=] { return std::unique_ptr<render_target>(target); },
        mod->context
    );
    factory.add_module("name", std::unique_ptr<mock_target_factory_module>(mod));
    auto returnedTarget = factory.create_by_name("name", {"hello", "world"});
    TEST_ASSERT_EQ(mod->setOptions.call_count(), 1);
    TEST_ASSERT_EQ(mod->createRenderTarget.call_count(), 1);
    TEST_ASSERT_LT(mod->setOptions.global_call_order(), mod->createRenderTarget.global_call_order());
    TEST_ASSERT_EQ(mod->setOptions.args(), std::make_tuple(std::vector<std::string>({"hello", "world"})));
    TEST_ASSERT_EQ(returnedTarget.get(), target);
}
