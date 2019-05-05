/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_test/test_json_api_transaction.h"

#include "multy_core/json_api.h"
#include "multy_core/src/u_ptr.h"
#include "multy_core/src/json_helpers.h"
#include "multy_test/utility.h"

#include "json/json.h"

#include "gtest/gtest.h"

#include <memory>
#include <string>

namespace
{
using namespace multy_core::internal;
using namespace test_utility;

TEST_P(JsonTransactionApiTestP, make_transaction_from_json)
{
    const auto& param = GetParam();
    ConstCharPtr actual_tx;

    HANDLE_ERROR(make_transaction_from_json(param.json_request, reset_sp(actual_tx)));
    EXPECT_NE(nullptr, actual_tx);
    const auto tx_json_object = parse_json(actual_tx.get());

    ASSERT_EQ(parse_json(param.expected_tx), tx_json_object);
}

TEST_P(JsonTransactionApiErrorTestP, make_transaction_from_json)
{
    const auto& param = GetParam();

    ConstCharPtr output;
    ErrorPtr error(make_transaction_from_json(param.json_request, reset_sp(output)));
    EXPECT_EQ(nullptr, output);
    ASSERT_SPECIFIC_ERROR(error, param.expected_error);
}

const JsonTransactionApiErrorTestCase JSON_ERROR_CASES[] = {
    {
        R"json({
    "blockchain": "Foo"
})json",
        ExpectedError{
            ".*Invalid name for Blockchain : \"Foo\".*",
            ExpectedError::ANY_CODE
        }
    }
};

INSTANTIATE_TEST_CASE_P(
        JsonAPI_ErrorCases,
        JsonTransactionApiErrorTestP,
        ::testing::ValuesIn(JSON_ERROR_CASES));

} // namesapce

std::ostream& operator<<(std::ostream& ostr, const JsonTransactionApiTestCase& test_case)
{
    return ostr << "JsonTransactionApiTestCase{\n"
                << "\trequest: \"" << test_case.json_request << "\",\n"
                << "\texpected: \"" << test_case.expected_tx << "\"\n}";
}

std::ostream& operator<<(std::ostream& ostr, const JsonTransactionApiErrorTestCase& test_case)
{
    return ostr << "JsonTransactionApiErrorTestCase{"
                << "\n\trequest: \"" << test_case.json_request << "\","
                << "\n" << test_case.expected_error
                << "\n}";
}
