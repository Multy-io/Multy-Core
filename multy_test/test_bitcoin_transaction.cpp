/* Copyright 2018 by Multy.io
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

GTEST_TEST(BitcoinTransactionTest, create_raw_transaction_public_api)
{
    AccountPtr account;
    HANDLE_ERROR(make_account(
            CURRENCY_BITCOIN,
            "cQeGKosJjWPn9GkB7QmvmotmBbVg1hm8UjdN6yLXEWZ5HAcRwam7",
            reset_sp(account)));

//    ASSERT_EQ(nullptr, error);
//    ASSERT_NE(nullptr, account);
//    EXPECT_EQ("mzqiDnETWkunRDZxjUQ34JzN1LDevh5DpU", account->get_address());

    TransactionPtr transaction;
    HANDLE_ERROR(make_transaction(account.get(), reset_sp(transaction)));
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
    HANDLE_ERROR(
            make_account(
                    CURRENCY_BITCOIN,
                    "cQeGKosJjWPn9GkB7QmvmotmBbVg1hm8UjdN6yLXEWZ5HAcRwam7",
                    reset_sp(account)));
    ASSERT_NE(nullptr, account);
    EXPECT_EQ("mzqiDnETWkunRDZxjUQ34JzN1LDevh5DpU", account->get_address());

    TransactionPtr transaction;
    HANDLE_ERROR(make_transaction(account.get(), reset_sp(transaction)));
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
        source.set_property("private_key",
                *account->get_private_key());
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
    HANDLE_ERROR(
            make_account(
                    CURRENCY_BITCOIN,
                    "cQeGKosJjWPn9GkB7QmvmotmBbVg1hm8UjdN6yLXEWZ5HAcRwam7",
                    reset_sp(account)));
    ASSERT_NE(nullptr, account);
    EXPECT_EQ("mzqiDnETWkunRDZxjUQ34JzN1LDevh5DpU", account->get_address());

    TransactionPtr transaction;
    HANDLE_ERROR(make_transaction(account.get(), reset_sp(transaction)));
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
        source.set_property("private_key", *account->get_private_key());
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

GTEST_TEST(BitcoinTransactionTest, SmokeTest_testnet2_with_key_to_source)
{
    AccountPtr account;
    HANDLE_ERROR(
            make_account(
                    CURRENCY_BITCOIN,
                    "cQeGKosJjWPn9GkB7QmvmotmBbVg1hm8UjdN6yLXEWZ5HAcRwam7",
                    reset_sp(account)));
    ASSERT_NE(nullptr, account);
    EXPECT_EQ("mzqiDnETWkunRDZxjUQ34JzN1LDevh5DpU", account->get_address());

    TransactionPtr transaction;
    HANDLE_ERROR(make_transaction(account.get(), reset_sp(transaction)));
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
        source.set_property("private_key", *account->get_private_key());
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

    {
        Amount total_fee = transaction->estimate_total_fee(1,1);

        uint64_t max_total_fee = serialied->len * fee_value.get_value_as_uint64() * 1.05;
        uint64_t min_total_fee = serialied->len * fee_value.get_value_as_uint64() * 0.95;
        ASSERT_LE(min_total_fee, total_fee.get_value_as_uint64());
        ASSERT_GE(max_total_fee, total_fee.get_value_as_uint64());
    }

    // TODO: should re-signing produce same result ?
    transaction->sign();
    const BinaryDataPtr serialied2 = transaction->serialize();
    std::cerr << "2. signed transaction: " << to_hex(*serialied2) << "\n";

    ASSERT_EQ(*serialied, *serialied2);
    ASSERT_EQ(to_binary_data(from_hex(
            "010000000191a34273f5cb57244de3d7b27678b3ad385ac46c7df2c440f3f7b5ad23929748000000006a473044022064d09103c9d48c8b094db03227621ced41732a74963578d3495bac4f7f65b40e02201f2f7adf872c1de2af5027edefdf29379faf9fe8f5751015c974e064a9d9d6e0012102163387c2c86f897b8aef15ee24e1f135da70c52e7dde12c06e122891c704d694ffffffff014062b007000000001976a914d3f68b887224cabcc90a9581c7bbdace878666db88ac00000000")),
            *serialied);
}

GTEST_TEST(BitcoinTransactionTest, SmokeTest_testnet3)
{
    AccountPtr account;
    ErrorPtr error;

    error.reset(make_account(
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

    const Amount available(Amount(12) * 1000 * 1000 * 100);
    const Amount dest_amount(Amount(100000000));
    const Amount change_value(Amount(999000000));
    const Amount fee_value(Amount(1) * 1000 * 1000);

    {
        Properties& source = transaction->add_source();
        source.set_property("amount", available);
        source.set_property(
                "prev_tx_hash",
                to_binary_data(
                        from_hex("13ae654ae5609bd74ee1840fb5e4694580659e4cfe477b303e68162f20a81cda")));
        source.set_property("prev_tx_out_index", 1u);
        source.set_property("prev_tx_out_script_pubkey",
                to_binary_data(from_hex("76a914d3f68b887224cabcc90a9581c7bbdace878666db88ac")));

        source.set_property("private_key",
                *account->get_private_key());
    }

    {
        Properties& destination = transaction->add_destination();
        destination.set_property(
                "address", "mfgq7S1Va1GREFgN66MVoxX35X6juKov6A");
        destination.set_property("amount", dest_amount);
    }

    {
        Properties& destination = transaction->add_destination();
        destination.set_property(
                "address", "mk6a6qeXNXuQDpA4DPxuouTJJTeFYJAkep");
        destination.set_property("amount", dest_amount);
    }

    {
        Properties& destination = transaction->add_destination();
        destination.set_property(
                "address", "mzqiDnETWkunRDZxjUQ34JzN1LDevh5DpU");
        destination.set_property("amount", change_value);
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
            "0100000001da1ca8202f16683e307b47fe4c9e65804569e4b50f84e14ed79b60e54a65ae13010000006b483045022100e217cfb5920878da55069a919029ab910ff106cfb20fd901e82de041b149d71902202756c5700377294837893cca854e60b6cca86423f4407b8faf92ff898aded00a012102163387c2c86f897b8aef15ee24e1f135da70c52e7dde12c06e122891c704d694ffffffff0300e1f505000000001976a91401de29d6f0aaf3467da7881a981c5c5ef90258bd88ac00e1f505000000001976a914323c1ea8756feaaaa85d0d0e51b0cc07b4c7ac5e88acc0878b3b000000001976a914d3f68b887224cabcc90a9581c7bbdace878666db88ac00000000")),
            *serialied);
}

GTEST_TEST(BitcoinTransactionTest, SmokeTest_with_many_input_from_one_addreses_testnet)
{
    AccountPtr account;
    ErrorPtr error;
    error.reset(make_account(
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

    const Amount available1("229999999");
    const Amount available2("459999999");
    const Amount dest_amount("100000000");
    const Amount change_value("588999998");
    const Amount fee_value("1000000");

    {
        Properties& source = transaction->add_source();
        source.set_property("amount", available1);
        source.set_property("prev_tx_hash",
                to_binary_data(
                        from_hex("4c0a9df13d1d85d20bfc5bb5d38937290d273b7655ff3d50d43db81900546f8a")));
        source.set_property("prev_tx_out_index", 0);
        source.set_property("prev_tx_out_script_pubkey",
                to_binary_data(from_hex("76a914d3f68b887224cabcc90a9581c7bbdace878666db88ac")));
        source.set_property("private_key", *account->get_private_key());
    }

    {
        Properties& source = transaction->add_source();
        source.set_property("amount", available2);
        source.set_property(
                "prev_tx_hash",
                to_binary_data(
                        from_hex("c51b8890ad84fab4577785908d12b6f8195c69efe5a348fc7d6d88fc1ce97d17")));
        source.set_property("prev_tx_out_index", 0);
        source.set_property("prev_tx_out_script_pubkey",
                to_binary_data(from_hex("76a914d3f68b887224cabcc90a9581c7bbdace878666db88ac")));
        source.set_property("private_key", *account->get_private_key());
    }

    {
        Properties& destination = transaction->add_destination();
        destination.set_property(
                "address", "mfgq7S1Va1GREFgN66MVoxX35X6juKov6A");
        destination.set_property("amount", dest_amount);
    }

    {
        Properties& change = transaction->add_destination();
        change.set_property(
                "address", "mzqiDnETWkunRDZxjUQ34JzN1LDevh5DpU");
        change.set_property("amount", change_value);
    }
    {
        Properties& fee = transaction->get_fee();
        fee.set_property("amount_per_byte", fee_value);
        //Ridicuosly high fee just to pass the checks.
        fee.set_property("max_amount_per_byte", available1);
    }

    transaction->update_state();

    transaction->sign();
    const BinaryDataPtr serialied = transaction->serialize();

    // TODO: should re-signing produce same result ?
    transaction->sign();
    const BinaryDataPtr serialied2 = transaction->serialize();
    std::cerr << "2. signed transaction: " << to_hex(*serialied) << "\n";

    ASSERT_EQ(*serialied, *serialied2);
    ASSERT_EQ(to_binary_data(from_hex("01000000028a6f540019b83dd4503dff55763b270d293789d3b55bfc0bd2851d3df19d0a4c000000006a4730440220661ae5dd08bb4576a04c76114e980d9431c0d2f477dd8c71acfb7f77c8dd1670022022037a1939ae556881a93f41efcf7c3da756b7141b7245971c7337e4a859623d012102163387c2c86f897b8aef15ee24e1f135da70c52e7dde12c06e122891c704d694ffffffff177de91cfc886d7dfc48a3e5ef695c19f8b6128d90857757b4fa84ad90881bc5000000006a4730440220757edec6ee1fbc52c9046dc80c618001a9a7d4162f0a3abf81f27f09005f77e70220042c890f4702dda612883ceb25db4590c1f8459b0bdd88cd9858d4b7565e997b012102163387c2c86f897b8aef15ee24e1f135da70c52e7dde12c06e122891c704d694ffffffff0200e1f505000000001976a91401de29d6f0aaf3467da7881a981c5c5ef90258bd88ac3e6d1b23000000001976a914d3f68b887224cabcc90a9581c7bbdace878666db88ac00000000")),
            *serialied);

}

GTEST_TEST(BitcoinTransactionTest, SmokeTest_with_many_input_from_different_addreses_testnet)
{
    AccountPtr account1;
    AccountPtr account;
    ErrorPtr error;
    error.reset(
            make_account(
                    CURRENCY_BITCOIN,
                    "cScuLx5taDyuAfCnin5WWZz65yGCHMuuaFv6mgearmqAHC4p53sz",
                    reset_sp(account)));
    ASSERT_EQ(nullptr, error);
    ASSERT_NE(nullptr, account);
    EXPECT_EQ("mfgq7S1Va1GREFgN66MVoxX35X6juKov6A", account->get_address());

    error.reset(
            make_account(
                    CURRENCY_BITCOIN,
                    "cVbMJKcfEGi4wgsN39rMPkYVAaLeRaPPbrPpJfcH9B9dZCPbS7kT",
                    reset_sp(account1)));
    ASSERT_EQ(nullptr, error);
    ASSERT_NE(nullptr, account1);
    EXPECT_EQ("mk6a6qeXNXuQDpA4DPxuouTJJTeFYJAkep", account1->get_address());

    TransactionPtr transaction;
    error.reset(make_transaction(account.get(), reset_sp(transaction)));
    ASSERT_EQ(nullptr, error);
    ASSERT_NE(nullptr, transaction);

    const Amount available1(Amount(1) * 1000 * 1000 * 100);
    const Amount available2(Amount(1) * 1000 * 1000 * 100);
    const Amount dest_amount(Amount("190000")*1000);
    const Amount change_value(Amount("9000")*1000);
    const Amount fee_value(Amount(1) * 1000 * 1000);

    {
        Properties& source = transaction->add_source();
        source.set_property("amount", available1);
        source.set_property(
                "prev_tx_hash",
                to_binary_data(
                        from_hex("a1fdb0d8776cfd43b66cfc0ee49cad2763fdbeca67af8ef40479624716ea8948")));
        source.set_property("prev_tx_out_index", 1);
        source.set_property("prev_tx_out_script_pubkey",
                to_binary_data(from_hex("76a914323c1ea8756feaaaa85d0d0e51b0cc07b4c7ac5e88ac")));
        source.set_property("private_key",
                *account1->get_private_key());
    }

    {
        Properties& source = transaction->add_source();
        source.set_property("amount", available2);
        source.set_property(
                "prev_tx_hash",
                to_binary_data(
                        from_hex("a1fdb0d8776cfd43b66cfc0ee49cad2763fdbeca67af8ef40479624716ea8948")));
        source.set_property("prev_tx_out_index", 0);
        source.set_property("prev_tx_out_script_pubkey",
                to_binary_data(from_hex("76a91401de29d6f0aaf3467da7881a981c5c5ef90258bd88ac")));
        source.set_property("private_key",
                *account->get_private_key());
    }

    {
        Properties& destination = transaction->add_destination();
        destination.set_property(
                "address", "mzqiDnETWkunRDZxjUQ34JzN1LDevh5DpU");
        destination.set_property("amount", dest_amount);
    }

    {
        Properties& destination = transaction->add_destination();
        destination.set_property(
                "address", "mfgq7S1Va1GREFgN66MVoxX35X6juKov6A");
        destination.set_property("amount", change_value);
    }
    {
        Properties& fee = transaction->get_fee();
        fee.set_property("amount_per_byte", fee_value);
        //Ridicuosly high fee just to pass the checks.
        fee.set_property("max_amount_per_byte", available1);
    }

    transaction->update_state();

    transaction->sign();
    const BinaryDataPtr serialied = transaction->serialize();

    // TODO: should re-signing produce same result ?
    transaction->sign();
    const BinaryDataPtr serialied2 = transaction->serialize();
    std::cerr << "2. signed transaction123: " << to_hex(*serialied) << "\n";

    ASSERT_EQ(*serialied, *serialied2);
    ASSERT_EQ(to_binary_data(from_hex("01000000024889ea1647627904f48eaf67cabefd6327ad9ce40efc6cb643fd6c77d8b0fda1010000006a47304402200efd6929fcf32210e32194fc8468354deaf67060466710441075dab31afa31b30220350c72e95803ad14ce3fe3baa73e0a2288bf46df44e8c3d686e9692e7689cb7301210217fc7a7cc7f8b41b8e886703b95f087cd6e82ccbe6ee2ff27101b6d69ca2e868ffffffff4889ea1647627904f48eaf67cabefd6327ad9ce40efc6cb643fd6c77d8b0fda1000000006a473044022063a2925d2693033aa9735f412258c93f80f9bf980c688fbe5634b7fd6af958f40220506064007962d15ed0473ec617f1c38c80bd82af864050bf5e406ed4cf2951cf012102a6492c6dd74e49c4b7a4bd507baac3abf25fb26b97e362c3c0cb28b91a043da2ffffffff02802b530b000000001976a914d3f68b887224cabcc90a9581c7bbdace878666db88ac40548900000000001976a91401de29d6f0aaf3467da7881a981c5c5ef90258bd88ac00000000")),
            *serialied);
}
