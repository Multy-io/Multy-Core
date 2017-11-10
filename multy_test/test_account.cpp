/* Copyright 2017 by Multy.io
 * Licensed under Multy.io license
 *
 * See LICENSE for details
 */

#include "multy_core/account.h"

#include "multy_core/common.h"
#include "multy_core/internal/account.h"
#include "multy_core/internal/key.h"
#include "multy_core/internal/u_ptr.h"

#include "multy_test/bip39_test_cases.h"
#include "multy_test/utility.h"
#include "multy_test/value_printers.h"

#include "gtest/gtest.h"

#include <memory>
#include <string>

namespace
{
using namespace wallet_core::internal;
using namespace test_utility;

const char* TEST_ADDRESS = "TEST_ADDRESS";
const HDPath TEST_PATH = {1, 2, 3};
const char* TEST_PATH_STRING = "m/1/2/3";
const Currency TEST_CURRENCY = CURRENCY_BITCOIN;

const uint32_t INVALID_INDEX = HARDENED_INDEX_BASE + 1;
const Currency INVALID_CURRENCY = static_cast<Currency>(-1);
const AddressType INVALID_ADDRESS = static_cast<AddressType>(-1);

struct TestPublicKey : public PublicKey
{
    std::string to_string() const override
    {
        return "test_public_key_string";
    }

    const BinaryData get_content() const override
    {

        return BinaryData{nullptr, 0};
    }

    PublicKeyPtr clone() const override
    {
        return make_clone(*this);
    }
};

struct TestPrivateKey : public PrivateKey
{
    std::string to_string() const override
    {
        return "test_private_key_string";
    }
    const BinaryData get_content() const override
    {

        return BinaryData{nullptr, 0};
    }

    PublicKeyPtr make_public_key() const override
    {
        return PublicKeyPtr(new TestPublicKey);
    }

    PrivateKeyPtr clone() const override
    {
        return make_clone(*this);
    }
};

PrivateKeyPtr make_test_private_key()
{
    return PrivateKeyPtr(new TestPrivateKey);
}

PublicKeyPtr make_test_public_key()
{
    return PublicKeyPtr(new TestPublicKey);
}

struct TestAccount : public Account
{
public:
    TestAccount(
            Currency currency,
            std::string address,
            HDPath path,
            PrivateKeyPtr private_key,
            PublicKeyPtr public_key)
        : currency(currency),
          address(address),
          path(std::move(path)),
          private_key(std::move(private_key)),
          public_key(std::move(public_key))
    {
    }

    HDPath get_path() const
    {
        return path;
    }

    Currency get_currency() const
    {
        return currency;
    }

    std::string get_address() const
    {
        return address;
    }

    PrivateKeyPtr get_private_key() const
    {
        return private_key->clone();
    }

    PublicKeyPtr get_public_key() const
    {
        return public_key->clone();
    }

    const Currency currency;
    const std::string address;
    const HDPath path;
    const PrivateKeyPtr private_key;
    const PublicKeyPtr public_key;
};

struct TestHDAccount : public HDAccount
{

    TestHDAccount(
            Currency currency,
            std::string address,
            HDPath path,
            PrivateKeyPtr private_key,
            PublicKeyPtr public_key)
        : m_currency(currency),
          m_address(address),
          m_path(path),
          m_private_key(std::move(private_key)),
          m_public_key(std::move(public_key))
    {
    }

    HDPath get_path() const override
    {
        return m_path;
    }

    Currency get_currency() const override
    {
        return m_currency;
    }

    AccountPtr make_leaf_account(
            AddressType type, uint32_t index) const override
    {
        return AccountPtr(
                new TestAccount(
                        m_currency, m_address, m_path, m_private_key->clone(),
                        m_public_key->clone()));
    }

private:
    Currency m_currency;
    std::string m_address;
    HDPath m_path;
    PrivateKeyPtr m_private_key;
    PublicKeyPtr m_public_key;
};

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
    const Currency EXPECTED_CURRENCY = CURRENCY_BITCOIN;

    ErrorPtr error;
    TestHDAccount root_account(
            EXPECTED_CURRENCY, EXPECTED_ADDRESS, EXPECTED_PATH,
            make_test_private_key(), make_test_public_key());

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
                get_account_address_string(
                        account.get(), reset_sp(address_str)));
        EXPECT_EQ(nullptr, error);
        ASSERT_NE(nullptr, address_str);
        ASSERT_STREQ(EXPECTED_ADDRESS, address_str.get());
    }

    {
        ConstCharPtr path_str;
        error.reset(
                get_account_address_path(account.get(), reset_sp(path_str)));
        EXPECT_EQ(nullptr, error);
        ASSERT_NE(nullptr, path_str);
        ASSERT_STREQ(EXPECTED_PATH_STRING, path_str.get());
    }

    {
        Currency currency = INVALID_CURRENCY;

        error.reset(get_account_currency(account.get(), &currency));
        EXPECT_EQ(nullptr, error);
        ASSERT_EQ(EXPECTED_CURRENCY, currency);
    }
}

GTEST_TEST(AccountTestInvalidArgs, make_account)
{
    const ExtendedKey master_key = make_dummy_extended_key();

    const char* INVALID_PRIVATE_KEY = "foo-bar-yadda-yadda";
    ErrorPtr error;
    AccountPtr account;

    error.reset(make_account(INVALID_CURRENCY, "", reset_sp(account)));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ(nullptr, account);

    error.reset(
            make_account(
                    CURRENCY_BITCOIN,
                    INVALID_PRIVATE_KEY,
                    reset_sp(account)));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ(nullptr, account);
}

GTEST_TEST(AccountTestInvalidArgs, get_account_key)
{
    const KeyType INVALID_KEY_TYPE = static_cast<KeyType>(-1);
    const TestAccount account(
            CURRENCY_BITCOIN, TEST_ADDRESS, TEST_PATH, make_test_private_key(),
            make_test_public_key());

    ErrorPtr error;
    KeyPtr key;

    error.reset(get_account_key(nullptr, KEY_TYPE_PRIVATE, reset_sp(key)));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ(nullptr, key);

    error.reset(get_account_key(&account, INVALID_KEY_TYPE, reset_sp(key)));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ(nullptr, key);

    error.reset(get_account_key(&account, KEY_TYPE_PRIVATE, nullptr));
    EXPECT_NE(nullptr, error);
}

GTEST_TEST(AccountTestInvalidArgs, get_account_address_string)
{
    ErrorPtr error;
    ConstCharPtr address_str;

    const TestAccount account(
            CURRENCY_BITCOIN, TEST_ADDRESS, TEST_PATH, make_test_private_key(),
            make_test_public_key());

    error.reset(get_account_address_string(nullptr, reset_sp(address_str)));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ(nullptr, address_str);

    error.reset(get_account_address_string(&account, nullptr));
    EXPECT_NE(nullptr, error);
}

GTEST_TEST(AccountTestInvalidArgs, get_account_address_path)
{
    ErrorPtr error;
    ConstCharPtr path_str;

    const TestAccount account(
            CURRENCY_BITCOIN, TEST_ADDRESS, TEST_PATH, make_test_private_key(),
            make_test_public_key());

    error.reset(get_account_address_path(nullptr, reset_sp(path_str)));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ(nullptr, path_str);

    error.reset(get_account_address_path(&account, nullptr));
    EXPECT_NE(nullptr, error);
}

GTEST_TEST(AccountTestInvalidArgs, get_account_currency)
{
    ErrorPtr error;
    Currency currency = INVALID_CURRENCY;

    const TestAccount account(
            CURRENCY_BITCOIN, TEST_ADDRESS, TEST_PATH, make_test_private_key(),
            make_test_public_key());

    error.reset(get_account_currency(nullptr, &currency));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ(INVALID_CURRENCY, currency);

    error.reset(get_account_currency(&account, nullptr));
    EXPECT_NE(nullptr, error);
}

class AccountTestCurrencySupportP : public ::testing::TestWithParam<Currency>
{
};

const Currency SUPPORTED_CURRENCIES[] = {CURRENCY_BITCOIN, CURRENCY_ETHEREUM};

INSTANTIATE_TEST_CASE_P(
        SupportedCurrencies,
        AccountTestCurrencySupportP,
        ::testing::ValuesIn(SUPPORTED_CURRENCIES));

TEST_P(AccountTestCurrencySupportP, generic)
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
        Currency actual_currency;
        error.reset(get_account_currency(account.get(), &actual_currency));
        EXPECT_EQ(nullptr, error);
        EXPECT_EQ(GetParam(), actual_currency);
    }

    {
        ConstCharPtr address_str;

        error.reset(
                get_account_address_string(
                        account.get(), reset_sp(address_str)));
        EXPECT_EQ(nullptr, error);
        EXPECT_NE(nullptr, address_str);
        EXPECT_STRNE("", address_str.get());
    }

    {
        ConstCharPtr path_str;

        error.reset(
                get_account_address_path(account.get(), reset_sp(path_str)));
        EXPECT_EQ(nullptr, error);
        EXPECT_NE(nullptr, path_str);
        EXPECT_STRNE("", path_str.get());
    }

    {
        KeyPtr private_key;
        error.reset(get_account_key(account.get(), KEY_TYPE_PRIVATE, reset_sp(private_key)));
        EXPECT_EQ(nullptr, error);
        EXPECT_NE(nullptr, private_key);

        KeyPtr public_key;
        error.reset(get_account_key(account.get(), KEY_TYPE_PUBLIC, reset_sp(public_key)));
        EXPECT_EQ(nullptr, error);
        EXPECT_NE(nullptr, public_key);

        EXPECT_NE(public_key, private_key);
    }

    //    {
    //        ConstCharPtr path_str;
    //        error.reset(get_account_address_path(account.get(),
    //        ADDRESS_EXTERNAL, 0, reset_sp(address_str)));
    //        EXPECT_EQ(nullptr, error);
    //        EXPECT_EQ(nullptr, address_str);
    //    }
}

} // namespace
