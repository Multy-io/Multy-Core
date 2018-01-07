/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license
 *
 * See LICENSE for details
 */

#include "multy_core/internal/utility.h"

#include "gtest/gtest.h"

#include <memory>

namespace
{
using namespace wallet_core::internal;

struct A
{
};

static const char* const TEST_VALUE1 = "test value1";
static const char* const TEST_VALUE2 = "test value2";
} // namespace

GTEST_TEST(UtilityTest, reset_sp)
{
    const char* old_value = nullptr;
    auto deleter_func =
            [&old_value](const char* to_reset) -> void
            {
                old_value = to_reset;
            };
    auto p_char = std::unique_ptr<const char, decltype(deleter_func)>(
            nullptr, deleter_func);

    auto update_value_func =
            [](const char* new_val, const char** val) -> void
            {
                *val = new_val;
            };

    update_value_func(TEST_VALUE1, reset_sp(p_char));
    EXPECT_EQ(nullptr, old_value);
    EXPECT_EQ(TEST_VALUE1, p_char.get());
    p_char.reset();
    EXPECT_EQ(TEST_VALUE1, old_value);

    update_value_func(TEST_VALUE2, reset_sp(p_char));
    EXPECT_EQ(TEST_VALUE1, old_value);
    EXPECT_EQ(TEST_VALUE2, p_char.get());

    p_char.reset();
    EXPECT_EQ(TEST_VALUE2, old_value);
}
