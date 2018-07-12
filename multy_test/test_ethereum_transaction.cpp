/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/ethereum.h"
#include "multy_core/src/ethereum/ethereum_transaction.h"

#include "multy_core/src/api/properties_impl.h"

#include "multy_core/account.h"
#include "multy_core/big_int.h"
#include "multy_core/properties.h"
#include "multy_core/transaction_builder.h"

#include "multy_core/src/api/account_impl.h"
#include "multy_core/src/api/key_impl.h"
#include "multy_core/src/api/transaction_builder_impl.h"
#include "multy_core/src/u_ptr.h"
#include "multy_core/src/utility.h"

#include "multy_test/supported_blockchains.h"
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
            ETHEREUM_TEST_NET,
            ACCOUNT_TYPE_DEFAULT,
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

        HANDLE_ERROR(properties_set_string_value(destination,
                "address",
                "d1b48a11e2251555c3c6d8b93e13f9aa2f51ea19"));
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

    const char* TX = "f85f800182520994d1b48a11e2251555c3c6d8b93e13f9aa2f51ea1901802ba033de58162abbfdf1e744f5fee2b7a3c92691d9c59fc3f9ad2fa3fb946c8ea90aa0787abc84d20457c12fdcf62b612247fb34e397f6bdec64fc6a3bc9444df3e946";

    {
        BinaryDataPtr serialied;
        HANDLE_ERROR(transaction_serialize(transaction.get(), reset_sp(serialied)));
        ASSERT_EQ(as_binary_data(from_hex(TX)), *serialied);
    }

    ConstCharPtr serialized_encoded;
    HANDLE_ERROR(transaction_serialize_encoded(transaction.get(),
            reset_sp(serialized_encoded)));
    ASSERT_STREQ(serialized_encoded.get(), (std::string("0x") + TX).c_str());
}

GTEST_TEST(EthereumTransactionTest, SmokeTest_testnet1)
{
    AccountPtr account;

    HANDLE_ERROR(make_account(
            ETHEREUM_TEST_NET,
            ACCOUNT_TYPE_DEFAULT,
            "5a37680b86fabdec299fa02bdfba8c9dfad08d796dc58c1d07527a751905bf71",
            reset_sp(account)));

    TransactionPtr transaction;
    HANDLE_ERROR(make_transaction(account.get(), reset_sp(transaction)));
    ASSERT_NE(nullptr, transaction);

    const BigInt balance(7.5_ETH);
    const BigInt value(1_WEI);
    const BigInt gas_limit(21001);
    const BigInt gas_price(1_WEI);

    {
        Properties& properties = transaction->get_transaction_properties();
        properties.set_property_value("nonce", BigInt("0"));
    }

    {
        Properties& source = transaction->add_source();
        source.set_property_value("amount", balance);
    }

    {
        Properties& destination = transaction->add_destination();
        destination.set_property_value("address", "d1b48a11e2251555c3c6d8b93e13f9aa2f51ea19");
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
            ETHEREUM_TEST_NET,
            ACCOUNT_TYPE_DEFAULT,
            "5a37680b86fabdec299fa02bdfba8c9dfad08d796dc58c1d07527a751905bf71",
            reset_sp(account)));
    ASSERT_NE(nullptr, account);

    TransactionPtr transaction;
    HANDLE_ERROR(make_transaction(account.get(), reset_sp(transaction)));
    ASSERT_NE(nullptr, transaction);

    const BigInt balance(7.5_ETH);
    const BigInt value(2305843009213693952_WEI);
    const BigInt gas_limit(21001);
    const BigInt gas_price(64424509440_WEI);

    {
        Properties& properties = transaction->get_transaction_properties();
        properties.set_property_value("nonce", BigInt("4"));
    }

    {
        Properties& source = transaction->add_source();
        source.set_property_value("amount", balance);
    }

    {
        Properties& destination = transaction->add_destination();
        destination.set_property_value("address", "d1b48a11e2251555c3c6d8b93e13f9aa2f51ea19");
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

    const char* TX = "f86c04850f0000000082520994d1b48a11e2251555c3c6d8b93e13f9aa2f51ea19882000000000000000802ba0098ee502619d5ba29d66b6c510265142f46ee0399456be7afb63ceefac0bd17ea07c19cc4145471b31f90af07f554611ac535cd006f64fb2141f1ed7bea7150386";
    ASSERT_EQ(as_binary_data(from_hex(TX)), *serialied);

    ConstCharPtr serialized_encoded;
    HANDLE_ERROR(transaction_serialize_encoded(transaction.get(),
            reset_sp(serialized_encoded)));
    ASSERT_STREQ(serialized_encoded.get(), (std::string("0x") + TX).c_str());
}

GTEST_TEST(EthereumTransactionTest, SmokeTest_testnet_withdata)
{
    AccountPtr account;
    HANDLE_ERROR(make_account(
            ETHEREUM_TEST_NET,
            ACCOUNT_TYPE_DEFAULT,
            "5a37680b86fabdec299fa02bdfba8c9dfad08d796dc58c1d07527a751905bf71",
            reset_sp(account)));
    ASSERT_NE(nullptr, account);

    TransactionPtr transaction;
    HANDLE_ERROR(make_transaction(account.get(), reset_sp(transaction)));
    ASSERT_NE(nullptr, transaction);

    const BigInt balance(7.5_ETH);
    const BigInt value(1000_WEI);
    const BigInt gas_limit(121000);
    const BigInt gas_price(3000.0_GWEI);

    {
        Properties& properties = transaction->get_transaction_properties();
        properties.set_property_value("nonce", BigInt("3"));
    }

    {
        Properties& source = transaction->add_source();
        source.set_property_value("amount", balance);
    }

    {
        Properties& destination = transaction->add_destination();
        destination.set_property_value("address", "d1b48a11e2251555c3c6d8b93e13f9aa2f51ea19");
        destination.set_property_value("amount", value);
    }

    {
        Properties& fee = transaction->get_fee();
        fee.set_property_value("gas_price", gas_price);
        fee.set_property_value("gas_limit", gas_limit);
    }

    BinaryDataPtr data;
    HANDLE_ERROR(make_binary_data_from_hex("ffff", reset_sp(data)));
    HANDLE_ERROR(transaction_set_message(transaction.get(), data.get()));

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
            ETHEREUM_TEST_NET,
            ACCOUNT_TYPE_DEFAULT,
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

GTEST_TEST(EthereumTransactionTest, transaction_get_total_spent)
{
    AccountPtr account;
    HANDLE_ERROR(make_account(
            ETHEREUM_TEST_NET,
            ACCOUNT_TYPE_DEFAULT,
            "5a37680b86fabdec299fa02bdfba8c9dfad08d796dc58c1d07527a751905bf71",
            reset_sp(account)));

    TransactionPtr transaction;
    HANDLE_ERROR(make_transaction(account.get(), reset_sp(transaction)));

    const BigInt available(10000.0_GWEI);
    const BigInt sent(10000_WEI);
    const BigInt gas_limit(121000);
    const BigInt gas_price(3000_WEI);

    {
        Properties& properties = transaction->get_transaction_properties();
        properties.set_property_value("nonce", BigInt(0));
    }

    {
        Properties& source = transaction->add_source();
        source.set_property_value("amount", available);
    }

    {
        Properties& destination = transaction->add_destination();

        destination.set_property_value("address", "d1b48a11e2251555c3c6d8b93e13f9aa2f51ea19");
        destination.set_property_value("amount", sent);
    }

    {
        Properties& fee = transaction->get_fee();
        fee.set_property_value("gas_price", gas_price);
        fee.set_property_value("gas_limit", gas_limit);
    }

    HANDLE_ERROR(transaction_update(transaction.get()));

    BigIntPtr total_fee;
    HANDLE_ERROR(transaction_get_total_fee(transaction.get(), reset_sp(total_fee)));
    EXPECT_NE(nullptr, total_fee);

    BigIntPtr total_spent;
    HANDLE_ERROR(transaction_get_total_spent(transaction.get(), reset_sp(total_spent)));
    EXPECT_NE(nullptr, total_spent);

    EXPECT_NE(0, *total_spent);
    EXPECT_NE(0, *total_fee);
    ASSERT_EQ(*total_spent, *total_fee + sent);
}

GTEST_TEST(EthereumTransactionTest, SmokeTest_mainnet)
{
    AccountPtr account;

    HANDLE_ERROR(make_account(
            ETHEREUM_MAIN_NET,
            ACCOUNT_TYPE_DEFAULT,
            "b81b3c491e397cbb4939787a81bd049d7a8c5ee819fd4e03afdab94813b06a00",
            reset_sp(account)));
    ASSERT_NE(nullptr, account);

    TransactionPtr transaction;
    HANDLE_ERROR(make_transaction(account.get(), reset_sp(transaction)));
    ASSERT_NE(nullptr, transaction);

    const BigInt balance(0.01_ETH);
    const BigInt value(0.002_ETH);
    const BigInt gas_limit(21001);
    const BigInt gas_price(4.0_GWEI);

    {
        Properties& properties = transaction->get_transaction_properties();
        properties.set_property_value("nonce", BigInt(0));
    }

    {
        Properties& source = transaction->add_source();
        source.set_property_value("amount", balance);
    }

    {
        Properties& destination = transaction->add_destination();
        destination.set_property_value("address", "6b4be1fc5fa05c5d959d27155694643b8af72fd8");
        destination.set_property_value("amount", value);
    }

    {
        Properties& fee = transaction->get_fee();
        fee.set_property_value("gas_price", gas_price);
        fee.set_property_value("gas_limit", gas_limit);
    }

    const BinaryDataPtr serialied = transaction->serialize();

    // TXid: 0x53247e308d9e269f020231eafea9abf9bf4fda42746119ce16c2d1de3df20fb9
    ASSERT_EQ(as_binary_data(from_hex(
            "f86a8084ee6b2800825209946b4be1fc5fa05c5d959d27155694643b8af72fd887071afd498d00008025a04f1acf"
            "3b7611aec1a5e8be0844c8ca1b1b6d12f0f41176787a7d9813e465231ea019a4664f2d812ab61d579c4cc9a391bd"
            "545437c361017525f56d81b9a14db563")), *serialied);
}

GTEST_TEST(EthereumTransactionTest, SmokeTest_mainnet_withdata)
{
    AccountPtr account;
    HANDLE_ERROR(make_account(
            ETHEREUM_MAIN_NET,
            ACCOUNT_TYPE_DEFAULT,
            "b81b3c491e397cbb4939787a81bd049d7a8c5ee819fd4e03afdab94813b06a00",
            reset_sp(account)));
    ASSERT_NE(nullptr, account);

    TransactionPtr transaction;
    HANDLE_ERROR(make_transaction(account.get(), reset_sp(transaction)));
    ASSERT_NE(nullptr, transaction);

    const BigInt balance(0.007916_ETH);
    const BigInt value(0.002_ETH);
    const BigInt gas_limit(121000);
    const BigInt gas_price(4.0_GWEI);

    {
        Properties& properties = transaction->get_transaction_properties();
        properties.set_property_value("nonce", BigInt(1));
    }

    {
        Properties& source = transaction->add_source();
        source.set_property_value("amount", balance);
    }

    {
        Properties& destination = transaction->add_destination();
        destination.set_property_value("address", "0x6b4be1fc5fa05c5d959d27155694643b8af72fd8");
        destination.set_property_value("amount", value);
    }

    {
        Properties& fee = transaction->get_fee();
        fee.set_property_value("gas_price", gas_price);
        fee.set_property_value("gas_limit", gas_limit);
    }

    BinaryDataPtr data;
    HANDLE_ERROR(make_binary_data_from_hex("4d554c5459207468652062657374", reset_sp(data)));
    HANDLE_ERROR(transaction_set_message(transaction.get(), data.get()));

    const BinaryDataPtr serialied = transaction->serialize();

    // TXid: 0x5e52cf6ea1796558671051964ffde971a430182c5966f0d9a0aba5946bc1e55e
    ASSERT_EQ(as_binary_data(from_hex(
            "f8790184ee6b28008301d8a8946b4be1fc5fa05c5d959d27155694643b8af72fd887071afd498d00008e4d554c54"
            "5920746865206265737426a03ae3469ccf47aaccfd8beb2c4f0b84e9db60aae7fd3b27aeae85c928e8f56131a00f"
            "f0b0db2532b40782640aad46cbd2bc7e3911af0547fcd7272ae801cc98cc4d")), *serialied);
}

GTEST_TEST(EthereumTransactionTest, SmokeTest_testnet_ERC20_transfer)
{
    AccountPtr account;
    HANDLE_ERROR(make_account(
            ETHEREUM_TEST_NET,
            ACCOUNT_TYPE_DEFAULT,
            "c8aea1b4d991e2bb7c17b1cb8b8dbda9fb59717df552e98ec3aca80410565a9f",
                 reset_sp(account)));
    ASSERT_NE(nullptr, account);

    TransactionPtr transaction;
    HANDLE_ERROR(make_transaction(account.get(), reset_sp(transaction)));
    ASSERT_NE(nullptr, transaction);

    const BigInt balance(0.1_ETH);
    const BigInt value("1000000000000000000"); //token
    const BigInt gas_limit(153327);
    const BigInt gas_price(1.0_GWEI);

    {
        Properties& properties = transaction->get_transaction_properties();
        properties.set_property_value("nonce", BigInt(11));
        properties.set_property_value("token_transfer", "ERC20:0xfdf88a23d6058789c6a37bd997d3ed4760feb3b2:transfer");
    }

    {
        Properties& source = transaction->add_source();
        source.set_property_value("amount", balance);
    }

    {
        Properties& destination = transaction->add_destination();
        destination.set_property_value("address", "0x2b74679d2a190fd679a85ce7767c05605237f030");
        destination.set_property_value("amount", value);
    }

    {
        Properties& fee = transaction->get_fee();
        fee.set_property_value("gas_price", gas_price);
        fee.set_property_value("gas_limit", gas_limit);
    }

    const BinaryDataPtr serialied = transaction->serialize();

    // TXid: 0x01f0ec13cb9714f38da904b8e5c9652912f7e332566ef27a6592235e6c9359ee
    ASSERT_EQ(as_binary_data(from_hex(
            "f8a90b843b9aca00830256ef94fdf88a23d6058789c6a37bd997d3ed4760feb3b280b844a9059cbb000000"
            "0000000000000000002b74679d2a190fd679a85ce7767c05605237f0300000000000000000000000000000"
            "000000000000000000000de0b6b3a76400002ba02e8d834c6b53c91aa6c69d9f1d4ffff761adc3f7d60df0"
            "ddcfdf2b6990b5f7f5a01efc5f0c3c6fc3863adef7f2bd2c4affaf4f138f77fbbd3e348fc674e8d070a9")), *serialied);
}

GTEST_TEST(EthereumTransactionTest, SmokeTest_testnet_ERC20_transfer_2)
{
    AccountPtr account;
    HANDLE_ERROR(make_account(
            ETHEREUM_TEST_NET,
            ACCOUNT_TYPE_DEFAULT,
            "b81b3c491e397cbb4939787a81bd049d7a8c5ee819fd4e03afdab94813b06a00",
            reset_sp(account)));
    ASSERT_NE(nullptr, account);
    ASSERT_EQ("0x2b74679d2a190fd679a85ce7767c05605237f030", account->get_address());

    TransactionPtr transaction;
    HANDLE_ERROR(make_transaction(account.get(), reset_sp(transaction)));
    ASSERT_NE(nullptr, transaction);

    const BigInt balance(0.1_ETH);
    const BigInt value("500000000000000000");
    const BigInt gas_limit(153327);
    const BigInt gas_price(1.0_GWEI);

    {
        Properties& properties = transaction->get_transaction_properties();
        properties.set_property_value("nonce", BigInt(0));
        properties.set_property_value("token_transfer", "ERC20:0xfdf88a23d6058789c6a37bd997d3ed4760feb3b2:transfer");
    }

    {
        Properties& source = transaction->add_source();
        source.set_property_value("amount", balance);
    }

    {
        Properties& destination = transaction->add_destination();
        destination.set_property_value("address", "0x6b4be1fc5fa05c5d959d27155694643b8af72fd8");
        destination.set_property_value("amount", value);
    }

    {
        Properties& fee = transaction->get_fee();
        fee.set_property_value("gas_price", gas_price);
        fee.set_property_value("gas_limit", gas_limit);
    }

    const BinaryDataPtr serialied = transaction->serialize();

    // TXid: 0x570eaad2551a64a3180b394c30df48226866adf9cd4280c7130a805aa9051f2f0x570ea
    ASSERT_EQ(as_binary_data(from_hex(
            "f8a980843b9aca00830256ef94fdf88a23d6058789c6a37bd997d3ed4760feb3b280b844a9059cbb0000"
            "000000000000000000006b4be1fc5fa05c5d959d27155694643b8af72fd800000000000000000000000000"
            "000000000000000000000006f05b59d3b200002ba0f85db5aacaea5a50d2761b9b6f52939c21848116b187"
            "c8c05da800011a963860a027e9189fdee28faac1092441903d59371c84a0cd5cacea0410bcc2ab000646d0")), *serialied);
}

GTEST_TEST(EthereumTransactionTest, token_transfer_API)
{
    AccountPtr account;
    HANDLE_ERROR(make_account(
            ETHEREUM_MAIN_NET,
            ACCOUNT_TYPE_DEFAULT,
            "b81b3c491e397cbb4939787a81bd049d7a8c5ee819fd4e03afdab94813b06a00",
            reset_sp(account)));
    ASSERT_NE(nullptr, account);

    TransactionPtr transaction;
    HANDLE_ERROR(make_transaction(account.get(), reset_sp(transaction)));
    ASSERT_NE(nullptr, transaction);

    Properties& transaction_properties = transaction->get_transaction_properties();
    HANDLE_ERROR(properties_set_string_value(
            &transaction_properties,
            "token_transfer",
            "ERC20:0x6b4be1fc5fa05c5d959d27155694643b8af72fd8:transfer"));

    HANDLE_ERROR(properties_set_string_value(
            &transaction_properties,
            "token_transfer",
            "ERC20:0x6b4be1fc5fa05c5d959d27155694643b8af72fd8:approve"));

    EXPECT_ERROR(properties_set_string_value(
            &transaction_properties,
            "token_transfer",
            "ERC20:0x6b4be1fc5fa05c5d959d27155694643b8af72fd8:"));

    EXPECT_ERROR(properties_set_string_value(
            &transaction_properties,
            "token_transfer",
            "ERC20::"));

    EXPECT_ERROR(properties_set_string_value(
            &transaction_properties,
            "token_transfer",
            "::"));

    EXPECT_ERROR(properties_set_string_value(
            &transaction_properties,
            "token_transfer",
            "ERC20:0x6b4be1fc5fa05c5d959d27155694643b8af72fd8:invalid_method"));

    EXPECT_ERROR(properties_set_string_value(
            &transaction_properties,
            "token_transfer",
            "ERC20:invalid_address:transfer"));

    EXPECT_ERROR(properties_set_string_value(
            &transaction_properties,
            "token_transfer",
            "invalid_standard:0x6b4be1fc5fa05c5d959d27155694643b8af72fd8:invalid_method"));
}

// This test is disabled, because not implimented transaction message with token transfer
GTEST_TEST(EthereumTransactionTest, DISABLED_SmokeTest_mainnet_ERC20_transfer_with_data)
{
    AccountPtr account;
    HANDLE_ERROR(make_account(
            ETHEREUM_MAIN_NET,
            ACCOUNT_TYPE_DEFAULT,
            "b81b3c491e397cbb4939787a81bd049d7a8c5ee819fd4e03afdab94813b06a00",
            reset_sp(account)));
    ASSERT_NE(nullptr, account);
    ASSERT_EQ("0x2b74679d2a190fd679a85ce7767c05605237f030", account->get_address());

    TransactionPtr transaction;
    HANDLE_ERROR(make_transaction(account.get(), reset_sp(transaction)));
    ASSERT_NE(nullptr, transaction);

    const BigInt balance(9909144192.0_GWEI);
    const BigInt value("1000000000000000");
    const BigInt gas_limit(83668);
    const BigInt gas_price(6.0_GWEI);

    {
        Properties& properties = transaction->get_transaction_properties();
        properties.set_property_value("nonce", BigInt(3));
        properties.set_property_value("token_transfer", "ERC20:0x86fa049857e0209aa7d9e616f7eb3b3b78ecfdb0:transfer");
    }

    {
        Properties& source = transaction->add_source();
        source.set_property_value("amount", balance);
    }

    {
        Properties& destination = transaction->add_destination();
        destination.set_property_value("address", "0x6b4be1fc5fa05c5d959d27155694643b8af72fd8");
        destination.set_property_value("amount", value);
    }

    {
        Properties& fee = transaction->get_fee();
        fee.set_property_value("gas_price", gas_price);
        fee.set_property_value("gas_limit", gas_limit);
    }

    BinaryDataPtr data;
    HANDLE_ERROR(make_binary_data_from_hex("a9059cbb", reset_sp(data)));
    HANDLE_ERROR(transaction_set_message(transaction.get(), data.get()));

    const BinaryDataPtr serialied = transaction->serialize();


    // TXid: 0xbbee00c42a7a686bedd091fd2f9a01bb9b2cb9e37e9535f6fec678506f4730db
    ASSERT_EQ(as_binary_data(from_hex(
            "f8ae03850165a0bc00830146d49486fa049857e0209aa7d9e616f7eb3b3b78ecfdb080b848a9059cbb000000000000000000000000"
            "6b4be1fc5fa05c5d959d27155694643b8af72fd8000000000000000000000000000000000000000000000000016345785d8a0000a9"
            "059cbb25a05f8a05c7ac915ab479c175b6049ae662e0eaaf69cec95194bdfa44cb69e8b776a016c1ade55ca91c0b57e0fb133629b0"
            "9d3bb1fde622b0f4413748d4a385fe1c9c")), *serialied);
}

GTEST_TEST(EthereumTransactionTest, SmokeTest_mainnet_ERC20_transfer)
{
    AccountPtr account;
    HANDLE_ERROR(make_account(
            ETHEREUM_MAIN_NET,
            ACCOUNT_TYPE_DEFAULT,
            "b81b3c491e397cbb4939787a81bd049d7a8c5ee819fd4e03afdab94813b06a00",
            reset_sp(account)));
    ASSERT_NE(nullptr, account);
    ASSERT_EQ("0x2b74679d2a190fd679a85ce7767c05605237f030", account->get_address());

    TransactionPtr transaction;
    HANDLE_ERROR(make_transaction(account.get(), reset_sp(transaction)));
    ASSERT_NE(nullptr, transaction);

    const BigInt balance(9909144192.0_GWEI);
    const BigInt value("100000000000000000");
    const BigInt gas_limit(83668);
    const BigInt gas_price(6.0_GWEI);

    {
        Properties& properties = transaction->get_transaction_properties();
        properties.set_property_value("nonce", BigInt(4));
        properties.set_property_value("token_transfer", "ERC20:0x86fa049857e0209aa7d9e616f7eb3b3b78ecfdb0:transfer");
    }

    {
        Properties& source = transaction->add_source();
        source.set_property_value("amount", balance);
    }

    {
        Properties& destination = transaction->add_destination();
        destination.set_property_value("address", "0x6b4be1fc5fa05c5d959d27155694643b8af72fd8");
        destination.set_property_value("amount", value);
    }

    {
        Properties& fee = transaction->get_fee();
        fee.set_property_value("gas_price", gas_price);
        fee.set_property_value("gas_limit", gas_limit);
    }

    BinaryDataPtr data;
    HANDLE_ERROR(make_binary_data_from_hex("a9059cbb", reset_sp(data)));
    HANDLE_ERROR(transaction_set_message(transaction.get(), data.get()));

    const BinaryDataPtr serialied = transaction->serialize();

    // TXid: 0xbce2467f9bfc1cfe3e6f98f6429970bec200cc48bf4de18d5540e02be14e19fe
    ASSERT_EQ(as_binary_data(from_hex(
            "f8aa04850165a0bc00830146d49486fa049857e0209aa7d9e616f7eb3b3b78ecfdb080b844a9059cbb000000000000000000000000"
            "6b4be1fc5fa05c5d959d27155694643b8af72fd8000000000000000000000000000000000000000000000000016345785d8a000026"
            "a0d8b4a62dfaba55e669afbf3204429a7d62ac548992bb63c1f77c2917bb44832ea01a153de112f0c89fc78667823bcca59dbb0cbc"
            "5327d5d6682ff7904e56126739")),
            *serialied);
}

GTEST_TEST(EthereumTransactionBuilderTest, Create_multisig_wallet)
{
    AccountPtr account;
    HANDLE_ERROR(make_account(
            ETHEREUM_TEST_NET,
            ACCOUNT_TYPE_DEFAULT,
            "d92c7ed86831ee78e76a9acbb91219ab1a7a399f69db20f04da8478e11a51900",
            reset_sp(account)));
    ASSERT_NE(nullptr, account);
    ASSERT_EQ("0x6b4be1fc5fa05c5d959d27155694643b8af72fd8", account->get_address());

    TransactionBuilderPtr builder;
    HANDLE_ERROR(
            make_transaction_builder(
                    account.get(),
                    ETHEREUM_TRANSACTION_BUILDER_MULTISIG,
                    "new_wallet",
                    reset_sp(builder)));
    EXPECT_NE(nullptr, builder);


    {
        Properties* builder_propertie;
        BigIntPtr balance;
        BigIntPtr price;
        HANDLE_ERROR(make_big_int("1000000000000000000", reset_sp(balance)));
        HANDLE_ERROR(make_big_int("0", reset_sp(price)));
        HANDLE_ERROR(transaction_builder_get_properties(builder.get(), &builder_propertie));
        HANDLE_ERROR(properties_set_big_int_value(builder_propertie, "price", price.get()));
        HANDLE_ERROR(properties_set_big_int_value(builder_propertie, "balance", balance.get()));
        HANDLE_ERROR(properties_set_string_value(builder_propertie, "factory_address", "0x116ffa11dd8829524767f561da5d33d3d170e17d"));
        HANDLE_ERROR(properties_set_string_value(builder_propertie, "owners", "[0x6b4be1fc5fa05c5d959d27155694643b8af72fd8, 0x2b74679d2a190fd679a85ce7767c05605237f030, 0xbc11d8f8d741515d2696e34333a0671adb6aee34]"));
        HANDLE_ERROR(properties_set_int32_value(builder_propertie, "confirmations", 2));
    }

    TransactionPtr transaction = builder->make_transaction();

    {
        Properties* transaction_properties = nullptr;
        const BigInt nonce("1");
        HANDLE_ERROR(transaction_get_properties(transaction.get(), &transaction_properties));
        HANDLE_ERROR(properties_set_big_int_value(transaction_properties, "nonce", &nonce));
    }

    {
        Properties* fee = nullptr;
        HANDLE_ERROR(transaction_get_fee(transaction.get(), &fee));

        BigIntPtr amount_gas_price;
        HANDLE_ERROR(make_big_int("6000000000", reset_sp(amount_gas_price)));
        HANDLE_ERROR(properties_set_big_int_value(fee, "gas_price", amount_gas_price.get()));

        BigIntPtr amount_gas_limit;
        HANDLE_ERROR(make_big_int("2029935", reset_sp(amount_gas_limit)));
        HANDLE_ERROR(properties_set_big_int_value(fee, "gas_limit", amount_gas_limit.get()));
    }

    const BinaryDataPtr serialied = transaction->serialize();

    // 0xbde331468064c6a40f3c756235df2e32139e5c5088e5703117f0b26603c652d4
    ASSERT_EQ(as_binary_data(from_hex("f9012a01850165a0bc00831ef96f94116ffa11dd8829524767f561da5d33d3d170e17d80b8c4f8f738080000000000000000000000000000000000000000000000000000000000000040000000000000000000000000000000000000000000000000000000000000000200000000000000000000000000000000000000000000000000000000000000030000000000000000000000006b4be1fc5fa05c5d959d27155694643b8af72fd80000000000000000000000002b74679d2a190fd679a85ce7767c05605237f030000000000000000000000000bc11d8f8d741515d2696e34333a0671adb6aee342ca08d0f4fb6be9aa2577aabe32a505a98f0afb6e40761555146e28673edfa90509aa02d704ab99b02e36f787607d975a3b0da18d94cb711afae5a3abe29078f0a27b3")), *serialied);

}

GTEST_TEST(EthereumTransactionBuilderTest, Submit_from_multisig)
{
    AccountPtr account;
    HANDLE_ERROR(make_account(
            ETHEREUM_TEST_NET,
            ACCOUNT_TYPE_DEFAULT,
            "d92c7ed86831ee78e76a9acbb91219ab1a7a399f69db20f04da8478e11a51900",
            reset_sp(account)));
    ASSERT_NE(nullptr, account);
    ASSERT_EQ("0x6b4be1fc5fa05c5d959d27155694643b8af72fd8", account->get_address());


    TransactionBuilderPtr builder;
    HANDLE_ERROR(
            make_transaction_builder(
                    account.get(),
                    ETHEREUM_TRANSACTION_BUILDER_MULTISIG,
                    "new_request",
                    reset_sp(builder)));
    EXPECT_NE(nullptr, builder);

    {
        Properties* builder_propertie;
        BigIntPtr amount;
        BigIntPtr balance;
        HANDLE_ERROR(make_big_int("1000000000000000000", reset_sp(balance)));
        HANDLE_ERROR(make_big_int("400000000000000000", reset_sp(amount)));
        HANDLE_ERROR(transaction_builder_get_properties(builder.get(), &builder_propertie));
        HANDLE_ERROR(transaction_builder_get_properties(builder.get(), &builder_propertie));
        HANDLE_ERROR(properties_set_big_int_value(builder_propertie, "balance", balance.get()));
        HANDLE_ERROR(properties_set_string_value(builder_propertie, "wallet_address", "0x9b9A4102fB0F17aa2eE8e1Dbf8E8e3a62Cc01A3F"));
        HANDLE_ERROR(properties_set_string_value(builder_propertie, "dest_address", "0x2B74679D2a190Fd679a85cE7767c05605237f030"));
        HANDLE_ERROR(properties_set_big_int_value(builder_propertie, "amount", amount.get()));

    }


    TransactionPtr transaction = builder->make_transaction();

    {
        Properties* transaction_properties = nullptr;
        const BigInt nonce("2");
        HANDLE_ERROR(transaction_get_properties(transaction.get(), &transaction_properties));
        HANDLE_ERROR(properties_set_big_int_value(transaction_properties, "nonce", &nonce));
    }

    {
        Properties* fee = nullptr;
        HANDLE_ERROR(transaction_get_fee(transaction.get(), &fee));

        BigIntPtr amount_gas_price;
        HANDLE_ERROR(make_big_int("5000000000", reset_sp(amount_gas_price)));
        HANDLE_ERROR(properties_set_big_int_value(fee, "gas_price", amount_gas_price.get()));

        BigIntPtr amount_gas_limit;
        HANDLE_ERROR(make_big_int("141346", reset_sp(amount_gas_limit)));
        HANDLE_ERROR(properties_set_big_int_value(fee, "gas_limit", amount_gas_limit.get()));
    }

    const BinaryDataPtr serialied = transaction->serialize();

    // 0x3f591a7f391d1aadee9cc389233ba1ed3899b8ffebe69d1887dec46c65396e9f
    ASSERT_EQ(as_binary_data(from_hex("f8ea0285012a05f20083022822949b9a4102fb0f17aa2ee8e1dbf8e8e3a62cc01a3f80b884c64274740000000000000000000000002b74679d2a190fd679a85ce7767c05605237f030000000000000000000000000000000000000000000000000058d15e176280000000000000000000000000000000000000000000000000000000000000000006000000000000000000000000000000000000000000000000000000000000000002ca067f31718d0d0371f0417df1bad027b7f13172be53002835f60c6e893b666d187a06f6aebea21b40a0d89ce16272b9db1ddbeebe3a7fdcc3c10f44da07b12b74f97")), *serialied);
}

GTEST_TEST(EthereumTransactionBuilderTest, Reject_multisig_transaction)
{
    AccountPtr account;
    HANDLE_ERROR(make_account(
            ETHEREUM_TEST_NET,
            ACCOUNT_TYPE_DEFAULT,
            "d92c7ed86831ee78e76a9acbb91219ab1a7a399f69db20f04da8478e11a51900",
            reset_sp(account)));
    ASSERT_NE(nullptr, account);
    ASSERT_EQ("0x6b4be1fc5fa05c5d959d27155694643b8af72fd8", account->get_address());


    TransactionBuilderPtr builder;
    HANDLE_ERROR(
            make_transaction_builder(
                    account.get(),
                    ETHEREUM_TRANSACTION_BUILDER_MULTISIG,
                    "request",
                    reset_sp(builder)));
    EXPECT_NE(nullptr, builder);
    {
        Properties* builder_propertie;
        BigIntPtr request;
        BigIntPtr balance;
        HANDLE_ERROR(make_big_int("1000000000000000000", reset_sp(balance)));
        HANDLE_ERROR(make_big_int("0", reset_sp(request)));
        HANDLE_ERROR(transaction_builder_get_properties(builder.get(), &builder_propertie));
        HANDLE_ERROR(properties_set_big_int_value(builder_propertie, "balance", balance.get()));
        HANDLE_ERROR(properties_set_string_value(builder_propertie, "wallet_address", "0x9b9A4102fB0F17aa2eE8e1Dbf8E8e3a62Cc01A3F"));
        HANDLE_ERROR(properties_set_string_value(builder_propertie, "action", "reject"));
        HANDLE_ERROR(properties_set_big_int_value(builder_propertie, "request_id", request.get()));

    }

    TransactionPtr transaction = builder->make_transaction();

    {
        Properties* transaction_properties = nullptr;
        const BigInt nonce("3");
        HANDLE_ERROR(transaction_get_properties(transaction.get(), &transaction_properties));
        HANDLE_ERROR(properties_set_big_int_value(transaction_properties, "nonce", &nonce));
    }

    {
        Properties* fee = nullptr;
        HANDLE_ERROR(transaction_get_fee(transaction.get(), &fee));

        BigIntPtr amount_gas_price;
        HANDLE_ERROR(make_big_int("5000000000", reset_sp(amount_gas_price)));
        HANDLE_ERROR(properties_set_big_int_value(fee, "gas_price", amount_gas_price.get()));

        BigIntPtr amount_gas_limit;
        HANDLE_ERROR(make_big_int("29747", reset_sp(amount_gas_limit)));
        HANDLE_ERROR(properties_set_big_int_value(fee, "gas_limit", amount_gas_limit.get()));
    }

    const BinaryDataPtr serialied = transaction->serialize();

    // 0x91ffa3574067325cd08cb25addd2a7d815b5359b686301abc25a7fbe8c2f63df
    ASSERT_EQ(as_binary_data(from_hex("f8880385012a05f200827433949b9a4102fb0f17aa2ee8e1dbf8e8e3a62cc01a3f80a420ea8d8600000000000000000000000000000000000000000000000000000000000000002ca0ba9332b6d80764428e28d2756fa0c773586844b1676d4d96cb8da5176105373ba079e628a3819dd86332030b1d3ccf0ed8fde94c33fcb9593c04af552776f7f63c")), *serialied);
}

GTEST_TEST(EthereumTransactionBuilderTest, Confirm_multisig_transaction)
{
    AccountPtr account;
    HANDLE_ERROR(make_account(
            ETHEREUM_TEST_NET,
            ACCOUNT_TYPE_DEFAULT,
            "d92c7ed86831ee78e76a9acbb91219ab1a7a399f69db20f04da8478e11a51900",
            reset_sp(account)));
    ASSERT_NE(nullptr, account);
    ASSERT_EQ("0x6b4be1fc5fa05c5d959d27155694643b8af72fd8", account->get_address());


    TransactionBuilderPtr builder;
    HANDLE_ERROR(
            make_transaction_builder(
                    account.get(),
                    ETHEREUM_TRANSACTION_BUILDER_MULTISIG,
                    "request",
                    reset_sp(builder)));
    EXPECT_NE(nullptr, builder);
    {
        Properties* builder_propertie;
        BigIntPtr request;
        BigIntPtr balance;
        HANDLE_ERROR(make_big_int("1000000000000000000", reset_sp(balance)));
        HANDLE_ERROR(make_big_int("0", reset_sp(request)));
        HANDLE_ERROR(transaction_builder_get_properties(builder.get(), &builder_propertie));
        HANDLE_ERROR(properties_set_big_int_value(builder_propertie, "balance", balance.get()));
        HANDLE_ERROR(properties_set_string_value(builder_propertie, "wallet_address", "0x9b9A4102fB0F17aa2eE8e1Dbf8E8e3a62Cc01A3F"));
        HANDLE_ERROR(properties_set_string_value(builder_propertie, "action", "confirm"));
        HANDLE_ERROR(properties_set_big_int_value(builder_propertie, "request_id", request.get()));

    }

    TransactionPtr transaction = builder->make_transaction();

    {
        Properties* transaction_properties = nullptr;
        const BigInt nonce("4");
        HANDLE_ERROR(transaction_get_properties(transaction.get(), &transaction_properties));
        HANDLE_ERROR(properties_set_big_int_value(transaction_properties, "nonce", &nonce));
    }

    {
        Properties* fee = nullptr;
        HANDLE_ERROR(transaction_get_fee(transaction.get(), &fee));

        BigIntPtr amount_gas_price;
        HANDLE_ERROR(make_big_int("5000000000", reset_sp(amount_gas_price)));
        HANDLE_ERROR(properties_set_big_int_value(fee, "gas_price", amount_gas_price.get()));

        BigIntPtr amount_gas_limit;
        HANDLE_ERROR(make_big_int("50861", reset_sp(amount_gas_limit)));
        HANDLE_ERROR(properties_set_big_int_value(fee, "gas_limit", amount_gas_limit.get()));
    }

    const BinaryDataPtr serialied = transaction->serialize();

    // 0xde7b6dac7283d46b2002f2da454a39af1ce75b3fbfdc40e0cac4ef1bd533a484
    ASSERT_EQ(as_binary_data(from_hex("f8880485012a05f20082c6ad949b9a4102fb0f17aa2ee8e1dbf8e8e3a62cc01a3f80a4c01a8c8400000000000000000000000000000000000000000000000000000000000000002ba007bd354b0f07bb63dbd72f535fda8b5c4abcab2382846bd6a147491773fc474aa06e1845b051cec2af3f9fc17f8014a4ec46649bac021348e7e94a8a6dededf215")), *serialied);
}

GTEST_TEST(EthereumTransactionBuilderTest, Execute_multisig_transaction)
{
    AccountPtr account;
    HANDLE_ERROR(make_account(
            ETHEREUM_TEST_NET,
            ACCOUNT_TYPE_DEFAULT,
            "d92c7ed86831ee78e76a9acbb91219ab1a7a399f69db20f04da8478e11a51900",
            reset_sp(account)));
    ASSERT_NE(nullptr, account);
    ASSERT_EQ("0x6b4be1fc5fa05c5d959d27155694643b8af72fd8", account->get_address());


    TransactionBuilderPtr builder;
    HANDLE_ERROR(
            make_transaction_builder(
                    account.get(),
                    ETHEREUM_TRANSACTION_BUILDER_MULTISIG,
                    "request",
                    reset_sp(builder)));
    EXPECT_NE(nullptr, builder);
    {
        Properties* builder_propertie;
        BigIntPtr request;
        BigIntPtr balance;
        HANDLE_ERROR(make_big_int("1000000000000000000", reset_sp(balance)));
        HANDLE_ERROR(make_big_int("0", reset_sp(request)));
        HANDLE_ERROR(transaction_builder_get_properties(builder.get(), &builder_propertie));
        HANDLE_ERROR(properties_set_big_int_value(builder_propertie, "balance", balance.get()));
        HANDLE_ERROR(properties_set_string_value(builder_propertie, "wallet_address", "0x9b9A4102fB0F17aa2eE8e1Dbf8E8e3a62Cc01A3F"));
        HANDLE_ERROR(properties_set_string_value(builder_propertie, "action", "send"));
        HANDLE_ERROR(properties_set_big_int_value(builder_propertie, "request_id", request.get()));

    }

    TransactionPtr transaction = builder->make_transaction();

    {
        Properties* transaction_properties = nullptr;
        const BigInt nonce("6");
        HANDLE_ERROR(transaction_get_properties(transaction.get(), &transaction_properties));
        HANDLE_ERROR(properties_set_big_int_value(transaction_properties, "nonce", &nonce));
    }

    {
        Properties* fee = nullptr;
        HANDLE_ERROR(transaction_get_fee(transaction.get(), &fee));

        BigIntPtr amount_gas_price;
        HANDLE_ERROR(make_big_int("5000000000", reset_sp(amount_gas_price)));
        HANDLE_ERROR(properties_set_big_int_value(fee, "gas_price", amount_gas_price.get()));

        BigIntPtr amount_gas_limit;
        HANDLE_ERROR(make_big_int("57572", reset_sp(amount_gas_limit)));
        HANDLE_ERROR(properties_set_big_int_value(fee, "gas_limit", amount_gas_limit.get()));
    }

    const BinaryDataPtr serialied = transaction->serialize();

    // 0x1bbadd33cf32d3f34c722e655d1ff4bce3f1bfef872674f24676ac3f96550c77
    ASSERT_EQ(as_binary_data(from_hex("f8880685012a05f20082e0e4949b9a4102fb0f17aa2ee8e1dbf8e8e3a62cc01a3f80a4ee22610b00000000000000000000000000000000000000000000000000000000000000002ca01ad7628b18c164e1dae82a88ca2ac70dbeaf749bcc533862b80215827da0ac47a0536c1153e0649a4af95c829146b8e022f809798ebbc473676271170a9a25dc81")), *serialied);
}

struct PayloadTestCase
{
    BigInt value;
    BigInt gas_limit;
    uint64_t nonce;
    const char* hex_payload;
    const char* hex_serialized_tx;
};

std::ostream& operator<<(std::ostream& ostr, const PayloadTestCase& test_case)
{
    return ostr << "PayloadTestCase{\n\t"
            << "value: " << test_case.value << ",\n\t"
            << "gas limit: " << test_case.gas_limit << ",\n\t"
            << "nonce:" << test_case.nonce << ",\n\t"
            << "hex_payload: \"" << test_case.hex_payload << "\",\n\t"
            << "hex_serialized_tx: \"" << test_case.hex_serialized_tx << "\" \n}";
}

class PayloadTestP : public ::testing::TestWithParam<PayloadTestCase>
{
};

TEST_P(PayloadTestP, transaction_set_message)
{
    const auto& param = GetParam();

    AccountPtr account;
    HANDLE_ERROR(make_account(
            ETHEREUM_TEST_NET,
            ACCOUNT_TYPE_DEFAULT,
            "d92c7ed86831ee78e76a9acbb91219ab1a7a399f69db20f04da8478e11a51900",
            reset_sp(account)));
    ASSERT_NE(nullptr, account);
    ASSERT_EQ("0x6b4be1fc5fa05c5d959d27155694643b8af72fd8", account->get_address());

    TransactionPtr transaction;
    HANDLE_ERROR(make_transaction(account.get(), reset_sp(transaction)));
    ASSERT_NE(nullptr, transaction);

    const BigInt balance(1.1_ETH);
    const BigInt gas_limit(param.gas_limit);
    const BigInt gas_price(5.0_GWEI);

    {
        Properties& properties = transaction->get_transaction_properties();
        properties.set_property_value("nonce", BigInt(param.nonce));
    }

    {
        Properties& source = transaction->add_source();
        source.set_property_value("amount", balance);
    }

    {
        Properties& destination = transaction->add_destination();
        destination.set_property_value("address", "0x2b74679d2a190fd679a85ce7767c05605237f030");
        destination.set_property_value("amount", param.value);
    }

    {
        Properties& fee = transaction->get_fee();
        fee.set_property_value("gas_price", gas_price);
        fee.set_property_value("gas_limit", gas_limit);
    }

    BinaryDataPtr data;
    HANDLE_ERROR(make_binary_data_from_hex(param.hex_payload, reset_sp(data)));
    HANDLE_ERROR(transaction_set_message(transaction.get(), data.get()));

    const BinaryDataPtr serialied = transaction->serialize();
    ASSERT_EQ(as_binary_data(from_hex(param.hex_serialized_tx)), *serialied);
}

static const PayloadTestCase PAYLOAD_CASES[] =
{
    {
        0.02_ETH,
        BigInt(221000),
        8,
        // Payload:
        "000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
        "000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
        "000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
        "000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
        "000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
        "000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
        "000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
        "000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
        "000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
        "000000",
        // TXid: 0x2a8f7816150ae4d7561ba23ef1bc88ea3e4fef7c5d9be1dd6553a8ade8f3c8f7
        "f901eb0885012a05f20083035f48942b74679d2a190fd679a85ce7767c05605237f03087470de4df82000"
        "0b9017d00000000000000000000000000000000000000000000000000000000000000000000000000000000"
        "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
        "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
        "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
        "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
        "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
        "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
        "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
        "00000000000000000000000000000000000000000000000000000000000000000000000002ca00f6af42701"
        "a3a7f833176ebf9bc0cbdd3cb231a8a65b91bf365c7349646e618fa06e0a29c194ef4da2d158460bb0220d5"
        "ef1a5a3c762597cff551ec2aa602a458a"
    },
    {
        0.02_ETH,
        BigInt(121000),
        9,
        // Payload (50 bytes):
        "5050505050505050505050505050505050505050505050505050505050505050505050"
        "505050505050505050505050505050",
        // TXid: 0xa3d785b33dfb7ae34f2c1c55f49f8b501f1d2a91507045464906644e59edce97
        "f89e0985012a05f2008301d8a8942b74679d2a190fd679a85ce7767c05605237f03087470de4df820000b2"
        "5050505050505050505050505050505050505050505050505050505050505050505050505050505050505050"
        "5050505050502ba0c7f83726ca0a82e8989196ffbcccad183070b2f1d806331f36f654d0f1ec2609a01d0981"
        "3149ec4e89c8ab2b97b710560027ce5e9da0c81bb65f052ab8aace9bb2"
    },
    {
        0.01_ETH,
        BigInt(121000),
        11,
        // Payload (100 bytes)
        "100100100100100100100100100100100100100100100100100100100100100100100"
        "1001001001001001001001001001001100100100100100100100100100100100100100"
        "1001001001001001001001001001001001001001001001001001001001001",
        // TXid: 0xa2a227bd50b9525277e3ec5e15601ada08b5c2860c16affb6e32d7b8925f3177
       "f8d10b85012a05f2008301d8a8942b74679d2a190fd679a85ce7767c05605237f030872386f26fc10000b8641"
       "00100100100100100100100100100100100100100100100100100100100100100100100100100100100100100"
       "100100100110010010010010010010010010010010010010010010010010010010010010010010010010010010"
       "010010010010010010012ca07c0be1862f99e06e2164b9bf87e96471be1b91c90b2065abb37e552afdb7bf79a"
       "0620216b3f5ae3a90aa4de3561ce64c4be2d46857a58c9e423a18c4a993c88d4c"
    },
    {
        0.01_ETH,
        BigInt(221000),
        12,
        // Payload (150 bytes):
        "150150150150150150150150150150150150150150150150150150150150150150150"
        "150150150150150150150150150150150150150150150150150150150150150150150"
        "150150150150150150150150150150150150150150150150150150150150150150150"
        "150150150150150150150150150150150150150150150150150150150150150150150"
        "150150150150150150150150",
        // TXid: 0x1ef3acfb1ad18dab1e14e7c9824ddb8291f3757570e99b28c9dfa84baf9f6c5d
        "f901030c85012a05f20083035f48942b74679d2a190fd679a85ce7767c05605237f030872386f26fc1000"
        "0b8961501501501501501501501501501501501501501501501501501501501501501501501501501501501"
        "501501501501501501501501501501501501501501501501501501501501501501501501501501501501501"
        "501501501501501501501501501501501501501501501501501501501501501501501501501501501501501"
        "501501501501501501501501501501501501501501502ba01869318456dc58aef11fd7adfa3d2ea8e134cbe"
        "3b80b4eccf36a685b5d4668f4a0714a8b32802a04b77c79ef581e2a037d0a1387760e2d669cc82b1fbf0753"
        "f329"
    }
};

INSTANTIATE_TEST_CASE_P(
        EthereumTransaction, PayloadTestP, ::testing::ValuesIn(PAYLOAD_CASES));
