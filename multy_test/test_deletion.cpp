/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license
 *
 * See LICENSE for details
 */

#include "multy_core/account.h"

#include "multy_core/big_int.h"
#include "multy_core/key.h"
#include "multy_core/bitcoin.h"
#include "multy_core/common.h"
#include "multy_core/ethereum.h"
#include "multy_core/src/u_ptr.h"

#include "multy_test/death_test.h"
#include "multy_test/mocks.h"
#include "multy_test/supported_blockchains.h"
#include "multy_test/utility.h"
#include "multy_test/value_printers.h"

#include "gtest/gtest.h"

namespace
{
using namespace multy_core::internal;
} // namespace

class DeletionTestP : public ::testing::TestWithParam<BlockchainType>
{
public:
    void SetUp()
    {
        HANDLE_ERROR(
                make_hd_account(
                        &master_key, GetParam(), 0, reset_sp(hd_account)));

        HANDLE_ERROR(
                make_hd_leaf_account(
                        hd_account.get(), ADDRESS_EXTERNAL, 0,
                        reset_sp(account)));
    }

    const ExtendedKey master_key = test_utility::make_dummy_extended_key();
    HDAccountPtr hd_account;
    AccountPtr account;
};

INSTANTIATE_TEST_CASE_P(
        free_objects,
        DeletionTestP,
        ::testing::ValuesIn(SUPPORTED_BLOCKCHAINS));

TEST_P(DeletionTestP, free_hd_account)
{
    HDAccount* hd_account_ptr = hd_account.get();
    hd_account.reset();

    // Ensuring that HDAccount is feed.
    ASSERT_FALSE(hd_account_ptr->is_valid());

    // Ensuring that double-free is not performed.
    MULTY_ASSERT_DEATH_IF_SUPPORTED(free_hd_account(hd_account_ptr),
            "trying to free invalid object");
}

TEST_P(DeletionTestP, free_account)
{
    Account* account_ptr = account.get();
    account.reset();

    // Ensuring that Account is feed.
    ASSERT_FALSE(account_ptr->is_valid());

    // Ensuring that double-free is not performed.
    MULTY_ASSERT_DEATH_IF_SUPPORTED(free_account(account_ptr),
            "trying to free invalid object");
}

TEST_P(DeletionTestP, free_private_key)
{
    KeyPtr private_key;
    HANDLE_ERROR(account_get_key(account.get(),
            KEY_TYPE_PRIVATE,
            reset_sp(private_key)));

    Key* key_ptr = private_key.get();
    private_key.reset();

    // Ensuring that private key is feed.
    ASSERT_FALSE(key_ptr->is_valid());

    // Ensuring that double-free is not performed.
    MULTY_ASSERT_DEATH_IF_SUPPORTED(free_key(key_ptr),
            "trying to free invalid object");
}

TEST_P(DeletionTestP, free_public_key)
{
    KeyPtr public_key;
    HANDLE_ERROR(account_get_key(account.get(),
            KEY_TYPE_PRIVATE,
            reset_sp(public_key)));

    Key* key_ptr = public_key.get();
    public_key.reset();

    // Ensuring that public key is feed.
    ASSERT_FALSE(key_ptr->is_valid());

    // Ensuring that double-free is not performed.
    MULTY_ASSERT_DEATH_IF_SUPPORTED(free_key(key_ptr),
            "trying to free invalid object");
}

TEST_P(DeletionTestP, free_transaction)
{
    TransactionPtr transaction;
    ErrorPtr error(make_transaction(account.get(), reset_sp(transaction)));
    if (error && error->code == ERROR_FEATURE_NOT_IMPLEMENTED_YET)
    {
        SUCCEED();
        return;
    }
    ASSERT_EQ(nullptr, error);

    Transaction* transaction_ptr = transaction.get();
    transaction.reset();

    // Ensuring that transaction is feed.
    ASSERT_FALSE(transaction_ptr->is_valid());

    // Ensuring that double-free is not performed.
    MULTY_ASSERT_DEATH_IF_SUPPORTED(free_transaction(transaction_ptr),
            "trying to free invalid object");
}

GTEST_TEST(DeletionTest, free_big_int)
{
    BigIntPtr big_int;
    HANDLE_ERROR(make_big_int("1", reset_sp(big_int)));

    BigInt* big_int_ptr = big_int.get();
    big_int.reset();

    // Ensuring that BigInt is feed.
    ASSERT_FALSE(big_int_ptr->is_valid());

    // Ensuring that double-free is not performed.
    MULTY_ASSERT_DEATH_IF_SUPPORTED(free_big_int(big_int_ptr),
            "trying to free invalid object");
}
