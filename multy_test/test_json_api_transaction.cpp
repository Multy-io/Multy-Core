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
        "type": "multisig",
        "action": "new_wallet",
        "payload": {
            "balance": "1000000000000000000",
            "price": 0,
            "factory_address": "0x116ffa11dd8829524767f561da5d33d3d170e17d",
            "owners": "[0x6b4be1fc5fa05c5d959d27155694643b8af72fd8,
                    0x2b74679d2a190fd679a85ce7767c05605237f030,
                    0xbc11d8f8d741515d2696e34333a0671adb6aee34]",
            "confirmations": 2
        }
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
        "type": "multisig",
        "action": "new_request",
        "payload": {
            "balance": "1000000000000000000",
            "amount": "400000000000000000",
            "wallet_address": "0x9b9A4102fB0F17aa2eE8e1Dbf8E8e3a62Cc01A3F",
            "dest_address": "0x2B74679D2a190Fd679a85cE7767c05605237f030"
        }
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
        "type": "multisig",
        "action": "request",
        "payload": {
            "balance": "1000000000000000000",
            "wallet_address": "0x9b9A4102fB0F17aa2eE8e1Dbf8E8e3a62Cc01A3F",
            "request_id": 0,
            "action": "reject"
        }
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
        "type": "multisig",
        "action": "request",
        "payload": {
            "balance": "1000000000000000000",
            "wallet_address": "0x9b9A4102fB0F17aa2eE8e1Dbf8E8e3a62Cc01A3F",
            "request_id": 0,
            "action": "confirm"
        }
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


const JsonTransactionApiTestCase ETH_ERC20_TEST_CASES[] = {
    // ERC20 transfer
    {
        R"json({
    "blockchain": "Ethereum",
    "net_type": 4,
    "account": {
        "type": 0,
        "private_key": "b81b3c491e397cbb4939787a81bd049d7a8c5ee819fd4e03afdab94813b06a00"
    },
    "builder": {
        "type": "erc20",
        "action": "transfer",
        "payload": {
            "balance_eth": "100000000000000000",
            "contract_address": "0xfdf88a23d6058789c6a37bd997d3ed4760feb3b2",
            "balance_token": "1000000000000000000",
            "transfer_amount_token": "500000000000000000",
            "destination_address": "0x6b4be1fc5fa05c5d959d27155694643b8af72fd8"
        }
    },
    "transaction": {
        "nonce": 0,
        "fee": {
            "gas_price": "1000000000",
            "gas_limit": "153327"
        }
    }
})json",
        // TX id: 0xde7b6dac7283d46b2002f2da454a39af1ce75b3fbfdc40e0cac4ef1bd533a484
        TX("0xf8a980843b9aca00830256ef94fdf88a23d6058789c6a37bd997d3ed4760feb3b280b844a9059cbb0000"
        "000000000000000000006b4be1fc5fa05c5d959d27155694643b8af72fd800000000000000000000000000"
        "000000000000000000000006f05b59d3b200002ba0f85db5aacaea5a50d2761b9b6f52939c21848116b187"
        "c8c05da800011a963860a027e9189fdee28faac1092441903d59371c84a0cd5cacea0410bcc2ab000646d0")
    },
    // ERC20 Approve
    {
        R"json({
    "blockchain": "Ethereum",
    "net_type": 4,
    "account": {
        "type": 0,
        "private_key": "942116b87f5846cd23737cc3d668606872a64b9db2e8e55e8c6a2dbdeeb52800"
    },
    "builder": {
        "type": "erc20",
        "action": "approve",
        "payload": {
            "balance_eth": "100000000000000000",
            "contract_address": "0x9e75dfAaC3d5Ae972DD26c65eC247d8E6916D0ef",
            "balance_token": "100000",
            "address_for_approve": "0xd97ca3b0ab9e2d110f48c7902456dfcf0cc9e092",
            "approve_amount_token": "1000"
        }
    },
    "transaction": {
        "nonce": 12,
        "fee": {
            "gas_price": "6000000000",
            "gas_limit": "43472"
        }
    }
})json",
        // TX id: 0xec5022503d57cbf963f0280e85f6bd943bbd5a7bdd5fc6e6a454383895cc3c47
        TX("0xf8a90c850165a0bc0082a9d0949e75dfaac3d5ae972dd26c65ec247d8e6916d0ef80b844095ea7b"
        "3000000000000000000000000d97ca3b0ab9e2d110f48c7902456dfcf0cc9e0920000000000000000000"
        "0000000000000000000000000000000000000000003e82ca065f5df2c685134e14025403f9c87281c452"
        "2b0414724a0a592fc01f2606f5c9aa05c86e11d4cc70ae54fbc9584687e69f960f928773d4710facaf18"
        "cd891d0f4a0")
    },
    // ERC20 Transfer from
    {
        R"json({
    "blockchain": "Ethereum",
    "net_type": 4,
    "account": {
        "type": 0,
        "private_key": "c27577172f62f399ec3104ddbe2d53341d4db6ca03a808f1574906ccaa51b8d0"
    },
    "builder": {
        "type": "erc20",
        "action": "transfer_from",
        "payload": {
            "balance_eth": "100000000000000000",
            "contract_address": "0x9e75dfAaC3d5Ae972DD26c65eC247d8E6916D0ef",
            "available_token_from": "1000",
            "from": "0x2f2e3598fb63f9512352d5ddf8b3a82871418628",
            "to": "0xD1210b6CC27dE74e820349efAa1f9f8E1ec2186B",
            "transfer_amount_token": "500"
        }
    },
    "transaction": {
        "nonce": 3,
        "fee": {
            "gas_price": "6000000000",
            "gas_limit": "59835"
        }
    }
})json",
        // TX id: 0x01e4894a2882b0b3dcc060d944d2df19f450e3408767cf145bda5d793317e3de
        TX("0xf8c903850165a0bc0082e9bb949e75dfaac3d5ae972dd26c65ec247d8e6916d0ef80b86423b872dd0000"
        "000000000000000000002f2e3598fb63f9512352d5ddf8b3a82871418628000000000000000000000000d1210"
        "b6cc27de74e820349efaa1f9f8e1ec2186b000000000000000000000000000000000000000000000000000000"
        "00000001f42ca09449499acc8281fdbfe32fd69961bc27d121952d926515d10aab53ab3761d1f7a0037fdfd99"
        "77bc10c66300453864af524469d7dfc4cf9dc10908786980b8798b9")
    },
};


const JsonTransactionApiTestCase ETH_TEST_CASES[] = {
    {
        R"json({
    "blockchain": "Ethereum",
    "net_type": 4,
    "account": {
        "type": 0,
        "private_key": "5a37680b86fabdec299fa02bdfba8c9dfad08d796dc58c1d07527a751905bf71"
    },
    "builder": {
        "type": "basic",
        "payload":{
            "balance": "7500000000000000000",
            "destination_address": "0xd1b48a11e2251555c3c6d8b93e13f9aa2f51ea19",
            "destination_amount": "2305843009213693952"
        }
    },
    "transaction": {
        "nonce": 4,
        "fee": {
            "gas_price": "64424509440",
            "gas_limit": "21001"
        }
    }
})json",
        TX("0xf86c04850f0000000082520994d1b48a11e2251555c3c6d8b93e13f9aa2f51ea19882"
        "000000000000000802ba0098ee502619d5ba29d66b6c510265142f46ee0399456be7afb6"
        "3ceefac0bd17ea07c19cc4145471b31f90af07f554611ac535cd006f64fb2141f1ed7bea"
        "7150386")
    },
    {
        R"json({
    "blockchain": "Ethereum",
    "net_type": 1,
    "account": {
        "type": 0,
        "private_key": "b81b3c491e397cbb4939787a81bd049d7a8c5ee819fd4e03afdab94813b06a00"
    },
    "builder": {
        "type": "basic",
        "payload":{
            "balance": "79160000000000000",
            "destination_address": "0x6b4be1fc5fa05c5d959d27155694643b8af72fd8",
            "destination_amount": "002000000000000000",
            "payload": "hex:4d554c5459207468652062657374"
        }
    },
    "transaction": {
        "nonce": 1,
        "fee": {
            "gas_price": "4000000000",
            "gas_limit": "121000"
        }
    }
})json",
         // TXid: 0x5e52cf6ea1796558671051964ffde971a430182c5966f0d9a0aba5946bc1e55e
        TX("0xf8790184ee6b28008301d8a8946b4be1fc5fa05c5d959d27155694643b8af72fd8870"
        "71afd498d00008e4d554c545920746865206265737426a03ae3469ccf47aaccfd8beb2c4f0"
        "b84e9db60aae7fd3b27aeae85c928e8f56131a00ff0b0db2532b40782640aad46cbd2bc7e3"
        "911af0547fcd7272ae801cc98cc4d")
    },
};

INSTANTIATE_TEST_CASE_P(
        JsonEthereum,
        JsonTransactionApiTestP,
        ::testing::ValuesIn(ETH_TEST_CASES));

INSTANTIATE_TEST_CASE_P(
        JsonEthMultisig,
        JsonTransactionApiTestP,
        ::testing::ValuesIn(ETH_MULTISIG_TEST_CASES));

INSTANTIATE_TEST_CASE_P(
        JsonEthERC20,
        JsonTransactionApiTestP,
        ::testing::ValuesIn(ETH_ERC20_TEST_CASES));


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
