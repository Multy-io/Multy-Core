/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/src/golos/golos_account.h"
#include "multy_core/binary_data.h"

#include "multy_core/account.h"
#include "multy_core/big_int.h"
#include "multy_core/properties.h"
#include "multy_core/transaction.h"

#include "multy_core/src/api/big_int_impl.h"
#include "multy_core/src/api/account_impl.h"
#include "multy_core/src/api/key_impl.h"
#include "multy_core/src/u_ptr.h"
#include "multy_core/src/utility.h"

#include "multy_test/utility.h"
#include "multy_test/value_printers.h"

#include "multy_core/golos.h"

#include "gtest/gtest.h"

namespace
{
using namespace multy_core::internal;
using namespace test_utility;
} // namespace

GTEST_TEST(GolosTransactionTest, DISABLED_SmokeTest_public_api)
{
    AccountPtr account;
    HANDLE_ERROR(make_account(BLOCKCHAIN_GOLOS,
            "5JpDgood17pE47zB6pDJixg9Sw47QiHcQ9qCc3MeKYoYzRiMcnF",
            reset_sp(account)));

    TransactionPtr transaction;
    HANDLE_ERROR(make_transaction(account.get(), reset_sp(transaction)));
    ASSERT_NE(nullptr, transaction);

    {
        Properties* properties = nullptr;
        const int32_t ref_block_num(14918824);
        BinaryDataPtr ref_block_hash;
        const std::string expiration("2018-03-22T14:42:00");

        HANDLE_ERROR(make_binary_data_from_hex("00e3a4a84407f2df4953c35614248b433e6db43e", reset_sp(ref_block_hash)));

        HANDLE_ERROR(transaction_get_properties(transaction.get(), &properties));
        HANDLE_ERROR(properties_set_int32_value(properties, "ref_block_num", ref_block_num));
        HANDLE_ERROR(properties_set_binary_data_value(properties, "ref_block_hash", ref_block_hash.get()));
        HANDLE_ERROR(properties_set_string_value(properties, "expiration", expiration.c_str()));
    }

    {
        Properties* source = nullptr;
        HANDLE_ERROR(transaction_add_source(transaction.get(), &source));

        // Address balance
        BigIntPtr balance;
        HANDLE_ERROR(make_big_int("2000", reset_sp(balance))); // balance = 2.000 GOLOS

        HANDLE_ERROR(properties_set_big_int_value(source, "amount", balance.get()));
        HANDLE_ERROR(properties_set_string_value(source, "address", "multytest"));
    }

    {
        Properties* destination = nullptr;
        HANDLE_ERROR(transaction_add_destination(transaction.get(), &destination));

        BigIntPtr amount;
        HANDLE_ERROR(make_big_int("5", reset_sp(amount))); // amount = 0.005 GOLOS
        HANDLE_ERROR(properties_set_big_int_value(destination, "amount", amount.get()));
        HANDLE_ERROR(properties_set_string_value(destination, "address", "multy"));
    }

    // transaction_id = "eb096e9f1e4a34c0d9b72c3193b8a77bc3480a01"
    BinaryDataPtr signatures;
    HANDLE_ERROR(transaction_serialize(transaction.get(), reset_sp(signatures)));
    ASSERT_EQ(as_binary_data(minify_json(R"({
            "expiration":"2018-03-22T14:42:00",
            "extensions":[],
            "operations":[
              [
                 "transfer",
                 {
                    "amount":"0.005 GOLOS",
                    "from":"multytest",
                    "memo":"",
                    "to":"multy"
                 }
              ]
            ],
            "ref_block_num":42152,
            "ref_block_prefix":3757180740,
            "signatures":[
                "1f59d0ab21787a29dfbb13c7e9b544ca0af1a8a4a58857015fe2bd1fe3bf5187365ca7dae0fdca14e40997b093b844a5b95b10546561b1d654731405a0b42fc4fd"
            ]
            })")),
            *signatures);
}

GTEST_TEST(GolosTransactionTest, DISABLED_SmokeTest_public_apis)
{
    AccountPtr account;
    HANDLE_ERROR(make_account(BLOCKCHAIN_GOLOS,
            "5JpDgood17pE47zB6pDJixg9Sw47QiHcQ9qCc3MeKYoYzRiMcnF",
            reset_sp(account)));

    TransactionPtr transaction;
    HANDLE_ERROR(make_transaction(account.get(), reset_sp(transaction)));
    ASSERT_NE(nullptr, transaction);

    {
        Properties* properties = nullptr;
        const int32_t ref_block_num(14918938);
        BinaryDataPtr ref_block_hash;
        const std::string expiration("2018-03-22T14:14:47");

        HANDLE_ERROR(make_binary_data_from_hex("00e3a51ae8050b1a18628acba614338e68ade170", reset_sp(ref_block_hash)));

        HANDLE_ERROR(transaction_get_properties(transaction.get(), &properties));
        HANDLE_ERROR(properties_set_int32_value(properties, "ref_block_num", ref_block_num));
        HANDLE_ERROR(properties_set_binary_data_value(properties, "ref_block_hash", ref_block_hash.get()));
        HANDLE_ERROR(properties_set_string_value(properties, "expiration", expiration.c_str()));
    }

    {
        Properties* source = nullptr;
        HANDLE_ERROR(transaction_add_source(transaction.get(), &source));

        // Address balance
        BigIntPtr balance;
        HANDLE_ERROR(make_big_int("1995", reset_sp(balance))); // balance = 1.995 GOLOS

        HANDLE_ERROR(properties_set_big_int_value(source, "amount", balance.get()));
        HANDLE_ERROR(properties_set_string_value(source, "address", "multytest"));
    }

    {
        Properties* destination = nullptr;
        HANDLE_ERROR(transaction_add_destination(transaction.get(), &destination));

        BigIntPtr amount;
        HANDLE_ERROR(make_big_int("1000", reset_sp(amount))); // amount = 1.000 GOLOS
        HANDLE_ERROR(properties_set_big_int_value(destination, "amount", amount.get()));

        HANDLE_ERROR(properties_set_string_value(destination, "address", "multy"));
    }

    // transaction_id = "21c532df873d31536358ab1cf2b906ddaad613cc"
    BinaryDataPtr signatures;
    HANDLE_ERROR(transaction_serialize(transaction.get(), reset_sp(signatures)));
    ASSERT_EQ(as_binary_data(minify_json(R"({
            "expiration":"2018-03-22T14:47:42",
            "extensions":[],
            "operations":[
            [
               "transfer",
               {
                  "amount":"1.000 GOLOS",
                  "from":"multytest",
                  "memo":"",
                  "to":"multy"
               }
            ]
            ],
            "ref_block_num":42266,
            "ref_block_hash":436930024,
            "signatures":[
                "1f00fd6f824457bfde1088810c8cca210f45fb456daa02ea4ab22c6f0feef5deaa5ba3b9657bb88c55dc47f796c2711652e540bca0342ffb8c475e107f1be0cc56"
            ]
            })")),
            *signatures);
}

GTEST_TEST(GolosTransactionTest, DISABLED_SmokeTest_public_api_with_message)
{
    AccountPtr account;
    HANDLE_ERROR(make_account(BLOCKCHAIN_GOLOS,
            "5JpDgood17pE47zB6pDJixg9Sw47QiHcQ9qCc3MeKYoYzRiMcnF",
            reset_sp(account)));

    TransactionPtr transaction;
    HANDLE_ERROR(make_transaction(account.get(), reset_sp(transaction)));
    ASSERT_NE(nullptr, transaction);

    {
        Properties* properties = nullptr;
        const int32_t ref_block_num(14919066);
        BinaryDataPtr ref_block_hash;
        const std::string expiration("2018-03-22T14:54:06");

        HANDLE_ERROR(make_binary_data_from_hex("00e3a59a74c605fe620694036c2d028602074c7f", reset_sp(ref_block_hash)));

        HANDLE_ERROR(transaction_get_properties(transaction.get(), &properties));
        HANDLE_ERROR(properties_set_int32_value(properties, "ref_block_num", ref_block_num));
        HANDLE_ERROR(properties_set_binary_data_value(properties, "ref_block_hash", ref_block_hash.get()));
        HANDLE_ERROR(properties_set_string_value(properties, "expiration", expiration.c_str()));
    }

    {
        Properties* source = nullptr;
        HANDLE_ERROR(transaction_add_source(transaction.get(), &source));

        // Address balance
        BigIntPtr balance;
        HANDLE_ERROR(make_big_int("995", reset_sp(balance))); // balance = 0.995 GOLOS

        HANDLE_ERROR(properties_set_big_int_value(source, "amount", balance.get()));
        HANDLE_ERROR(properties_set_string_value(source, "address", "multytest"));
    }

    {
        Properties* destination = nullptr;
        HANDLE_ERROR(transaction_add_destination(transaction.get(), &destination));

        BigIntPtr amount;
        HANDLE_ERROR(make_big_int("50", reset_sp(amount))); // amount = 0.050 GOLOS
        HANDLE_ERROR(properties_set_big_int_value(destination, "amount", amount.get()));

        HANDLE_ERROR(properties_set_string_value(destination, "address", "pashaklybik"));
    }

    BinaryDataPtr message;
    make_binary_data_from_hex("74657374696e67", reset_sp(message));
    transaction_set_message(transaction.get(), message.get());

    // transaction_id = "e8d79092e217d38f15dae49901271fa49b27a3f8"
    BinaryDataPtr signatures;
    HANDLE_ERROR(transaction_serialize(transaction.get(), reset_sp(signatures)));
    ASSERT_EQ(as_binary_data(minify_json(R"({
            "expiration":"2018-03-22T14:54:06",
            "extensions":[],
            "operations":[
            [
               "transfer",
               {
                  "amount":"0.050 GOLOS",
                  "from":"multytest",
                  "memo":"testing",
                  "to":"pashaklybik"
               }
            ]
            ],
            "ref_block_num":42394,
            "ref_block_hash":4261791348,
            "signatures":[
                "1f69b05d86c589f6069f2895767b57c259296f2cc224bee782e177749320a94e4b15b5b1ec52e381b6b9eea989e7bbb6079d65209cf47869c36d404659781c9b49"
             ]
            })")),
            *signatures);
}
