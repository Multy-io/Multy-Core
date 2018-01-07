/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license
 *
 * See LICENSE for details
 */

#include "multy_core/internal/u_ptr.h"
#include "multy_core/internal/utility.h"
#include "multy_core/error.h"
#include "multy_core/account.h"
#include "multy_core/common.h"

#include "multy_transaction/transaction.h"
#include "multy_transaction/internal/transaction_base.h"
#include "multy_transaction/internal/u_ptr.h"

#include "gtest/gtest.h"

#include <memory>


namespace
{
using namespace multy_transaction::internal;
using namespace wallet_core::internal;

} //namespace

struct TestTransaction: Transaction
{
    TestTransaction(): m_properties(""), m_total("5"){}
    Currency get_currency() const override
    {
        return m_currency;
    }

    uint32_t get_traits() const override
    {
        return m_traits;
    }

    BinaryDataPtr serialize() const override
    {
        return BinaryDataPtr(new BinaryData{nullptr, 0});
    }

    Amount estimate_total_fee(size_t sources_count, size_t destinations_count) const override
    {
        return (m_total-1);
    }

    Amount get_total_fee() const override
    {
        return (m_total-2);
    }

    void update_state() override
    {
        return;
    }

    void sign() override
    {
        return;
    }

    Properties& add_source() override
    {
        return m_properties;
    }

    Properties& add_destination() override
    {
        return m_properties;
    }

    Properties& get_fee() override
    {
        return m_properties;
    }


    Properties& get_transaction_properties() override
    {
        return m_properties;
    }

private:
    Currency m_currency = CURRENCY_BITCOIN;
    uint32_t m_traits = TRANSACTION_REQUIRES_EXPLICIT_SOURCE;
    Amount m_total;
    Properties m_properties;
    BinaryDataPtr m_binarydata;
};


GTEST_TEST(TransactionTestInvalidArgs, make_transaction)
{
    ErrorPtr error;
    AccountPtr account;
    TransactionPtr transaction;

    error.reset(make_account(
                    CURRENCY_BITCOIN,
                    "5HxWvvfubhXpYYpS3tJkw6fq9jE9j18THftkZjHHfmFiWtmAbrj",
                    reset_sp(account)));

    error.reset(make_transaction(account.get(), nullptr));
    EXPECT_NE(nullptr, error);

    error.reset(make_transaction(nullptr, reset_sp(transaction)));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ(nullptr, transaction);
}

GTEST_TEST(TransactionTestInvalidArgs, transaction_has_trait)
{
    ErrorPtr error;
    TestTransaction transaction;
    bool has_capability = false;

    error.reset(
            transaction_has_trait(&transaction, TRANSACTION_SUPPORTS_FEE, nullptr));
    EXPECT_NE(nullptr, error);

    error.reset(
            transaction_has_trait(nullptr, TRANSACTION_SUPPORTS_FEE, &has_capability));
    EXPECT_NE(nullptr, error);
}

GTEST_TEST(TransactionTestInvalidArgs, transaction_get_currency)
{
    ErrorPtr error;
    TestTransaction transaction;
    Currency currensy;

    error.reset(transaction_get_currency(&transaction, nullptr));
    EXPECT_NE(nullptr, error);

    error.reset(transaction_get_currency(nullptr, &currensy));
    EXPECT_NE(nullptr, error);
}

GTEST_TEST(TransactionTestInvalidArgs, transaction_add_source)
{
    ErrorPtr error;
    TestTransaction transaction;
    Properties* properties;

    error.reset(transaction_add_source(nullptr, &properties));
    EXPECT_NE(nullptr, error);

    error.reset(transaction_add_source(&transaction, nullptr));
    EXPECT_NE(nullptr, error);
}

GTEST_TEST(TransactionTestInvalidArgs, transaction_add_destination)
{
    ErrorPtr error;
    TestTransaction transaction;
    Properties* properties;

    error.reset(transaction_add_destination(nullptr, &properties ));
    EXPECT_NE(nullptr, error);

    error.reset(transaction_add_destination(&transaction, nullptr));
    EXPECT_NE(nullptr, error);
}

GTEST_TEST(TransactionTestInvalidArgs, transaction_get_fee)
{
    ErrorPtr error;
    TestTransaction transaction;
    Properties* transaction_fee;

    error.reset(transaction_get_fee(nullptr, &transaction_fee));
    EXPECT_NE(nullptr, error);

    error.reset(transaction_get_fee(&transaction, nullptr));
    EXPECT_NE(nullptr, error);
}

GTEST_TEST(TransactionTestInvalidArgs, transaction_estimate_total_fee)
{
    ErrorPtr error;
    TestTransaction transaction;
    AmountPtr amount;

    error.reset(transaction_estimate_total_fee(&transaction, 1, 1, nullptr ));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ(nullptr, amount);

    error.reset(transaction_estimate_total_fee(&transaction, 1, 0, reset_sp(amount)));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ(nullptr, amount);

    error.reset(transaction_estimate_total_fee(&transaction, 0, 1, reset_sp(amount)));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ(nullptr, amount);

    error.reset(transaction_estimate_total_fee(nullptr, 1, 1, reset_sp(amount)));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ(nullptr, amount);
}

GTEST_TEST(TransactionTestInvalidArgs, transaction_get_total_fee)
{
    ErrorPtr error;
    TestTransaction transaction;
    AmountPtr amount;

    error.reset(transaction_get_total_fee(&transaction, nullptr ));
    EXPECT_NE(nullptr, error);

    error.reset(transaction_get_total_fee(nullptr, reset_sp(amount)));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ(nullptr, amount);
}

GTEST_TEST(TransactionTestInvalidArgs, transaction_update)
{
    ErrorPtr error;
    error.reset(transaction_update(nullptr));
    EXPECT_NE(nullptr, error);
}

GTEST_TEST(TransactionTestInvalidArgs, sign)
{
    ErrorPtr error;
    error.reset(transaction_sign(nullptr));
    EXPECT_NE(nullptr, error);
}


GTEST_TEST(TransactionTestInvalidArgs, transaction_serialize)
{
    ErrorPtr error;
    TestTransaction transaction;
    BinaryDataPtr binary_data;

    error.reset(transaction_serialize(nullptr, reset_sp(binary_data) ));
    EXPECT_NE(nullptr, error);

    error.reset(transaction_serialize(&transaction, nullptr));
    EXPECT_NE(nullptr, error);
}


GTEST_TEST(TransactionTest, make_transaction)
{
    ErrorPtr error;
    AccountPtr account_transaction;
    TransactionPtr transaction(new TestTransaction());

    error.reset(
            make_account(
                CURRENCY_BITCOIN,
                "5HxWvvfubhXpYYpS3tJkw6fq9jE9j18THftkZjHHfmFiWtmAbrj",
                reset_sp(account_transaction)));

    error.reset(make_transaction(account_transaction.get(), reset_sp(transaction)));
    EXPECT_EQ(nullptr, error);
}

GTEST_TEST(TransactionTest, transaction_has_trait)
{
    ErrorPtr error;
    TestTransaction transaction;
    bool has_capability = false;


    error.reset(
            transaction_has_trait(&transaction,
                              TRANSACTION_SUPPORTS_FEE, &has_capability));
    EXPECT_EQ(nullptr, error);
}

GTEST_TEST(TransactionTest, transaction_get_currency)
{
    ErrorPtr error;
    TestTransaction transaction;
    Currency currensy;

    error.reset(transaction_get_currency(&transaction, &currensy));
    EXPECT_EQ(nullptr, error);
}
GTEST_TEST(TransactionTest, transaction_add_source)
{
    ErrorPtr error;
    TestTransaction transaction;
    Properties* properties;

    error.reset(transaction_add_source(&transaction, &properties));
    EXPECT_EQ(nullptr, error);
}

GTEST_TEST(TransactionTest, transaction_add_destination)
{
    ErrorPtr error;
    TestTransaction transaction;
    Properties* properties;

    error.reset(transaction_add_destination(&transaction, &properties));
    EXPECT_EQ(nullptr, error);
}

GTEST_TEST(TransactionTest, transaction_get_fee)
{
    ErrorPtr error;
    TestTransaction transaction;
    Properties* transaction_fee;

    error.reset(transaction_get_fee(&transaction, &transaction_fee));
    EXPECT_EQ(nullptr, error);
}

GTEST_TEST(TransactionTest, transaction_estimate_total_fee)
{
    ErrorPtr error;
    TestTransaction transaction;
    AmountPtr amount;

    error.reset(transaction_estimate_total_fee(&transaction, 1, 1, reset_sp(amount)));
    EXPECT_EQ(nullptr, error);
    EXPECT_EQ("4", *amount);
}

GTEST_TEST(TransactionTest, transaction_get_total_fee)
{
    ErrorPtr error;
    TestTransaction transaction;
    AmountPtr amount;

    error.reset(transaction_get_total_fee(&transaction, reset_sp(amount)));
    EXPECT_EQ(nullptr, error);
    EXPECT_EQ("3", *amount);
}

GTEST_TEST(TransactionTest, transaction_update)
{
    ErrorPtr error;
    TestTransaction transaction;
    error.reset(transaction_update(&transaction));
    EXPECT_EQ(nullptr, error);
}

GTEST_TEST(TransactionTest, transaction_sign)
{
    ErrorPtr error;
    TestTransaction transaction;
    error.reset(transaction_sign(&transaction));
    EXPECT_EQ(nullptr, error);
}

GTEST_TEST(TransactionTest, transaction_serialize)
{
    ErrorPtr error;
    TestTransaction transaction;
    BinaryDataPtr serialize_binary_data;

    error.reset(
            transaction_serialize(&transaction, reset_sp(serialize_binary_data)));
    EXPECT_EQ(nullptr, error);
}
