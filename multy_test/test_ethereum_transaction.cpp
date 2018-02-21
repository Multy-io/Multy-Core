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
#include "multy_core/src/ethereum/ethereum_transaction.h"
#include "multy_core/src/u_ptr.h"
#include "multy_core/src/utility.h"

#include "multy_test/utility.h"
#include "multy_test/value_printers.h"

#include "gtest/gtest.h"

#include <memory>

namespace
{
using namespace multy_core::internal;
using namespace test_utility;
} // namespace

GTEST_TEST(EthereumTransactionTest, SmokeTest_public_api)
{
    AccountPtr account;

    HANDLE_ERROR(make_account(
                     CURRENCY_ETHEREUM,
                     "5a37680b86fabdec299fa02bdfba8c9dfad08d796dc58c1d07527a751905bf71",
                     reset_sp(account)));

    TransactionPtr transaction;
    HANDLE_ERROR(make_transaction(account.get(), reset_sp(transaction)));
    ASSERT_NE(nullptr, transaction);

    {
        Properties* properties = nullptr;
        const BigInt nonce("0");

        HANDLE_ERROR(transaction_get_properties(transaction.get(), &properties));
        HANDLE_ERROR(properties_set_big_int_value(properties, "nonce", &nonce));
        HANDLE_ERROR(properties_set_int32_value(properties, "chain_id", ETHEREUM_CHAIN_ID_RINKEBY));
    }

    {
        Properties* source = nullptr;
        HANDLE_ERROR(transaction_add_source(transaction.get(), &source));

        // Address balance
        BigIntPtr balance;
        HANDLE_ERROR(make_big_int("7500000000000000000", reset_sp(balance)));

        HANDLE_ERROR(properties_set_big_int_value(source, "amount", balance.get()));
    }

    {
        Properties* destination = nullptr;
        HANDLE_ERROR(transaction_add_destination(transaction.get(), &destination));

        BigIntPtr amount;
        HANDLE_ERROR(make_big_int("1", reset_sp(amount)));
        HANDLE_ERROR(properties_set_big_int_value(destination, "amount", amount.get()));

        const bytes address = from_hex("d1b48a11e2251555c3c6d8b93e13f9aa2f51ea19");
        const BinaryData address_data = as_binary_data(address);
        HANDLE_ERROR(properties_set_binary_data_value(destination, "address", &address_data));
    }

    {
        Properties* fee = nullptr;
        HANDLE_ERROR(transaction_get_fee(transaction.get(), &fee));

        BigIntPtr amount_gas_price;
        HANDLE_ERROR(make_big_int("1", reset_sp(amount_gas_price)));
        HANDLE_ERROR(properties_set_big_int_value(fee, "gas_price", amount_gas_price.get()));

        BigIntPtr amount_gas_limit;
        HANDLE_ERROR(make_big_int("21001", reset_sp(amount_gas_limit)));
        HANDLE_ERROR(properties_set_big_int_value(fee, "gas_limit", amount_gas_limit.get()));
    }

    {
        BinaryDataPtr serialied;
        HANDLE_ERROR(transaction_serialize(transaction.get(), reset_sp(serialied)));
        ASSERT_EQ(as_binary_data(from_hex("f85f800182520994d1b48a11e2251555c3c6d8b93e13f9aa2f51ea1901802ba033de58162abbfdf1e744f5fee2b7a3c92691d9c59fc3f9ad2fa3fb946c8ea90aa0787abc84d20457c12fdcf62b612247fb34e397f6bdec64fc6a3bc9444df3e946")),
                  *serialied);
    }

}

GTEST_TEST(EthereumTransactionTest, SmokeTest_testnet1)
{
    AccountPtr account;

    HANDLE_ERROR(make_account(
                     CURRENCY_ETHEREUM,
                     "5a37680b86fabdec299fa02bdfba8c9dfad08d796dc58c1d07527a751905bf71",
                     reset_sp(account)));

    TransactionPtr transaction;
    HANDLE_ERROR(make_transaction(account.get(), reset_sp(transaction)));
    ASSERT_NE(nullptr, transaction);

    const BigInt balance("7500000000000000000");
    const BigInt value("1");
    const BigInt gas_limit("21001");
    const BigInt gas_price("1");

    {
        Properties& properties = transaction->get_transaction_properties();
        properties.set_property_value("nonce", BigInt("0"));
        properties.set_property_value("chain_id", ETHEREUM_CHAIN_ID_RINKEBY);
    }

    {
        Properties& source = transaction->add_source();
        source.set_property_value("amount", balance);
    }

    {
        const bytes address = from_hex("d1b48a11e2251555c3c6d8b93e13f9aa2f51ea19");
        Properties& destination = transaction->add_destination();
        destination.set_property_value("address", as_binary_data(address));
        destination.set_property_value("amount", value);
    }

    {
        Properties& fee = transaction->get_fee();
        fee.set_property_value("gas_price", gas_price);
        fee.set_property_value("gas_limit", gas_limit);
    }
    BinaryDataPtr serialied = transaction->serialize();  
    ASSERT_EQ(as_binary_data(from_hex(
            "f85f800182520994d1b48a11e2251555c3c6d8b93e13f9aa2f51ea1901802ba033de58162abbfdf1e744f5fee2b7a3c92691d9c59fc3f9ad2fa3fb946c8ea90aa0787abc84d20457c12fdcf62b612247fb34e397f6bdec64fc6a3bc9444df3e946")),
            *serialied);
}

GTEST_TEST(EthereumTransactionTest, SmokeTest_testnet2)
{
    AccountPtr account;

    HANDLE_ERROR(make_account(
                     CURRENCY_ETHEREUM,
                     "5a37680b86fabdec299fa02bdfba8c9dfad08d796dc58c1d07527a751905bf71",
                     reset_sp(account)));
    ASSERT_NE(nullptr, account);

    TransactionPtr transaction;
    HANDLE_ERROR(make_transaction(account.get(), reset_sp(transaction)));
    ASSERT_NE(nullptr, transaction);

    const BigInt balance("7500000000000000000");
    const BigInt value("2305843009213693952");
    const BigInt gas_limit("21001");
    const BigInt gas_price("64424509440");

    {
        Properties& properties = transaction->get_transaction_properties();
        properties.set_property_value("nonce", BigInt("4"));
        properties.set_property_value("chain_id", ETHEREUM_CHAIN_ID_RINKEBY);
    }

    {
        Properties& source = transaction->add_source();
        source.set_property_value("amount", balance);
    }

    {
        const bytes address = from_hex("d1b48a11e2251555c3c6d8b93e13f9aa2f51ea19");
        Properties& destination = transaction->add_destination();
        destination.set_property_value("address", as_binary_data(address));
        destination.set_property_value("amount", value);
    }

    {
        Properties& fee = transaction->get_fee();
        fee.set_property_value("gas_price", gas_price);
        fee.set_property_value("gas_limit", gas_limit);
    }

    BigInt estimated_fee = transaction->estimate_total_fee(1, 1);
    std::cerr << "estimated_fee: " << estimated_fee.get_value() << "\n";

    const BinaryDataPtr serialied = transaction->serialize();
    std::cerr << "signed transaction:" << to_hex(*serialied) << "\n";

    ASSERT_EQ(as_binary_data(from_hex(
            "f86c04850f0000000082520994d1b48a11e2251555c3c6d8b93e13f9aa2f51ea19882000000000000000802ba0098ee502619d5ba29d66b6c510265142f46ee0399456be7afb63ceefac0bd17ea07c19cc4145471b31f90af07f554611ac535cd006f64fb2141f1ed7bea7150386")),
            *serialied);
}


GTEST_TEST(EthereumTransactionTest, SmokeTest_testnet_withdata)
{
    AccountPtr account;
    HANDLE_ERROR(make_account(
                     CURRENCY_ETHEREUM,
                     "5a37680b86fabdec299fa02bdfba8c9dfad08d796dc58c1d07527a751905bf71",
                     reset_sp(account)));
    ASSERT_NE(nullptr, account);

    TransactionPtr transaction;
    HANDLE_ERROR(make_transaction(account.get(), reset_sp(transaction)));
    ASSERT_NE(nullptr, transaction);

    const BigInt balance("7500000000000000000");
    const BigInt value("1000");
    const BigInt gas_limit("121000");
    const BigInt gas_price("3000000000000");

    {
        const bytes payload = from_hex("ffff");
        Properties& properties = transaction->get_transaction_properties();
        properties.set_property_value("nonce", BigInt("3"));
        properties.set_property_value("chain_id", ETHEREUM_CHAIN_ID_RINKEBY);
        properties.set_property_value("payload", as_binary_data(payload));
    }

    {
        Properties& source = transaction->add_source();
        source.set_property_value("amount", balance);
    }

    {
        const bytes address = from_hex("d1b48a11e2251555c3c6d8b93e13f9aa2f51ea19");
        Properties& destination = transaction->add_destination();
        destination.set_property_value("address", as_binary_data(address));
        destination.set_property_value("amount", value);
    }

    {
        Properties& fee = transaction->get_fee();
        fee.set_property_value("gas_price", gas_price);
        fee.set_property_value("gas_limit", gas_limit);
    }

    BigInt estimated_fee = transaction->estimate_total_fee(1, 1);
    std::cerr << "estimated_fee: " << estimated_fee.get_value() << "\n";

    const BinaryDataPtr serialied = transaction->serialize();
    std::cerr << "signed transaction:" << to_hex(*serialied) << "\n";

    ASSERT_EQ(as_binary_data(from_hex(
            "f86a038602ba7def30008301d8a894d1b48a11e2251555c3c6d8b93e13f9aa2f51ea198203e882ffff2ca0122bf1a37f949f0fc34354ca737eec7fd654e2172ecf893497d6e8356217512da05f01213f5d1c25d4b55e8c7219e572f92b00ec74a2662ae93c45928eb5133942")),
            *serialied);
}

GTEST_TEST(EthereumTransactionTest, transaction_update_empty_tx)
{
    // Verify that transaction_update() fails when called on empty TX.
    AccountPtr account;
    HANDLE_ERROR(make_account(
                     CURRENCY_ETHEREUM,
                     "5a37680b86fabdec299fa02bdfba8c9dfad08d796dc58c1d07527a751905bf71",
                     reset_sp(account)));

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

