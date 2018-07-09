/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license
 *
 * See LICENSE for details
 */
#include "multy_core/transaction_builder.h"

#include "multy_core/common.h"
#include "multy_core/error.h"
#include "multy_core/src/u_ptr.h"
#include "multy_core/src/utility.h"

#include "multy_test/mocks.h"
#include "multy_test/supported_blockchains.h"
#include "multy_test/utility.h"

#include "gtest/gtest.h"

#include <memory>

namespace
{
using namespace multy_core::internal;
} //namespace

GTEST_TEST(TransactionBuilderTestInvalidArgs, make_transaction_builder)
{
    const TestAccount account(ETHEREUM_MAIN_NET, "address");
    const TestAccount invalid_account(
            BlockchainType{
                    static_cast<Blockchain>(BLOCKCHAIN_BITCOIN - 1),
                    BITCOIN_NET_TYPE_MAINNET
            },
            "address");

    TransactionBuilderPtr builder;
    EXPECT_ERROR(
        make_transaction_builder(
                nullptr,
                ETHEREUM_TRANSACTION_BUILDER_MULTISIG,
                "new_wallet",
                reset_sp(builder)));
    EXPECT_EQ(nullptr, builder);

    EXPECT_ERROR(
        make_transaction_builder(
                &invalid_account,
                ETHEREUM_TRANSACTION_BUILDER_MULTISIG,
                "new_wallet",
                reset_sp(builder)));
    EXPECT_EQ(nullptr, builder);

    EXPECT_ERROR(
        make_transaction_builder(
                &account,
                0xFFFF,
                "new_wallet",
                reset_sp(builder)));
    EXPECT_EQ(nullptr, builder);

    EXPECT_ERROR(
        make_transaction_builder(
                &account,
                ETHEREUM_TRANSACTION_BUILDER_MULTISIG,
                "",
                reset_sp(builder)));
    EXPECT_EQ(nullptr, builder);

    EXPECT_ERROR(
        make_transaction_builder(
                &account,
                ETHEREUM_TRANSACTION_BUILDER_MULTISIG,
                nullptr,
                reset_sp(builder)));
    EXPECT_EQ(nullptr, builder);

    EXPECT_ERROR(
        make_transaction_builder(
                &account,
                ETHEREUM_TRANSACTION_BUILDER_MULTISIG,
                "new_wallet",
                nullptr));
}

GTEST_TEST(TransactionBuilderTestInvalidArgs, transaction_builder_get_properties)
{
    TestTransactionBuilder builder;
    Properties* properties = nullptr;

    EXPECT_ERROR(transaction_builder_get_properties(&builder, nullptr));
    EXPECT_ERROR(transaction_builder_get_properties(nullptr, &properties));

    EXPECT_EQ(nullptr, properties);
}

GTEST_TEST(TransactionBuilderTestInvalidArgs, transaction_builder_make_transaction)
{
    TestTransactionBuilder builder;
    TransactionPtr transaction;

    EXPECT_ERROR(transaction_builder_make_transaction(nullptr, reset_sp(transaction)));
    EXPECT_ERROR(transaction_builder_make_transaction(&builder, nullptr));

    EXPECT_EQ(nullptr, transaction);
}

GTEST_TEST(TransactionBuilderTestInvalidArgs, free_transaction_builder)
{
    free_transaction_builder(nullptr);
}

GTEST_TEST(TransactionBuilderTest, transaction_builder_make_transaction)
{
    TestTransactionBuilder builder;
    TransactionPtr transaction;

    HANDLE_ERROR(transaction_builder_make_transaction(
            &builder, reset_sp(transaction)));
    ASSERT_NE(nullptr, transaction);

    EXPECT_NO_THROW(transaction->serialize());
    EXPECT_NO_THROW(transaction.reset());
}

GTEST_TEST(TransactionBuilderTest, transaction_builder_get_properties)
{
    TestTransactionBuilder builder;
    Properties* properties = nullptr;

    HANDLE_ERROR(transaction_builder_get_properties(
            &builder, &properties));
    EXPECT_NE(nullptr, properties);

    EXPECT_NO_THROW(properties->get_name());
}
