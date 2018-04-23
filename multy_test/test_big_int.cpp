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
#include "multy_test/utility.h"

#include "gtest/gtest.h"

#include <cmath>
#include <limits>
#include <memory>
#include <unordered_map>

namespace
{
using namespace multy_core::internal;

const int64_t DEFAULT_INT64_VALUE = static_cast<int64_t>(std::numeric_limits<int>::max()) + 1;

enum ArithmeticOperation {
    ADD,
    SUB,
    MUL,
    DIV,
};

struct BigIntArithmeticTestCase
{
    const ArithmeticOperation op;
    const char* left;
    const BigInt right;
    const char * answer;
};

std::ostream& operator<<(std::ostream& ostr, const ArithmeticOperation& op)
{
    const static std::unordered_map<int, const char*> OP_NAMES =
    {
        {ADD, "+"},
        {SUB, "-"},
        {MUL, "*"},
        {DIV, "/"}
    };

    return ostr << OP_NAMES.at(op);
}

std::ostream& operator<<(std::ostream& ostr, const BigIntArithmeticTestCase& test_case)
{
    return ostr << "BigIntArithmeticTestCase{\n"
            << "\t" << test_case.left << " " << test_case.op
            << " " << test_case.right << " = " << test_case.answer << "\n"
            << "}";
}

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

    { DIV, "4", BigInt("-9999999999999999999999999999999999999999"), "0" },
    { DIV, "3", BigInt(5), "0" },
    { DIV, "5", BigInt(3), "1" },
    { DIV, "3", BigInt(3), "1" },
    { DIV, "5", BigInt(-5), "-1" },
};

template <typename L, typename R>
auto do_binary_op(ArithmeticOperation op, const L& left, const R& right) -> decltype(left + right)
{
    switch (op)
    {
        case ADD:
            return left + right;
        case SUB:
            return left - right;
        case MUL:
            return left * right;
        case DIV:
            return left / right;
        default:
            assert(false && "Unknown operator.");
    }
}

template <typename L, typename R>
const L& do_binary_eq_op(ArithmeticOperation op, L& left, const R& right)
{
    switch (op)
    {
        case ADD:
            return left += right;
        case SUB:
            return left -= right;
        case MUL:
            return left *= right;
        case DIV:
            return left /= right;
        default:
            assert(false && "Unknown operator.");
    }
}

const BigInt& do_binary_op_api_big_int(ArithmeticOperation op, BigInt& left, const BigInt& right)
{
    static const std::unordered_map<int, decltype(big_int_add)*> functions =
    {
        {ADD, big_int_add},
        {SUB, big_int_sub},
        {MUL, big_int_mul},
        {DIV, big_int_div},
    };
    ErrorPtr error(functions.at(op)(&left, &right));
    test_utility::throw_exception_if_error(error.get());

    return left;
}

const BigInt& do_binary_op_api_int64(ArithmeticOperation op, BigInt& left, const int64_t& right)
{
    static const std::unordered_map<int, decltype(big_int_add_int64)*> functions =
    {
        {ADD, big_int_add_int64},
        {SUB, big_int_sub_int64},
        {MUL, big_int_mul_int64},
        {DIV, big_int_div_int64},
    };
    ErrorPtr error(functions.at(op)(&left, right));
    test_utility::throw_exception_if_error(error.get());

    return left;
}

const BigInt& do_binary_op_api_double(ArithmeticOperation op, BigInt& left, const double& right)
{
    static const std::unordered_map<int, decltype(big_int_add_double)*> functions =
    {
        {ADD, big_int_add_double},
        {SUB, big_int_sub_double},
        {MUL, big_int_mul_double},
        {DIV, big_int_div_double},
    };
    ErrorPtr error(functions.at(op)(&left, right));
    test_utility::throw_exception_if_error(error.get());

    return left;
}

bool is_commutative(ArithmeticOperation op)
{
    return op == ADD || op == MUL;
}

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

GTEST_TEST(BigIntTest, BasicMethod)
{
    EXPECT_STREQ("1", BigInt("1").get_value().c_str());
    BigInt temp(4);
    EXPECT_EQ("2", BigInt("2"));
    EXPECT_EQ("3", BigInt(3));
    EXPECT_EQ("4", BigInt(temp));
    temp.set_value("5");
    EXPECT_EQ("5", temp);
}

GTEST_TEST(BigIntTest, get_value)
{
    // get_value used to return sting with extra '\0' symbols at the end.
    BigInt amount("3");
    std::string str = "3";
    EXPECT_EQ(1, amount.get_value().size());
    EXPECT_TRUE(amount.get_value() == str);
}

GTEST_TEST(BigIntTest, CreateFromDouble)
{
    EXPECT_EQ(0, BigInt(0.0).get_value_as_int64());
    EXPECT_EQ(0, BigInt(0.5).get_value_as_int64());
    EXPECT_EQ(1, BigInt(1.0).get_value_as_int64());
    EXPECT_EQ(1, BigInt(1.1).get_value_as_int64());
}

GTEST_TEST(BigIntTest_Math, BasicArithmeticAPI)
{
    for (BigIntArithmeticTestCase test_case: TEST_CASES)
    {
        SCOPED_TRACE(test_case);
        const BigInt answer(test_case.answer);
        {
            BigInt tmp(test_case.left);
            EXPECT_EQ(answer,
                    do_binary_op_api_big_int(test_case.op, tmp, test_case.right));
        }

        {
            BigInt tmp(test_case.left);
            int64_t right = 0;
            ErrorPtr error(big_int_get_int64_value(&test_case.right, &right));
            if (error == nullptr)
            {
                EXPECT_EQ(answer,
                        do_binary_op_api_int64(test_case.op, tmp, right));

                tmp = BigInt(test_case.left);
                const double double_right = 1.0 * right;
                EXPECT_EQ(answer,
                        do_binary_op_api_double(test_case.op, tmp, double_right));
            }
        }

        if (is_commutative(test_case.op))
        {
            {
                BigInt tmp(test_case.right);
                EXPECT_EQ(answer,
                        do_binary_op_api_big_int(test_case.op, tmp, BigInt(test_case.left)));
            }

            {
                BigInt tmp(test_case.right);

                int64_t left = 0;
                BigInt left_big_int(test_case.left);
                ErrorPtr error(big_int_get_int64_value(&left_big_int, &left));
                if (error == nullptr)
                {
                    EXPECT_EQ(answer,
                            do_binary_op_api_int64(test_case.op, tmp, left));

                    tmp = BigInt(test_case.right);
                    const double double_left = 1.0 * left;
                    EXPECT_EQ(answer,
                            do_binary_op_api_double(test_case.op, tmp, double_left));
                }
            }
        }
    }
}

GTEST_TEST(BigIntTest_Math, same_result_as_int64)
{
    // Verify that BigInt works just as builtin int type for all operations
    // in given range, for BigInt, int64 and double values

    const int64_t OFFSETS[] = {0, 1 << 8, 1 << 16, 1 << 32};
    const ArithmeticOperation OPERATIONS[] = {ADD, SUB, MUL, DIV};
    const int LIMIT = 42;

    for (int l = 1; l < LIMIT; ++l)
    {
        for (int r = 1; r < LIMIT; ++r)
        {
            for (const auto op : OPERATIONS)
            {
                for (const auto offset : OFFSETS)
                {
                    const int64_t left = l + offset;
                    const int64_t right = r + offset;

                    SCOPED_TRACE(left);
                    SCOPED_TRACE(op);
                    SCOPED_TRACE(right);

                    const int64_t result = do_binary_op(op, left, right);
                    const int64_t double_result =
                            static_cast<int64_t>(do_binary_op(op, left, right * 1.0));

                    EXPECT_EQ(
                            result,
                            // BigInt @ BigInt
                            do_binary_op(op, BigInt(left), BigInt(right))
                    );

                    EXPECT_EQ(
                            result,
                            // int64 @ BigInt
                            do_binary_op(op, left, BigInt(right))
                    );

                    EXPECT_EQ(
                            result,
                            // BigInt @ int64
                            do_binary_op(op, BigInt(left), right)
                    );

                    EXPECT_EQ(
                            double_result,
                            // BigInt @ double
                            do_binary_op(op, BigInt(left), right * 1.0)
                    );

                    {
                        BigInt big_int_tmp(left);

                        EXPECT_EQ(
                                result,
                                // BigInt @= BigInt
                                do_binary_eq_op(op, big_int_tmp, BigInt(right))
                        );
                    }

                    {
                        BigInt big_int_tmp(left);

                        EXPECT_EQ(
                                result,
                                // BigInt @= int64
                                do_binary_eq_op(op, big_int_tmp, right)
                        );
                    }

                    {
                        BigInt big_int_tmp(left);

                        EXPECT_EQ(
                                double_result,
                                // BigInt @= double
                                do_binary_eq_op(op, big_int_tmp, right * 1.0)
                        );
                    }
                } // OFFSETS
            } // OPERATIONS
        } // r
    } // l
}

GTEST_TEST(BigIntTest_Math, same_result_as_int64_with_double_fractions)
{
    // Common double math with fractional values on all operators.

    const int64_t OFFSETS[] = {0, 1 << 8, 1L << 32};
    const int64_t DIVISORS[] = {1, 2, 3, 5, 7, 11, 13};
    const ArithmeticOperation OPERATIONS[] = {ADD, SUB, MUL, DIV};
    const int LIMIT = 17;

    for (int l = 1; l < LIMIT; ++l)
    {
        for (int r = 1; r < LIMIT; ++r)
        {
            for (const auto op : OPERATIONS)
            {
                for (const auto divisor : DIVISORS)
                {
                    for (const auto offset : OFFSETS)
                    {
                        SCOPED_TRACE(::testing::Message()
                                << "l:" << l << ", r:" << r << ", op:" << op
                                << ", divisor:" << divisor << ", offset:" << offset);

                        const int64_t left = l + offset;
                        const double right = (r * 1.0) / divisor;
                        const double dresult = do_binary_op(op, left, right);
                        const int64_t result = static_cast<int64_t>(dresult);

                        SCOPED_TRACE(::testing::Message("Test: ") << left << " "
                                << op << " " << right << " = " << result << " (" << dresult << ")");

                        EXPECT_EQ(
                                result,
                                // BigInt @ double
                                do_binary_op(op, BigInt(left), right)
                        );

                        {
                            BigInt big_int_tmp(left);

                            EXPECT_EQ(
                                    result,
                                    // BigInt @= double
                                    do_binary_eq_op(op, big_int_tmp, right)
                            );
                        }
                    } // OFFSETS
                } // DIVISORS
            } // OPERATIONS
        } // r
    } // l
}

GTEST_TEST(BigIntTest_Math, zero_division)
{
    EXPECT_THROW(BigInt(1) / 0, Exception);
    EXPECT_THROW(BigInt(1) / 0.0, Exception);
    EXPECT_THROW(BigInt(1) / -0.0, Exception);
}

GTEST_TEST(BigIntTest_Math, double_special_values)
{
    const double SPECIAL_VALUES[] = {
        std::numeric_limits<double>::quiet_NaN(),
        std::numeric_limits<double>::signaling_NaN(),
        std::numeric_limits<double>::infinity(),
        -1 * std::numeric_limits<double>::infinity()
    };

    const ArithmeticOperation OPERATIONS[] = {ADD, SUB, MUL, DIV};

    const BigInt DEFAULT_VALUE(1);
    for (const auto special_value : SPECIAL_VALUES)
    {
        for (const auto op : OPERATIONS)
        {
            EXPECT_THROW(do_binary_op(op, DEFAULT_VALUE, special_value), Exception);

            BigInt tmp(DEFAULT_VALUE);
            EXPECT_THROW(do_binary_eq_op(op, tmp, special_value), Exception);
            ASSERT_EQ(tmp, DEFAULT_VALUE);
        }
    }
}

int cmp(const BigInt& left, const BigInt& right)
{
    return left.compare(right);
}

template <typename T>
int cmp(const BigInt& left, const T& right)
{
    return left.compare(right);
}

template <typename T>
int cmp(const T& left, const BigInt& right)
{
    return right.compare(left) * -1;
}

// Overload when both left and right are fundamental types, like int64_t or double.
template <typename L, typename R>
auto cmp(const L& left, const R& right)
        -> typename std::enable_if<
                std::is_same<
                        typename std::is_fundamental<L>::type,
                        typename std::is_fundamental<R>::type
                >::value,
                int
        >::type
{
    const auto diff = left - right;
    if (diff)
    {
        return diff / std::abs(diff);
    }
    return diff;
}

GTEST_TEST(BigIntTest_Math, cmp_api)
{
    // Verify that BigInt works just as builtin int type for compare operations
    // in given range.

    const int LIMIT = 42;
    const int64_t OFFSETS[] = {-1*LIMIT/2, 1000L*1000L*1000L*1000L, -1000L*1000L*1000L*1000L};

    for (int l = 0; l < LIMIT; ++l)
    {
        for (int r = 0; r < LIMIT; ++r)
        {
            for (const auto offset : OFFSETS)
            {
                const int64_t left = l + offset;
                const int64_t right = r + offset;

                SCOPED_TRACE(left);
                SCOPED_TRACE(" compare ");
                SCOPED_TRACE(right);

                const int result = cmp(left, right);

                EXPECT_EQ(result,
                    cmp(BigInt(left), BigInt(right))
                );

                EXPECT_EQ(result,
                    cmp(BigInt(left), right)
                );

                EXPECT_EQ(result,
                    cmp(BigInt(left), static_cast<double>(right))
                );
            } // offset
        } // r
    } // l
}

GTEST_TEST(BigIntTest_API_InvalidArgs, make_big_int)
{
    ErrorPtr error;
    BigIntPtr amount;

    error.reset(make_big_int(nullptr, reset_sp(amount)));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ(nullptr, amount);

    error.reset(make_big_int("1", nullptr));
    EXPECT_NE(nullptr, error);
}

GTEST_TEST(BigIntTest_API_InvalidArgs, make_big_int_clone)
{
    BigInt origin(1);
    BigIntPtr dest;

    EXPECT_ERROR(make_big_int_clone(&origin,  nullptr));

    EXPECT_ERROR(make_big_int_clone(nullptr,  reset_sp(dest)));
    ASSERT_EQ(nullptr, dest);
}

GTEST_TEST(BigIntTest_API_InvalidArgs, big_int_get_value)
{
    ErrorPtr error;
    BigInt amount(1);
    ConstCharPtr str;

    error.reset(big_int_get_value(nullptr, reset_sp(str)));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ(nullptr, str);

    error.reset(big_int_get_value(&amount, nullptr));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ("1", amount);
}

GTEST_TEST(BigIntTest_API_InvalidArgs, big_int_set_value)
{
    ErrorPtr error;
    BigInt amount(1);

    error.reset(big_int_set_value(nullptr, "1"));
    EXPECT_NE(nullptr, error);

    error.reset(big_int_set_value(&amount, nullptr));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ("1", amount);
}


GTEST_TEST(BigIntTest_API_InvalidValue, make_big_int)
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

GTEST_TEST(BigIntTest_API_InvalidValue, big_int_set_value)
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

GTEST_TEST(BigIntTest_API, big_int_get_int64_value)
{
    const int64_t expected = DEFAULT_INT64_VALUE;

    BigInt big_int(expected);

    int64_t actual = 0;
    HANDLE_ERROR(big_int_get_int64_value(&big_int, &actual));
    EXPECT_EQ(expected, actual);
}

GTEST_TEST(BigIntTest_API, big_int_set_int64_value)
{
    const int64_t expected = DEFAULT_INT64_VALUE;

    BigInt big_int(0);

    HANDLE_ERROR(big_int_set_int64_value(&big_int, expected));
    EXPECT_EQ(expected, big_int);
}

GTEST_TEST(BigIntTest_API, make_big_int)
{
    ErrorPtr error;
    BigIntPtr amount;

    error.reset(make_big_int("1",  reset_sp(amount)));
    EXPECT_EQ(nullptr, error);
    EXPECT_EQ("1", *amount);
}

GTEST_TEST(BigIntTest_API, make_big_int_clone)
{
    BigInt origin(1);
    BigIntPtr result;

    HANDLE_ERROR(make_big_int_clone(&origin, reset_sp(result)));
    EXPECT_EQ(origin, *result);
}

GTEST_TEST(BigIntTest_API, big_int_get_value)
{
    ErrorPtr error;
    BigInt amount(1);
    ConstCharPtr test_str;

    error.reset(big_int_get_value(&amount, reset_sp(test_str)));
    EXPECT_EQ(nullptr, error);
    EXPECT_NE(nullptr, test_str);
    EXPECT_EQ("1", amount);
}

GTEST_TEST(BigIntTest_API, big_int_set_value)
{
    ErrorPtr error;
    BigInt amount(5);

    error.reset(big_int_set_value(&amount, "1"));
    EXPECT_EQ(nullptr, error);
    EXPECT_EQ("1", amount);
}

GTEST_TEST(BigIntTest_API, free_big_int)
{
    BigInt* amount;
    make_big_int("1", &amount);

    EXPECT_NO_THROW(free_big_int(amount));
}

GTEST_TEST(BigIntTest_API, get_value_as_int64)
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

GTEST_TEST(BigIntTest_API, get_value_as_uint64)
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
