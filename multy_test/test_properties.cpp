/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license
 *
 * See LICENSE for details
 */

#include "multy_core/src/api/properties_impl.h"
#include "multy_core/properties.h"

#include "multy_core/common.h"
#include "multy_core/error.h"
#include "multy_core/src/api/account_impl.h"
#include "multy_core/src/api/big_int_impl.h"
#include "multy_core/src/api/key_impl.h"
#include "multy_core/src/exception.h"
#include "multy_core/src/u_ptr.h"
#include "multy_core/src/utility.h"

#include "multy_test/utility.h"

#include "gtest/gtest.h"

#include <memory>

struct BinaryData;
namespace
{
using namespace multy_core::internal;
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
    Properties properties("TEST");
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
    Properties properties("TEST");
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

GTEST_TEST(PropertiesTestInvalidArgs, properties_set_big_int_value)
{
    ErrorPtr error;
    Properties properties("TEST");
    BigInt amount_property(0);
    const BigInt invalid_amount(1);

    error.reset(properties_set_big_int_value(&properties, "v", &invalid_amount));
    EXPECT_NE(nullptr, error);

    properties.bind_property("v", &amount_property);

    error.reset(properties_set_big_int_value(&properties, "", &invalid_amount));
    EXPECT_NE(nullptr, error);
    EXPECT_STREQ("0", amount_property.get_value().c_str());

    error.reset(
            properties_set_big_int_value(&properties, nullptr, &invalid_amount));
    EXPECT_NE(nullptr, error);
    EXPECT_STREQ("0", amount_property.get_value().c_str());

    error.reset(properties_set_big_int_value(nullptr, "v", &invalid_amount));
    EXPECT_NE(nullptr, error);
    EXPECT_STREQ("0", amount_property.get_value().c_str());
}

GTEST_TEST(PropertiesTestInvalidArgs, properties_set_binary_data_value)
{
    ErrorPtr error;
    Properties properties("TEST");
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
    Properties properties("TEST");
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
    Properties properties("TEST");
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
    Properties properties("TEST");
    ConstCharPtr specification;

    error.reset(properties_get_specification(&properties, nullptr));
    EXPECT_NE(nullptr, error);

    error.reset(properties_get_specification(nullptr, reset_sp(specification)));
    EXPECT_NE(nullptr, error);
}

GTEST_TEST(PropertiesTestInvalidValue, properties_validate)
{
    ErrorPtr error;
    Properties properties("TEST");
    int32_t int_property = 1;

    properties.bind_property("v", &int_property);
    properties_reset_value(&properties, "v");

    error.reset(properties_validate(&properties));
    EXPECT_NE(nullptr, error);
}

GTEST_TEST(PropertiesTestInvalidType, int32_properties)
{
    ErrorPtr error;
    Properties properties("TEST");
    int32_t int_value = 1;
    const std::string str = "0";
    const BigInt amount(0);

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

    error.reset(properties_set_big_int_value(&properties, "v", &amount));
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
    Properties properties("TEST");
    const int32_t int_value = 1;
    std::string str = "0";
    const BigInt amount(0);

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

    error.reset(properties_set_big_int_value(&properties, "v", &amount));
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
    Properties properties("TEST");
    const std::string str = "0";
    const int32_t int_value = 1;
    BigInt amount(2);

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
    Properties properties("TEST");
    const std::string str = "0";
    const int32_t int_value = 1;
    const BigInt amount(2);

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

    error.reset(properties_set_big_int_value(&properties, "v", &amount));
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
    Properties properties("TEST");
    const std::string str = "0";
    const int32_t int_value = 1;
    const BigInt amount(2);

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

    error.reset(properties_set_big_int_value(&properties, "v", &amount));
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
    Properties properties("TEST");
    int32_t int_property = 0;

    properties.bind_property("v", &int_property);

    error.reset(properties_set_int32_value(&properties, "v", 42));
    EXPECT_EQ(nullptr, error);
    EXPECT_EQ(42, int_property);
}

GTEST_TEST(PropertiesTest, properties_set_string_value)
{
    ErrorPtr error;
    Properties properties("TEST");
    std::string str = "2";

    properties.bind_property("v", &str);

    error.reset(properties_set_string_value(&properties, "v", "42"));
    EXPECT_EQ(nullptr, error);
    EXPECT_STREQ("42", str.c_str());
}

GTEST_TEST(PropertiesTest, properties_set_big_int_value)
{
    ErrorPtr error;
    Properties properties("TEST");
    BigInt amount(0);
    const BigInt amount1(1);

    properties.bind_property("v", &amount);

    error.reset(properties_set_big_int_value(&properties, "v", &amount1));
    EXPECT_EQ(nullptr, error);
    EXPECT_STREQ("1", amount.get_value().c_str());
}

GTEST_TEST(PropertiesTest, properties_set_binary_data_value)
{
    ErrorPtr error;
    Properties properties("TEST");
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
    Properties properties("TEST");
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
    Properties properties("TEST");
    int32_t int_property = 1;
    properties.bind_property("v", &int_property);

    error.reset(properties_reset_value(&properties, "v"));
    EXPECT_EQ(nullptr, error);
    EXPECT_FALSE(properties.get_property("v").is_set());
}

GTEST_TEST(PropertiesTest, properties_validate)
{
    ErrorPtr error;
    Properties properties("TEST");
    int32_t int_property = 1;

    error.reset(properties_validate(&properties));
    EXPECT_EQ(nullptr, error);

    properties.bind_property("v", &int_property);
    properties.set_property_value("v", 5);

    error.reset(properties_validate(&properties));
    EXPECT_EQ(nullptr, error);

    properties.reset_property("v");
    error.reset(properties_validate(&properties));
    EXPECT_NE(nullptr, error);
}

GTEST_TEST(PropertiesTest, properties_get_specification)
{
    ErrorPtr error;
    Properties properties("TEST");
    ConstCharPtr specification;
    int int_property = 3;

    properties.bind_property("MANY_LONG_STRING_FOR_CHECKING", &int_property);
    properties.set_property_value("MANY_LONG_STRING_FOR_CHECKING", 5);

    error.reset(
            properties_get_specification(&properties, reset_sp(specification)));
    EXPECT_EQ(nullptr, error);

    EXPECT_NE(
            nullptr,
            strstr(specification.get(), "MANY_LONG_STRING_FOR_CHECKING"));
}


template <typename T>
T get_test_data();

template <>
int32_t get_test_data<int32_t>()
{
    return 100;
}

template <>
std::string get_test_data<std::string>()
{
    return "std::string";
}

template <>
BigInt get_test_data<BigInt>()
{
    return BigInt(100);
}

template <>
BinaryDataPtr get_test_data<BinaryDataPtr>()
{
    const char data[] = "BinaryData";
    BinaryDataPtr new_value;
    throw_if_error(make_binary_data_from_bytes(reinterpret_cast<const unsigned char*>(data),
            array_size(data), reset_sp(new_value)));
    return new_value;
}

template <>
PrivateKeyPtr get_test_data<PrivateKeyPtr>()
{
    struct TestPrivateKey : public ::PrivateKey
    {
        std::string to_string() const
        {
            return "test_key";
        }

        multy_core::internal::PublicKeyPtr make_public_key() const
        {
            throw std::runtime_error("not implemented");
        }

        PrivateKeyPtr clone() const
        {
            return PrivateKeyPtr(new TestPrivateKey);
        }
        BinaryDataPtr sign(const BinaryData& /*data*/) const
        {
            throw std::runtime_error("not implemented");
        }
    };

    return PrivateKeyPtr(new TestPrivateKey);
}

template<typename>
struct is_unique_ptr_helper : public std::false_type
{};

template<typename T, typename D>
struct is_unique_ptr_helper<std::unique_ptr<T, D>> : public std::true_type
{};

template<typename T>
struct is_unique_ptr : public ::is_unique_ptr_helper<typename std::remove_cv<T>::type>::type
{};

template <typename T>
const T& strip_unique_ptr(const T& value)
{
    return value;
}

template <typename T, typename D>
const T& strip_unique_ptr(const std::unique_ptr<T, D>& value)
{
    if (!value)
    {
        THROW_EXCEPTION("value is nullptr");
    }

    return *value;
}

template <typename T>
struct PropertyT_TestP : public ::testing::Test
{
    /** It would be a good idea to put all common stuff here, like:
     *      static T INIT_VALUE = T();
     *      static const T DATA = get_test_data<T>();
     *      Properties properties
     *      const char* property_name = "p";
     * but for some reason that makes it impossible to refer to above variables in test body without 'this':
     *      this->NEW_VALUE
     * which feels clumsy and too verbose.
     */
};

TYPED_TEST_CASE_P(PropertyT_TestP);

TYPED_TEST_P(PropertyT_TestP, Properties_SmokeTest)
{
    const TypeParam INIT_VALUE = TypeParam();
    const TypeParam& DATA = get_test_data<TypeParam>();
    const auto& NEW_VALUE = strip_unique_ptr(DATA);
    const char* property_name = "p";
    Properties properties("Test");

    PropertyT<TypeParam> property(properties, property_name, Property::REQUIRED);

    TypeParam out;
    if (is_unique_ptr<TypeParam>::value)
    {
        // default value for std::unique_ptr-based values is nullptr, which causes an exception.
        EXPECT_THROW(properties.get_property_value(property_name, &out), Exception);
    }
    else
    {
        properties.get_property_value(property_name, &out);
    }
    EXPECT_EQ(INIT_VALUE, out);

    properties.set_property_value(property_name, NEW_VALUE);
    properties.get_property_value(property_name, &out);
    EXPECT_EQ(NEW_VALUE, strip_unique_ptr(out));
}

TYPED_TEST_P(PropertyT_TestP, DynamicProperty)
{
    const TypeParam& DATA = get_test_data<TypeParam>();
    const auto& NEW_VALUE = strip_unique_ptr(DATA);
    const char* property_name = "p";
    Properties properties("Test");

    {
        // Testing that PropertyT unbinds itself when it gets out of scope.
        PropertyT<TypeParam> property(properties, property_name);
        properties.set_property_value(property_name, NEW_VALUE);
        EXPECT_TRUE(property.is_set());
        EXPECT_EQ(NEW_VALUE, strip_unique_ptr(*property));
    }

    // property is unbound now, setting a value throws an exception rather than corrupting some
    // random piece of memory.
    EXPECT_THROW(properties.set_property_value(property_name, NEW_VALUE), Exception);
}

TYPED_TEST_P(PropertyT_TestP, is_dirty)
{
    const TypeParam& DATA = get_test_data<TypeParam>();
    const auto& NEW_VALUE = strip_unique_ptr(DATA);
    const char* property_name = "p";
    Properties properties("Test");

    PropertyT<TypeParam> property(properties, property_name, Property::REQUIRED);

    // it is dirty initially
    EXPECT_TRUE(properties.is_dirty());

    EXPECT_FALSE(properties.validate(nullptr));
    // still dirty since validation failed
    EXPECT_TRUE(properties.is_dirty());

    properties.set_property_value(property_name, NEW_VALUE);
    // value was set, but validation wasn't performed - still dirty.
    EXPECT_TRUE(properties.is_dirty());
    EXPECT_TRUE(properties.validate(nullptr));
    // value is set, and validation succeed - not dirty.
    EXPECT_FALSE(properties.is_dirty());

    properties.set_property_value(property_name, NEW_VALUE);
    // value was set, dirty again.
    EXPECT_TRUE(properties.is_dirty());
}

TYPED_TEST_P(PropertyT_TestP, set_value)
{
    const TypeParam& DATA = get_test_data<TypeParam>();
    const auto& NEW_VALUE = strip_unique_ptr(DATA);
    const char* property_name = "p";
    Properties properties("Test");

    PropertyT<TypeParam> property(properties, property_name, Property::REQUIRED);

    EXPECT_FALSE(property.is_set());
    property.set_value(NEW_VALUE);

    EXPECT_TRUE(property.is_set());
    EXPECT_EQ(NEW_VALUE, strip_unique_ptr(property.get_value()));
}

TYPED_TEST_P(PropertyT_TestP, get_value)
{
    const TypeParam& DATA = get_test_data<TypeParam>();
    const auto& NEW_VALUE = strip_unique_ptr(DATA);
    const char* property_name = "p";
    Properties properties("Test");

    PropertyT<TypeParam> property(properties, property_name, Property::REQUIRED);

    EXPECT_FALSE(property.is_set());
    EXPECT_THROW(property.get_value(), multy_core::internal::Exception);

    properties.set_property_value(property_name, NEW_VALUE);
    EXPECT_EQ(NEW_VALUE, strip_unique_ptr(property.get_value()));
}

TYPED_TEST_P(PropertyT_TestP, operator_star)
{
    // testing 'operator*' which is shorthand for 'get_value()'

    const TypeParam& DATA = get_test_data<TypeParam>();
    const auto& NEW_VALUE = strip_unique_ptr(DATA);
    const char* property_name = "p";
    Properties properties("Test");

    PropertyT<TypeParam> property(properties, property_name, Property::REQUIRED);

    EXPECT_FALSE(property.is_set());
    EXPECT_THROW(*property, multy_core::internal::Exception);

    properties.set_property_value(property_name, NEW_VALUE);
    EXPECT_EQ(NEW_VALUE, strip_unique_ptr(*property));
}

TYPED_TEST_P(PropertyT_TestP, get_default_value)
{
    const TypeParam& DATA = get_test_data<TypeParam>();
    const char* property_name = "p";
    Properties properties("Test");

    PropertyT<TypeParam> property(properties, property_name, Property::REQUIRED);

    EXPECT_FALSE(property.is_set());
    EXPECT_EQ(DATA, property.get_default_value(DATA));
    EXPECT_FALSE(property.is_set());
}

REGISTER_TYPED_TEST_CASE_P(PropertyT_TestP,
        Properties_SmokeTest,
        DynamicProperty,
        is_dirty,
        set_value,
        get_value,
        operator_star,
        get_default_value);

typedef ::testing::Types<int32_t, std::string, BigInt, BinaryDataPtr, PrivateKeyPtr> PropertyT_Types;
INSTANTIATE_TYPED_TEST_CASE_P(PropertiesTest, PropertyT_TestP, PropertyT_Types);
