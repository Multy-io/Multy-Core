/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license
 *
 * See LICENSE for details
 */

#include "multy_core/common.h"

#include "multy_core/src/u_ptr.h"

#include "multy_test/utility.h"

#include "gtest/gtest.h"

#include <memory>
#include <stdio.h>

namespace
{
using namespace test_utility;
using namespace multy_core::internal;

struct HexTestCase
{
    const char* hex_string;
    bytes hex_data;
};

const HexTestCase TestCases[] =
{
    {
        "",
        {}
    },
    {
        "01",
        {0x01}
    },
    {
        "ffffffff",
        {0xff, 0xff, 0xff, 0xff}
    },
    {
        "010203040506070809A0B0C0D0E0F0",
        {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
         0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xF0}
    },
    {
        "0x00",
        {0x00}
    }
};

const char* InvalidHexTestCases[] = {
    "q",
    "-x",
    "1.1",
};

class BinaryDataTestValidHex : public ::testing::TestWithParam<HexTestCase>
{};

class BinaryDataTestInvalidHex : public ::testing::TestWithParam<const char*>
{};

TEST_P(BinaryDataTestValidHex, make_binary_data_from_hex)
{
    const HexTestCase& param = GetParam();

    BinaryDataPtr result;
    HANDLE_ERROR(make_binary_data_from_hex(param.hex_string, reset_sp(result)));
    ASSERT_NE(nullptr, result);

    ASSERT_EQ(as_binary_data(param.hex_data), *result);
}

TEST_P(BinaryDataTestValidHex, make_binary_data_from_bytes)
{
    const HexTestCase& param = GetParam();

    BinaryDataPtr result;
    HANDLE_ERROR(make_binary_data_from_bytes(
            param.hex_data.data(), param.hex_data.size(), reset_sp(result)));
    ASSERT_NE(nullptr, result);

    ASSERT_EQ(as_binary_data(param.hex_data), *result);
}

TEST_P(BinaryDataTestInvalidHex, make_binary_data_from_hex)
{
    const char* param = GetParam();

    BinaryDataPtr result;
    EXPECT_ERROR(make_binary_data_from_hex(param, reset_sp(result)));
    EXPECT_EQ(nullptr, result);
}

INSTANTIATE_TEST_CASE_P(Hex, BinaryDataTestValidHex,
        ::testing::ValuesIn(TestCases));

INSTANTIATE_TEST_CASE_P(Error, BinaryDataTestInvalidHex,
        ::testing::ValuesIn(InvalidHexTestCases));

GTEST_TEST(VersionTest, get_version)
{
    Version version;
    HANDLE_ERROR(get_version(&version));

    ASSERT_NE(nullptr, version.commit);
    ASSERT_NE(nullptr, version.note);

    // Builds in version branch should be properly versioned.
    uint32_t major = 0;
    uint32_t minor = 0;
    const int result = std::sscanf("version.commit", "v%u.%u", &major, &minor);
    if (result != 0 && result != EOF)
    {
        ASSERT_NE(0, version.major + version.minor + version.build);
        ASSERT_EQ(major, version.major);
        ASSERT_EQ(minor, version.minor);
    }
}

GTEST_TEST(VersionTest, make_version_string)
{
    ConstCharPtr version_string;
    HANDLE_ERROR(make_version_string(reset_sp(version_string)));

    ASSERT_NE(nullptr, version_string);
    std::cout << "multy_core version:" << version_string.get() << std::endl;
}

GTEST_TEST(VersionTestInvalidArgs, get_version)
{
    EXPECT_ERROR(get_version(nullptr));
}

GTEST_TEST(VersionTestInvalidArgs, make_version_string)
{
    EXPECT_ERROR(make_version_string(nullptr));
}

} // namespace
