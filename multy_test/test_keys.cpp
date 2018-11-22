/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license
 *
 * See LICENSE for details
 */

#include "multy_core/common.h"
#include "multy_core/src/api/key_impl.h"
#include "multy_core/key.h"

#include "multy_test/bip39_test_cases.h"
#include "multy_test/utility.h"
#include "multy_test/value_printers.h"

#include "gtest/gtest.h"

#include <memory>
#include <string>
#include <unordered_set>

template <typename S>
S& operator<<(S& s, const BinaryData& binary_data)
{
    PrintTo(binary_data, &s);
    return s;
}

namespace
{
using namespace multy_core::internal;
using namespace test_utility;

class KeysTestValidCasesP : public ::testing::TestWithParam<BIP39TestCase>
{
};

INSTANTIATE_TEST_CASE_P(
        BIP39,
        KeysTestValidCasesP,
        ::testing::ValuesIn(BIP39_DEFAULT_TEST_CASES));

TEST_P(KeysTestValidCasesP, Test)
{
    const BIP39TestCase& param = GetParam();
    const bytes seed_data = from_hex(param.seed);
    const BinaryData seed{seed_data.data(), seed_data.size()};

    ExtendedKeyPtr key;
    ConstCharPtr key_string;

    HANDLE_ERROR(make_master_key(&seed, reset_sp(key)));
    ASSERT_NE(nullptr, key);

    HANDLE_ERROR(extended_key_to_string(key.get(), reset_sp(key_string)));
    EXPECT_STREQ(param.root_key, key_string.get());
}

GTEST_TEST(KeysTestInvalidArgs, make_master_key)
{
    const unsigned char data_vals[] = {1U, 2U, 3U, 4U};
    const BinaryData data{data_vals, 3};

    ExtendedKeyPtr key;

    EXPECT_ERROR(make_master_key(nullptr, reset_sp(key)));
    EXPECT_EQ(nullptr, key);

    EXPECT_ERROR(make_master_key(&data, nullptr));

    // Even though all arguments are present, data is still invalid
    EXPECT_ERROR(make_master_key(&data, reset_sp(key)));
    EXPECT_EQ(nullptr, key);
}

GTEST_TEST(KeysTestInvalidArgs, make_child_key)
{
    const ExtendedKey parent_key = make_dummy_extended_key();

    ExtendedKeyPtr child_key;
    EXPECT_ERROR(make_child_key(nullptr, 0, reset_sp(child_key)));
    EXPECT_EQ(nullptr, child_key);

    EXPECT_ERROR(make_child_key(&parent_key, 0, nullptr));
}

GTEST_TEST(KeysTestInvalidArgs, make_user_id_from_master_key)
{
    const ExtendedKey root_key = make_dummy_extended_key();

    ConstCharPtr user_id;
    EXPECT_ERROR(make_user_id_from_master_key(nullptr, reset_sp(user_id)));
    EXPECT_ERROR(make_user_id_from_master_key(&root_key, nullptr));

    ExtendedKeyPtr derived_key;
    HANDLE_ERROR(make_child_key(&root_key, 0, reset_sp(derived_key)));

    // Using non-master key is an error.
    EXPECT_ERROR(make_user_id_from_master_key(derived_key.get(),
            reset_sp(user_id)));
}

std::string user_id_from_seed(const bytes& data)
{
    const BinaryData seed{data.data(), data.size()};

    ErrorPtr error;
    ExtendedKeyPtr key;
    ConstCharPtr key_string;

    error.reset(make_master_key(&seed, reset_sp(key)));
    EXPECT_EQ(nullptr, error);
    EXPECT_NE(nullptr, key);

    ConstCharPtr user_id;
    error.reset(make_user_id_from_master_key(key.get(), reset_sp(user_id)));
    EXPECT_EQ(nullptr, error);

    return std::string(user_id ? user_id.get() : "");
}

GTEST_TEST(KeysTest, KeyId)
{
    const bytes seed_data = from_hex("00000000000000000000000000000000");
    const std::string reference_id = user_id_from_seed(seed_data);
    ASSERT_NE("", reference_id);

    std::unordered_set<std::string> ids;
    ids.insert(reference_id);

    // Verify that changing 1 bit in seed produces new key_id
    for (int byte_index = 0; byte_index != seed_data.size(); ++byte_index)
    {
        for (int bit_index = 0; bit_index < 8; ++bit_index)
        {
            bytes test_data = seed_data;
            test_data[byte_index] |= (1 << bit_index);

            SCOPED_TRACE(as_binary_data(test_data));
            const std::string id = user_id_from_seed(test_data);

            auto result = ids.insert(id);
            ASSERT_NE("", id);
            EXPECT_TRUE(result.second);
        }
    }
}

} // namespace
