/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license
 *
 * See LICENSE for details
 */
#include "multy_core/transaction.h"

#include "multy_core/account.h"
#include "multy_core/common.h"
#include "multy_core/error.h"
#include "multy_core/src/transaction_base.h"
#include "multy_core/src/u_ptr.h"
#include "multy_core/src/utility.h"

#include "multy_test/mocks.h"
#include "multy_test/utility.h"

#include "gtest/gtest.h"

#include <memory>

namespace
{
using namespace multy_core::internal;
} //namespace

GTEST_TEST(TransactionTestInvalidArgs, make_transaction)
{
    AccountPtr account;
    TransactionPtr transaction;

    HANDLE_ERROR(make_account(
                    BLOCKCHAIN_BITCOIN,
                    "5HxWvvfubhXpYYpS3tJkw6fq9jE9j18THftkZjHHfmFiWtmAbrj",
                    reset_sp(account)));

    EXPECT_ERROR(make_transaction(account.get(), nullptr));

    EXPECT_ERROR(make_transaction(nullptr, reset_sp(transaction)));
    EXPECT_EQ(nullptr, transaction);
}

GTEST_TEST(TransactionTestInvalidArgs, transaction_has_trait)
{
    TestTransaction transaction;
    bool has_capability = false;

    EXPECT_ERROR(
            transaction_has_trait(&transaction, TRANSACTION_SUPPORTS_FEE, nullptr));

    EXPECT_ERROR(
            transaction_has_trait(nullptr, TRANSACTION_SUPPORTS_FEE, &has_capability));
}

GTEST_TEST(TransactionTestInvalidArgs, transaction_get_blockchain)
{
    TestTransaction transaction;
    BlockchainType blockchain_type;

    EXPECT_ERROR(transaction_get_blockchain_type(&transaction, nullptr));
    EXPECT_ERROR(transaction_get_blockchain_type(nullptr, &blockchain_type));
}

GTEST_TEST(TransactionTestInvalidArgs, transaction_add_source)
{
    TestTransaction transaction;
    Properties* properties;

    EXPECT_ERROR(transaction_add_source(nullptr, &properties));
    EXPECT_ERROR(transaction_add_source(&transaction, nullptr));
}

GTEST_TEST(TransactionTestInvalidArgs, transaction_add_destination)
{
    TestTransaction transaction;
    Properties* properties;

    EXPECT_ERROR(transaction_add_destination(nullptr, &properties ));
    EXPECT_ERROR(transaction_add_destination(&transaction, nullptr));
}

GTEST_TEST(TransactionTestInvalidArgs, transaction_get_fee)
{
    TestTransaction transaction;
    Properties* transaction_fee;

    EXPECT_ERROR(transaction_get_fee(nullptr, &transaction_fee));
    EXPECT_ERROR(transaction_get_fee(&transaction, nullptr));
}

GTEST_TEST(TransactionTestInvalidArgs, transaction_estimate_total_fee)
{
    TestTransaction transaction;
    BigIntPtr amount;

    EXPECT_ERROR(transaction_estimate_total_fee(&transaction, 1, 1, nullptr ));

    EXPECT_ERROR(transaction_estimate_total_fee(&transaction, 1, 0, reset_sp(amount)));
    EXPECT_EQ(nullptr, amount);

    EXPECT_ERROR(transaction_estimate_total_fee(&transaction, 0, 1, reset_sp(amount)));
    EXPECT_EQ(nullptr, amount);

    EXPECT_ERROR(transaction_estimate_total_fee(nullptr, 1, 1, reset_sp(amount)));
    EXPECT_EQ(nullptr, amount);
}

GTEST_TEST(TransactionTestInvalidArgs, transaction_get_total_fee)
{
    TestTransaction transaction;
    BigIntPtr amount;

    EXPECT_ERROR(transaction_get_total_fee(&transaction, nullptr ));

    EXPECT_ERROR(transaction_get_total_fee(nullptr, reset_sp(amount)));
    EXPECT_EQ(nullptr, amount);
}

GTEST_TEST(TransactionTestInvalidArgs, transaction_get_total_spent)
{
    TestTransaction transaction;
    BigIntPtr amount;

    EXPECT_ERROR(transaction_get_total_spent(&transaction, nullptr ));

    EXPECT_ERROR(transaction_get_total_spent(nullptr, reset_sp(amount)));
    EXPECT_EQ(nullptr, amount);
}

GTEST_TEST(TransactionTestInvalidArgs, transaction_serialize)
{
    TestTransaction transaction;
    BinaryDataPtr binary_data;

    EXPECT_ERROR(transaction_serialize(nullptr, reset_sp(binary_data) ));
    EXPECT_ERROR(transaction_serialize(&transaction, nullptr));
}

GTEST_TEST(TransactionTestInvalidArgs, transaction_update)
{
    EXPECT_ERROR(transaction_update(nullptr));
}


GTEST_TEST(TransactionTest, make_transaction)
{
    AccountPtr account_transaction;
    TransactionPtr transaction;

    HANDLE_ERROR(
            make_account(
                BLOCKCHAIN_BITCOIN,
                "5HxWvvfubhXpYYpS3tJkw6fq9jE9j18THftkZjHHfmFiWtmAbrj",
                reset_sp(account_transaction)));

    HANDLE_ERROR(make_transaction(account_transaction.get(), reset_sp(transaction)));
    EXPECT_NE(nullptr, transaction.get());
}

GTEST_TEST(TransactionTest, transaction_has_trait)
{
    TestTransaction transaction;
    bool has_capability = false;


    HANDLE_ERROR(
            transaction_has_trait(&transaction,
                              TRANSACTION_SUPPORTS_FEE, &has_capability));
}

GTEST_TEST(TransactionTest, transaction_get_blockchain)
{
    TestTransaction transaction;
    BlockchainType blockchain_type;

    HANDLE_ERROR(transaction_get_blockchain_type(&transaction, &blockchain_type));
}

GTEST_TEST(TransactionTest, transaction_add_source)
{
    TestTransaction transaction;
    Properties* properties;

    HANDLE_ERROR(transaction_add_source(&transaction, &properties));
}

GTEST_TEST(TransactionTest, transaction_add_destination)
{
    TestTransaction transaction;
    Properties* properties;

    HANDLE_ERROR(transaction_add_destination(&transaction, &properties));
}

GTEST_TEST(TransactionTest, transaction_get_fee)
{
    TestTransaction transaction;
    Properties* transaction_fee;

    HANDLE_ERROR(transaction_get_fee(&transaction, &transaction_fee));
}

GTEST_TEST(TransactionTest, transaction_estimate_total_fee)
{
    TestTransaction transaction(BigInt(5));
    BigIntPtr amount;

    HANDLE_ERROR(transaction_estimate_total_fee(&transaction, 1, 1, reset_sp(amount)));
    EXPECT_EQ("5", *amount);
}

GTEST_TEST(TransactionTest, transaction_get_total_fee)
{
    TestTransaction transaction(BigInt(5));
    BigIntPtr amount;

    HANDLE_ERROR(transaction_get_total_fee(&transaction, reset_sp(amount)));
    EXPECT_EQ("5", *amount);
}

GTEST_TEST(TransactionTest, transaction_get_total_spent)
{
    TestTransaction transaction(BigInt(5));
    BigIntPtr amount;

    HANDLE_ERROR(transaction_get_total_spent(&transaction, reset_sp(amount)));
    EXPECT_EQ("5", *amount);
}

GTEST_TEST(TransactionTest, transaction_serialize)
{
    TestTransaction transaction;
    BinaryDataPtr serialize_binary_data;

    HANDLE_ERROR(
            transaction_serialize(&transaction, reset_sp(serialize_binary_data)));
}
