/* Copyright 2017 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_transaction/internal/bitcoin_transaction.h"
#include "multy_transaction/internal/properties.h"

#include "multy_core/account.h"
#include "multy_core/internal/account.h"
#include "multy_core/internal/key.h"
#include "multy_core/internal/u_ptr.h"
#include "multy_core/internal/utility.h"

#include "multy_transaction/amount.h"
#include "multy_transaction/properties.h"

#include "multy_test/utility.h"
#include "multy_test/value_printers.h"

#include "gtest/gtest.h"

#include <memory>

namespace
{
using namespace multy_transaction::internal;
using namespace wallet_core::internal;
using namespace test_utility;
} // namespace

#define HANDLE_ERROR(statement) \
        do { error.reset(statement); ASSERT_EQ(nullptr, error); } while(0)

GTEST_TEST(BitcoinTransactionTest, create_raw_transaction_public_api)
{
    AccountPtr account;
    ErrorPtr error(
            make_account(
                    CURRENCY_BITCOIN,
                    "cQeGKosJjWPn9GkB7QmvmotmBbVg1hm8UjdN6yLXEWZ5HAcRwam7",
                    reset_sp(account)));
//    ASSERT_EQ(nullptr, error);
//    ASSERT_NE(nullptr, account);
//    EXPECT_EQ("mzqiDnETWkunRDZxjUQ34JzN1LDevh5DpU", account->get_address());

    TransactionPtr transaction;
    error.reset(make_transaction(account.get(), reset_sp(transaction)));
    ASSERT_EQ(nullptr, error);
    ASSERT_NE(nullptr, transaction);

    {
        Properties* source = nullptr;
        HANDLE_ERROR(transaction_add_source(transaction.get(), &source));

        AmountPtr amount;
        HANDLE_ERROR(make_amount("1000000", reset_sp(amount)));
        HANDLE_ERROR(properties_set_amount_value(source, "amount", amount.get()));

        const bytes prev_tx_hash_bytes = from_hex("48979223adb5f7f340c4f27d6cc45a38adb37876b2d7e34d2457cbf57342a391");
        const BinaryData prev_tx_hash = to_binary_data(prev_tx_hash_bytes);
        HANDLE_ERROR(properties_set_binary_data_value(source, "prev_tx_hash", &prev_tx_hash));

        HANDLE_ERROR(properties_set_int32_value(source, "prev_tx_out_index", 0));

        const bytes prev_tx_out_pubkey_script_bytes = from_hex("76a914d3f68b887224cabcc90a9581c7bbdace878666db88ac");
        const BinaryData prev_tx_out_pubkey_script = to_binary_data(prev_tx_out_pubkey_script_bytes);
        HANDLE_ERROR(properties_set_binary_data_value(source, "prev_tx_out_script_pubkey", &prev_tx_out_pubkey_script));
    }

    {
        Properties* destination = nullptr;
        HANDLE_ERROR(transaction_add_destination(transaction.get(), &destination));

        AmountPtr amount;
        HANDLE_ERROR(make_amount("1000000", reset_sp(amount)));
        HANDLE_ERROR(properties_set_amount_value(destination, "amount", amount.get()));
        HANDLE_ERROR(properties_set_string_value(destination, "address", "mzqiDnETWkunRDZxjUQ34JzN1LDevh5DpU"));
    }

    {
        // change:
        Properties* change = nullptr;
        HANDLE_ERROR(transaction_add_destination(transaction.get(), &change));

        AmountPtr amount;
        HANDLE_ERROR(make_amount("1000000", reset_sp(amount)));
        HANDLE_ERROR(properties_set_amount_value(change, "amount", amount.get()));
        HANDLE_ERROR(properties_set_string_value(change, "address", "mzqiDnETWkunRDZxjUQ34JzN1LDevh5DpU"));
    }

    {
        Properties* fee = nullptr;
        HANDLE_ERROR(transaction_get_fee(transaction.get(), &fee));

        AmountPtr amount;
        HANDLE_ERROR(make_amount("10000", reset_sp(amount)));
        HANDLE_ERROR(properties_set_amount_value(fee, "amount_per_byte", amount.get()));
    }
}

GTEST_TEST(BitcoinTransactionTest, SmokeTest_testnet)
{
    AccountPtr account;
    ErrorPtr error(
            make_account(
                    CURRENCY_BITCOIN,
                    "cQeGKosJjWPn9GkB7QmvmotmBbVg1hm8UjdN6yLXEWZ5HAcRwam7",
                    reset_sp(account)));
    ASSERT_EQ(nullptr, error);
    ASSERT_NE(nullptr, account);
    EXPECT_EQ("mzqiDnETWkunRDZxjUQ34JzN1LDevh5DpU", account->get_address());

    TransactionPtr transaction;
    error.reset(make_transaction(account.get(), reset_sp(transaction)));
    ASSERT_EQ(nullptr, error);
    ASSERT_NE(nullptr, transaction);

    Amount available(Amount(1000) * 1000 * 1000 * 1000 * 1000);
    Amount out_1(Amount(500) * 1000 * 1000 * 1000 * 1000);
    Amount out_2(Amount(400) * 1000 * 1000 * 1000 * 1000);
    Amount fee_value(Amount(1000) * 1000 * 1000 * 1000);
    {
        Properties& source = transaction->add_source();
        source.set_property("amount", available);
        source.set_property(
                "prev_tx_hash",
                to_binary_data(from_hex("48979223adb5f7f340c4f27d6cc45a38adb37876b2d7e34d2457cbf57342a391")));
        source.set_property("prev_tx_out_index", 0u);
        source.set_property("prev_tx_out_script_pubkey",
                to_binary_data(from_hex("76a914d3f68b887224cabcc90a9581c7bbdace878666db88ac")));
    }

    {
        Properties& destination = transaction->add_destination();
        destination.set_property(
                "address", "mzqiDnETWkunRDZxjUQ34JzN1LDevh5DpU");
        destination.set_property("amount", out_1);
    }

    {
        Properties& change = transaction->add_destination();
        change.set_property(
                "address", "mpJDSHJcytfxp9asgo2pqihabHmmJkqJuM");
        change.set_property("amount", out_1);
    }

    {
        Properties& fee = transaction->get_fee();
        fee.set_property("amount_per_byte", Amount(1000));
        // allowing zero-fee transaction.
        fee.set_property("min_amount_per_byte", Amount(0));
    }

    transaction->update_state();
    transaction->sign();
    BinaryDataPtr serialied = transaction->serialize();
}

GTEST_TEST(BitcoinTransactionTest, SmokeTest_testnet2)
{
    AccountPtr account;
    ErrorPtr error(
            make_account(
                    CURRENCY_BITCOIN,
                    "cQeGKosJjWPn9GkB7QmvmotmBbVg1hm8UjdN6yLXEWZ5HAcRwam7",
                    reset_sp(account)));
    ASSERT_EQ(nullptr, error);
    ASSERT_NE(nullptr, account);
    EXPECT_EQ("mzqiDnETWkunRDZxjUQ34JzN1LDevh5DpU", account->get_address());

    TransactionPtr transaction;
    error.reset(make_transaction(account.get(), reset_sp(transaction)));
    ASSERT_EQ(nullptr, error);
    ASSERT_NE(nullptr, transaction);

    const Amount available(Amount(1000) * 1000 * 1000 * 1000 * 1000);
    const Amount dest_amount(Amount(129) * 1000 * 1000);
    const Amount fee_value(Amount(1) * 1000 * 1000);

    {
        Properties& source = transaction->add_source();
        source.set_property("amount", available);
        source.set_property(
                "prev_tx_hash",
                to_binary_data(
                        from_hex("48979223adb5f7f340c4f27d6cc45a38adb37876b2d7e34d2457cbf57342a391")));
        source.set_property("prev_tx_out_index", 0u);
        source.set_property("prev_tx_out_script_pubkey",
                to_binary_data(from_hex("76a914d3f68b887224cabcc90a9581c7bbdace878666db88ac")));
    }

    {
        Properties& destination = transaction->add_destination();
        destination.set_property(
                "address", "mzqiDnETWkunRDZxjUQ34JzN1LDevh5DpU");
        destination.set_property("amount", dest_amount);
    }

    {
        Properties& fee = transaction->get_fee();
        fee.set_property("amount_per_byte", fee_value);
        //Ridicuosly high fee just to pass the checks.
        fee.set_property("max_amount_per_byte", available);
    }

    transaction->update_state();

    transaction->sign();
    const BinaryDataPtr serialied = transaction->serialize();

    // TODO: should re-signing produce same result ?
    transaction->sign();
    const BinaryDataPtr serialied2 = transaction->serialize();
    std::cerr << "2. signed transaction: " << to_hex(*serialied2) << "\n";

    ASSERT_EQ(*serialied, *serialied2);
    ASSERT_EQ(to_binary_data(from_hex(
            "010000000191a34273f5cb57244de3d7b27678b3ad385ac46c7df2c440f3f7b5ad23929748000000006a473044022064d09103c9d48c8b094db03227621ced41732a74963578d3495bac4f7f65b40e02201f2f7adf872c1de2af5027edefdf29379faf9fe8f5751015c974e064a9d9d6e0012102163387c2c86f897b8aef15ee24e1f135da70c52e7dde12c06e122891c704d694ffffffff014062b007000000001976a914d3f68b887224cabcc90a9581c7bbdace878666db88ac00000000")),
            *serialied);
}
