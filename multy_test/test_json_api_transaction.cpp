/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/json_api.h"
#include "multy_core/src/u_ptr.h"
#include "multy_core/src/json_helpers.h"
#include "multy_test/utility.h"

#include "json/json.h"

#include "gtest/gtest.h"

#include <memory>
#include <string>
#include <regex>

#define TX(tx_string) R"json({"transaction":{"serialized":")json" tx_string "\"}}"

namespace
{
using namespace multy_core::internal;

struct JsonTransactionApiTestCase
{
    const char* json_request;
    const char* expected_tx;
};

const char* string_or_default(const char* str, const char* default_str)
{
    return str != nullptr ? str : default_str;
}

std::ostream& operator<<(std::ostream& ostr, const JsonTransactionApiTestCase& test_case)
{
    return ostr << "JsonTransactionApiTestCase{\n"
                << "\trequest: \"" << test_case.json_request << "\",\n"
                << "\texpected: \"" << test_case.expected_tx << "\"\n}";

}

class JsonTransactionApiTestP : public ::testing::TestWithParam<JsonTransactionApiTestCase>
{};

TEST_P(JsonTransactionApiTestP, make_transaction_from_json)
{
    const auto& param = GetParam();
    ConstCharPtr actual_tx;

    HANDLE_ERROR(make_transaction_from_json(param.json_request, reset_sp(actual_tx)));
    EXPECT_NE(nullptr, actual_tx);
    const auto tx_json_object = parse_json(actual_tx.get());

    ASSERT_EQ(parse_json(param.expected_tx), tx_json_object);
}

const JsonTransactionApiTestCase ETH_MULTISIG_TEST_CASES[] = {
    // MS new wallet
    {
        R"json({
    "blockchain": "Ethereum",
    "net_type": 4,
    "account": {
        "type": 0,
        "private_key": "d92c7ed86831ee78e76a9acbb91219ab1a7a399f69db20f04da8478e11a51900"
    },
    "builder": {
        "builder_type": 0,
        "builder_action": "new_wallet",
        "balance": "1000000000000000000",
        "price": 0,
        "factory_address": "0x116ffa11dd8829524767f561da5d33d3d170e17d",
        "owners": "[0x6b4be1fc5fa05c5d959d27155694643b8af72fd8,
                0x2b74679d2a190fd679a85ce7767c05605237f030,
                0xbc11d8f8d741515d2696e34333a0671adb6aee34]",
        "confirmations": 2
    },
    "transaction": {
        "nonce": 1,
        "fee": {
            "gas_price": "6000000000",
            "gas_limit": "2029935"
        }
    }
})json",
        TX("0xf9012a01850165a0bc00831ef96f94116ffa11dd8829524767f561da5d33d3d170e1"
        "7d80b8c4f8f73808000000000000000000000000000000000000000000000000000000"
        "0000000040000000000000000000000000000000000000000000000000000000000000"
        "0002000000000000000000000000000000000000000000000000000000000000000300"
        "00000000000000000000006b4be1fc5fa05c5d959d27155694643b8af72fd800000000"
        "00000000000000002b74679d2a190fd679a85ce7767c05605237f03000000000000000"
        "0000000000bc11d8f8d741515d2696e34333a0671adb6aee342ca08d0f4fb6be9aa257"
        "7aabe32a505a98f0afb6e40761555146e28673edfa90509aa02d704ab99b02e36f7876"
        "07d975a3b0da18d94cb711afae5a3abe29078f0a27b3")
    },
    // MS new request
    {
        R"json({
    "blockchain": "Ethereum",
    "net_type": 4,
    "account": {
        "type": 0,
        "private_key": "d92c7ed86831ee78e76a9acbb91219ab1a7a399f69db20f04da8478e11a51900"
    },
    "builder": {
        "builder_type": 0,
        "builder_action": "new_request",
        "balance": "1000000000000000000",
        "amount": "400000000000000000",
        "wallet_address": "0x9b9A4102fB0F17aa2eE8e1Dbf8E8e3a62Cc01A3F",
        "dest_address": "0x2B74679D2a190Fd679a85cE7767c05605237f030"
    },
    "transaction": {
        "nonce": 2,
        "fee": {
            "gas_price": "5000000000",
            "gas_limit": "141346"
        }
    }
})json",
        TX("0xf8ea0285012a05f20083022822949b9a4102fb0f17aa2ee8e1dbf8e8e3a62cc01"
        "a3f80b884c64274740000000000000000000000002b74679d2a190fd679a85ce7767c0"
        "5605237f030000000000000000000000000000000000000000000000000058d15e1762"
        "8000000000000000000000000000000000000000000000000000000000000000000600"
        "0000000000000000000000000000000000000000000000000000000000000002ca067f"
        "31718d0d0371f0417df1bad027b7f13172be53002835f60c6e893b666d187a06f6aebe"
        "a21b40a0d89ce16272b9db1ddbeebe3a7fdcc3c10f44da07b12b74f97")
    },
    // MS reject request #0
    {
        R"json({
    "blockchain": "Ethereum",
    "net_type": 4,
    "account": {
        "type": 0,
        "private_key": "d92c7ed86831ee78e76a9acbb91219ab1a7a399f69db20f04da8478e11a51900"
    },
    "builder": {
        "builder_type": 0,
        "builder_action": "request",
        "balance": "1000000000000000000",
        "wallet_address": "0x9b9A4102fB0F17aa2eE8e1Dbf8E8e3a62Cc01A3F",
        "request_id": 0,
        "action": "reject"
    },
    "transaction": {
        "nonce": 3,
        "fee": {
            "gas_price": "5000000000",
            "gas_limit": "29747"
        }
    }
})json",
        TX("0xf8880385012a05f200827433949b9a4102fb0f17aa2ee8e1dbf8e8e3a62cc01a3f8"
        "0a420ea8d8600000000000000000000000000000000000000000000000000000000000"
        "000002ca0ba9332b6d80764428e28d2756fa0c773586844b1676d4d96cb8da51761053"
        "73ba079e628a3819dd86332030b1d3ccf0ed8fde94c33fcb9593c04af552776f7f63c")
    },
    // MS confirm request #0
    {
        R"json({
    "blockchain": "Ethereum",
    "net_type": 4,
    "account": {
        "type": 0,
        "private_key": "d92c7ed86831ee78e76a9acbb91219ab1a7a399f69db20f04da8478e11a51900"
    },
    "builder": {
        "builder_type": 0,
        "builder_action": "request",
        "balance": "1000000000000000000",
        "wallet_address": "0x9b9A4102fB0F17aa2eE8e1Dbf8E8e3a62Cc01A3F",
        "request_id": 0,
        "action": "confirm"
    },
    "transaction": {
        "nonce": 4,
        "fee": {
            "gas_price": "5000000000",
            "gas_limit": "50861"
        }
    }
})json",
        // TX id: 0xde7b6dac7283d46b2002f2da454a39af1ce75b3fbfdc40e0cac4ef1bd533a484
        TX("0xf8880485012a05f20082c6ad949b9a4102fb0f17aa2ee8e1dbf8e8e3a62cc01a3"
        "f80a4c01a8c84000000000000000000000000000000000000000000000000000000000"
        "00000002ba007bd354b0f07bb63dbd72f535fda8b5c4abcab2382846bd6a147491773f"
        "c474aa06e1845b051cec2af3f9fc17f8014a4ec46649bac021348e7e94a8a6dededf215")
    },
};

INSTANTIATE_TEST_CASE_P(
        JsonEthMultisig,
        JsonTransactionApiTestP,
        ::testing::ValuesIn(ETH_MULTISIG_TEST_CASES));


const int ANY_ERROR = -1;
struct JsonTransactionApiErrorTestCase
{
    const char* json_request;
    const char* expected_error_message;
    int expected_error_code;
};

std::ostream& operator<<(std::ostream& ostr, const JsonTransactionApiErrorTestCase& test_case)
{
    return ostr << "JsonTransactionApiErrorTestCase{"
                << "\n\trequest: \"" << test_case.json_request << "\","
                << "\n\texpected_error_message: \""
                << string_or_default(test_case.expected_error_message, "<EMPTY>") << "\","
                << "\n\texpected_error_code: " << test_case.expected_error_code
                << "\n}";
}

class JsonTransactionApiErrorTestP : public ::testing::TestWithParam<JsonTransactionApiErrorTestCase>
{};

TEST_P(JsonTransactionApiErrorTestP, make_transaction_from_json)
{
    const auto& param = GetParam();


    ConstCharPtr output;
    multy_core::internal::ErrorPtr error(make_transaction_from_json(param.json_request, reset_sp(output)));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ(nullptr, output);

    if (param.expected_error_message)
    {
        SCOPED_TRACE(param.expected_error_message);
        EXPECT_TRUE(std::regex_match(error->message, std::regex(param.expected_error_message)));
    }

    if (param.expected_error_code != ANY_ERROR)
    {
        ASSERT_EQ(param.expected_error_code, error->code);
    }
}

const JsonTransactionApiErrorTestCase JSON_ERROR_CASES[] = {
    {
        R"json({
    "blockchain": "Foo"
})json",
        ".*Invalid name for Blockchain : \"Foo\".*",
        ANY_ERROR
    }
};

INSTANTIATE_TEST_CASE_P(
        JsonErrorCases,
        JsonTransactionApiErrorTestP,
        ::testing::ValuesIn(JSON_ERROR_CASES));

} // namespace
