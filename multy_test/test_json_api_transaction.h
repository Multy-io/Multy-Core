#ifndef MULTY_TEST_JSON_API_TRANSACRION_H
#define MULTY_TEST_JSON_API_TRANSACRION_H

#include "multy_test/utility.h"

#include "gtest/gtest.h"

#include <iosfwd>

#define TX(tx_string) R"json({"transaction":{"serialized":")json" tx_string "\"}}"

struct JsonTransactionApiTestCase
{
    const char* json_request;
    const char* expected_tx;
};

class JsonTransactionApiTestP : public ::testing::TestWithParam<JsonTransactionApiTestCase>
{};

struct JsonTransactionApiErrorTestCase
{
    const char* json_request;
    const ::test_utility::ExpectedError expected_error;
};

class JsonTransactionApiErrorTestP : public ::testing::TestWithParam<JsonTransactionApiErrorTestCase>
{};

std::ostream& operator<<(std::ostream& ostr, const JsonTransactionApiTestCase& test_case);
std::ostream& operator<<(std::ostream& ostr, const JsonTransactionApiErrorTestCase& test_case);

#endif // TEST_JSON_API_TRANSACTION_H
