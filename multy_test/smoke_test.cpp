/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license
 *
 * See LICENSE for details
 */

#include "multy_core/account.h"
#include "multy_core/common.h"
#include "multy_core/key.h"
#include "multy_core/mnemonic.h"
#include "multy_core/properties.h"
#include "multy_core/transaction.h"
#include "multy_core/transaction_builder.h"

#include "multy_core/src/api/account_impl.h"
#include "multy_core/src/api/key_impl.h"
#include "multy_core/src/u_ptr.h"

#include "multy_test/supported_blockchains.h"
#include "multy_test/bip39_test_cases.h"
#include "multy_test/utility.h"
#include "multy_test/value_printers.h"

#include "gtest/gtest.h"

#include <memory>
#include <string>
#include <tuple>
#include <string.h>

namespace
{
using namespace multy_core::internal;
using namespace test_utility;

void make_account_from_random_seed(const BlockchainType blockchain_type, Account** new_account)
{
    assert(new_account != nullptr);

    ConstCharPtr mnemonic_str;

    HANDLE_ERROR(
            make_mnemonic(make_dummy_entropy_source(), reset_sp(mnemonic_str)));
    ASSERT_NE(nullptr, mnemonic_str);

    BinaryDataPtr seed;
    HANDLE_ERROR(make_seed(mnemonic_str.get(), "", reset_sp(seed)));
    ASSERT_NE(nullptr, seed);

    ExtendedKeyPtr root_key;
    HANDLE_ERROR(make_master_key(seed.get(), reset_sp(root_key)));
    ASSERT_NE(nullptr, root_key);

    ConstCharPtr root_id;
    HANDLE_ERROR(make_user_id_from_master_key(root_key.get(), reset_sp(root_id)));
    ASSERT_NE(nullptr, root_id);
    ASSERT_STRCASENE("", root_id.get());

    HDAccountPtr root_account;
    HANDLE_ERROR(
            make_hd_account(
                    root_key.get(),
                    blockchain_type,
                    BITCOIN_ACCOUNT_DEFAULT,
                    0,
                    reset_sp(root_account)));
    ASSERT_NE(nullptr, root_account);

    AccountPtr account;
    HANDLE_ERROR(
            make_hd_leaf_account(
                    root_account.get(), ADDRESS_EXTERNAL, 0,
                    reset_sp(account)));
    ASSERT_NE(nullptr, account);

    *new_account = account.release();
}

class AccountSmokeTestP : public ::testing::TestWithParam<BlockchainType>
{
};

INSTANTIATE_TEST_CASE_P(
        Smoke, AccountSmokeTestP, ::testing::ValuesIn(SUPPORTED_BLOCKCHAINS));

TEST_P(AccountSmokeTestP, AccountFromEntropy)
{
    const BlockchainType expected_blockchain = GetParam();
    AccountPtr account;
    ASSERT_NO_FATAL_FAILURE(make_account_from_random_seed(expected_blockchain, reset_sp(account)));

    if (blockchain_can_derive_address_from_private_key(expected_blockchain.blockchain))
    {
        ConstCharPtr address;
        HANDLE_ERROR(account_get_address_string(account.get(), reset_sp(address)));
        ASSERT_NE(nullptr, address);
        ASSERT_LT(0, strlen(address.get()));
    }
    else
    {
        ConstCharPtr address;
        EXPECT_ERROR(account_get_address_string(account.get(), reset_sp(address)));
    }


    KeyPtr private_key;
    HANDLE_ERROR(
            account_get_key(
                    account.get(), KEY_TYPE_PRIVATE, reset_sp(private_key)));
    ASSERT_NE(nullptr, private_key);

    ConstCharPtr private_key_str;
    HANDLE_ERROR(key_to_string(private_key.get(), reset_sp(private_key_str)));
    ASSERT_NE(nullptr, private_key_str);

    KeyPtr public_key;
    HANDLE_ERROR(
            account_get_key(
                    account.get(), KEY_TYPE_PUBLIC, reset_sp(public_key)));
    ASSERT_NE(nullptr, public_key);

    ConstCharPtr public_key_str;
    HANDLE_ERROR(key_to_string(public_key.get(), reset_sp(public_key_str)));
    ASSERT_NE(nullptr, public_key_str);

    ASSERT_STRNE(public_key_str.get(), private_key_str.get());

    BlockchainType blockchain;
    HANDLE_ERROR(account_get_blockchain_type(account.get(), &blockchain));

    ASSERT_EQ(expected_blockchain, blockchain);
}

struct TransactionBuilderSmokeTestCase
{
    uint32_t transaction_builder_type;
    const char* action;
};

const TransactionBuilderSmokeTestCase SUPPORTED_TX_BUILDERS[] =
{
    {
        ETHEREUM_TRANSACTION_BUILDER_MULTISIG,
        "new_wallet"
    },
    {
        ETHEREUM_TRANSACTION_BUILDER_MULTISIG,
        "new_request"
    },
    {
        ETHEREUM_TRANSACTION_BUILDER_MULTISIG,
        "request"
    },
};

class TransactionBuilderSmokeTestP : public ::testing::TestWithParam<std::tuple<BlockchainType, TransactionBuilderSmokeTestCase>>
{};

INSTANTIATE_TEST_CASE_P(
        Smoke,
        TransactionBuilderSmokeTestP,
        ::testing::Combine(
            ::testing::Values(ETHEREUM_MAIN_NET, ETHEREUM_TEST_NET),
            ::testing::ValuesIn(SUPPORTED_TX_BUILDERS)
       )
);

TEST_P(TransactionBuilderSmokeTestP, builder)
{
    const auto& param = GetParam();
    const TransactionBuilderSmokeTestCase& builder_case = std::get<1>(param);

    AccountPtr account;
    ASSERT_NO_FATAL_FAILURE(make_account_from_random_seed(std::get<0>(param), reset_sp(account)));

    TransactionBuilderPtr builder;
    HANDLE_ERROR(make_transaction_builder(
            account.get(),
            builder_case.transaction_builder_type,
            builder_case.action, reset_sp(builder)));
    ASSERT_NE(nullptr, builder);

    {
        Properties* builder_properties = nullptr;
        HANDLE_ERROR(transaction_builder_get_properties(
                builder.get(),
                &builder_properties));
        ASSERT_NE(nullptr, builder_properties);

        EXPECT_ERROR(properties_validate(builder_properties));
    }

    {
        TransactionPtr transaction;
        EXPECT_ERROR(transaction_builder_make_transaction(
                builder.get(),
                reset_sp(transaction)));
        EXPECT_EQ(nullptr, transaction);
    }

    // Explicit deallocation.
    free_transaction_builder(builder.release());
}

} // namespace
