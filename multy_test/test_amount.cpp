/* Copyright 2017 by Multy.io
 * Licensed under Multy.io license
 *
 * See LICENSE for details
 */

#include "multy_transaction/internal/amount.h"
#include "multy_transaction/amount.h"

#include "multy_core/internal/u_ptr.h"
#include "multy_core/internal/utility.h"

#include "multy_transaction/internal/u_ptr.h"

#include "multy_test/value_printers.h"

#include "gtest/gtest.h"

#include <memory>

namespace
{

using namespace wallet_core::internal;
using namespace multy_transaction::internal;

enum ArithmeticOperation {
    ADD,
    SUB,
    MUL
};

struct AmountArithmeticTestCase
{
    const ArithmeticOperation op;
    const char* number;
    const Amount amount;
    const char * answer;
};

AmountArithmeticTestCase TEST_CASES[] = {
    { ADD, "1", Amount("1"), "2" },
    { ADD, "3", Amount("6"), "9" },
    { ADD, "0", Amount("1"), "1" },
    { ADD, "9999999999999999999999999999999999999999", Amount("9999999999999999999999999999999999999999"), "19999999999999999999999999999999999999998" },
    { ADD, "0", Amount("0"), "0" },
    { ADD, "1", Amount("-1"), "0" },
    { ADD, "-3", Amount("6"), "3" },
    { ADD, "9999999999999999999999999999999999999999", Amount("-9999999999999999999999999999999999999999"), "0" },


    { MUL, "1", Amount("1"), "1" },
    { MUL, "3", Amount("6"), "18" },
    { MUL, "0", Amount("1"), "0" },
    { MUL, "9999999999999999999999999999999999999999", Amount("9999999999999999999999999999999999999999"), "99999999999999999999999999999999999999980000000000000000000000000000000000000001" },
    { MUL, "0", Amount("0"), "0" },
    { MUL, "1", Amount("-1"), "-1" },
    { MUL, "3", Amount("-6"), "-18" },
    { MUL, "-9999999999999999999999999999999999999999", Amount("9999999999999999999999999999999999999999"), "-99999999999999999999999999999999999999980000000000000000000000000000000000000001" },


    { SUB, "1", Amount("1"), "0" },
    { SUB, "6", Amount("3"), "3" },
    { SUB, "0", Amount("1"), "-1" },
    { SUB, "9999999999999999999999999999999999999999", Amount("9999999999999999999999999999999999999999"), "0" },
    { SUB, "0", Amount("0"), "0" },
    { SUB, "-5", Amount("1"), "-6" },
    { SUB, "6", Amount("-3"), "9" },
    { SUB, "-9999999999999999999999999999999999999999", Amount("-9999999999999999999999999999999999999999"), "0" },
};

} // namespace

GTEST_TEST(AmountClassTest, BasicMethod)
{
    EXPECT_STREQ("1", Amount("1").get_value().c_str());
    Amount temp(4);
    EXPECT_EQ("2", Amount("2"));
    EXPECT_EQ("3", Amount(3));
    EXPECT_EQ("4", Amount(temp));
    temp.set_value("5");
    EXPECT_EQ("5", temp);
}

GTEST_TEST(AmountClassTest, get_value)
{
    // get_value used to return sting with extra '\0' symbols at the end.
    Amount amount("3");
    std::string str = "3";
    EXPECT_EQ(1, amount.get_value().size());
    EXPECT_TRUE(amount.get_value() == str);
}


GTEST_TEST(AmountClassTest, BasicArithmetic)
{
    for (AmountArithmeticTestCase test_case: TEST_CASES) {
        switch (test_case.op) {
        case ADD:
        {
            ASSERT_EQ(test_case.answer, (test_case.number + test_case.amount));
            ASSERT_EQ(test_case.answer, (test_case.amount + test_case.number));

            {
                // Testing commutativity, i.e. A + B == B + A
                Amount temp_ADD_amount1(test_case.number);
                temp_ADD_amount1 += test_case.amount;
                ASSERT_EQ(test_case.answer, temp_ADD_amount1);

                Amount temp_ADD_amount2(test_case.amount);
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
                Amount temp_MUL_amount1(test_case.number);
                temp_MUL_amount1 *= test_case.amount;
                ASSERT_EQ(test_case.answer, temp_MUL_amount1);

                Amount temp_MUL_amount2(test_case.amount);
                temp_MUL_amount2 *= test_case.number;
                ASSERT_EQ(test_case.answer, temp_MUL_amount2);
            }
            break;
        }
        case SUB:
        {
            ASSERT_EQ(test_case.answer, (test_case.number - test_case.amount));

            Amount temp_SUB_amount(test_case.number);
            temp_SUB_amount -= test_case.amount;
            ASSERT_EQ(test_case.answer, temp_SUB_amount);

            break;
        }
        default:
        break;
        }
    }
}

GTEST_TEST(AmountTestInvalidArgs, make_amount)
{
    ErrorPtr error;
    AmountPtr amount;

    error.reset(make_amount(nullptr, reset_sp(amount)));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ(nullptr, amount);

    error.reset(make_amount("1", nullptr));
    EXPECT_NE(nullptr, error);
}

GTEST_TEST(AmountTestInvalidArgs, amount_to_string)
{
    ErrorPtr error;
    Amount amount(1);
    ConstCharPtr str;

    error.reset(amount_to_string(nullptr, reset_sp(str)));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ(nullptr, str);

    error.reset(amount_to_string(&amount, nullptr));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ("1", amount);
}

GTEST_TEST(AmountTestInvalidArgs, amount_set_value)
{
    ErrorPtr error;
    Amount amount(1);

    error.reset(amount_set_value(nullptr, "1"));
    EXPECT_NE(nullptr, error);

    error.reset(amount_set_value(&amount, nullptr));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ("1", amount);
}


GTEST_TEST(AmountTestInvalidValue, make_amount)
{
    ErrorPtr error;
    AmountPtr amount(new Amount(1));

    error.reset(make_amount("foo",  reset_sp(amount)));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ("1", *amount);

    error.reset(make_amount("",  reset_sp(amount)));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ("1", *amount);

    error.reset(make_amount("1.1",  reset_sp(amount)));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ("1", *amount);

    error.reset(make_amount(" ",  reset_sp(amount)));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ("1", *amount);
}

GTEST_TEST(AmountTestInvalidValue, amount_set_value)
{
    ErrorPtr error;
    Amount amount("1");

    error.reset(amount_set_value(&amount, "foo"));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ("1", amount);

    error.reset(amount_set_value(&amount, "1.1"));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ("1", amount);

    error.reset(amount_set_value(&amount, " "));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ("1", amount);

    error.reset(amount_set_value(&amount, ""));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ("1", amount);
}


GTEST_TEST(AmountTest, make_amount)
{
    ErrorPtr error;
    AmountPtr amount;

    error.reset(make_amount("1",  reset_sp(amount)));
    EXPECT_EQ(nullptr, error);
    EXPECT_EQ("1", *amount);
}

GTEST_TEST(AmountTest, amount_to_string)
{
    ErrorPtr error;
    Amount amount(1);
    ConstCharPtr test_str;

    error.reset(amount_to_string(&amount, reset_sp(test_str)));
    EXPECT_EQ(nullptr, error);
    EXPECT_NE(nullptr, test_str);
    EXPECT_EQ("1", amount);
}

GTEST_TEST(AmountTest, amount_set_value)
{
    ErrorPtr error;
    Amount amount(5);

    error.reset(amount_set_value(&amount, "1"));
    EXPECT_EQ(nullptr, error);
    EXPECT_EQ("1", amount);
}

GTEST_TEST(AmountTest, free_amount)
{
    Amount* amount;
    make_amount("1", &amount);

    EXPECT_NO_THROW(free_amount(amount));
}
