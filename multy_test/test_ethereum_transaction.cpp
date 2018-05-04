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

#include "multy_core/src/api/account_impl.h"
#include "multy_core/src/api/key_impl.h"
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
                     ETHEREUM_TEST_NET,
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

    ASSERT_EQ(as_binary_data(from_hex(
            "f86c04850f0000000082520994d1b48a11e2251555c3c6d8b93e13f9aa2f51ea19882000000000000000802ba0098ee502619d5ba29d66b6c510265142f46ee0399456be7afb63ceefac0bd17ea07c19cc4145471b31f90af07f554611ac535cd006f64fb2141f1ed7bea7150386")),
            *serialied);
}


GTEST_TEST(EthereumTransactionTest, SmokeTest_testnet_withdata)
{
    AccountPtr account;
    HANDLE_ERROR(make_account(
                     ETHEREUM_TEST_NET,
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
            "5a37680b86fabdec299fa02bdfba8c9dfad08d796dc58c1d07527a751905bf71",
            reset_sp(account)));

    TransactionPtr transaction;
    HANDLE_ERROR(make_transaction(account.get(), reset_sp(transaction)));

    const BigInt available(1000000);
    const BigInt sent(10000);

    const BigInt gas_limit("121000");
    const BigInt gas_price("3000000000000");

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

    BigIntPtr total_fee;
    HANDLE_ERROR(transaction_get_total_fee(transaction.get(), reset_sp(total_fee)));
    EXPECT_NE(nullptr, total_fee);

    BigIntPtr total_spent;
    HANDLE_ERROR(transaction_get_total_spent(transaction.get(), reset_sp(total_spent)));
    EXPECT_NE(nullptr, total_spent);

    ASSERT_EQ(*total_spent, *total_fee + sent);
}

GTEST_TEST(EthereumTransactionTest, SmokeTest_mainnet)
{
    AccountPtr account;

    HANDLE_ERROR(make_account(
                     ETHEREUM_MAIN_NET,
                     "b81b3c491e397cbb4939787a81bd049d7a8c5ee819fd4e03afdab94813b06a00",
                     reset_sp(account)));
    ASSERT_NE(nullptr, account);

    TransactionPtr transaction;
    HANDLE_ERROR(make_transaction(account.get(), reset_sp(transaction)));
    ASSERT_NE(nullptr, transaction);

    const BigInt balance("10000000000000000");
    const BigInt value("2000000000000000");
    const BigInt gas_limit(21001);
    const BigInt gas_price("4000000000");

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
                     "b81b3c491e397cbb4939787a81bd049d7a8c5ee819fd4e03afdab94813b06a00",
                     reset_sp(account)));
    ASSERT_NE(nullptr, account);

    TransactionPtr transaction;
    HANDLE_ERROR(make_transaction(account.get(), reset_sp(transaction)));
    ASSERT_NE(nullptr, transaction);

    const BigInt balance("7916000000000000");
    const BigInt value("2000000000000000");
    const BigInt gas_limit(121000);
    const BigInt gas_price("4000000000");

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

GTEST_TEST(EthereumTransactionTest, DISABLED_SmokeTest_testnet_ERC20_transfer)
{
    AccountPtr account;
    HANDLE_ERROR(make_account(
                     ETHEREUM_MAIN_NET,
                     "c8aea1b4d991e2bb7c17b1cb8b8dbda9fb59717df552e98ec3aca80410565a9f",
                     reset_sp(account)));
    ASSERT_NE(nullptr, account);

    TransactionPtr transaction;
    HANDLE_ERROR(make_transaction(account.get(), reset_sp(transaction)));
    ASSERT_NE(nullptr, transaction);

    const BigInt balance("100000000000000000");
    const BigInt value("1000000000000000000");
    const BigInt gas_limit(153327);
    const BigInt gas_price("1000000000");

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


GTEST_TEST(EthereumTransactionTest, DISABLED_SmokeTest_testnet_ERC20_transfer_2)
{
    AccountPtr account;
    HANDLE_ERROR(make_account(
                     ETHEREUM_MAIN_NET,
                     "0xb81b3c491e397cbb4939787a81bd049d7a8c5ee819fd4e03afdab94813b06a00",
                     reset_sp(account)));
    ASSERT_NE(nullptr, account);

    TransactionPtr transaction;
    HANDLE_ERROR(make_transaction(account.get(), reset_sp(transaction)));
    ASSERT_NE(nullptr, transaction);

    const BigInt balance("100000000000000000");
    const BigInt value("500000000000000000");
    const BigInt gas_limit(153327);
    const BigInt gas_price("1000000000");

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
