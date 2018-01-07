/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license
 *
 * See LICENSE for details
 */

#include "multy_transaction/internal/amount.h"
#include "multy_transaction/internal/properties.h"
#include "multy_transaction/properties.h"

#include "multy_core/common.h"
#include "multy_core/error.h"
#include "multy_core/internal/account.h"
#include "multy_core/internal/key.h"
#include "multy_core/internal/u_ptr.h"
#include "multy_core/internal/utility.h"

#include "multy_test/utility.h"

#include "gtest/gtest.h"

#include <memory>

struct BinaryData;
namespace
{
using namespace wallet_core::internal;
using namespace test_utility;
}

#define ASSERT_ERROR(statement)                                                \
    do                                                                         \
    {                                                                          \
        ErrorPtr error(statement);                                             \
        SCOPED_TRACE(#statement);                                              \
        ASSERT_NE(nullptr, error);                                             \
    } while (false)

GTEST_TEST(PropertiesTestInvalidArgs, properties_set_int32_value)
{
    //    ErrorPtr error;
    Properties properties("");
    int32_t int_property = 0;

    ASSERT_ERROR(properties_set_int32_value(&properties, "v", 2));

    properties.bind_property("v", &int_property);

    ASSERT_ERROR(properties_set_int32_value(&properties, "", 1));
    EXPECT_EQ(0, int_property);

    ASSERT_ERROR(properties_set_int32_value(&properties, nullptr, 1));
    EXPECT_EQ(0, int_property);

    ASSERT_ERROR(properties_set_int32_value(nullptr, "v", 1));
    EXPECT_EQ(0, int_property);
}

GTEST_TEST(PropertiesTestInvalidArgs, properties_set_string_value)
{
    ErrorPtr error;
    Properties properties("");
    std::string value_property = "2";

    error.reset(properties_set_string_value(&properties, "v", "1"));
    EXPECT_NE(nullptr, error);

    properties.bind_property("v", &value_property);

    error.reset(properties_set_string_value(&properties, "v", nullptr));
    EXPECT_NE(nullptr, error);
    EXPECT_STREQ("2", value_property.c_str());

    error.reset(properties_set_string_value(&properties, "", "1"));
    EXPECT_NE(nullptr, error);
    EXPECT_STREQ("2", value_property.c_str());

    error.reset(properties_set_string_value(&properties, nullptr, "1"));
    EXPECT_NE(nullptr, error);
    EXPECT_STREQ("2", value_property.c_str());

    error.reset(properties_set_string_value(nullptr, "v", "1"));
    EXPECT_NE(nullptr, error);
    EXPECT_STREQ("2", value_property.c_str());
}

GTEST_TEST(PropertiesTestInvalidArgs, properties_set_amount_value)
{
    ErrorPtr error;
    Properties properties("");
    Amount amount_property(0);
    const Amount invalid_amount(1);

    error.reset(properties_set_amount_value(&properties, "v", &invalid_amount));
    EXPECT_NE(nullptr, error);

    properties.bind_property("v", &amount_property);

    error.reset(properties_set_amount_value(&properties, "", &invalid_amount));
    EXPECT_NE(nullptr, error);
    EXPECT_STREQ("0", amount_property.get_value().c_str());

    error.reset(
            properties_set_amount_value(&properties, nullptr, &invalid_amount));
    EXPECT_NE(nullptr, error);
    EXPECT_STREQ("0", amount_property.get_value().c_str());

    error.reset(properties_set_amount_value(nullptr, "v", &invalid_amount));
    EXPECT_NE(nullptr, error);
    EXPECT_STREQ("0", amount_property.get_value().c_str());
}

GTEST_TEST(PropertiesTestInvalidArgs, properties_set_binary_data_value)
{
    ErrorPtr error;
    Properties properties("");
    const unsigned char data_2_vals[] = {4U, 2U};
    BinaryDataPtr binaty_data_property;
    BinaryData binary_data2{data_2_vals, 2};
    BinaryDataPtr binary_data_check_not_nullptr(
            make_clone(BinaryData{data_2_vals, 2}));

    error.reset(
            properties_set_binary_data_value(&properties, "v", &binary_data2));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ(nullptr, binaty_data_property);

    properties.bind_property("v", &binaty_data_property);

    error.reset(
            properties_set_binary_data_value(
                    &properties, nullptr, &binary_data2));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ(nullptr, binaty_data_property);

    error.reset(properties_set_binary_data_value(nullptr, "v", &binary_data2));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ(nullptr, binaty_data_property);

    properties.bind_property("nv", &binary_data_check_not_nullptr);

    error.reset(properties_set_binary_data_value(&properties, "nv", nullptr));
    EXPECT_NE(nullptr, error);
    EXPECT_NE(nullptr, binary_data_check_not_nullptr);
}

GTEST_TEST(PropertiesTestInvalidArgs, properties_set_private_key_value)
{
    ErrorPtr error;
    Properties properties("");
    PrivateKeyPtr priv_key_property;
    PrivateKeyPtr priv_key_property1;

    AccountPtr account1;
    make_account(
            CURRENCY_BITCOIN,
            "cScuLx5taDyuAfCnin5WWZz65yGCHMuuaFv6mgearmqAHC4p53sz",
            reset_sp(account1));

    priv_key_property1 = account1->get_private_key();

    error.reset(
            properties_set_private_key_value(
                    &properties, "v", priv_key_property.get()));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ(nullptr, priv_key_property);

    properties.bind_property("v", &priv_key_property);

    error.reset(
            properties_set_private_key_value(
                    &properties, nullptr, priv_key_property1.get()));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ(nullptr, priv_key_property);

    error.reset(
            properties_set_private_key_value(
                    nullptr, "v", priv_key_property1.get()));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ(nullptr, priv_key_property);

    properties.bind_property("vn", &priv_key_property1);

    error.reset(properties_set_private_key_value(&properties, "vn", nullptr));
    EXPECT_NE(nullptr, error);
    EXPECT_NE(nullptr, priv_key_property1);
}

GTEST_TEST(PropertiesTestInvalidArgs, properties_reset_value)
{
    ErrorPtr error;
    Properties properties("");
    int32_t int_property = 1;

    properties.bind_property("v", &int_property);

    error.reset(properties_reset_value(&properties, ""));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ(1, int_property);

    error.reset(properties_reset_value(&properties, nullptr));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ(1, int_property);

    error.reset(properties_reset_value(nullptr, "v"));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ(1, int_property);
}

GTEST_TEST(PropertiesTestInvalidArgs, properties_validate)
{
    ErrorPtr error;

    error.reset(properties_validate(nullptr));
    EXPECT_NE(nullptr, error);
}

GTEST_TEST(PropertiesTestInvalidArgs, properties_get_specification)
{
    ErrorPtr error;
    Properties properties("");
    ConstCharPtr specification;

    error.reset(properties_get_specification(&properties, nullptr));
    EXPECT_NE(nullptr, error);

    error.reset(properties_get_specification(nullptr, reset_sp(specification)));
    EXPECT_NE(nullptr, error);
}

GTEST_TEST(PropertiesTestInvalidValue, properties_validate)
{
    ErrorPtr error;
    Properties properties("");
    int32_t int_property = 1;

    properties.bind_property("v", &int_property);
    properties_reset_value(&properties, "v");

    error.reset(properties_validate(&properties));
    EXPECT_NE(nullptr, error);
}

GTEST_TEST(PropertiesTestInvalidType, int32_properties)
{
    ErrorPtr error;
    Properties properties("");
    int32_t int_value = 1;
    const std::string str = "0";
    const Amount amount(0);

    const unsigned char data_4_vals[] = {1U, 2U, 3U, 4U};
    const BinaryDataPtr binaty_data_4_property(
            make_clone(BinaryData{data_4_vals, 4}));

    AccountPtr account;
    error.reset(
            make_account(
                    CURRENCY_BITCOIN,
                    "cQeGKosJjWPn9GkB7QmvmotmBbVg1hm8UjdN6yLXEWZ5HAcRwam7",
                    reset_sp(account)));

    const PrivateKeyPtr priv_key_property = account->get_private_key();

    properties.bind_property("v", &int_value);

    error.reset(properties_set_string_value(&properties, "v", str.c_str()));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ(1, int_value);

    error.reset(properties_set_amount_value(&properties, "v", &amount));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ(1, int_value);

    error.reset(
            properties_set_binary_data_value(
                    &properties, "v", binaty_data_4_property.get()));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ(1, int_value);

    error.reset(
            properties_set_private_key_value(
                    &properties, "v", priv_key_property.get()));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ(1, int_value);
}

GTEST_TEST(PropertiesTestInvalidType, String_properties)
{
    ErrorPtr error;
    Properties properties("");
    const int32_t int_value = 1;
    std::string str = "0";
    const Amount amount(0);

    const unsigned char data_4_vals[] = {1U, 2U, 3U, 4U};
    const BinaryDataPtr binaty_data_4_property(
            make_clone(BinaryData{data_4_vals, 4}));

    AccountPtr account;
    error.reset(
            make_account(
                    CURRENCY_BITCOIN,
                    "cQeGKosJjWPn9GkB7QmvmotmBbVg1hm8UjdN6yLXEWZ5HAcRwam7",
                    reset_sp(account)));

    const PrivateKeyPtr priv_key_property = account->get_private_key();

    properties.bind_property("v", &str);

    error.reset(properties_set_int32_value(&properties, "v", int_value));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ("0", str);

    error.reset(properties_set_amount_value(&properties, "v", &amount));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ("0", str);

    error.reset(
            properties_set_binary_data_value(
                    &properties, "v", binaty_data_4_property.get()));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ("0", str);

    error.reset(
            properties_set_private_key_value(
                    &properties, "v", priv_key_property.get()));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ("0", str);
}

GTEST_TEST(PropertiesTestInvalidType, Amount_properties)
{
    ErrorPtr error;
    Properties properties("");
    const std::string str = "0";
    const int32_t int_value = 1;
    Amount amount(2);

    const unsigned char data_4_vals[] = {1U, 2U, 3U, 4U};
    const BinaryDataPtr binaty_data_4_property(
            make_clone(BinaryData{data_4_vals, 4}));

    AccountPtr account;
    error.reset(
            make_account(
                    CURRENCY_BITCOIN,
                    "cQeGKosJjWPn9GkB7QmvmotmBbVg1hm8UjdN6yLXEWZ5HAcRwam7",
                    reset_sp(account)));

    const PrivateKeyPtr priv_key_property = account->get_private_key();

    properties.bind_property("v", &amount);

    error.reset(properties_set_int32_value(&properties, "v", int_value));
    EXPECT_NE(nullptr, error);
    EXPECT_STREQ("2", amount.get_value().c_str());

    error.reset(properties_set_string_value(&properties, "v", str.c_str()));
    EXPECT_NE(nullptr, error);
    EXPECT_STREQ("2", amount.get_value().c_str());

    error.reset(
            properties_set_binary_data_value(
                    &properties, "v", binaty_data_4_property.get()));
    EXPECT_NE(nullptr, error);
    EXPECT_STREQ("2", amount.get_value().c_str());

    error.reset(
            properties_set_private_key_value(
                    &properties, "v", priv_key_property.get()));
    EXPECT_NE(nullptr, error);
    EXPECT_STREQ("2", amount.get_value().c_str());
}

GTEST_TEST(PropertiesTestInvalidType, BinaryData_properties)
{
    ErrorPtr error;
    Properties properties("");
    const std::string str = "0";
    const int32_t int_value = 1;
    const Amount amount(2);

    const unsigned char data_4_vals[] = {1U, 2U, 3U, 4U};
    const BinaryData reference_value{data_4_vals, 4};
    BinaryDataPtr binaty_data_4_property(
            make_clone(BinaryData{data_4_vals, 4}));

    AccountPtr account;
    error.reset(
            make_account(
                    CURRENCY_BITCOIN,
                    "cQeGKosJjWPn9GkB7QmvmotmBbVg1hm8UjdN6yLXEWZ5HAcRwam7",
                    reset_sp(account)));

    const PrivateKeyPtr priv_key_property = account->get_private_key();

    properties.bind_property("v", &binaty_data_4_property);

    error.reset(properties_set_int32_value(&properties, "v", int_value));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ(reference_value, *binaty_data_4_property);

    error.reset(properties_set_string_value(&properties, "v", str.c_str()));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ(reference_value, *binaty_data_4_property);

    error.reset(properties_set_amount_value(&properties, "v", &amount));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ(reference_value, *binaty_data_4_property);

    error.reset(
            properties_set_private_key_value(
                    &properties, "v", priv_key_property.get()));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ(reference_value, *binaty_data_4_property);
}

GTEST_TEST(PropertiesTestInvalidType, PublicKey_properties)
{
    ErrorPtr error;
    Properties properties("");
    const std::string str = "0";
    const int32_t int_value = 1;
    const Amount amount(2);

    const unsigned char data_4_vals[] = {1U, 2U, 3U, 4U};
    const BinaryDataPtr binaty_data_4_property(
            make_clone(BinaryData{data_4_vals, 4}));

    PrivateKeyPtr priv_key_property;

    properties.bind_property("v", &priv_key_property);

    error.reset(properties_set_int32_value(&properties, "v", int_value));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ(nullptr, priv_key_property);

    error.reset(properties_set_string_value(&properties, "v", str.c_str()));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ(nullptr, priv_key_property);

    error.reset(properties_set_amount_value(&properties, "v", &amount));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ(nullptr, priv_key_property);

    error.reset(
            properties_set_binary_data_value(
                    &properties, "v", binaty_data_4_property.get()));
    EXPECT_NE(nullptr, error);
    EXPECT_EQ(nullptr, priv_key_property);
}

GTEST_TEST(PropertiesTest, properties_set_int32_value)
{
    ErrorPtr error;
    Properties properties("");
    int32_t int_property = 0;

    properties.bind_property("v", &int_property);

    error.reset(properties_set_int32_value(&properties, "v", 42));
    EXPECT_EQ(nullptr, error);
    EXPECT_EQ(42, int_property);
}

GTEST_TEST(PropertiesTest, properties_set_string_value)
{
    ErrorPtr error;
    Properties properties("");
    std::string str = "2";

    properties.bind_property("v", &str);

    error.reset(properties_set_string_value(&properties, "v", "42"));
    EXPECT_EQ(nullptr, error);
    EXPECT_STREQ("42", str.c_str());
}

GTEST_TEST(PropertiesTest, properties_set_amount_value)
{
    ErrorPtr error;
    Properties properties("");
    Amount amount(0);
    const Amount amount1(1);

    properties.bind_property("v", &amount);

    error.reset(properties_set_amount_value(&properties, "v", &amount1));
    EXPECT_EQ(nullptr, error);
    EXPECT_STREQ("1", amount.get_value().c_str());
}

GTEST_TEST(PropertiesTest, properties_set_binary_data_value)
{
    ErrorPtr error;
    Properties properties("");
    const unsigned char data_4_vals[] = {1U, 2U, 3U, 4U};
    const unsigned char data_2_vals[] = {4U, 2U};
    const BinaryData reference_data_2_value{data_2_vals, 2};
    const BinaryData reference_data_4_value{data_4_vals, 4};
    BinaryDataPtr binaty_data_property(make_clone(BinaryData{data_4_vals, 4}));
    const BinaryDataPtr binaty_data_2_property(
            make_clone(BinaryData{data_2_vals, 2}));

    properties.bind_property("v", &binaty_data_property);

    error.reset(
            properties_set_binary_data_value(
                    &properties, "v", binaty_data_2_property.get()));
    EXPECT_EQ(nullptr, error);
    EXPECT_EQ(reference_data_2_value, *binaty_data_property);

    error.reset(
            properties_set_binary_data_value(
                    &properties, "v", binaty_data_property.get()));
    EXPECT_EQ(nullptr, error);
    EXPECT_NE(reference_data_4_value, *binaty_data_property);
}

GTEST_TEST(PropertiesTest, properties_set_private_key_value)
{
    ErrorPtr error;
    Properties properties("");
    PrivateKeyPtr priv_key_property;

    AccountPtr account1;
    error.reset(
            make_account(
                    CURRENCY_BITCOIN,
                    "cScuLx5taDyuAfCnin5WWZz65yGCHMuuaFv6mgearmqAHC4p53sz",
                    reset_sp(account1)));
    const PrivateKeyPtr priv_key_property1 = account1->get_private_key();

    properties.bind_property("v", &priv_key_property);

    error.reset(
            properties_set_private_key_value(
                    &properties, "v", priv_key_property1.get()));
    EXPECT_EQ(nullptr, error);
    EXPECT_EQ(*priv_key_property, *priv_key_property1);
}

GTEST_TEST(PropertiesTest, properties_reset_value)
{
    ErrorPtr error;
    Properties properties("");
    int32_t int_property = 1;
    properties.bind_property("v", &int_property);

    error.reset(properties_reset_value(&properties, "v"));
    EXPECT_EQ(nullptr, error);
    EXPECT_FALSE(properties.get_property("v").is_set());
}

GTEST_TEST(PropertiesTest, properties_validate)
{
    ErrorPtr error;
    Properties properties("");
    int32_t int_property = 1;

    error.reset(properties_validate(&properties));
    EXPECT_EQ(nullptr, error);

    properties.bind_property("v", &int_property);
    properties.set_property("v", 5);

    error.reset(properties_validate(&properties));
    EXPECT_EQ(nullptr, error);

    properties.reset_property("v");
    error.reset(properties_validate(&properties));
    EXPECT_NE(nullptr, error);
}

GTEST_TEST(PropertiesTest, properties_get_specification)
{
    ErrorPtr error;
    Properties properties("");
    ConstCharPtr specification;
    int int_property = 3;

    properties.bind_property("MANY_LONG_STRING_FOR_CHECKING", &int_property);
    properties.set_property("MANY_LONG_STRING_FOR_CHECKING", 5);

    error.reset(
            properties_get_specification(&properties, reset_sp(specification)));
    EXPECT_EQ(nullptr, error);

    EXPECT_NE(
            nullptr,
            strstr(specification.get(), "MANY_LONG_STRING_FOR_CHECKING"));
}
