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

#include "gtest/gtest.h"

#include <memory>

namespace
{

using namespace wallet_core::internal;
using namespace multy_transaction::internal;

struct AmountArithmeticTestCase
{
    enum op {
        add,
        sub,
        mul
    };
    op operat;
    const char* number;
    Amount amount;
    const char * answer;
};

AmountArithmeticTestCase TEST_CASES[] = {
    { AmountArithmeticTestCase::add, "1", Amount("1"), "2" },
    { AmountArithmeticTestCase::add, "3", Amount("6"), "9" },
    { AmountArithmeticTestCase::add, "0", Amount("1"), "1" },
    { AmountArithmeticTestCase::add, "9999999999999999999999999999999999999999", Amount("9999999999999999999999999999999999999999"), "19999999999999999999999999999999999999998" },
    { AmountArithmeticTestCase::add, "0", Amount("0"), "0" },
    { AmountArithmeticTestCase::add, "1", Amount("-1"), "0" },
    { AmountArithmeticTestCase::add, "-3", Amount("6"), "3" },
    { AmountArithmeticTestCase::add, "9999999999999999999999999999999999999999", Amount("-9999999999999999999999999999999999999999"), "0" },


    { AmountArithmeticTestCase::mul, "1", Amount("1"), "1" },
    { AmountArithmeticTestCase::mul, "3", Amount("6"), "18" },
    { AmountArithmeticTestCase::mul, "0", Amount("1"), "0" },
    { AmountArithmeticTestCase::mul, "9999999999999999999999999999999999999999", Amount("9999999999999999999999999999999999999999"), "99999999999999999999999999999999999999980000000000000000000000000000000000000001" },
    { AmountArithmeticTestCase::mul, "0", Amount("0"), "0" },
    { AmountArithmeticTestCase::mul, "1", Amount("-1"), "-1" },
    { AmountArithmeticTestCase::mul, "3", Amount("-6"), "-18" },
    { AmountArithmeticTestCase::mul, "-9999999999999999999999999999999999999999", Amount("9999999999999999999999999999999999999999"), "-99999999999999999999999999999999999999980000000000000000000000000000000000000001" },


    { AmountArithmeticTestCase::sub, "1", Amount("1"), "0" },
    { AmountArithmeticTestCase::sub, "6", Amount("3"), "3" },
    { AmountArithmeticTestCase::sub, "0", Amount("1"), "-1" },
    { AmountArithmeticTestCase::sub, "9999999999999999999999999999999999999999", Amount("9999999999999999999999999999999999999999"), "0" },
    { AmountArithmeticTestCase::sub, "0", Amount("0"), "0" },
    { AmountArithmeticTestCase::sub, "-5", Amount("1"), "-6" },
    { AmountArithmeticTestCase::sub, "6", Amount("-3"), "9" },
    { AmountArithmeticTestCase::sub, "-9999999999999999999999999999999999999999", Amount("-9999999999999999999999999999999999999999"), "0" },
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
    // This test for check Amount::get_value() because get_value return sting with /0 symbols in the end

    Amount amount("3");
    std::string str = "3";
    EXPECT_TRUE(amount.get_value()==str);
}


GTEST_TEST(AmountClassTest, BasicArithmetic)
{
    for (AmountArithmeticTestCase test_case: TEST_CASES) {
        switch (test_case.operat) {
        case AmountArithmeticTestCase::add:
        {
            ASSERT_EQ(test_case.answer, (test_case.number + test_case.amount));
            ASSERT_EQ(test_case.answer, (test_case.amount + test_case.number));

            {
                // Testing commutativity, i.e. A + B == B + A
                Amount temp_add_amount1(test_case.number);
                temp_add_amount1 += test_case.amount;
                ASSERT_EQ(test_case.answer, temp_add_amount1);

                Amount temp_add_amount2(test_case.amount);
                temp_add_amount2 += test_case.number;
                ASSERT_EQ(test_case.answer, temp_add_amount2);
            }
            break;
        }
        case AmountArithmeticTestCase::mul:
        {
            ASSERT_EQ(test_case.answer, (test_case.number * test_case.amount));
            ASSERT_EQ(test_case.answer, (test_case.amount * test_case.number));
            {
                // Testing commutativity, i.e. A * B == B * A
                Amount temp_mul_amount1(test_case.number);
                temp_mul_amount1 *= test_case.amount;
                ASSERT_EQ(test_case.answer, temp_mul_amount1);

                Amount temp_mul_amount2(test_case.amount);
                temp_mul_amount2 *= test_case.number;
                ASSERT_EQ(test_case.answer, temp_mul_amount2);
            }
            break;
        }
        case AmountArithmeticTestCase::sub:
        {
            ASSERT_EQ(test_case.answer, (test_case.number - test_case.amount));

            Amount temp_sub_amount(test_case.number);
            temp_sub_amount -= test_case.amount;
            ASSERT_EQ(test_case.answer, temp_sub_amount);

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


///=================================================START VALID CASES=================================================

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
