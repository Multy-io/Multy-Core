/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license
 *
 * See LICENSE for details
 */

#include "multy_core/big_int.h"
#include "multy_core/src/api/big_int_impl.h"

#include "multy_core/src/exception.h"
#include "multy_core/src/u_ptr.h"
#include "multy_core/src/utility.h"

#include "multy_test/value_printers.h"

#include "gtest/gtest.h"

#include <memory>

namespace
{

using namespace multy_core::internal;

enum ArithmeticOperation {
    ADD,
    SUB,
    MUL
};

struct BigIntArithmeticTestCase
{
    const ArithmeticOperation op;
    const char* number;
    const BigInt amount;
    const char * answer;
};

BigIntArithmeticTestCase TEST_CASES[] = {
    { ADD, "1", BigInt("1"), "2" },
    { ADD, "3", BigInt("6"), "9" },
    { ADD, "0", BigInt("1"), "1" },
    { ADD, "9999999999999999999999999999999999999999", BigInt("9999999999999999999999999999999999999999"), "19999999999999999999999999999999999999998" },
    { ADD, "0", BigInt("0"), "0" },
    { ADD, "1", BigInt("-1"), "0" },
    { ADD, "-3", BigInt("6"), "3" },
    { ADD, "9999999999999999999999999999999999999999", BigInt("-9999999999999999999999999999999999999999"), "0" },


    { MUL, "1", BigInt("1"), "1" },
    { MUL, "3", BigInt("6"), "18" },
    { MUL, "0", BigInt("1"), "0" },
    { MUL, "9999999999999999999999999999999999999999", BigInt("9999999999999999999999999999999999999999"), "99999999999999999999999999999999999999980000000000000000000000000000000000000001" },
    { MUL, "0", BigInt("0"), "0" },
    { MUL, "1", BigInt("-1"), "-1" },
    { MUL, "3", BigInt("-6"), "-18" },
    { MUL, "-9999999999999999999999999999999999999999", BigInt("9999999999999999999999999999999999999999"), "-99999999999999999999999999999999999999980000000000000000000000000000000000000001" },


    { SUB, "1", BigInt("1"), "0" },
    { SUB, "6", BigInt("3"), "3" },
    { SUB, "0", BigInt("1"), "-1" },
    { SUB, "9999999999999999999999999999999999999999", BigInt("9999999999999999999999999999999999999999"), "0" },
    { SUB, "0", BigInt("0"), "0" },
    { SUB, "-5", BigInt("1"), "-6" },
    { SUB, "6", BigInt("-3"), "9" },
    { SUB, "-9999999999999999999999999999999999999999", BigInt("-9999999999999999999999999999999999999999"), "0" },
};

template <typename T>
struct BigIntInitTestCase
{
    const T value;
    const char* value_str;
};

#define BIG_INT_INIT_TEST_CASE(x) {x, #x}

BigIntInitTestCase<int64_t> INT64_INIT_TEST_CASES[] = {
    BIG_INT_INIT_TEST_CASE(-1),
    BIG_INT_INIT_TEST_CASE(-2),
    BIG_INT_INIT_TEST_CASE(-100),
    BIG_INT_INIT_TEST_CASE(-10000),
    BIG_INT_INIT_TEST_CASE(-999999999),
    BIG_INT_INIT_TEST_CASE(-1234567891011121314),
    BIG_INT_INIT_TEST_CASE(0),
    BIG_INT_INIT_TEST_CASE(1),
    BIG_INT_INIT_TEST_CASE(2),
    BIG_INT_INIT_TEST_CASE(100),
    BIG_INT_INIT_TEST_CASE(10000),
    BIG_INT_INIT_TEST_CASE(999999999),
    BIG_INT_INIT_TEST_CASE(1234567891011121314),
};

BigIntInitTestCase<uint64_t> UINT64_INIT_TEST_CASES[] = {
    BIG_INT_INIT_TEST_CASE(0),
    BIG_INT_INIT_TEST_CASE(1),
    BIG_INT_INIT_TEST_CASE(2),
    BIG_INT_INIT_TEST_CASE(100),
    BIG_INT_INIT_TEST_CASE(10000),
    BIG_INT_INIT_TEST_CASE(999999999),
    BIG_INT_INIT_TEST_CASE(1234567891011121314),
};

} // namespace

GTEST_TEST(AmountClassTest, BasicMethod)
{
    EXPECT_STREQ("1", BigInt("1").get_value().c_str());
    BigInt temp(4);
    EXPECT_EQ("2", BigInt("2"));
    EXPECT_EQ("3", BigInt(3));
    EXPECT_EQ("4", BigInt(temp));
    temp.set_value("5");
    EXPECT_EQ("5", temp);
}

GTEST_TEST(AmountClassTest, get_value)
{
    // get_value used to return sting with extra '\0' symbols at the end.
    BigInt amount("3");
    std::string str = "3";
    EXPECT_EQ(1, amount.get_value().size());
    EXPECT_TRUE(amount.get_value() == str);
}


GTEST_TEST(AmountClassTest, BasicArithmetic)
{
    for (BigIntArithmeticTestCase test_case: TEST_CASES) {
        switch (test_case.op) {
        case ADD:
        {
            ASSERT_EQ(test_case.answer, (test_case.number + test_case.amount));
            ASSERT_EQ(test_case.answer, (test_case.amount + test_case.number));

            {
                // Testing commutativity, i.e. A + B == B + A
                BigInt temp_ADD_amount1(test_case.number);
                temp_ADD_amount1 += test_case.amount;
                ASSERT_EQ(test_case.answer, temp_ADD_amount1);

                BigInt temp_ADD_amount2(test_case.amount);
                temp_ADD_amount2 += test_case.number;
                ASSERT_EQ(test_case.answer, temp_ADD_amount2);
            }
            break;
        }
        case MUL:
        {
            ASSERT_EQ(test_case.answer, (test_case.number * test_case.amount));
            ASSERT_EQ(test_case.answer, (test_case.amount * test_case.number));
            {
                // Testing commutativity, i.e. A * B == B * A
                BigInt temp_MUL_amount1(test_case.number);
                temp_MUL_amount1 *= test_case.amount;
                ASSERT_EQ(test_case.answer, temp_MUL_amount1);

                BigInt temp_MUL_amount2(test_case.amount);
                temp_MUL_amount2 *= test_case.number;
                ASSERT_EQ(test_case.answer, temp_MUL_amount2);
            }
            break;
        }
        case SUB:
        {
            ASSERT_EQ(test_case.answer, (test_case.number - test_case.amount));

            BigInt temp_SUB_amount(test_case.number);
            temp_SUB_amount -= test_case.amount;
            ASSERT_EQ(test_case.answer, temp_SUB_amount);

            break;
        }
        default:
        break;
        }
    }
}

GTEST_TEST(BigIntTestInvalidArgs, make_big_int)
{
    ErrorPtr error;
    BigIntPtr amount;

    error.reset(make_big_int(nullptr, reset_sp(amount)));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ(nullptr, amount);

    error.reset(make_big_int("1", nullptr));
    EXPECT_NE(nullptr, error);
}

GTEST_TEST(BigIntTestInvalidArgs, big_int_to_string)
{
    ErrorPtr error;
    BigInt amount(1);
    ConstCharPtr str;

    error.reset(big_int_to_string(nullptr, reset_sp(str)));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ(nullptr, str);

    error.reset(big_int_to_string(&amount, nullptr));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ("1", amount);
}

GTEST_TEST(BigIntTestInvalidArgs, big_int_set_value)
{
    ErrorPtr error;
    BigInt amount(1);

    error.reset(big_int_set_value(nullptr, "1"));
    EXPECT_NE(nullptr, error);

    error.reset(big_int_set_value(&amount, nullptr));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ("1", amount);
}


GTEST_TEST(BigIntTestInvalidValue, make_big_int)
{
    ErrorPtr error;
    BigIntPtr amount(new BigInt(1));

    error.reset(make_big_int("foo",  reset_sp(amount)));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ("1", *amount);

    error.reset(make_big_int("",  reset_sp(amount)));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ("1", *amount);

    error.reset(make_big_int("1.1",  reset_sp(amount)));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ("1", *amount);

    error.reset(make_big_int(" ",  reset_sp(amount)));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ("1", *amount);
}

GTEST_TEST(BigIntTestInvalidValue, big_int_set_value)
{
    ErrorPtr error;
    BigInt amount("1");

    error.reset(big_int_set_value(&amount, "foo"));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ("1", amount);

    error.reset(big_int_set_value(&amount, "1.1"));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ("1", amount);

    error.reset(big_int_set_value(&amount, " "));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ("1", amount);

    error.reset(big_int_set_value(&amount, ""));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ("1", amount);
}


GTEST_TEST(BigIntTest, make_big_int)
{
    ErrorPtr error;
    BigIntPtr amount;

    error.reset(make_big_int("1",  reset_sp(amount)));
    EXPECT_EQ(nullptr, error);
    EXPECT_EQ("1", *amount);
}

GTEST_TEST(BigIntTest, big_int_to_string)
{
    ErrorPtr error;
    BigInt amount(1);
    ConstCharPtr test_str;

    error.reset(big_int_to_string(&amount, reset_sp(test_str)));
    EXPECT_EQ(nullptr, error);
    EXPECT_NE(nullptr, test_str);
    EXPECT_EQ("1", amount);
}

GTEST_TEST(BigIntTest, big_int_set_value)
{
    ErrorPtr error;
    BigInt amount(5);

    error.reset(big_int_set_value(&amount, "1"));
    EXPECT_EQ(nullptr, error);
    EXPECT_EQ("1", amount);
}

GTEST_TEST(BigIntTest, free_big_int)
{
    BigInt* amount;
    make_big_int("1", &amount);

    EXPECT_NO_THROW(free_big_int(amount));
}

GTEST_TEST(BigIntTest, get_value_as_int64)
{
    const int64_t MAX_INT64 = std::numeric_limits<int64_t>::max();
    BigInt value(MAX_INT64);
    EXPECT_EQ(MAX_INT64, value.get_value_as_int64());
    EXPECT_EQ(-1, BigInt(-1).get_value_as_int64());

    value += 1;
    EXPECT_THROW(value.get_value_as_int64(), Exception);
    EXPECT_EQ(static_cast<uint64_t>(MAX_INT64) + 1, value.get_value_as_uint64());

    EXPECT_NO_THROW(value.get_value());
}

GTEST_TEST(BigIntTest, get_value_as_uint64)
{
    const uint64_t MAX_UINT64 = std::numeric_limits<uint64_t>::max();
    BigInt value(MAX_UINT64);
    EXPECT_EQ(MAX_UINT64, value.get_value_as_uint64());

    value += 1;
    EXPECT_THROW(value.get_value_as_uint64(), Exception);
    EXPECT_THROW(BigInt(-1).get_value_as_uint64(), Exception);

    EXPECT_NO_THROW(value.get_value());
}

template <typename T>
struct BigIntTestP : public ::testing::TestWithParam<BigIntInitTestCase<T>>
{};

typedef BigIntTestP<int64_t> P_int64_t;
typedef BigIntTestP<uint64_t> P_uint64_t;

TEST_P(P_int64_t, initialization)
{
    const auto& param = GetParam();
    const BigInt value(param.value);

    EXPECT_EQ(param.value, value.get_value_as_int64());
    EXPECT_EQ(param.value_str, value.get_value());
}
TEST_P(P_uint64_t, initialization)
{
    const auto& param = GetParam();
    const BigInt value(param.value);

    EXPECT_EQ(param.value, value.get_value_as_uint64());
    EXPECT_EQ(param.value_str, value.get_value());
}

INSTANTIATE_TEST_CASE_P(BigIntTest, P_int64_t, ::testing::ValuesIn(INT64_INIT_TEST_CASES));
INSTANTIATE_TEST_CASE_P(BigIntTest, P_uint64_t, ::testing::ValuesIn(UINT64_INIT_TEST_CASES));
