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
using namespace test_utility;
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

template <typename T, size_t N>
std::array<T, N> wrap_array(const T (&array)[N])
{
    std::array<T, N> result;
    std::copy(std::begin(array), std::end(array), std::begin(result));

    return result;
}

GTEST_TEST(UtilityTest, slice_implicit_convertion_to_BinaryData)
{
    // Verify that automatic convertion to BinaryData for slice<T>(t)
    // hash same effect as explicit slice(as_binary_data(t)).

    const unsigned char C_array[11] = {'S', 'a', 'm', 'p', 'l', 'e', ' ', 'd', 'a', 't', 'a'};
    const BinaryData binary_data = as_binary_data(C_array);
    ASSERT_EQ(sizeof(C_array), binary_data.len);

    const std::string string(reinterpret_cast<const char*>(C_array), sizeof(C_array));
    const char* C_string = string.c_str();
    const std::vector<unsigned char> vector(std::begin(C_array), std::end(C_array));
    const std::array<unsigned char, sizeof(C_array)> array = wrap_array(C_array);

    for (int offset = 0; offset < binary_data.len; ++offset)
    {
        SCOPED_TRACE(offset);

        for (int size = binary_data.len - offset - 1; size >= 0; --size)
        {
            SCOPED_TRACE(size);

            const BinaryData sliced_binary_data = slice(binary_data, offset, size);

            EXPECT_EQ(sliced_binary_data, slice(C_array, offset, size));
            EXPECT_EQ(sliced_binary_data, slice(C_string, offset, size));
            EXPECT_EQ(sliced_binary_data, slice(string, offset, size));
            EXPECT_EQ(sliced_binary_data, slice(vector, offset, size));
            EXPECT_EQ(sliced_binary_data, slice(array, offset, size));
        }
    }
}

GTEST_TEST(UtilityTest, power_slice_implicit_convertion_to_BinaryData)
{
    // Verify that automatic convertion to BinaryData for power_slice<T>(t)
    // hash same effect as explicit power_slice(as_binary_data(t)).

    const unsigned char C_array[11] = {'S', 'a', 'm', 'p', 'l', 'e', ' ', 'd', 'a', 't', 'a'};
    const BinaryData binary_data = as_binary_data(C_array);
    ASSERT_EQ(sizeof(C_array), binary_data.len);

    const std::string string(reinterpret_cast<const char*>(C_array), sizeof(C_array));
    const char* C_string = string.c_str();
    const std::vector<unsigned char> vector(std::begin(C_array), std::end(C_array));
    const std::array<unsigned char, sizeof(C_array)> array = wrap_array(C_array);

    for (int offset = 0; offset < binary_data.len; ++offset)
    {
        SCOPED_TRACE(offset);

        for (int size = binary_data.len - offset - 1; size >= 0; --size)
        {
            SCOPED_TRACE(size);

            const BinaryData sliced_binary_data = power_slice(binary_data, offset, size);

            EXPECT_EQ(sliced_binary_data, power_slice(C_array, offset, size));
            EXPECT_EQ(sliced_binary_data, power_slice(C_string, offset, size));
            EXPECT_EQ(sliced_binary_data, power_slice(string, offset, size));
            EXPECT_EQ(sliced_binary_data, power_slice(vector, offset, size));
            EXPECT_EQ(sliced_binary_data, power_slice(array, offset, size));
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

GTEST_TEST(UtilityTest, minify_json)
{
    ASSERT_EQ(minify_json("\t\n\v\f\r a"), "a");
    ASSERT_EQ(minify_json(R"( " \" " )"), R"(" \" ")");
    ASSERT_EQ(minify_json(R"( " a \\ a" \\ )"), R"(" a \\ a"\\)");
}

GTEST_TEST(UtilityTestInvalidArgs, parse_iso8601_string)
{
    using namespace multy_core::internal;
    EXPECT_THROW(parse_iso8601_string("2018-07-99T16:35:37Z"), Exception);
    EXPECT_THROW(parse_iso8601_string("2018-07-99T16:35:37:99999Z"), Exception);
    EXPECT_THROW(parse_iso8601_string("9999999999999999999999999Z"), Exception);
    EXPECT_THROW(parse_iso8601_string("9999999999999999999999999999Z"), Exception);
    EXPECT_THROW(parse_iso8601_string("--T::Z"), Exception);
    EXPECT_THROW(parse_iso8601_string(""), Exception);

    EXPECT_THROW(parse_iso8601_string("2018-99-19T16:35:37:99999Z"), Exception);
    EXPECT_THROW(parse_iso8601_string("0000-07-19T16:35:37:99999Z"), Exception);
}

struct Iso8601ParseTestCase
{
    const char* time_string;
    std::time_t time_value;
};

struct Iso8601ParseTestP : public ::testing::TestWithParam<Iso8601ParseTestCase>
{};

std::ostream& operator<<(std::ostream& ostr, const Iso8601ParseTestCase& test_case)
{
    return ostr << "Iso8601ParseTestCase{\n\t"
                << "\"" << test_case.time_string << "\",\n\t"
                << test_case.time_value << "\n"
                << "}";
}


TEST_P(Iso8601ParseTestP, parse_iso8601_string)
{
    using namespace multy_core::internal;

    const auto& p = GetParam();
    EXPECT_EQ(p.time_value, parse_iso8601_string(p.time_string));
}

INSTANTIATE_TEST_CASE_P(
        UtilityTest,
        Iso8601ParseTestP,
        ::testing::Values(
                Iso8601ParseTestCase{
                    "2018-07-19T16:35:37Z",
                    1532018137
                },
                Iso8601ParseTestCase{
                    "2002-09-14T13:28:20Z",
                    1032010100
                }
        ));

GTEST_TEST(UtilityTest, skip_leading_zeroes)
{
    const BinaryData null_binary_data{nullptr, 0};
    static const BinaryData zero_binary_data{reinterpret_cast<const unsigned char*>('Z'), 0};

    ASSERT_EQ(null_binary_data, skip_leading_zeroes(null_binary_data));
    ASSERT_EQ(null_binary_data, skip_leading_zeroes(zero_binary_data));
    ASSERT_EQ(null_binary_data, skip_leading_zeroes(as_binary_data({'\0'})));
    ASSERT_EQ(null_binary_data, skip_leading_zeroes(as_binary_data({'\0', '\0'})));
    ASSERT_EQ(as_binary_data({'t', 'e', 's', 't'}), skip_leading_zeroes(as_binary_data({'\0', '\0', 't', 'e', 's', 't'})));
    ASSERT_EQ(as_binary_data({'t', 'e', 's', 't', '\0'}), skip_leading_zeroes(as_binary_data({'\0', '\0', 't', 'e', 's', 't', '\0'})));
}
