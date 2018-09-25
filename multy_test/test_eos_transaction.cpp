/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/properties.h"
#include "multy_core/transaction.h"
#include "multy_core/big_int.h"
#include "multy_core/transaction_builder.h"
#include "multy_core/eos.h"


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


GTEST_TEST(EosTransactionTest, SmokeTest_testnet_1)
{
    AccountPtr account;
    HANDLE_ERROR(make_account(
            EOS_TEST_NET,
            ACCOUNT_TYPE_DEFAULT,
            "5JViCsGPFdFBUxzsLzXyYX4XdSuovADMFdkucEzTkioMSVK9NPG",
            reset_sp(account)));

    TransactionBuilderPtr builder;
    HANDLE_ERROR(make_transaction_builder(account.get(),
            EOS_TRANSACTION_BUILDER_TRANSFER,
            "",
            reset_sp(builder)));
    EXPECT_NE(nullptr, builder);

    {
        Properties* builder_properties = nullptr;
        HANDLE_ERROR(transaction_builder_get_properties(builder.get(), &builder_properties));

        const int32_t block_num(6432047);
        BigIntPtr ref_block_prefix;
        const std::string expiration("2018-07-19T16:35:07Z"); // + 30s
        HANDLE_ERROR(make_big_int("262535889", reset_sp(ref_block_prefix)));

        HANDLE_ERROR(properties_set_int32_value(builder_properties, "block_num", block_num));
        HANDLE_ERROR(properties_set_big_int_value(builder_properties, "ref_block_prefix", ref_block_prefix.get()));
        HANDLE_ERROR(properties_set_string_value(builder_properties, "expiration", expiration.c_str()));

        BigIntPtr balance;
        HANDLE_ERROR(make_big_int("20000", reset_sp(balance))); // balance = 2.0000 EOS

        HANDLE_ERROR(properties_set_big_int_value(builder_properties, "balance", balance.get()));
        HANDLE_ERROR(properties_set_string_value(builder_properties, "from", "pasha"));

        BigIntPtr amount;
        HANDLE_ERROR(make_big_int("10", reset_sp(amount))); // amount = 0.0010 EOS

        HANDLE_ERROR(properties_set_big_int_value(builder_properties, "amount", amount.get()));
        HANDLE_ERROR(properties_set_string_value(builder_properties, "to", "test.pasha"));
    }

    TransactionPtr transaction;
    HANDLE_ERROR(transaction_builder_make_transaction(builder.get(), reset_sp(transaction)));

    // transaction_id = "6b24ce668ff02ecdd438f799012b83b0e8d1904bfb0cbadfce02fda3e78124ec"
    ConstCharPtr signatures;
    HANDLE_ERROR(transaction_serialize_encoded(transaction.get(), reset_sp(signatures)));
    ASSERT_EQ(std::string("{\"signatures\":[\"SIG_K1_K48VoX8vnNYyx7fkmb5LivJn9XizfUCVdiF5HpDNYqgrYJp4khEHp8jvxEJtgAYFfw3ifpC9kZxeJpV1vhpndqfwuNjN8G\"]"
            ",\"packed_trx\":\"d9bd505b2f25d1faa50f000000000100a6823403ea3055000000572d3ccdcd010000000000d3b0a900000000a8ed3232210000000000d3b0a900806"
                          "9d85490b1ca0a0000000000000004454f53000000000000\",\"compression\":\"none\",\"packed_context_free_data\":\"\"}"), std::string(signatures.get()));
}

GTEST_TEST(EosTransactionTest, SmokeTest_testnet_2)
{
    AccountPtr account;
    HANDLE_ERROR(make_account(
            EOS_TEST_NET,
            ACCOUNT_TYPE_DEFAULT,
            "5JViCsGPFdFBUxzsLzXyYX4XdSuovADMFdkucEzTkioMSVK9NPG",
            reset_sp(account)));

    TransactionBuilderPtr builder;
    HANDLE_ERROR(make_transaction_builder(account.get(),
            EOS_TRANSACTION_BUILDER_TRANSFER,
            "",
            reset_sp(builder)));
    EXPECT_NE(nullptr, builder);

    {
        Properties* builder_propertie = nullptr;
        HANDLE_ERROR(transaction_builder_get_properties(builder.get(), &builder_propertie));

        const int32_t block_num(13669);
        BigIntPtr ref_block_prefix;
        const std::string expiration("2018-07-23T08:23:04Z"); // + 30s
        HANDLE_ERROR(make_big_int("2845038847", reset_sp(ref_block_prefix)));


        HANDLE_ERROR(properties_set_int32_value(builder_propertie, "block_num", block_num));
        HANDLE_ERROR(properties_set_big_int_value(builder_propertie, "ref_block_prefix", ref_block_prefix.get()));
        HANDLE_ERROR(properties_set_string_value(builder_propertie, "expiration", expiration.c_str()));

        BigIntPtr balance;
        HANDLE_ERROR(make_big_int("20000", reset_sp(balance))); // balance = 2.0000 EOS

        HANDLE_ERROR(properties_set_big_int_value(builder_propertie, "balance", balance.get()));
        HANDLE_ERROR(properties_set_string_value(builder_propertie, "from", "pasha"));

        BigIntPtr amount;
        HANDLE_ERROR(make_big_int("10000", reset_sp(amount))); // amount = 1.0000 EOS

        HANDLE_ERROR(properties_set_big_int_value(builder_propertie, "amount", amount.get()));
        HANDLE_ERROR(properties_set_string_value(builder_propertie, "to", "test.pasha"));

        BinaryDataPtr message;
        make_binary_data_from_hex("6d756c7479", reset_sp(message));
        HANDLE_ERROR(properties_set_binary_data_value(builder_propertie, "memo", message.get()));
    }


    TransactionPtr transaction;
    HANDLE_ERROR(transaction_builder_make_transaction(builder.get(), reset_sp(transaction)));

    // transaction_id = "6981ea5ac7da255c24b939216ca13d9348c483334700fdeba6e1539b7827ec39"
    ConstCharPtr signatures;
    HANDLE_ERROR(transaction_serialize_encoded(transaction.get(), reset_sp(signatures)));
    ASSERT_EQ(std::string("{\"signatures\":[\"SIG_K1_KBB8vFNJJyHcwMiVCvsYsUJ94FfbLQp9RnfUhU2E4vZLYYB2Rpzb7skzWM16ScNFTkiztcKZKM9A7XfmvnQAEyYFv7Tbp3\"]"
            ",\"packed_trx\":\"8690555b6535ffd893a9000000000100a6823403ea3055000000572d3ccdcd010000000000d3b0a900000000a8ed3232260000000000d3b0a900806"
            "9d85490b1ca102700000000000004454f5300000000056d756c747900\",\"compression\":\"none\",\"packed_context_free_data\":\"\"}"), std::string(signatures.get()));
}
