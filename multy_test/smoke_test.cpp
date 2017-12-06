/* Copyright 2017 by Multy.io
 * Licensed under Multy.io license
 *
 * See LICENSE for details
 */

#include "multy_core/account.h"
#include "multy_core/common.h"
#include "multy_core/keys.h"
#include "multy_core/mnemonic.h"

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

class AccountSmokeTestP : public ::testing::TestWithParam<Currency>
{
};

const Currency ALL_CURRENCIES[] = {
        CURRENCY_BITCOIN, CURRENCY_ETHEREUM,
};

INSTANTIATE_TEST_CASE_P(
        Smoke, AccountSmokeTestP, ::testing::ValuesIn(ALL_CURRENCIES));

TEST_P(AccountSmokeTestP, AccountFromEntropy)
{
    const Currency expected_currency = GetParam();

    ConstCharPtr mnemonic_str;
    ErrorPtr error;

    error.reset(
            make_mnemonic(make_dummy_entropy_source(), reset_sp(mnemonic_str)));
    ASSERT_EQ(nullptr, error);
    ASSERT_NE(nullptr, mnemonic_str);

    BinaryDataPtr seed;
    error.reset(make_seed(mnemonic_str.get(), "", reset_sp(seed)));
    ASSERT_EQ(nullptr, error);
    ASSERT_NE(nullptr, seed);

    ExtendedKeyPtr root_key;
    error.reset(make_master_key(seed.get(), reset_sp(root_key)));
    ASSERT_EQ(nullptr, error);
    ASSERT_NE(nullptr, root_key);

    ConstCharPtr root_id;
    error.reset(make_key_id(root_key.get(), reset_sp(root_id)));
    ASSERT_EQ(nullptr, error);
    ASSERT_NE(nullptr, root_id);
    ASSERT_STRCASENE("", root_id.get());

    HDAccountPtr root_account;
    error.reset(
            make_hd_account(
                    root_key.get(), expected_currency, 0,
                    reset_sp(root_account)));
    ASSERT_EQ(nullptr, error);
    ASSERT_NE(nullptr, root_account);

    AccountPtr account;
    error.reset(
            make_hd_leaf_account(
                    root_account.get(), ADDRESS_EXTERNAL, 0,
                    reset_sp(account)));
    ASSERT_EQ(nullptr, error);
    ASSERT_NE(nullptr, account);

    ConstCharPtr address;
    error.reset(get_account_address_string(account.get(), reset_sp(address)));
    ASSERT_EQ(nullptr, error);
    ASSERT_NE(nullptr, address);
    ASSERT_LT(0, strlen(address.get()));
    if (expected_currency == CURRENCY_BITCOIN && *address != '1' && *address != '3')
    {
        std::cerr <<
        "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"
        "!!!!!        ATTENTION: BITCOIN IS IN THE TESTNET MODE       !!!!!\n"
        "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"
        "\nGenerated address: " << address.get() << "\n\n";
    }

    KeyPtr private_key;
    error.reset(
            get_account_key(
                    account.get(), KEY_TYPE_PRIVATE, reset_sp(private_key)));
    ASSERT_EQ(nullptr, error);
    ASSERT_NE(nullptr, private_key);

    ConstCharPtr private_key_str;
    error.reset(key_to_string(private_key.get(), reset_sp(private_key_str)));
    ASSERT_EQ(nullptr, error);
    ASSERT_NE(nullptr, private_key_str);

    KeyPtr public_key;
    error.reset(
            get_account_key(
                    account.get(), KEY_TYPE_PUBLIC, reset_sp(public_key)));
    ASSERT_EQ(nullptr, error);
    ASSERT_NE(nullptr, public_key);

    ConstCharPtr public_key_str;
    error.reset(key_to_string(public_key.get(), reset_sp(public_key_str)));
    ASSERT_EQ(nullptr, error);
    ASSERT_NE(nullptr, public_key_str);

    ASSERT_STRNE(public_key_str.get(), private_key_str.get());

    Currency currency = static_cast<Currency>(-1);
    error.reset(get_account_currency(account.get(), &currency));
    ASSERT_EQ(nullptr, error);
    ASSERT_EQ(expected_currency, currency);
}

} // namespace
