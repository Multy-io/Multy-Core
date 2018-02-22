/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license
 *
 * See LICENSE for details
 */

#include "multy_core/account.h"

#include "multy_core/key.h"
#include "multy_core/common.h"
#include "multy_core/src/api/account_impl.h"
#include "multy_core/src/api/key_impl.h"
#include "multy_core/src/u_ptr.h"

#include "multy_test/bip39_test_cases.h"
#include "multy_test/mocks.h"
#include "multy_test/utility.h"
#include "multy_test/value_printers.h"

#include "gtest/gtest.h"

#include <memory>
#include <string>

namespace
{
using namespace multy_core::internal;
using namespace test_utility;

const char* TEST_ADDRESS = "TEST_ADDRESS";
const HDPath TEST_PATH = {1, 2, 3};
const char* TEST_PATH_STRING = "m/1/2/3";

const Blockchain INVALID_BLOCKCHAIN = static_cast<Blockchain>(-1);
const BlockchainType INVALID_BLOCKCHAIN_TYPE{INVALID_BLOCKCHAIN, BLOCKCHAIN_NET_TYPE_MAINNET};

GTEST_TEST(AccountTest, free_account)
{
    free_account(nullptr);
    GTEST_SUCCEED();
}

GTEST_TEST(AccountTest, fake_account)
{
    const ExtendedKey expected_key = make_dummy_extended_key();
    const char* EXPECTED_ADDRESS = TEST_ADDRESS;
    const HDPath EXPECTED_PATH = TEST_PATH;
    const char* EXPECTED_PATH_STRING = TEST_PATH_STRING;
    const BlockchainType EXPECTED_BLOCKCHAIN_TYPE{BLOCKCHAIN_BITCOIN, BLOCKCHAIN_NET_TYPE_MAINNET};

    ErrorPtr error;
    TestHDAccount root_account(
            EXPECTED_BLOCKCHAIN_TYPE,
            EXPECTED_ADDRESS,
            EXPECTED_PATH,
            make_test_private_key(),
            make_test_public_key());

    AccountPtr account;
    {
        ExtendedKeyPtr key;
        error.reset(
                make_hd_leaf_account(
                        &root_account, ADDRESS_EXTERNAL, 0, reset_sp(account)));
        EXPECT_EQ(nullptr, error);
        ASSERT_NE(nullptr, account);
    }

    {
        ConstCharPtr address_str;
        error.reset(
                account_get_address_string(
                        account.get(), reset_sp(address_str)));
        EXPECT_EQ(nullptr, error);
        ASSERT_NE(nullptr, address_str);
        ASSERT_STREQ(EXPECTED_ADDRESS, address_str.get());
    }

    {
        ConstCharPtr path_str;
        error.reset(
                account_get_address_path(account.get(), reset_sp(path_str)));
        EXPECT_EQ(nullptr, error);
        ASSERT_NE(nullptr, path_str);
        ASSERT_STREQ(EXPECTED_PATH_STRING, path_str.get());
    }

    {
        BlockchainType blockchain_type;

        error.reset(account_get_blockchain_type(account.get(), &blockchain_type));
        EXPECT_EQ(nullptr, error);
        ASSERT_EQ(EXPECTED_BLOCKCHAIN_TYPE, blockchain_type);
    }
}

GTEST_TEST(AccountTestInvalidArgs, make_account)
{
    const ExtendedKey master_key = make_dummy_extended_key();

    const char* INVALID_PRIVATE_KEY = "foo-bar-yadda-yadda";
    ErrorPtr error;
    AccountPtr account;

    error.reset(make_account(INVALID_BLOCKCHAIN, "", reset_sp(account)));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ(nullptr, account);

    error.reset(
            make_account(
                    BLOCKCHAIN_BITCOIN,
                    INVALID_PRIVATE_KEY,
                    reset_sp(account)));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ(nullptr, account);
}

GTEST_TEST(AccountTestInvalidArgs, account_get_key)
{
    const KeyType INVALID_KEY_TYPE = static_cast<KeyType>(-1);
    const TestAccount account(
            {BLOCKCHAIN_BITCOIN, BLOCKCHAIN_NET_TYPE_MAINNET},
            TEST_ADDRESS, TEST_PATH, make_test_private_key(),
            make_test_public_key());

    ErrorPtr error;
    KeyPtr key;

    error.reset(account_get_key(nullptr, KEY_TYPE_PRIVATE, reset_sp(key)));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ(nullptr, key);

    error.reset(account_get_key(&account, INVALID_KEY_TYPE, reset_sp(key)));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ(nullptr, key);

    error.reset(account_get_key(&account, KEY_TYPE_PRIVATE, nullptr));
    EXPECT_NE(nullptr, error);
}

GTEST_TEST(AccountTestInvalidArgs, account_get_address_string)
{
    ErrorPtr error;
    ConstCharPtr address_str;

    const TestAccount account(
            {BLOCKCHAIN_BITCOIN, BLOCKCHAIN_NET_TYPE_MAINNET},
            TEST_ADDRESS, TEST_PATH, make_test_private_key(),
            make_test_public_key());

    error.reset(account_get_address_string(nullptr, reset_sp(address_str)));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ(nullptr, address_str);

    error.reset(account_get_address_string(&account, nullptr));
    EXPECT_NE(nullptr, error);
}

GTEST_TEST(AccountTestInvalidArgs, account_get_address_path)
{
    ErrorPtr error;
    ConstCharPtr path_str;

    const TestAccount account(
            {BLOCKCHAIN_BITCOIN, BLOCKCHAIN_NET_TYPE_MAINNET},
            TEST_ADDRESS, TEST_PATH, make_test_private_key(),
            make_test_public_key());

    error.reset(account_get_address_path(nullptr, reset_sp(path_str)));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ(nullptr, path_str);

    error.reset(account_get_address_path(&account, nullptr));
    EXPECT_NE(nullptr, error);
}

GTEST_TEST(AccountTestInvalidArgs, account_get_blockchain)
{
    ErrorPtr error;
    BlockchainType blockchain_type = INVALID_BLOCKCHAIN_TYPE;

    const TestAccount account(
            {BLOCKCHAIN_BITCOIN, BLOCKCHAIN_NET_TYPE_MAINNET},
            TEST_ADDRESS, TEST_PATH, make_test_private_key(),
            make_test_public_key());

    error.reset(account_get_blockchain_type(nullptr, &blockchain_type));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ(INVALID_BLOCKCHAIN_TYPE, blockchain_type);

    error.reset(account_get_blockchain_type(&account, nullptr));
    EXPECT_NE(nullptr, error);
}

GTEST_TEST(AccountTestInvalidArgs, validate_address)
{
    EXPECT_ERROR(validate_address({BLOCKCHAIN_BITCOIN, BLOCKCHAIN_NET_TYPE_MAINNET}, nullptr));
    EXPECT_ERROR(validate_address(INVALID_BLOCKCHAIN_TYPE, "test"));
}

GTEST_TEST(AccountTestInvalidAddress, validate_address)
{
    // Invalid Checksum
    EXPECT_ERROR(validate_address({BLOCKCHAIN_BITCOIN, BLOCKCHAIN_NET_TYPE_MAINNET},
            "12pWhnTAfMro4rJVk32YjvFq1NqtwmBNwe"));
    EXPECT_ERROR(validate_address({BLOCKCHAIN_BITCOIN, BLOCKCHAIN_NET_TYPE_MAINNET},
            "12pshnTAfMro4rJVk32YjvFq1NqtwmBNwe"));

    // Invalid address
    EXPECT_ERROR(validate_address({BLOCKCHAIN_ETHEREUM, BLOCKCHAIN_NET_TYPE_MAINNET},
            "12pshnTAfMro4rJVk32YjvFq1NqtwmBNwe"));
}


class AccountTestBlockchainSupportP : public ::testing::TestWithParam<BlockchainType>
{
};

const BlockchainType SUPPORTED_BLOCKCHAINS[] =
{
    {
        BLOCKCHAIN_BITCOIN,
        BLOCKCHAIN_NET_TYPE_MAINNET
    },
    {
        BLOCKCHAIN_BITCOIN,
        BLOCKCHAIN_NET_TYPE_TESTNET
    },
    {
        BLOCKCHAIN_ETHEREUM,
        BLOCKCHAIN_NET_TYPE_MAINNET
    }
};

INSTANTIATE_TEST_CASE_P(
        SupportedBlockchains,
        AccountTestBlockchainSupportP,
        ::testing::ValuesIn(SUPPORTED_BLOCKCHAINS));

TEST_P(AccountTestBlockchainSupportP, generic)
{
    const ExtendedKey master_key = make_dummy_extended_key();

    ErrorPtr error;
    HDAccountPtr root_account;

    error.reset(
            make_hd_account(
                    &master_key, GetParam(), 0, reset_sp(root_account)));
    EXPECT_EQ(nullptr, error);
    EXPECT_NE(nullptr, root_account);

    AccountPtr account;
    error.reset(
            make_hd_leaf_account(
                    root_account.get(), ADDRESS_EXTERNAL, 0,
                    reset_sp(account)));

    {
        BlockchainType actual_blockchain;
        error.reset(account_get_blockchain_type(account.get(), &actual_blockchain));
        EXPECT_EQ(nullptr, error);
        EXPECT_EQ(GetParam(), actual_blockchain);
    }

    {
        ConstCharPtr address_str;

        error.reset(
                account_get_address_string(
                        account.get(), reset_sp(address_str)));
        EXPECT_EQ(nullptr, error);
        EXPECT_NE(nullptr, address_str);
        EXPECT_STRNE("", address_str.get());
    }

    {
        ConstCharPtr path_str;

        error.reset(
                account_get_address_path(account.get(), reset_sp(path_str)));
        EXPECT_EQ(nullptr, error);
        EXPECT_NE(nullptr, path_str);
        EXPECT_STRNE("", path_str.get());
    }

    {
        KeyPtr private_key;
        error.reset(account_get_key(account.get(), KEY_TYPE_PRIVATE, reset_sp(private_key)));
        EXPECT_EQ(nullptr, error);
        EXPECT_NE(nullptr, private_key);

        KeyPtr public_key;
        error.reset(account_get_key(account.get(), KEY_TYPE_PUBLIC, reset_sp(public_key)));
        EXPECT_EQ(nullptr, error);
        EXPECT_NE(nullptr, public_key);

        EXPECT_NE(public_key, private_key);
    }

    //    {
    //        ConstCharPtr path_str;
    //        error.reset(account_get_address_path(account.get(),
    //        ADDRESS_EXTERNAL, 0, reset_sp(address_str)));
    //        EXPECT_EQ(nullptr, error);
    //        EXPECT_EQ(nullptr, address_str);
    //    }
}

} // namespace
