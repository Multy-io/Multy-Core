/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license
 *
 * See LICENSE for details
 */

#include "multy_core/src/utility.h"
#include "multy_core/src/exception.h"

#include "multy_test/value_printers.h"
#include "multy_test/utility.h"

#include "gtest/gtest.h"

#include <memory>

namespace
{
using namespace multy_core::internal;

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

GTEST_TEST(UtilityTest, slice)
{
    const BinaryData data = as_binary_data("Sample BinaryData");

    const BinaryData sliced = slice(data, 0, 0);
    EXPECT_EQ(0, sliced.len);
    EXPECT_EQ(data.data, sliced.data);

    for (int offset = 0; offset < data.len; ++offset)
    {
        SCOPED_TRACE(offset);

        for (int size = data.len - offset - 1; size >= 0; --size)
        {
            SCOPED_TRACE(size);

            const BinaryData sliced = slice(data, offset, size);
            EXPECT_EQ(size, sliced.len);
            EXPECT_EQ(data.data + offset, sliced.data);
        }
    }
}

GTEST_TEST(UtilityInvalidArgsTest, slice)
{
    const BinaryData data = as_binary_data("Sample BinaryData");
    const int32_t TOO_BIG = data.len + 1;

    EXPECT_THROW(slice(data, 0, TOO_BIG), Exception);
    EXPECT_THROW(slice(data, TOO_BIG, 0), Exception);
    EXPECT_THROW(slice(data, TOO_BIG, TOO_BIG), Exception);
}

GTEST_TEST(UtilityTest, power_slice)
{
    const BinaryData data = as_binary_data("Sample BinaryData");

    const BinaryData sliced = power_slice(data, 0, 0);
    EXPECT_EQ(0, sliced.len);
    EXPECT_EQ(data.data, sliced.data);

    for (int offset = 0; offset < data.len; ++offset)
    {
        SCOPED_TRACE(offset);

        for (int size = data.len - offset - 1; size >= 0; --size)
        {
            SCOPED_TRACE(size);

            const BinaryData sliced = slice(data, offset, size);
            EXPECT_EQ(size, sliced.len);
            EXPECT_EQ(data.data + offset, sliced.data);
        }
    }
}

GTEST_TEST(UtilityInvalidArgsTest, power_slice)
{
    const BinaryData data = as_binary_data("Sample BinaryData");
    const int32_t TOO_BIG = data.len + 1;
    const int32_t TOO_SMALL = -TOO_BIG;

    EXPECT_THROW(power_slice(data, 0, TOO_BIG), Exception);
    EXPECT_THROW(power_slice(data, TOO_BIG, 0), Exception);
    EXPECT_THROW(power_slice(data, TOO_BIG, TOO_BIG), Exception);

    EXPECT_THROW(power_slice(data, 0, TOO_SMALL), Exception);
    EXPECT_THROW(power_slice(data, TOO_SMALL, 0), Exception);
    EXPECT_THROW(power_slice(data, TOO_SMALL, TOO_SMALL), Exception);
}
