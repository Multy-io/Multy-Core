/* Copyright 2017 by Multy.io
 * Licensed under Multy.io license
 *
 * See LICENSE for details
 */

#include "multy_core/mnemonic.h"

#include "multy_test/bip39_test_cases.h"
#include "multy_test/utility.h"
#include "multy_test/value_printers.h"

#include "gtest/gtest.h"

#include <memory>
#include <string>
#include <vector>

namespace
{
using namespace wallet_core::internal;
using namespace test_utility;
typedef std::vector<unsigned char> bytes;

struct MnemonicTestCase
{
    const bytes entropy;
    const std::string expected_mnemonic;
    const bytes expected_seed;

    MnemonicTestCase(
            const char* entropy,
            const char* expected_mnemonic,
            const char* expected_seed)
        : entropy(from_hex(entropy)),
          expected_mnemonic(expected_mnemonic),
          expected_seed(from_hex(expected_seed))
    {
    }

    MnemonicTestCase(const BIP39TestCase& bip39_test_case)
        : MnemonicTestCase(
                  bip39_test_case.entropy,
                  bip39_test_case.mnemonic,
                  bip39_test_case.seed)
    {
    }
};

class MnemonicTestValidCasesP : public ::testing::TestWithParam<BIP39TestCase>
{
};

} // namespace

INSTANTIATE_TEST_CASE_P(
        BIP39,
        MnemonicTestValidCasesP,
        ::testing::ValuesIn(BIP39_DEFAULT_TEST_CASES));

TEST_P(MnemonicTestValidCasesP, Test)
{
    const BIP39TestCase& param = GetParam();
    const bytes entropy(from_hex(param.entropy));
    const std::string expected_mnemonic(param.mnemonic);
    const bytes expected_seed(from_hex(param.seed));

    ConstCharPtr mnemonic_str;
    ErrorPtr error;

    ASSERT_EQ(nullptr, mnemonic_str);
    ASSERT_EQ(nullptr, error);

    auto fill_entropy = [](void* data, ::size_t size, void* dest) -> ::size_t {
        const bytes* entropy = (const bytes*)(data);
        const size_t result_size = std::min(size, entropy->size());
        memcpy(dest, entropy->data(), result_size);
        return result_size;
    };
    auto entropy_source = EntropySource{(void*)&entropy, fill_entropy};
    error.reset(make_mnemonic(entropy_source, reset_sp(mnemonic_str)));
    EXPECT_EQ(nullptr, error);
    EXPECT_NE(nullptr, mnemonic_str);

    EXPECT_STREQ(expected_mnemonic.c_str(), mnemonic_str.get());

    BinaryDataPtr seed;
    error.reset(make_seed(expected_mnemonic.c_str(), "TREZOR", reset_sp(seed)));
    ASSERT_NE(nullptr, seed);
    EXPECT_EQ(to_binary_data(expected_seed), *seed);

    ConstCharPtr dictionary;
    error.reset(mnemonic_get_dictionary(reset_sp(dictionary)));
    ASSERT_NE(nullptr, dictionary);
    ASSERT_NE(0, strlen(dictionary.get()));
}

GTEST_TEST(MnemonicTest, empty_null_password)
{
    ConstCharPtr mnemonic_str;
    ErrorPtr error;

    error.reset(
            make_mnemonic(make_dummy_entropy_source(), reset_sp(mnemonic_str)));
    ASSERT_NE(nullptr, mnemonic_str);

    BinaryDataPtr empty_pass_seed;
    error.reset(make_seed(mnemonic_str.get(), "", reset_sp(empty_pass_seed)));
    ASSERT_NE(nullptr, empty_pass_seed);

    BinaryDataPtr null_pass_seed;
    error.reset(
            make_seed(mnemonic_str.get(), nullptr, reset_sp(null_pass_seed)));
    ASSERT_NE(nullptr, null_pass_seed);

    ASSERT_EQ(*null_pass_seed, *empty_pass_seed);
}

GTEST_TEST(MnemonicTestInvalidArgs, make_mnemonic)
{
    ConstCharPtr mnemonic_str;
    ErrorPtr error;

    error.reset(
            make_mnemonic(
                    EntropySource{nullptr, nullptr}, reset_sp(mnemonic_str)));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ(nullptr, mnemonic_str);

    error.reset(make_mnemonic(make_dummy_entropy_source(), nullptr));
    EXPECT_NE(nullptr, error);
}

GTEST_TEST(MnemonicTestInvalidArgs, make_seed)
{
    BinaryDataPtr binary_data;
    ErrorPtr error;

    error.reset(make_seed(nullptr, "pass", reset_sp(binary_data)));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ(nullptr, binary_data);

    error.reset(make_seed("mnemonic", nullptr, reset_sp(binary_data)));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ(nullptr, binary_data);

    error.reset(make_seed("mnemonic", "pass", nullptr));
    EXPECT_NE(nullptr, error);

    // Invalid mnemonic value
    error.reset(make_seed("mnemonic", "pass", reset_sp(binary_data)));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ(nullptr, binary_data);
}

GTEST_TEST(MnemonicTestInvalidArgs, seed_to_string)
{
    unsigned char data_vals[] = {1U, 2U, 3U, 4U};
    const BinaryData data{data_vals, 3};
    const BinaryData null_data{nullptr, 0};
    const BinaryData zero_len_data{nullptr, 0};

    ConstCharPtr seed_str;
    ErrorPtr error;

    error.reset(seed_to_string(nullptr, reset_sp(seed_str)));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ(nullptr, seed_str);

    error.reset(seed_to_string(&data, nullptr));
    EXPECT_NE(nullptr, error);

    error.reset(seed_to_string(&null_data, reset_sp(seed_str)));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ(nullptr, seed_str);

    error.reset(seed_to_string(&zero_len_data, reset_sp(seed_str)));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ(nullptr, seed_str);
}
