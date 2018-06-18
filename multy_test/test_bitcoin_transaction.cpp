/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/src/bitcoin/bitcoin_transaction.h"
#include "multy_core/src/api/properties_impl.h"

#include "multy_core/account.h"
#include "multy_core/big_int.h"
#include "multy_core/properties.h"
#include "multy_core/src/api/account_impl.h"
#include "multy_core/src/api/key_impl.h"
#include "multy_core/src/exception.h"
#include "multy_core/src/exception_stream.h"
#include "multy_core/src/u_ptr.h"
#include "multy_core/src/utility.h"

#include "multy_test/supported_blockchains.h"
#include "multy_test/utility.h"
#include "multy_test/value_printers.h"

#include "gtest/gtest.h"

#include <memory>
#include <string>

namespace
{
using namespace multy_core::internal;
using namespace test_utility;

bool is_between(const BigInt& left, const BigInt& value, const BigInt& right)
{
    if (left > right)
    {
        THROW_EXCEPTION("Invalid values: left > right")
                << " left: " << left << ", right: " << right;
    }
    return (left <= value) && (value <= right);
}

AccountPtr make_account(
        BlockchainType blockchain,
        const char* serialized_private_key)
{
    AccountPtr account;
    throw_if_error(make_account(
            blockchain,
            BITCOIN_ACCOUNT_DEFAULT,
            serialized_private_key,
            reset_sp(account)));

    return account;
}

struct TransactionFee
{
    BigInt amount_per_byte;
};

struct TransactionSource
{
    BigInt available;
    bytes prev_tx_hash;
    size_t prev_tx_index;
    bytes prev_tx_scrip_pubkey;
    PrivateKey* private_key;
};

struct TransactionDestination
{
    std::string address;
    BigInt amount;
    bool is_change = false;

    TransactionDestination(
            std::string address,
            BigInt amount)
        : TransactionDestination(std::move(address), std::move(amount), false)
    {}

protected:
    TransactionDestination(
            std::string address,
            BigInt amount,
            bool is_change)
        : address(std::move(address)),
          amount(std::move(amount)),
          is_change(is_change)
    {}
};

struct TransactionChangeDestination : public TransactionDestination
{
    TransactionChangeDestination(const std::string& address)
        : TransactionDestination(address, BigInt{}, true)
    {}
};

struct TransactionTemplate
{
    Account* account;
    TransactionFee fee;
    std::vector<TransactionSource> sources;
    std::vector<TransactionDestination> destinations;
};

const TransactionTemplate DEFAULT_TX_TEMPLATE
{
    nullptr,
    TransactionFee
    { // fee:
        100_SATOSHI
    },
    { // Sources
        {
            2000500_SATOSHI,
            from_hex("48979223adb5f7f340c4f27d6cc45a38adb37876b2d7e34d2457cbf57342a391"),
            0,
            from_hex("76a914d3f68b887224cabcc90a9581c7bbdace878666db88ac"),
            nullptr
        }
    },
    { // Destinations
        TransactionDestination
        {
            "mzqiDnETWkunRDZxjUQ34JzN1LDevh5DpU",
            0.01_BTC
        },
        TransactionChangeDestination
        {
            "mzqiDnETWkunRDZxjUQ34JzN1LDevh5DpU"
        }
    }
};

// Make Transaction from template using non-public API, note that returned value must not outlive the argument.
TransactionPtr make_transaction_from_template(const TransactionTemplate& tx,
        const AccountPtr& account = AccountPtr(),
        const PrivateKeyPtr& default_private_key = PrivateKeyPtr())
{
    TransactionPtr transaction;
    throw_if_error(make_transaction(tx.account ? tx.account : account.get(),
            reset_sp(transaction)));

    {
        Properties& fee = transaction->get_fee();
        fee.set_property_value("amount_per_byte", tx.fee.amount_per_byte);
    }

    for (const TransactionDestination& dest : tx.destinations)
    {
        Properties& destination = transaction->add_destination();
        destination.set_property_value("amount", dest.amount);
        destination.set_property_value("address", dest.address);
        destination.set_property_value("is_change", static_cast<int32_t>(dest.is_change));
    }

    for (const TransactionSource& src : tx.sources)
    {
        Properties& source = transaction->add_source();
        source.set_property_value("amount", src.available);
        source.set_property_value("prev_tx_hash",
                as_binary_data(src.prev_tx_hash));
        source.set_property_value("prev_tx_out_index", src.prev_tx_index);
        source.set_property_value("prev_tx_out_script_pubkey",
                as_binary_data(src.prev_tx_scrip_pubkey));
        if (src.private_key)
        {
            source.set_property_value("private_key", *src.private_key);
        }
        else if (default_private_key)
        {
            source.set_property_value("private_key", *default_private_key);
        }
    }
    return transaction;
}

} // namespace

const TransactionTemplate TEST_TRANSACTIONS[] =
{
    // Transaction 1
    {
        nullptr,
        TransactionFee
        { // fee:
            1_SATOSHI
        },
        { // Sources
            {
                0.1_BTC,
                from_hex("a1fdb0d8776cfd43b66cfc0ee49cad2763fdbeca67af8ef40479624716ea8948"),
                0,
                from_hex("76a91401de29d6f0aaf3467da7881a981c5c5ef90258bd88ac"),
                nullptr
            }
        },
        { // Destinations
            TransactionDestination
            {
                "mfgq7S1Va1GREFgN66MVoxX35X6juKov6A",
                9999227_SATOSHI
            },
            TransactionChangeDestination
            {
                "mzqiDnETWkunRDZxjUQ34JzN1LDevh5DpU"
            }
        }
    },

    // Transaction 2
    {
        nullptr,
        TransactionFee
        { // fee:
            100_SATOSHI
        },
        { // Sources
            {
                1.0_BTC,
                from_hex("a1fdb0d8776cfd43b66cfc0ee49cad2763fdbeca67af8ef40479624716ea8948"),
                0,
                from_hex("76a91401de29d6f0aaf3467da7881a981c5c5ef90258bd88ac"),
                nullptr
            }
        },
        { // Destinations
            TransactionDestination
            {
                "mfgq7S1Va1GREFgN66MVoxX35X6juKov6A",
                99976950_SATOSHI
            },
            TransactionChangeDestination
            {
                "mzqiDnETWkunRDZxjUQ34JzN1LDevh5DpU"
            }
        }
    }
};

class BitcoinTransactionTestP : public ::testing::TestWithParam<TransactionTemplate>
{
public:
    void SetUp()
    {
        account = make_account(BITCOIN_TEST_NET,
                "cScuLx5taDyuAfCnin5WWZz65yGCHMuuaFv6mgearmqAHC4p53sz");
        TransactionTemplate temp = GetParam();

        transaction = make_transaction_from_template(temp, account,
                account->get_private_key());
    }
    TransactionPtr transaction;
    AccountPtr account;
};

INSTANTIATE_TEST_CASE_P(
        test_update,
        BitcoinTransactionTestP,
        ::testing::ValuesIn(TEST_TRANSACTIONS));

TEST_P(BitcoinTransactionTestP, iteratively_change_price_fee)
{
    BinaryDataPtr serialized = transaction->serialize();
    BigInt total_fee = transaction->get_total_fee();
    uint64_t tx_size = serialized->len;
    BigInt tx_cost = tx_size * GetParam().fee.amount_per_byte;
    EXPECT_LE(tx_cost, total_fee);
}

GTEST_TEST(BitcoinTransactionTest, create_raw_transaction_public_api)
{
    AccountPtr account;
    HANDLE_ERROR(make_account(
            BITCOIN_TEST_NET,
            BITCOIN_ACCOUNT_P2PKH,
            "cQeGKosJjWPn9GkB7QmvmotmBbVg1hm8UjdN6yLXEWZ5HAcRwam7",
            reset_sp(account)));
    ASSERT_NE(nullptr, account);

    TransactionPtr transaction;
    HANDLE_ERROR(make_transaction(account.get(), reset_sp(transaction)));
    ASSERT_NE(nullptr, transaction);

    {
        Properties* source = nullptr;
        HANDLE_ERROR(transaction_add_source(transaction.get(), &source));

        BigIntPtr amount;
        HANDLE_ERROR(make_big_int("2000500", reset_sp(amount)));
        HANDLE_ERROR(properties_set_big_int_value(source, "amount", amount.get()));

        const bytes prev_tx_hash_bytes = from_hex("48979223adb5f7f340c4f27d6cc45a38adb37876b2d7e34d2457cbf57342a391");
        const BinaryData prev_tx_hash = as_binary_data(prev_tx_hash_bytes);
        HANDLE_ERROR(properties_set_binary_data_value(source, "prev_tx_hash", &prev_tx_hash));

        HANDLE_ERROR(properties_set_int32_value(source, "prev_tx_out_index", 0));

        const bytes prev_tx_out_pubkey_script_bytes = from_hex("76a914d3f68b887224cabcc90a9581c7bbdace878666db88ac");
        const BinaryData prev_tx_out_pubkey_script = as_binary_data(prev_tx_out_pubkey_script_bytes);
        HANDLE_ERROR(properties_set_binary_data_value(source, "prev_tx_out_script_pubkey", &prev_tx_out_pubkey_script));

        PrivateKey* private_key_ptr;
        HANDLE_ERROR(account_get_key(account.get(), KEY_TYPE_PRIVATE, reinterpret_cast<Key**>(&private_key_ptr)));
        PrivateKeyPtr private_key(private_key_ptr);

        HANDLE_ERROR(properties_set_private_key_value(source, "private_key", private_key.get()));
    }

    {
        Properties* destination = nullptr;
        HANDLE_ERROR(transaction_add_destination(transaction.get(), &destination));

        BigIntPtr amount;
        HANDLE_ERROR(make_big_int("1000000", reset_sp(amount)));
        HANDLE_ERROR(properties_set_big_int_value(destination, "amount", amount.get()));
        HANDLE_ERROR(properties_set_string_value(destination, "address", "mzqiDnETWkunRDZxjUQ34JzN1LDevh5DpU"));
    }

    {
        // change:
        Properties* change = nullptr;
        HANDLE_ERROR(transaction_add_destination(transaction.get(), &change));

        HANDLE_ERROR(properties_set_int32_value(change, "is_change", 1));
        HANDLE_ERROR(properties_set_string_value(change, "address", "mzqiDnETWkunRDZxjUQ34JzN1LDevh5DpU"));
    }

    {
        Properties* fee = nullptr;
        HANDLE_ERROR(transaction_get_fee(transaction.get(), &fee));

        BigIntPtr amount;
        HANDLE_ERROR(make_big_int("10", reset_sp(amount)));
        HANDLE_ERROR(properties_set_big_int_value(fee, "amount_per_byte", amount.get()));
    }

    BinaryDataPtr serialized;
    HANDLE_ERROR(transaction_serialize(transaction.get(), reset_sp(serialized)));
    ASSERT_NE(nullptr, serialized);
    EXPECT_NE(0, serialized->len);
    EXPECT_NE(nullptr, serialized->data);
}

GTEST_TEST(BitcoinTransactionTest, SmokeTest_testnet)
{
    AccountPtr account;
    HANDLE_ERROR(
            make_account(
                    BITCOIN_TEST_NET,
                    BITCOIN_ACCOUNT_P2PKH,
                    "cQeGKosJjWPn9GkB7QmvmotmBbVg1hm8UjdN6yLXEWZ5HAcRwam7",
                    reset_sp(account)));
    ASSERT_NE(nullptr, account);
    EXPECT_EQ("mzqiDnETWkunRDZxjUQ34JzN1LDevh5DpU", account->get_address());

    const PrivateKeyPtr private_key = account->get_private_key();

    const BigInt available(10000000.0_BTC);
    const BigInt dest_amount(5000000.0_BTC);
    const TransactionTemplate TEST_TX
    {
        nullptr,
        TransactionFee
        { // fee:
            1000_SATOSHI
        },
        { // Sources
            {
                available,
                from_hex("48979223adb5f7f340c4f27d6cc45a38adb37876b2d7e34d2457cbf57342a391"),
                0,
                from_hex("76a914d3f68b887224cabcc90a9581c7bbdace878666db88ac"),
                private_key.get()
            },
        },
        { // Destinations
            TransactionDestination
            {
                "mzqiDnETWkunRDZxjUQ34JzN1LDevh5DpU",
                dest_amount
            },
            TransactionChangeDestination
            {
                "mpJDSHJcytfxp9asgo2pqihabHmmJkqJuM"
            }
        }
    };
    TransactionPtr transaction = make_transaction_from_template(TEST_TX, account);

    BinaryDataPtr serialized = transaction->serialize();
    ASSERT_NE(nullptr, serialized);
    EXPECT_NE(0, serialized->len);
    EXPECT_NE(nullptr, serialized->data);
}

GTEST_TEST(BitcoinTransactionTest, SmokeTest_explicit_change)
{
    AccountPtr account;
    HANDLE_ERROR(
            make_account(
                    BITCOIN_TEST_NET,
                    BITCOIN_ACCOUNT_P2PKH,
                    "cQeGKosJjWPn9GkB7QmvmotmBbVg1hm8UjdN6yLXEWZ5HAcRwam7",
                    reset_sp(account)));
    ASSERT_NE(nullptr, account);
    EXPECT_EQ("mzqiDnETWkunRDZxjUQ34JzN1LDevh5DpU", account->get_address());

    const PrivateKeyPtr private_key = account->get_private_key();

    const BigInt available(10000000.0_BTC);
    const BigInt dest_amount(5000000.0_BTC);
    const BigInt fee_per_byte(1000_SATOSHI);

    const TransactionTemplate TEST_TX
    {
        nullptr,
        TransactionFee
        { // fee:
            1000_SATOSHI
        },
        { // Sources
            {
                available,
                from_hex("48979223adb5f7f340c4f27d6cc45a38adb37876b2d7e34d2457cbf57342a391"),
                0,
                from_hex("76a914d3f68b887224cabcc90a9581c7bbdace878666db88ac"),
                private_key.get()
            },
        },
        { // Destinations
            TransactionDestination
            {
                "mzqiDnETWkunRDZxjUQ34JzN1LDevh5DpU",
                dest_amount
            }
        }
    };
    TransactionPtr transaction = make_transaction_from_template(TEST_TX, account);

    Properties& change = transaction->add_destination();
    change.set_property_value("address", "mpJDSHJcytfxp9asgo2pqihabHmmJkqJuM");
    change.set_property_value("is_change", 1);
    EXPECT_THROW(change.set_property_value("amount", BigInt(1)), Exception);

    transaction->get_transaction_properties()
            .set_property_value("is_replaceable", 0);

    BinaryDataPtr serialized = transaction->serialize();
    ASSERT_NE(nullptr, serialized);
    EXPECT_NE(0, serialized->len);
    EXPECT_NE(nullptr, serialized->data);

    // NOTE: this would not work for SegWit transactions.
    const BigInt expected_total_fee = static_cast<uint64_t>(serialized->len) * fee_per_byte;

    const double delta_factor = 0.00001;

    // check that actual fee is within delta of value set by user.
    const BigInt delta(static_cast<uint64_t>(expected_total_fee.get<uint64_t>() * delta_factor));
    EXPECT_PRED3(is_between,
            expected_total_fee - delta,
            transaction->get_total_fee(),
            expected_total_fee + delta);

    BigInt change_amount;
    change.get_property_value("amount", &change_amount);

    ASSERT_LT(0, change_amount);
    EXPECT_PRED3(is_between,
            available - dest_amount - static_cast<uint64_t>(expected_total_fee.get<uint64_t>() * (1 + delta_factor)),
            change_amount,
            available - dest_amount - static_cast<uint64_t>(expected_total_fee.get<uint64_t>() * (1 - delta_factor)));

    ASSERT_GE(available - dest_amount - expected_total_fee, change_amount);
}

GTEST_TEST(BitcoinTransactionTest, Unprofitable_change)
{
    AccountPtr account;
    HANDLE_ERROR(
            make_account(
                    BITCOIN_TEST_NET,
                    BITCOIN_ACCOUNT_P2PKH,
                    "cQeGKosJjWPn9GkB7QmvmotmBbVg1hm8UjdN6yLXEWZ5HAcRwam7",
                    reset_sp(account)));
    ASSERT_NE(nullptr, account);
    EXPECT_EQ("mzqiDnETWkunRDZxjUQ34JzN1LDevh5DpU", account->get_address());

    TransactionPtr transaction;
    HANDLE_ERROR(make_transaction(account.get(), reset_sp(transaction)));
    ASSERT_NE(nullptr, transaction);

    const BigInt available(10000_SATOSHI);
    const BigInt out_without_change(9709_SATOSHI);
    const BigInt out_with_change(9000_SATOSHI);
    const BigInt fee_per_byte_zero(0_SATOSHI);
    const BigInt fee_per_byte_one(1_SATOSHI);

    {
        Properties& source = transaction->add_source();
        source.set_property_value("amount", available);
        source.set_property_value("prev_tx_hash",
                as_binary_data(from_hex("6e555ea9f989b755802ee69b3b7cf83777f1248fb2a678f148f71e674951cffa")));
        source.set_property_value("prev_tx_out_index", 1u);
        source.set_property_value("prev_tx_out_script_pubkey",
                as_binary_data(from_hex("76a914d3f68b887224cabcc90a9581c7bbdace878666db88ac")));
        source.set_property_value("private_key",
                *account->get_private_key());
    }

    Properties& Recipient = transaction->add_destination();
    Recipient.set_property_value("address",
                                   "mpJDSHJcytfxp9asgo2pqihabHmmJkqJuM");
    Recipient.set_property_value("amount", out_without_change);

    Properties& change = transaction->add_destination();
    change.set_property_value("address", "mzqiDnETWkunRDZxjUQ34JzN1LDevh5DpU");
    change.set_property_value("is_change", 1);

    {
        Properties& fee = transaction->get_fee();
        EXPECT_THROW(fee.set_property_value("amount_per_byte", fee_per_byte_zero), Exception);
        fee.set_property_value("amount_per_byte", fee_per_byte_one);
    }

    transaction->get_transaction_properties()
            .set_property_value("is_replaceable", 0);

    BinaryDataPtr serialized_without_change = transaction->serialize();
    ASSERT_NE(nullptr, serialized_without_change);
    EXPECT_NE(0, serialized_without_change->len);
    EXPECT_NE(nullptr, serialized_without_change->data);

    BigInt change_amount;
    change.get_property_value("amount", &change_amount);
    ASSERT_EQ("0", change_amount);

    //NOTE: if change address amount == 0 serialize without change address
    //      TXid: d58e823d1d34c610bdaa561fc342c17a007615b1e0e01a7628cdcb2e1d91b965
    ASSERT_EQ(as_binary_data(from_hex(
            "0100000001facf5149671ef748f178a6b28f24f17737f87c3b9be62e8055b789f9a95e556e010000006b483045022100cea16d07d1b421e612d9a9656ec3a1207adf171039c315eca9e6d54337a8da1002206e95cfa55bb38167726e1c915f8121fdbaf9b310fb2137e2970684f4ea6a5861012102163387c2c86f897b8aef15ee24e1f135da70c52e7dde12c06e122891c704d694ffffffff01ed250000000000001976a91460505d4554b5f7b939142cf1efa566d95a31268788ac00000000")),
            *serialized_without_change);
    const BigInt fee_without_change = transaction->get_total_fee();

    // Change destination amount for core lib create Tx with change destination
    Recipient.set_property_value("amount", out_with_change);
    BinaryDataPtr serialized_with_change = transaction->serialize();
    ASSERT_GT(serialized_with_change->len, serialized_without_change->len);

    change.get_property_value("amount", &change_amount);
    ASSERT_NE("0", change_amount);

    const BigInt fee_with_change = transaction->get_total_fee();
    ASSERT_GT(fee_without_change, fee_with_change);

    // NOTE: Serrialize check. Add checking amount per byte > 1 satoshi
    ASSERT_GE(fee_without_change, ((uint64_t)serialized_without_change->len * fee_per_byte_one));
    ASSERT_GE(fee_with_change, ((uint64_t)serialized_with_change->len * fee_per_byte_one));
}

GTEST_TEST(BitcoinTransactionTest, SmokeTest_testnet2)
{
    AccountPtr account;
    HANDLE_ERROR(
            make_account(
                    BITCOIN_TEST_NET,
                    BITCOIN_ACCOUNT_P2PKH,
                    "cQeGKosJjWPn9GkB7QmvmotmBbVg1hm8UjdN6yLXEWZ5HAcRwam7",
                    reset_sp(account)));
    ASSERT_NE(nullptr, account);
    EXPECT_EQ("mzqiDnETWkunRDZxjUQ34JzN1LDevh5DpU", account->get_address());

    const PrivateKeyPtr private_key = account->get_private_key();

    const BigInt available(10000000.0_BTC);
    const BigInt dest_amount(1.29_BTC);

    const TransactionTemplate TEST_TX
    {
        nullptr,
        TransactionFee
        { // fee:
            0.01_BTC
        },
        { // Sources
            {
                available,
                from_hex("48979223adb5f7f340c4f27d6cc45a38adb37876b2d7e34d2457cbf57342a391"),
                0,
                from_hex("76a914d3f68b887224cabcc90a9581c7bbdace878666db88ac"),
                private_key.get()
            },
        },
        { // Destinations
            TransactionDestination
            {
                "mzqiDnETWkunRDZxjUQ34JzN1LDevh5DpU",
                dest_amount
            }
        }
    };
    TransactionPtr transaction = make_transaction_from_template(TEST_TX, account);

    transaction->get_transaction_properties()
            .set_property_value("is_replaceable", 0);

    const BinaryDataPtr serialied = transaction->serialize();
    // TODO: should re-serializing (and re-signing) produce same result ?
    const BinaryDataPtr serialied2 = transaction->serialize();

    ASSERT_EQ(*serialied, *serialied2);
    ASSERT_EQ(as_binary_data(from_hex(
            "010000000191a34273f5cb57244de3d7b27678b3ad385ac46c7df2c440f3f7b5ad23929748000000006a473044022064d09103c9d48c8b094db03227621ced41732a74963578d3495bac4f7f65b40e02201f2f7adf872c1de2af5027edefdf29379faf9fe8f5751015c974e064a9d9d6e0012102163387c2c86f897b8aef15ee24e1f135da70c52e7dde12c06e122891c704d694ffffffff014062b007000000001976a914d3f68b887224cabcc90a9581c7bbdace878666db88ac00000000")),
            *serialied);
}

GTEST_TEST(BitcoinTransactionTest, SmokeTest_testnet2_with_key_to_source)
{
    AccountPtr account;
    HANDLE_ERROR(
            make_account(
                    BITCOIN_TEST_NET,
                    BITCOIN_ACCOUNT_P2PKH,
                    "cQeGKosJjWPn9GkB7QmvmotmBbVg1hm8UjdN6yLXEWZ5HAcRwam7",
                    reset_sp(account)));
    ASSERT_NE(nullptr, account);
    EXPECT_EQ("mzqiDnETWkunRDZxjUQ34JzN1LDevh5DpU", account->get_address());

    const PrivateKeyPtr private_key = account->get_private_key();

    const BigInt available(1.3_BTC);
    const BigInt dest_amount(1.29_BTC);
    const BigInt fee_per_byte(1_SATOSHI);
    const TransactionTemplate TEST_TX
    {
        nullptr,
        TransactionFee
        { // fee:
            fee_per_byte
        },
        { // Sources
            {
                available,
                from_hex("48979223adb5f7f340c4f27d6cc45a38adb37876b2d7e34d2457cbf57342a391"),
                0,
                from_hex("76a914d3f68b887224cabcc90a9581c7bbdace878666db88ac"),
                private_key.get()
            },
        },
        { // Destinations
            TransactionDestination
            {
                "mzqiDnETWkunRDZxjUQ34JzN1LDevh5DpU",
                dest_amount
            }
        }
    };
    TransactionPtr transaction = make_transaction_from_template(TEST_TX, account);

    transaction->get_transaction_properties()
            .set_property_value("is_replaceable", 0);

    const BinaryDataPtr serialied = transaction->serialize();
    {
        //Multy-core don't return excess fee to change addres
        //This test check what estimated fee falls into between +25% -5% real size
        BigInt estimated_fee = transaction->estimate_total_fee(1,1);

        // if we have uncompressed format public key, mistake about +25%
        uint64_t max_total_fee = serialied->len * fee_per_byte.get_value_as_uint64() * 1.25;
        uint64_t min_total_fee = serialied->len * fee_per_byte.get_value_as_uint64() * 0.95;
        EXPECT_LE(min_total_fee, estimated_fee.get_value_as_uint64());
        EXPECT_GE(max_total_fee, estimated_fee.get_value_as_uint64());
    }

    const char* TX = "010000000191a34273f5cb57244de3d7b27678b3ad385ac46c7df2c440f3f7b5ad23929748000000006a473044022064d09103c9d48c8b094db03227621ced41732a74963578d3495bac4f7f65b40e02201f2f7adf872c1de2af5027edefdf29379faf9fe8f5751015c974e064a9d9d6e0012102163387c2c86f897b8aef15ee24e1f135da70c52e7dde12c06e122891c704d694ffffffff014062b007000000001976a914d3f68b887224cabcc90a9581c7bbdace878666db88ac00000000";
    ASSERT_EQ(as_binary_data(from_hex(TX)), *serialied);

    ConstCharPtr serialized_encoded;
    HANDLE_ERROR(transaction_serialize_encoded(transaction.get(),
            reset_sp(serialized_encoded)));
    ASSERT_STREQ(serialized_encoded.get(), TX);
}

GTEST_TEST(BitcoinTransactionTest, SmokeTest_testnet3)
{
    AccountPtr account;

    HANDLE_ERROR(make_account(
                    BITCOIN_TEST_NET,
                    BITCOIN_ACCOUNT_P2PKH,
                    "cQeGKosJjWPn9GkB7QmvmotmBbVg1hm8UjdN6yLXEWZ5HAcRwam7",
                    reset_sp(account)));
    ASSERT_NE(nullptr, account);
    EXPECT_EQ("mzqiDnETWkunRDZxjUQ34JzN1LDevh5DpU", account->get_address());

    const PrivateKeyPtr private_key = account->get_private_key();

    const BigInt available(12.0_BTC);
    const BigInt dest_amount(1.0_BTC);
    const BigInt change_value(9.99_BTC);

    const TransactionTemplate TEST_TX
    {
        nullptr,
        TransactionFee
        { // fee:
            1_SATOSHI
        },
        { // Sources
            {
                available,
                from_hex("13ae654ae5609bd74ee1840fb5e4694580659e4cfe477b303e68162f20a81cda"),
                1,
                from_hex("76a914d3f68b887224cabcc90a9581c7bbdace878666db88ac"),
                private_key.get()
            },
        },
        { // Destinations
            TransactionDestination
            {
                "mfgq7S1Va1GREFgN66MVoxX35X6juKov6A",
                dest_amount
            },
            TransactionDestination
            {
                "mk6a6qeXNXuQDpA4DPxuouTJJTeFYJAkep",
                dest_amount
            },
            TransactionDestination
            {
                "mzqiDnETWkunRDZxjUQ34JzN1LDevh5DpU",
                change_value
            },
        }
    };
    TransactionPtr transaction = make_transaction_from_template(TEST_TX, account);

    transaction->get_transaction_properties()
            .set_property_value("is_replaceable", 0);

    const char* TX = "0100000001da1ca8202f16683e307b47fe4c9e65804569e4b50f84e14ed79b60e54a65ae13010000006b483045022100e217cfb5920878da55069a919029ab910ff106cfb20fd901e82de041b149d71902202756c5700377294837893cca854e60b6cca86423f4407b8faf92ff898aded00a012102163387c2c86f897b8aef15ee24e1f135da70c52e7dde12c06e122891c704d694ffffffff0300e1f505000000001976a91401de29d6f0aaf3467da7881a981c5c5ef90258bd88ac00e1f505000000001976a914323c1ea8756feaaaa85d0d0e51b0cc07b4c7ac5e88acc0878b3b000000001976a914d3f68b887224cabcc90a9581c7bbdace878666db88ac00000000";
    const BinaryDataPtr serialied = transaction->serialize();
    ASSERT_EQ(as_binary_data(from_hex(TX)), *serialied);

    ConstCharPtr serialized_encoded;
    HANDLE_ERROR(transaction_serialize_encoded(transaction.get(),
            reset_sp(serialized_encoded)));
    ASSERT_STREQ(serialized_encoded.get(), TX);
}

GTEST_TEST(BitcoinTransactionTest, SmokeTest_with_many_input_from_one_addreses_testnet)
{
    AccountPtr account;
    HANDLE_ERROR(make_account(
                    BITCOIN_TEST_NET,
                     BITCOIN_ACCOUNT_P2PKH,
                    "cQeGKosJjWPn9GkB7QmvmotmBbVg1hm8UjdN6yLXEWZ5HAcRwam7",
                    reset_sp(account)));
    ASSERT_NE(nullptr, account);
    EXPECT_EQ("mzqiDnETWkunRDZxjUQ34JzN1LDevh5DpU", account->get_address());

    const PrivateKeyPtr private_key = account->get_private_key();

    const BigInt available1(2.29999999_BTC);
    const BigInt available2(4.59999999_BTC);
    const BigInt dest_amount(1.0_BTC);
    const BigInt change_value(5.88999998_BTC);

    const TransactionTemplate TEST_TX
    {
        nullptr,
        TransactionFee
        { // fee:
            100_SATOSHI
        },
        { // Sources
            TransactionSource
            {
                available1,
                from_hex("4c0a9df13d1d85d20bfc5bb5d38937290d273b7655ff3d50d43db81900546f8a"),
                0,
                from_hex("76a914d3f68b887224cabcc90a9581c7bbdace878666db88ac"),
                private_key.get()
            },
            TransactionSource
            {
                available2,
                from_hex("c51b8890ad84fab4577785908d12b6f8195c69efe5a348fc7d6d88fc1ce97d17"),
                0,
                from_hex("76a914d3f68b887224cabcc90a9581c7bbdace878666db88ac"),
                private_key.get()
            }
        },
        { // Destinations
            TransactionDestination
            {
                "mfgq7S1Va1GREFgN66MVoxX35X6juKov6A",
                dest_amount
            },
            TransactionDestination
            {
                "mzqiDnETWkunRDZxjUQ34JzN1LDevh5DpU",
                change_value
            }
        }
    };
    TransactionPtr transaction = make_transaction_from_template(TEST_TX, account);

    transaction->get_transaction_properties()
            .set_property_value("is_replaceable", 0);

    const BinaryDataPtr serialied = transaction->serialize();
    ASSERT_EQ(as_binary_data(from_hex("01000000028a6f540019b83dd4503dff55763b270d293789d3b55bfc0bd2851d3df19d0a4c000000006a4730440220661ae5dd08bb4576a04c76114e980d9431c0d2f477dd8c71acfb7f77c8dd1670022022037a1939ae556881a93f41efcf7c3da756b7141b7245971c7337e4a859623d012102163387c2c86f897b8aef15ee24e1f135da70c52e7dde12c06e122891c704d694ffffffff177de91cfc886d7dfc48a3e5ef695c19f8b6128d90857757b4fa84ad90881bc5000000006a4730440220757edec6ee1fbc52c9046dc80c618001a9a7d4162f0a3abf81f27f09005f77e70220042c890f4702dda612883ceb25db4590c1f8459b0bdd88cd9858d4b7565e997b012102163387c2c86f897b8aef15ee24e1f135da70c52e7dde12c06e122891c704d694ffffffff0200e1f505000000001976a91401de29d6f0aaf3467da7881a981c5c5ef90258bd88ac3e6d1b23000000001976a914d3f68b887224cabcc90a9581c7bbdace878666db88ac00000000")),
            *serialied);
}

GTEST_TEST(BitcoinTransactionTest, SmokeTest_with_many_input_from_different_addreses_testnet)
{
    AccountPtr account1;
    AccountPtr account;

    HANDLE_ERROR(
            make_account(
                    BITCOIN_TEST_NET,
                    BITCOIN_ACCOUNT_P2PKH,
                    "cScuLx5taDyuAfCnin5WWZz65yGCHMuuaFv6mgearmqAHC4p53sz",
                    reset_sp(account)));
    ASSERT_NE(nullptr, account);
    EXPECT_EQ("mfgq7S1Va1GREFgN66MVoxX35X6juKov6A", account->get_address());

    HANDLE_ERROR(
            make_account(
                    BITCOIN_TEST_NET,
                    BITCOIN_ACCOUNT_P2PKH,
                    "cVbMJKcfEGi4wgsN39rMPkYVAaLeRaPPbrPpJfcH9B9dZCPbS7kT",
                    reset_sp(account1)));
    ASSERT_NE(nullptr, account1);
    EXPECT_EQ("mk6a6qeXNXuQDpA4DPxuouTJJTeFYJAkep", account1->get_address());

    const PrivateKeyPtr private_key = account->get_private_key();
    const PrivateKeyPtr private_key1 = account1->get_private_key();

    const BigInt available1(1.0_BTC);
    const BigInt available2(1.0_BTC);
    const BigInt dest_amount(1.9_BTC);
    const BigInt change_value(0.09_BTC);

    const TransactionTemplate TEST_TX
    {
        nullptr,
        TransactionFee
        { // fee:
            100_SATOSHI
        },
        { // Sources
            TransactionSource
            {
                available1,
                from_hex("a1fdb0d8776cfd43b66cfc0ee49cad2763fdbeca67af8ef40479624716ea8948"),
                1,
                from_hex("76a914323c1ea8756feaaaa85d0d0e51b0cc07b4c7ac5e88ac"),
                private_key1.get()
            },
            TransactionSource
            {
                available2,
                from_hex("a1fdb0d8776cfd43b66cfc0ee49cad2763fdbeca67af8ef40479624716ea8948"),
                0,
                from_hex("76a91401de29d6f0aaf3467da7881a981c5c5ef90258bd88ac"),
                private_key.get()
            }
        },
        { // Destinations
            TransactionDestination
            {
                "mzqiDnETWkunRDZxjUQ34JzN1LDevh5DpU",
                dest_amount
            },
            TransactionDestination
            {
                "mfgq7S1Va1GREFgN66MVoxX35X6juKov6A",
                change_value
            }
        }
    };
    TransactionPtr transaction = make_transaction_from_template(TEST_TX, account);

    transaction->get_transaction_properties()
            .set_property_value("is_replaceable", 0);

    const BinaryDataPtr serialied = transaction->serialize();
    ASSERT_EQ(as_binary_data(from_hex("01000000024889ea1647627904f48eaf67cabefd6327ad9ce40efc6cb643fd6c77d8b0fda1010000006a47304402200efd6929fcf32210e32194fc8468354deaf67060466710441075dab31afa31b30220350c72e95803ad14ce3fe3baa73e0a2288bf46df44e8c3d686e9692e7689cb7301210217fc7a7cc7f8b41b8e886703b95f087cd6e82ccbe6ee2ff27101b6d69ca2e868ffffffff4889ea1647627904f48eaf67cabefd6327ad9ce40efc6cb643fd6c77d8b0fda1000000006a473044022063a2925d2693033aa9735f412258c93f80f9bf980c688fbe5634b7fd6af958f40220506064007962d15ed0473ec617f1c38c80bd82af864050bf5e406ed4cf2951cf012102a6492c6dd74e49c4b7a4bd507baac3abf25fb26b97e362c3c0cb28b91a043da2ffffffff02802b530b000000001976a914d3f68b887224cabcc90a9581c7bbdace878666db88ac40548900000000001976a91401de29d6f0aaf3467da7881a981c5c5ef90258bd88ac00000000")),
            *serialied);
}

GTEST_TEST(BitcoinTransactionTest, SmokeTest_SegWit_testnet1)
{
    AccountPtr account;

    HANDLE_ERROR(
            make_account(
                    BITCOIN_TEST_NET,
                    BITCOIN_ACCOUNT_SEGWIT,
                    "cNDJeJQZgLDzWS5yz6Pf1a9LzC26nDn6SZpJHcRc4212aGJ6NSXJ",
                    reset_sp(account)));
    ASSERT_NE(nullptr, account);
    EXPECT_EQ("2Mwd414ETKsgn4BvLdYNSkK6Qa5bboxDwes", account->get_address());

    const PrivateKeyPtr private_key = account->get_private_key();

    TransactionPtr transaction;
    throw_if_error(make_transaction(account.get(),
            reset_sp(transaction)));

    {
        Properties& fee = transaction->get_fee();
        fee.set_property_value("amount_per_byte", 12_SATOSHI);
    }

    {
        Properties& source = transaction->add_source();
        source.set_property_value("amount", 1.30049879_BTC);
        source.set_property_value("prev_tx_hash",
                as_binary_data(from_hex("b84f0713dbfbc9091d426e2e2cef3691e305746b7dec3aceaee39ec431d564ab")));
        source.set_property_value("prev_tx_out_index", 0u);
        source.set_property_value("prev_tx_out_script_pubkey",
                as_binary_data(from_hex("a9143001515d9592fa730931df154306f24ba8ece52f87")));
        source.set_property_value("private_key",
                *account->get_private_key());
        source.set_property_value("sequence", static_cast<int32_t>(0xfffffffd));
    }

    {
        Properties& destination = transaction->add_destination();

        destination.set_property_value("amount", 0.30048219_BTC);
        destination.set_property_value("address", "2MzwcmDo5WBjrjHzfYuyzsdZHmo682krVnf");
    }

    {
        Properties& destination = transaction->add_destination();

        destination.set_property_value("amount", 1.0_BTC);
        destination.set_property_value("address", "2N9YrGCkmGNzZfU3aMXqZkMw4iH2f2x9U37");
    }

    {
        Properties& properties = transaction->get_transaction_properties();
        properties.set_property_value("nlocktime", as_binary_data(from_hex("8cfd1300")));
    }

    const BinaryDataPtr serialied = transaction->serialize();
    // Txid: e2f1f958168543d25e7bc6a7897af57f21f6a016d3cf95a99ce78c112b545c07
    ASSERT_EQ(as_binary_data(from_hex(
            "02000000000101ab64d531c49ee3aece3aec7d6b7405e39136ef2c2e6e421d09c9fbdb13074fb80000000017160014b57593a5a0e96bcd52fc2f7a325fb913916db674fdffffff"
            "02db7fca010000000017a914546c87c7a5187edac7ad3fcf22dc3597ce37b1998700e1f5050000000017a914b2d75d4297ba8de492351d4701cee065e9c852ce87024730440220"
            "666aa02ce08113b43422b73b3d33cf3c40e86a77c8876d9c1e9a664982febc9802206f4c2d836a77e4c7f09851e1c534da3cedc6e70fb915db7a77180e07246b42eb012103295d"
            "829f209b8b1e7c56029f20f8440382be6e43c7b03ae637ad4327f36fb2ab8cfd1300")), *serialied);
}

GTEST_TEST(BitcoinTransactionTest, SmokeTest_SegWit_testnet2)
{
    AccountPtr account;
    AccountPtr account1;

    HANDLE_ERROR(
            make_account(
                    BITCOIN_TEST_NET,
                    BITCOIN_ACCOUNT_SEGWIT,
                    "cRBMwm5iA2yV99cUBxSz39vKp5CmE7eZ9rjXPAPz8S2kbgf5nf3i",
                    reset_sp(account)));
    ASSERT_NE(nullptr, account);
    EXPECT_EQ("2MzwcmDo5WBjrjHzfYuyzsdZHmo682krVnf", account->get_address());

    HANDLE_ERROR(
            make_account(
                    BITCOIN_TEST_NET,
                    BITCOIN_ACCOUNT_SEGWIT,
                    "cSBjA9u8YhQfDX4Yj2DyrgzQ1xhu5w4h9HxkHttMv4D6aWL1zyMm",
                    reset_sp(account1)));
    ASSERT_NE(nullptr, account1);
    EXPECT_EQ("2N9YrGCkmGNzZfU3aMXqZkMw4iH2f2x9U37", account1->get_address());

    TransactionPtr transaction;
    throw_if_error(make_transaction(account.get(),
            reset_sp(transaction)));

    {
        Properties& source = transaction->add_source();
        source.set_property_value("amount", 0.30048219_BTC);
        source.set_property_value("prev_tx_hash",
                as_binary_data(from_hex("e2f1f958168543d25e7bc6a7897af57f21f6a016d3cf95a99ce78c112b545c07")));
        source.set_property_value("prev_tx_out_index", 0u);
        source.set_property_value("prev_tx_out_script_pubkey",
                as_binary_data(from_hex("a914546c87c7a5187edac7ad3fcf22dc3597ce37b19987")));
        source.set_property_value("private_key",
                *account->get_private_key());
        source.set_property_value("sequence", static_cast<int32_t>(0xfffffffd));
    }

    {
        Properties& source = transaction->add_source();
        source.set_property_value("amount", 1.0_BTC);
        source.set_property_value("prev_tx_hash",
                as_binary_data(from_hex("e2f1f958168543d25e7bc6a7897af57f21f6a016d3cf95a99ce78c112b545c07")));
        source.set_property_value("prev_tx_out_index", 1u);
        source.set_property_value("prev_tx_out_script_pubkey",
                as_binary_data(from_hex("a914b2d75d4297ba8de492351d4701cee065e9c852ce87")));
        source.set_property_value("private_key",
                *account1->get_private_key());
        source.set_property_value("sequence",static_cast<int32_t>(0xfffffffd));
    }

    {
        Properties& destination = transaction->add_destination();

        destination.set_property_value("amount", 1.30002819_BTC);
        destination.set_property_value("address", "n46Gz2w1q8qtrGHc6yQbHNYRccoVYtzGt4");
    }

    {
        Properties& properties = transaction->get_transaction_properties();
        properties.set_property_value("nlocktime", as_binary_data(from_hex("a8fd1300")));
    }
    {
        Properties& fee = transaction->get_fee();
        fee.set_property_value("amount_per_byte", 263_SATOSHI);
    }

    const BinaryDataPtr serialied = transaction->serialize();
    // Txid: 94d365be6dfaf9d1743e29d59b689f3c158ba48cbc7a215fc0a6e5eef2681658
    ASSERT_EQ(as_binary_data(from_hex(
            "02000000000102075c542b118ce79ca995cfd316a0f6217ff57a89a7c67b5ed243851658f9f1e200000000171600143b8fce371ebdc9edd882f5b530b84b55bbf3462ffdffffff"
            "075c542b118ce79ca995cfd316a0f6217ff57a89a7c67b5ed243851658f9f1e20100000017160014d48c68dfbe25632ef3a2880c617dbf6b9724d14bfdffffff0183afbf070000"
            "00001976a914f7a0185427d6b08bc3c4296a434f0ee6c623bc8f88ac024830450221009684cb807eebe8d336af9bd88431764816aecd5d291563780fe69a45407fb0c202201bd9"
            "c3d17de88aeb90c2eb2b16ed24e459a25c88b7e595296943ed9d3662ffde012103ae1151592a23cd42edf749cde0f654722087ed7199c78422c187fe2644d91e3a024830450221"
            "008e52bf2d0d45ddcae79a39707635acaef5995725668f6a79150bdd19e89baa2702205a0758d92e00d1b2df5deb070e07a851bbc10e66f2f82205f71197a52faea6b30121026f"
            "6932ebbdff50f3424ee1e86889710b773fd6cd38fe5d89b1b29646ac9e4cb4a8fd1300")), *serialied);
}

GTEST_TEST(BitcoinTransactionTest, SmokeTest_SegWit_mainnet1)
{
    AccountPtr account;

    HANDLE_ERROR(
            make_account(
                    BITCOIN_MAIN_NET,
                    BITCOIN_ACCOUNT_SEGWIT,
                    "L4or2WoyKWhCk8jJsUBm82AY4qvdnrmxSXKhEMB5NT6hdTD7ZPn1",
                    reset_sp(account)));
    ASSERT_NE(nullptr, account);
    EXPECT_EQ("3PBYzxFsYExpSDAg4HnqN9iJJEQY8GuKqi", account->get_address());


    TransactionPtr transaction;
    throw_if_error(make_transaction(account.get(),
            reset_sp(transaction)));

    {
        Properties& source = transaction->add_source();
        source.set_property_value("amount", 60736_SATOSHI);
        source.set_property_value("prev_tx_hash",
                as_binary_data(from_hex("dda79578adf5283ab22747e0c519248c1e0a1a102ce01833c5bfba2e07ae6c75")));
        source.set_property_value("prev_tx_out_index", 0u);
        source.set_property_value("prev_tx_out_script_pubkey",
                as_binary_data(from_hex("a914ebc03ab6e3c5cf72226b9f83d2d5bb42dfc6d9d087")));
        source.set_property_value("private_key",
                *account->get_private_key());
        source.set_property_value("sequence", static_cast<int32_t>(0xfffffffd));
    }

    {
        Properties& destination = transaction->add_destination();

        destination.set_property_value("amount", 29906_SATOSHI);
        destination.set_property_value("address", "3E8rBvkT3phgqA1DtnL9QDsKrUd4HFjvry");
    }

    {
        Properties& destination = transaction->add_destination();

        destination.set_property_value("amount", 30000_SATOSHI);
        destination.set_property_value("address", "37yb1X1QyuGeXyHDXqwy5RgHjs7MYdSN18");
    }

    {
        Properties& properties = transaction->get_transaction_properties();
        properties.set_property_value("nlocktime", as_binary_data(from_hex("b3fe0700")));
    }
    {
        Properties& fee = transaction->get_fee();
        fee.set_property_value("amount_per_byte", 6_SATOSHI);
    }

    const BinaryDataPtr serialied = transaction->serialize();
    // Txid: e8a561cad92f3f0682688612dca308595a34f472e7a749bc0e45bf0cabdd5dfa
    ASSERT_EQ(as_binary_data(from_hex(
            "02000000000101756cae072ebabfc53318e02c101a0a1e8c2419c5e04727b23a28f5ad7895a7dd0000000017160014b1746f8fd5e518a09567e471c0f14553a4bbca4afdffffff"
            "02d27400000000000017a914888432bff12b8bb8952ae0f862e340671e3870a487307500000000000017a91444f30758be7440343c34ea01531fa5f3314ef71b87024830450221"
            "0087d3ff620a64b4ebf4be70d7918b19b09ded519e44b99df034e15d844e6c27f002207685f8f3e280f638c4e72c356602284b8ef9eef07a3c7579b42427567425822d012102cf"
            "38a3306c3a30e08575f6a4651c17e62a6206487dfda800dca9cd265b5eab35b3fe0700")), *serialied);
}

GTEST_TEST(BitcoinTransactionTest, SmokeTest_SegWit_mainnet2)
{
    AccountPtr account;
    AccountPtr account1;

    HANDLE_ERROR(
            make_account(
                    BITCOIN_MAIN_NET,
                    BITCOIN_ACCOUNT_SEGWIT,
                    "L3aeJKV7YEfpSUC6KNfbKPtXYNmFrjqG5E1WwuvsZgWU7wyZwJ4B",
                    reset_sp(account)));
    ASSERT_NE(nullptr, account);
    EXPECT_EQ("3E8rBvkT3phgqA1DtnL9QDsKrUd4HFjvry", account->get_address());

    HANDLE_ERROR(
            make_account(
                    BITCOIN_MAIN_NET,
                    BITCOIN_ACCOUNT_SEGWIT,
                    "L3jREKUzn4v1eJXbfowNYAm9HknYSEyjFKRbBmFGuTxmQA8cEC28",
                    reset_sp(account1)));
    ASSERT_NE(nullptr, account1);
    EXPECT_EQ("37yb1X1QyuGeXyHDXqwy5RgHjs7MYdSN18", account1->get_address());



    TransactionPtr transaction;
    throw_if_error(make_transaction(account.get(),
            reset_sp(transaction)));

    {
        Properties& source = transaction->add_source();
        source.set_property_value("amount", 29906_SATOSHI);
        source.set_property_value("prev_tx_hash",
                as_binary_data(from_hex("e8a561cad92f3f0682688612dca308595a34f472e7a749bc0e45bf0cabdd5dfa")));
        source.set_property_value("prev_tx_out_index", 0u);
        source.set_property_value("prev_tx_out_script_pubkey",
                as_binary_data(from_hex("a914888432bff12b8bb8952ae0f862e340671e3870a487")));
        source.set_property_value("private_key",
                *account->get_private_key());
        source.set_property_value("sequence", static_cast<int32_t>(0xfffffffd));
    }

    {
        Properties& source = transaction->add_source();
        source.set_property_value("amount", 30000_SATOSHI);
        source.set_property_value("prev_tx_hash",
                as_binary_data(from_hex("e8a561cad92f3f0682688612dca308595a34f472e7a749bc0e45bf0cabdd5dfa")));
        source.set_property_value("prev_tx_out_index", 1u);
        source.set_property_value("prev_tx_out_script_pubkey",
                as_binary_data(from_hex("a91444f30758be7440343c34ea01531fa5f3314ef71b87")));
        source.set_property_value("private_key",
                *account1->get_private_key());
        source.set_property_value("sequence", static_cast<int32_t>(0xfffffffd));
    }

    {
        Properties& destination = transaction->add_destination();

        destination.set_property_value("amount", 58998_SATOSHI);
        destination.set_property_value("address", "1DaBnJ28DocZBxPy7g6Lv8W6Yhcu98YSrE");
    }

    {
        Properties& properties = transaction->get_transaction_properties();
        properties.set_property_value("nlocktime", as_binary_data(from_hex("b5fe0700")));
    }

    {
        Properties& fee = transaction->get_fee();
        fee.set_property_value("amount_per_byte", 5_SATOSHI);
    }


    const BinaryDataPtr serialied = transaction->serialize();
    // Txid: 40154d4a6b857a661c5163ed97d3c27d139a2a05edce6435e039f3e2244d0db8
    ASSERT_EQ(as_binary_data(from_hex(
            "02000000000102fa5dddab0cbf450ebc49a7e772f4345a5908a3dc12866882063f2fd9ca61a5e800000000171600147d17b05e8f91b94d68a4808ac5a90ae9adab8e04fdffffff"
            "fa5dddab0cbf450ebc49a7e772f4345a5908a3dc12866882063f2fd9ca61a5e80100000017160014e2277180898c1b2a2c728a1ba2010d60025b18eefdffffff0176e600000000"
            "00001976a91489e83c5090456162cc2f1afd408ee9aa815fd6cb88ac024830450221008fbd3fb24ae0879684b4d7e85584780e73473306c130df4d053e60a2eb9c6436022037e8"
            "777c9782fd63593d956e4f52f1c777c5730a619a2546695ffdb5c32b768501210256a77decb929792841e6a686b825d8d516c4d60146de3c8e32329fdfbd81238f024730440220"
            "256368911e9d95faabff6e1c93904753b4e50485597db6d59d8ffd9a3a8296f5022046629cdba5fbb76500c92c26e06cf8403cbdf710f3936c6419deba9f92abe6ec012103368d"
            "e58420371eb97cc574f2870cc0c88abfb699c5a87f28bb6bbf50b55f158fb5fe0700")), *serialied);
}

GTEST_TEST(BitcoinTransactionTest, transaction_update)
{
    // Verify that transaction_update() modifies TX internal state.
    const AccountPtr account = make_account(BITCOIN_TEST_NET,
            "cQeGKosJjWPn9GkB7QmvmotmBbVg1hm8UjdN6yLXEWZ5HAcRwam7");

    TransactionPtr transaction = make_transaction_from_template(
            DEFAULT_TX_TEMPLATE,
            account,
            account->get_private_key());

    // Single destination spends only portion of data, and the change is not updated
    // untill you invoke transaction_update(), hence the fee is really high.
    BigIntPtr total_fee;
    HANDLE_ERROR(transaction_get_total_fee(transaction.get(), reset_sp(total_fee)));
    EXPECT_NE("0", *total_fee);

    // Change is going to be updated and next call to transaction_get_total_fee()
    // should return fee that is much smaller.
    HANDLE_ERROR(transaction_update(transaction.get()));

    BigIntPtr updated_total_fee;
    HANDLE_ERROR(transaction_get_total_fee(transaction.get(), reset_sp(updated_total_fee)));
    EXPECT_NE("0", *updated_total_fee);

    // Verifying that excess value was moved to the change address and fee is minimized.
    EXPECT_LT(*updated_total_fee, *total_fee);

    BinaryDataPtr serialized;
    HANDLE_ERROR(transaction_serialize(transaction.get(), reset_sp(serialized)));

    // Verifying that fee is within some bounds of user set value.
    const double delta = 0.0001;
    EXPECT_PRED3(is_between,
            DEFAULT_TX_TEMPLATE.fee.amount_per_byte * static_cast<int64_t>(serialized->len * (1 - delta)),
            *updated_total_fee,
            DEFAULT_TX_TEMPLATE.fee.amount_per_byte * static_cast<int64_t>(serialized->len * (1 + delta))
    );
}

GTEST_TEST(BitcoinTransactionTest, transaction_get_total_spent)
{
    const AccountPtr account = make_account(BITCOIN_TEST_NET,
            "cQeGKosJjWPn9GkB7QmvmotmBbVg1hm8UjdN6yLXEWZ5HAcRwam7");

    const BigInt available(0.01_BTC);
    const BigInt sent(10000_SATOSHI);

    TransactionTemplate TEST_TX = DEFAULT_TX_TEMPLATE;
    TEST_TX.sources[0].available = available;
    TEST_TX.destinations[0].amount = sent;
    TEST_TX.destinations.erase(TEST_TX.destinations.end() - 1); // change is last

    TransactionPtr transaction = make_transaction_from_template(
            TEST_TX, account, account->get_private_key());

    Properties& change = transaction->add_destination();
    change.set_property_value("address", "mfgq7S1Va1GREFgN66MVoxX35X6juKov6A");
    change.set_property_value("is_change", 1);

    HANDLE_ERROR(transaction_update(transaction.get()));

    BigInt change_value;
    change.get_property_value("amount", &change_value);

    BigIntPtr total_fee;
    HANDLE_ERROR(transaction_get_total_fee(transaction.get(), reset_sp(total_fee)));
    EXPECT_NE(nullptr, total_fee);

    BigIntPtr total_spent;
    HANDLE_ERROR(transaction_get_total_spent(transaction.get(), reset_sp(total_spent)));
    EXPECT_NE(nullptr, total_spent);

    ASSERT_EQ(*total_spent, *total_fee + sent);
    ASSERT_EQ(*total_spent, available - change_value);
}

GTEST_TEST(BitcoinTransactionTest, invalid_script_pubkey)
{
    const AccountPtr account = make_account(BITCOIN_TEST_NET,
            "cQeGKosJjWPn9GkB7QmvmotmBbVg1hm8UjdN6yLXEWZ5HAcRwam7");

    TransactionTemplate TEST_TX = DEFAULT_TX_TEMPLATE;
    {
        // First, check that TX is Ok and can be serialized.
        TransactionPtr transaction = make_transaction_from_template(
                    TEST_TX, account, account->get_private_key());
        BinaryDataPtr serialized_tx;
        HANDLE_ERROR(transaction_serialize(transaction.get(), reset_sp(serialized_tx)));
    }

    // Add extra byte
    const auto original_script_pubkey = TEST_TX.sources[0].prev_tx_scrip_pubkey;
    for (int i = 0; i < original_script_pubkey.size(); ++i)
    {
        auto& script_pubkey = TEST_TX.sources[0].prev_tx_scrip_pubkey;
        script_pubkey.insert(script_pubkey.begin() + i, 0xFF);
        TransactionPtr transaction = make_transaction_from_template(
                    TEST_TX, account, account->get_private_key());

        BinaryDataPtr serialized_tx;
        EXPECT_ERROR(transaction_serialize(transaction.get(), reset_sp(serialized_tx)));
    }

    // Replace byte
    for (int i = 0; i < original_script_pubkey.size(); ++i)
    {
        TEST_TX.sources[0].prev_tx_scrip_pubkey = original_script_pubkey;
        auto& script_pubkey = TEST_TX.sources[0].prev_tx_scrip_pubkey;
        script_pubkey[i] += 1;
        TransactionPtr transaction = make_transaction_from_template(
                    TEST_TX, account, account->get_private_key());

        BinaryDataPtr serialized_tx;
        EXPECT_ERROR(transaction_serialize(transaction.get(), reset_sp(serialized_tx)));
    }

    // Erase byte
    for (int i = 0; i < original_script_pubkey.size(); ++i)
    {
        TEST_TX.sources[0].prev_tx_scrip_pubkey = original_script_pubkey;
        auto& script_pubkey = TEST_TX.sources[0].prev_tx_scrip_pubkey;
        script_pubkey.erase(script_pubkey.begin() + i);

        TransactionPtr transaction = make_transaction_from_template(
                    TEST_TX, account, account->get_private_key());

        BinaryDataPtr serialized_tx;
        EXPECT_ERROR(transaction_serialize(transaction.get(), reset_sp(serialized_tx)));
    }
}

GTEST_TEST(BitcoinTransactionTest, SmokeTest_mainnet_with_op_return)
{
    AccountPtr account;
    HANDLE_ERROR(make_account(
                    BITCOIN_MAIN_NET,
                    BITCOIN_ACCOUNT_P2PKH,
                    "5KWNESNNyn68focSAoUm3zrgnGZ4fABoWf9DccbEUHwWFhx5ouj",
                    reset_sp(account)));
    const PrivateKeyPtr private_key = account->get_private_key();

    const BigInt available(107527_SATOSHI);
    const BigInt sent(106777_SATOSHI);

    const TransactionTemplate TEST_TX
    {
        account.get(),
        TransactionFee
        { // fee:
            3_SATOSHI
        },
        { // Sources
            {
                available,
                from_hex("a0868fa9ba2dc6fcf2822b2a11cea8e37a6e381f05a1ef11ccc3abae35721454"),
                0,
                from_hex("76a91441d29eaed90c0a26021daa5139dcc1d45c4e34a188ac"),
                private_key.get()
            }
        },
        { // Destinations
            TransactionDestination
            {
                "1713EbQ9gh7kgxEVPpSqDBhV36CbFpptT8",
                sent
            }
        }
    };
    TransactionPtr transaction = make_transaction_from_template(TEST_TX);

    BinaryDataPtr message;
    HANDLE_ERROR(make_binary_data_from_hex("4d554c5459207468652062657374",
            reset_sp(message)));
    HANDLE_ERROR(transaction_set_message(transaction.get(), message.get()));

    {
        Properties* transaction_properties = nullptr;
        HANDLE_ERROR(transaction_get_properties(transaction.get(),
                &transaction_properties));
        HANDLE_ERROR(properties_set_int32_value(transaction_properties,
                "is_replaceable", 0));
    }

    const BinaryDataPtr serialied = transaction->serialize();
    EXPECT_PRED2((contains_sequence<BinaryData, BinaryData>),
            *serialied, *message);

    // Txid: a08020fecb81dff920c3a00160f519c986e1b3205d08390b71632e1046a6f885
    ASSERT_EQ(as_binary_data(from_hex(
            "010000000154147235aeabc3cc11efa1051f386e7ae3a8ce112a2b82f2fcc62dbaa98f86a0000000008b483045022100eebf874"
            "3bd2ed75e277f2d7b42302293f1700d5286c1f002009ba937be84ea810220098794c9b320158742b48c9ca94175a45bc044c9e5"
            "1c5933c2a083c775dca88b0141044c6efb7f684e02090c0f60ec67517678f706282b3bb06ed9e86e49dafefb7561fa79ac1df98"
            "beb234dba313a9f0928fb08ab7ca086a74a1ae18da0e663c0aac0ffffffff020000000000000000106a0e4d554c545920746865"
            "206265737419a10100000000001976a91441d29eaed90c0a26021daa5139dcc1d45c4e34a188ac00000000")),
            *serialied);
}

GTEST_TEST(BitcoinTransactionTest, SmokeTest_testnet_to_P2SH_addres)
{
    AccountPtr account = make_account(BITCOIN_TEST_NET, "cQeGKosJjWPn9GkB7QmvmotmBbVg1hm8UjdN6yLXEWZ5HAcRwam7");

    const BigInt available(9809_SATOSHI);
    const BigInt sent(9209_SATOSHI);

    const TransactionTemplate TEST_TX
    {
        nullptr,
        TransactionFee
        { // fee:
            3_SATOSHI
        },
        { // Sources
            {
                available,
                from_hex("e067d9e5f1d90b646be952ac8c6671c67023145e71ead7019827c39015f2da95"),
                0,
                from_hex("76a914d3f68b887224cabcc90a9581c7bbdace878666db88ac"),
                nullptr
            }
        },
        { // Destinations
            TransactionDestination
            {
                "2MwsRtyuZyW2HuPUcUKGa4dAm9M9RkUPacK",
                sent
            }
        }
    };
    TransactionPtr transaction = make_transaction_from_template(TEST_TX, account, account->get_private_key());

    transaction->get_transaction_properties()
            .set_property_value("is_replaceable", 0);


    const BinaryDataPtr serialied = transaction->serialize();

    // Txid: afc0688e06f4779fe71d45b889ddf9c867341ea2a9bc14d9133b2aa3c591c9cd
    ASSERT_EQ(as_binary_data(from_hex(
            "010000000195daf21590c3279801d7ea715e142370c671668cac52e96b640bd9f1e5d967e0000000006a473044022056a97dbfb"
            "6a72d5efea94636e01eee865953e784bdfef7dbbbcd24fd7dd73da102204e1c757134a3da11cbd91dfe8ee6c07b62a9ca1dfe5f"
            "f017f779d2a5493ccd23012102163387c2c86f897b8aef15ee24e1f135da70c52e7dde12c06e122891c704d694ffffffff01f92"
            "300000000000017a91432b96b25e20037514ce9469e86fdfad58bf2e2278700000000")),
            *serialied);
}

GTEST_TEST(BitcoinTransactionTest, SmokeTest_mainnet_to_P2SH_addres)
{
    AccountPtr account = make_account(BITCOIN_MAIN_NET, "5KHD87PD4WetNsrUfo7Z55xwWDEY2VrAh1VpfwHEV8wgbprCSxL");

    const BigInt available(115197_SATOSHI);
    const BigInt sent(114500_SATOSHI);

    const TransactionTemplate TEST_TX
    {
        nullptr,
        TransactionFee
        { // fee:
            3_SATOSHI
        },
        { // Sources
            {
                available,
                from_hex("82f2704eafa63d69a94c479153e4c1427f12110281a848fdaca951167319cb7c"),
                2,
                from_hex("76a9146b6e514825a406e84456950cafb6911095aa61f688ac"),
                nullptr
            }
        },
        { // Destinations
            TransactionDestination
            {
                "3HGwSrGh7ARwDbzr3SwmRcavk6UWJtyUd2",
                sent
            }
        }
    };
    TransactionPtr transaction = make_transaction_from_template(TEST_TX, account, account->get_private_key());
    transaction->get_transaction_properties()
            .set_property_value("is_replaceable", 0);

    const BinaryDataPtr serialied = transaction->serialize();

    // Txid: c8a8fe4e36d006e28dc55f00bbc7cfea9622b6262710973f6625f4fb9710cba4
    ASSERT_EQ(as_binary_data(from_hex(
            "01000000017ccb19731651a9acfd48a8810211127f42c1e45391474ca9693da6af4e70f282020000008b4830450221009c526a9"
            "d76f125df9b874d7803b6f72c7c116ee847338771c68919a189e666fb02203a6f6ebea415561d9846816c02d0a93596ddb92ae3"
            "57436a36745c02c92bff2f014104a7364b8ad3f02ae0a9dbe355aa3ac9b2a661e230976f505f55f66d26ea3ae906ed3bbfd24ab"
            "384fe7c15a99ae1227276629820da47f10c29fd9c9a026bc602deffffffff0144bf01000000000017a914aaf44ab6c1076eae84"
            "9bf717adf955e0456639ab8700000000")),
            *serialied);
}

GTEST_TEST(BitcoinTransactionTest, replaceable)
{
    // Verify that TX can be explicitly made FINAL or REPLACEABLE (BIP125).

    AccountPtr account = make_account(BITCOIN_MAIN_NET,
            "5KHD87PD4WetNsrUfo7Z55xwWDEY2VrAh1VpfwHEV8wgbprCSxL");

    const TransactionTemplate TEST_TX
    {
        nullptr,
        TransactionFee
        { // fee:
            3_SATOSHI
        },
        { // Sources
            {
                115197_SATOSHI,
                from_hex("82f2704eafa63d69a94c479153e4c1427f12110281a848fdaca951167319cb7c"),
                2,
                from_hex("76a9146b6e514825a406e84456950cafb6911095aa61f688ac"),
                nullptr
            }
        },
        { // Destinations
            TransactionDestination
            {
                "3HGwSrGh7ARwDbzr3SwmRcavk6UWJtyUd2",
                114500_SATOSHI
            }
        }
    };
    const unsigned char FINAL_TX_SEQ[] = {0xff, 0xff, 0xff, 0xff};

    TransactionPtr transaction = make_transaction_from_template(TEST_TX, account, account->get_private_key());

    //
    // Explicitly making TX FINAL.
    //
    transaction->get_transaction_properties()
            .set_property_value("is_replaceable", 0);

    const BinaryDataPtr final_tx = transaction->serialize();
    ASSERT_EQ(as_binary_data(from_hex(
            "01000000017ccb19731651a9acfd48a8810211127f42c1e45391474ca9693da6af4e70f282020000008b4830450221009c526a9"
            "d76f125df9b874d7803b6f72c7c116ee847338771c68919a189e666fb02203a6f6ebea415561d9846816c02d0a93596ddb92ae3"
            "57436a36745c02c92bff2f014104a7364b8ad3f02ae0a9dbe355aa3ac9b2a661e230976f505f55f66d26ea3ae906ed3bbfd24ab"
            "384fe7c15a99ae1227276629820da47f10c29fd9c9a026bc602deffffffff0144bf01000000000017a914aaf44ab6c1076eae84"
            "9bf717adf955e0456639ab8700000000")),
            *final_tx);

    EXPECT_PRED2((contains_sequence<BinaryData, BinaryData>),
            *final_tx, as_binary_data(FINAL_TX_SEQ));

    //
    // Explitily making TX REPLACEABLE
    //
    transaction->get_transaction_properties()
            .set_property_value("is_replaceable", 1);
    const BinaryDataPtr replaceable_tx = transaction->serialize();
    EXPECT_NE(*final_tx, *replaceable_tx);

    // It is hard to express negation with EXPECT_PRED2(), simplest way is:
    EXPECT_FALSE(contains_sequence(*replaceable_tx,
            as_binary_data(FINAL_TX_SEQ)));

    //
    // Turning 'is_replacible' OFF again and verifying that TX is FINAL.
    //
    transaction->get_transaction_properties()
            .set_property_value("is_replaceable", 0);
    const BinaryDataPtr final_tx2 = transaction->serialize();
    EXPECT_EQ(*final_tx, *final_tx2);
}

GTEST_TEST(BitcoinTransactionTest, SmokeTest_mainnet_dust_outputs)
{
    AccountPtr account = make_account(BITCOIN_MAIN_NET, "5KDejYL1XnGkhwhH1ubSqiFCqXGxYzuJYHXhU7UqeVLEDeqBJ22");

    const BigInt available(9809_SATOSHI);
    const BigInt dust_sent_amount(500_SATOSHI);
    const BigInt for_dust_change(8500_SATOSHI);

    const TransactionTemplate TEST_TX
    {
        nullptr,
        TransactionFee
        { // fee:
            3_SATOSHI
        },
        { // Sources
            {
                available,
                from_hex("63ebc8dd4de286a1ab32029f1986876ac981b8b9ccd597be3f2d6ac1961e02ee"),
                0,
                from_hex("76a914dda3945cee6bb677de3b090db38ef3053fc45acb88ac"),
                nullptr
            }
        }
    };
    TransactionPtr transaction = make_transaction_from_template(TEST_TX, account, account->get_private_key());

    Properties& send_destination = transaction->add_destination();
    send_destination.set_property_value("address", "1MCvJ6pqJrGJEjo55RhLhr1de2wFLoDBXF");
    send_destination.set_property_value("amount", dust_sent_amount);

    Properties& change_destination = transaction->add_destination();
    change_destination.set_property_value("address", "1Q46s1EJXDMvVTCVQGKfrjEudvgu39uXu5");
    change_destination.set_property_value("is_change", 1);

    // Destination amount considered dust, hence TX verification fails.
    EXPECT_ERROR(transaction_update(transaction.get()));

    send_destination.set_property_value("amount", for_dust_change);
    // Now change amount is dust, hence change amount should be implicitly set to 0 by TX itself.
    HANDLE_ERROR(transaction_update(transaction.get()));
    BigInt change_amount;
    change_destination.get_property_value("amount" , &change_amount);
    EXPECT_EQ(BigInt(0), change_amount);
}

GTEST_TEST(BitcoinTransactionTest, SmokeTest_zero_destination)
{
    AccountPtr account = make_account(BITCOIN_MAIN_NET, "5KDejYL1XnGkhwhH1ubSqiFCqXGxYzuJYHXhU7UqeVLEDeqBJ22");

    const TransactionTemplate TEST_TX
    {
        nullptr,
        TransactionFee
        { // fee:
            3_SATOSHI
        },
        { // Sources
            {
                1000_SATOSHI,
                from_hex("63ebc8dd4de286a1ab32029f1986876ac981b8b9ccd597be3f2d6ac1961e02ee"),
                0,
                from_hex("76a914dda3945cee6bb677de3b090db38ef3053fc45acb88ac"),
                nullptr
            }
        },
        { // Destinations
            TransactionChangeDestination
            {
                "1Q46s1EJXDMvVTCVQGKfrjEudvgu39uXu5"
            }
        }
    };
    TransactionPtr transaction = make_transaction_from_template(TEST_TX, account, account->get_private_key());

    EXPECT_ERROR(transaction_update(transaction.get()));
}

GTEST_TEST(BitcoinTransactionTest, transaction_update_empty_tx)
{
    // Verify that transaction_update() fails when called on empty TX.
    const AccountPtr account = make_account(BITCOIN_TEST_NET,
            "cQeGKosJjWPn9GkB7QmvmotmBbVg1hm8UjdN6yLXEWZ5HAcRwam7");

    TransactionPtr transaction;
    HANDLE_ERROR(make_transaction(account.get(), reset_sp(transaction)));
    EXPECT_ERROR(transaction_update(transaction.get()));

    {
        Properties* fee = nullptr;
        HANDLE_ERROR(transaction_get_fee(transaction.get(), &fee));
        EXPECT_ERROR(transaction_update(transaction.get()));
    }

    {
        Properties* source = nullptr;
        HANDLE_ERROR(transaction_add_source(transaction.get(), &source));
        EXPECT_ERROR(transaction_update(transaction.get()));
    }

    {
        Properties* destination = nullptr;
        HANDLE_ERROR(transaction_add_destination(transaction.get(), &destination));
        EXPECT_ERROR(transaction_update(transaction.get()));
    }
}

GTEST_TEST(BitcoinTransactionTest, is_replaceable_default_value)
{
    //
    // Verify that all TXs are REPLACEABLE by-default.
    //

    const AccountPtr account = make_account(BITCOIN_MAIN_NET, "5KHD87PD4WetNsrUfo7Z55xwWDEY2VrAh1VpfwHEV8wgbprCSxL");

    TransactionPtr transaction;
    HANDLE_ERROR(make_transaction(account.get(), reset_sp(transaction)));

    int32_t is_replaceable = std::numeric_limits<int32_t>::max();
    transaction->get_transaction_properties()
            .get_property_value("is_replaceable", &is_replaceable);

    ASSERT_EQ(1, is_replaceable);
}

GTEST_TEST(BitcoinTransactionTest, destination_address_verification)
{
    const AccountPtr account = make_account(BITCOIN_TEST_NET,
            "cQeGKosJjWPn9GkB7QmvmotmBbVg1hm8UjdN6yLXEWZ5HAcRwam7");

    TransactionPtr transaction;
    make_transaction(account.get(), reset_sp(transaction));
    {
        Properties* destination = nullptr;
        HANDLE_ERROR(transaction_add_destination(transaction.get(), &destination));

        EXPECT_ERROR(properties_set_string_value(destination, "address", ""));
        EXPECT_ERROR(properties_set_string_value(destination, "address", " "));
        EXPECT_ERROR(properties_set_string_value(destination, "address", "123"));
        // valid address mzqiDnETWkunRDZxjUQ34JzN1LDevh5DpU
        EXPECT_ERROR(properties_set_string_value(destination, "address", "mzqiDnETWkunRDZxjUQ34JzN1LDevh5D"));

        HANDLE_ERROR(properties_set_string_value(destination, "address", "mzqiDnETWkunRDZxjUQ34JzN1LDevh5DpU"));
    }
}

GTEST_TEST(BitcoinTransactionTest, transaction_set_destination_address_testnet)
{
    const AccountPtr account_testnet = make_account(BITCOIN_TEST_NET, "cQeGKosJjWPn9GkB7QmvmotmBbVg1hm8UjdN6yLXEWZ5HAcRwam7");

    TransactionPtr transaction_testnet;
    HANDLE_ERROR(make_transaction(account_testnet.get(), reset_sp(transaction_testnet)));

    Properties& destination_testnet = transaction_testnet->add_destination();
    // valid testnet addresses
    HANDLE_ERROR(properties_set_string_value(&destination_testnet, "address", "2MwsRtyuZyW2HuPUcUKGa4dAm9M9RkUPacK"));
    HANDLE_ERROR(properties_set_string_value(&destination_testnet, "address", "mzqiDnETWkunRDZxjUQ34JzN1LDevh5DpU"));

    // invalid testnet addresses
    EXPECT_ERROR(properties_set_string_value(&destination_testnet, "address", "3HGwSrGh7ARwDbzr3SwmRcavk6UWJtyUd2"));
    EXPECT_ERROR(properties_set_string_value(&destination_testnet, "address", "TEST"));
}

GTEST_TEST(BitcoinTransactionTest, transaction_set_destination_address_mainnet)
{
    const AccountPtr account_mainnet = make_account(BITCOIN_MAIN_NET, "5KHD87PD4WetNsrUfo7Z55xwWDEY2VrAh1VpfwHEV8wgbprCSxL");

    TransactionPtr transaction_mainnet;
    HANDLE_ERROR(make_transaction(account_mainnet.get(), reset_sp(transaction_mainnet)));

    Properties& destination_mainnet = transaction_mainnet->add_destination();
    // valid mainnet addresses
    HANDLE_ERROR(properties_set_string_value(&destination_mainnet, "address", "3HGwSrGh7ARwDbzr3SwmRcavk6UWJtyUd2"));
    HANDLE_ERROR(properties_set_string_value(&destination_mainnet, "address", "1713EbQ9gh7kgxEVPpSqDBhV36CbFpptT8"));

    // invalid mainnet addresses
    EXPECT_ERROR(properties_set_string_value(&destination_mainnet, "address", "mzqiDnETWkunRDZxjUQ34JzN1LDevh5DpU"));
    EXPECT_ERROR(properties_set_string_value(&destination_mainnet, "address", "TEST"));
}
