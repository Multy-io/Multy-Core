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
#include "multy_transaction/internal/ethereum_transaction.h"

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

GTEST_TEST(EthereumTransactionTest, SmokeTest_public_api)
{
    AccountPtr account;
    ErrorPtr error;
    HANDLE_ERROR(make_account(
                     CURRENCY_ETHEREUM,
                     "5a37680b86fabdec299fa02bdfba8c9dfad08d796dc58c1d07527a751905bf71",
                     reset_sp(account)));

    TransactionPtr transaction;
    HANDLE_ERROR(make_transaction(account.get(), reset_sp(transaction)));
    ASSERT_NE(nullptr, transaction);

    {
        Properties* properties = nullptr;
        const Amount nonce("0");

        HANDLE_ERROR(transaction_get_properties(transaction.get(), &properties));
        HANDLE_ERROR(properties_set_amount_value(properties, "nonce", &nonce));
        HANDLE_ERROR(properties_set_int32_value(properties, "chain_id", ETHEREUM_CHAIN_ID_RINKEBY));
    }

    {
        Properties* source = nullptr;
        HANDLE_ERROR(transaction_add_source(transaction.get(), &source));

        // Address balance
        AmountPtr balance;
        HANDLE_ERROR(make_amount("7500000000000000000", reset_sp(balance)));

        HANDLE_ERROR(properties_set_amount_value(source, "amount", balance.get()));
    }

    {
        Properties* destination = nullptr;
        HANDLE_ERROR(transaction_add_destination(transaction.get(), &destination));

        AmountPtr amount;
        HANDLE_ERROR(make_amount("1", reset_sp(amount)));
        HANDLE_ERROR(properties_set_amount_value(destination, "amount", amount.get()));

        const bytes address = from_hex("d1b48a11e2251555c3c6d8b93e13f9aa2f51ea19");
        const BinaryData address_data = to_binary_data(address);
        HANDLE_ERROR(properties_set_binary_data_value(destination, "address", &address_data));
    }

    {
        Properties* fee = nullptr;
        HANDLE_ERROR(transaction_get_fee(transaction.get(), &fee));

        AmountPtr amount_gas_price;
        HANDLE_ERROR(make_amount("1", reset_sp(amount_gas_price)));
        HANDLE_ERROR(properties_set_amount_value(fee, "gas_price", amount_gas_price.get()));

        AmountPtr amount_gas_limit;
        HANDLE_ERROR(make_amount("21001", reset_sp(amount_gas_limit)));
        HANDLE_ERROR(properties_set_amount_value(fee, "gas_limit", amount_gas_limit.get()));
    }

    {
        HANDLE_ERROR(transaction_update(transaction.get()));

        HANDLE_ERROR(transaction_sign(transaction.get()));

        BinaryDataPtr serialied;
        HANDLE_ERROR(transaction_serialize(transaction.get(), reset_sp(serialied)));
        ASSERT_EQ(to_binary_data(from_hex("f85f800182520994d1b48a11e2251555c3c6d8b93e13f9aa2f51ea1901802ba033de58162abbfdf1e744f5fee2b7a3c92691d9c59fc3f9ad2fa3fb946c8ea90aa0787abc84d20457c12fdcf62b612247fb34e397f6bdec64fc6a3bc9444df3e946")),
                  *serialied);
    }

}

GTEST_TEST(EthereumTransactionTest, SmokeTest_testnet1)
{
    AccountPtr account;
    ErrorPtr error;
    HANDLE_ERROR(make_account(
                     CURRENCY_ETHEREUM,
                     "5a37680b86fabdec299fa02bdfba8c9dfad08d796dc58c1d07527a751905bf71",
                     reset_sp(account)));

    TransactionPtr transaction;
    HANDLE_ERROR(make_transaction(account.get(), reset_sp(transaction)));
    ASSERT_NE(nullptr, transaction);

    const Amount balance("7500000000000000000");
    const Amount value("1");
    const Amount gas_limit("21001");
    const Amount gas_price("1");

    {
        Properties& properties = transaction->get_transaction_properties();
        properties.set_property("nonce", Amount("0"));
        properties.set_property("chain_id", ETHEREUM_CHAIN_ID_RINKEBY);
    }

    {
        Properties& source = transaction->add_source();
        source.set_property("amount", balance);
    }

    {
        const bytes address = from_hex("d1b48a11e2251555c3c6d8b93e13f9aa2f51ea19");
        Properties& destination = transaction->add_destination();
        destination.set_property("address", to_binary_data(address));
        destination.set_property("amount", value);
    }

    {
        Properties& fee = transaction->get_fee();
        fee.set_property("gas_price", gas_price);
        fee.set_property("gas_limit", gas_limit);
    }

    transaction->update_state();
    transaction->sign();
    BinaryDataPtr serialied = transaction->serialize();  
    ASSERT_EQ(to_binary_data(from_hex(
            "f85f800182520994d1b48a11e2251555c3c6d8b93e13f9aa2f51ea1901802ba033de58162abbfdf1e744f5fee2b7a3c92691d9c59fc3f9ad2fa3fb946c8ea90aa0787abc84d20457c12fdcf62b612247fb34e397f6bdec64fc6a3bc9444df3e946")),
            *serialied);
}

GTEST_TEST(EthereumTransactionTest, SmokeTest_testnet2)
{
    AccountPtr account;
    ErrorPtr error;
    HANDLE_ERROR(make_account(
                     CURRENCY_ETHEREUM,
                     "5a37680b86fabdec299fa02bdfba8c9dfad08d796dc58c1d07527a751905bf71",
                     reset_sp(account)));
    ASSERT_NE(nullptr, account);

    TransactionPtr transaction;
    HANDLE_ERROR(make_transaction(account.get(), reset_sp(transaction)));
    ASSERT_NE(nullptr, transaction);

    const Amount balance("7500000000000000000");
    const Amount value("2305843009213693952");
    const Amount gas_limit("21001");
    const Amount gas_price("64424509440");

    {
        Properties& properties = transaction->get_transaction_properties();
        properties.set_property("nonce", Amount("4"));
        properties.set_property("chain_id", ETHEREUM_CHAIN_ID_RINKEBY);
    }

    {
        Properties& source = transaction->add_source();
        source.set_property("amount", balance);
    }

    {
        const bytes address = from_hex("d1b48a11e2251555c3c6d8b93e13f9aa2f51ea19");
        Properties& destination = transaction->add_destination();
        destination.set_property("address", to_binary_data(address));
        destination.set_property("amount", value);
    }

    {
        Properties& fee = transaction->get_fee();
        fee.set_property("gas_price", gas_price);
        fee.set_property("gas_limit", gas_limit);
    }

    transaction->update_state();
    Amount estimated_fee = transaction->estimate_total_fee(1, 1);
    std::cerr << "estimated_fee: " << estimated_fee.get_value() << "\n";

    transaction->sign();
    const BinaryDataPtr serialied = transaction->serialize();
    std::cerr << "signed transaction:" << to_hex(*serialied) << "\n";

    ASSERT_EQ(to_binary_data(from_hex(
            "f86c04850f0000000082520994d1b48a11e2251555c3c6d8b93e13f9aa2f51ea19882000000000000000802ba0098ee502619d5ba29d66b6c510265142f46ee0399456be7afb63ceefac0bd17ea07c19cc4145471b31f90af07f554611ac535cd006f64fb2141f1ed7bea7150386")),
            *serialied);
}


GTEST_TEST(EthereumTransactionTest, SmokeTest_testnet_withdata)
{
    AccountPtr account;
    ErrorPtr error;
    HANDLE_ERROR(make_account(
                     CURRENCY_ETHEREUM,
                     "5a37680b86fabdec299fa02bdfba8c9dfad08d796dc58c1d07527a751905bf71",
                     reset_sp(account)));
    ASSERT_NE(nullptr, account);

    TransactionPtr transaction;
    HANDLE_ERROR(make_transaction(account.get(), reset_sp(transaction)));
    ASSERT_NE(nullptr, transaction);

    const Amount balance("7500000000000000000");
    const Amount value("1000");
    const Amount gas_limit("121000");
    const Amount gas_price("3000000000000");

    {
        const bytes payload = from_hex("ffff");
        Properties& properties = transaction->get_transaction_properties();
        properties.set_property("nonce", Amount("3"));
        properties.set_property("chain_id", ETHEREUM_CHAIN_ID_RINKEBY);
        properties.set_property("payload", to_binary_data(payload));
    }

    {
        Properties& source = transaction->add_source();
        source.set_property("amount", balance);
    }

    {
        const bytes address = from_hex("d1b48a11e2251555c3c6d8b93e13f9aa2f51ea19");
        Properties& destination = transaction->add_destination();
        destination.set_property("address", to_binary_data(address));
        destination.set_property("amount", value);
    }

    {
        Properties& fee = transaction->get_fee();
        fee.set_property("gas_price", gas_price);
        fee.set_property("gas_limit", gas_limit);
    }

    transaction->update_state();
    Amount estimated_fee = transaction->estimate_total_fee(1, 1);
    std::cerr << "estimated_fee: " << estimated_fee.get_value() << "\n";

    transaction->sign();
    const BinaryDataPtr serialied = transaction->serialize();
    std::cerr << "signed transaction:" << to_hex(*serialied) << "\n";

    ASSERT_EQ(to_binary_data(from_hex(
            "f86a038602ba7def30008301d8a894d1b48a11e2251555c3c6d8b93e13f9aa2f51ea198203e882ffff2ca0122bf1a37f949f0fc34354ca737eec7fd654e2172ecf893497d6e8356217512da05f01213f5d1c25d4b55e8c7219e572f92b00ec74a2662ae93c45928eb5133942")),
            *serialied);
}

