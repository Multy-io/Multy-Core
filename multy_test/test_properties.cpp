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

#include "multy_test/supported_blockchains.h"
#include "multy_test/utility.h"
#include "multy_test/mocks.h"

#include "gtest/gtest.h"

#include <memory>

struct BinaryData;
namespace
{
using namespace multy_core::internal;
using namespace test_utility;
} // namespace


GTEST_TEST(PropertiesTestInvalidArgs, properties_set_int32_value)
{
    Properties properties(ERROR_SCOPE_GENERIC, "TEST");
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
    Properties properties(ERROR_SCOPE_GENERIC, "TEST");
    std::string value_property = "2";

    EXPECT_ERROR(properties_set_string_value(&properties, "v", "1"));

    properties.bind_property("v", &value_property);

    EXPECT_ERROR(properties_set_string_value(&properties, "v", nullptr));
    EXPECT_STREQ("2", value_property.c_str());

    EXPECT_ERROR(properties_set_string_value(&properties, "", "1"));
    EXPECT_STREQ("2", value_property.c_str());

    EXPECT_ERROR(properties_set_string_value(&properties, nullptr, "1"));
    EXPECT_STREQ("2", value_property.c_str());

    EXPECT_ERROR(properties_set_string_value(nullptr, "v", "1"));
    EXPECT_STREQ("2", value_property.c_str());
}

GTEST_TEST(PropertiesTestInvalidArgs, properties_set_big_int_value)
{
    Properties properties(ERROR_SCOPE_GENERIC, "TEST");
    BigInt amount_property(0);
    const BigInt invalid_amount(1);

    EXPECT_ERROR(properties_set_big_int_value(&properties, "v", &invalid_amount));

    properties.bind_property("v", &amount_property);

    EXPECT_ERROR(properties_set_big_int_value(&properties, "", &invalid_amount));
    EXPECT_STREQ("0", amount_property.get_value().c_str());

    EXPECT_ERROR(
            properties_set_big_int_value(&properties, nullptr, &invalid_amount));
    EXPECT_STREQ("0", amount_property.get_value().c_str());

    EXPECT_ERROR(properties_set_big_int_value(nullptr, "v", &invalid_amount));
    EXPECT_STREQ("0", amount_property.get_value().c_str());
}

GTEST_TEST(PropertiesTestInvalidArgs, properties_set_binary_data_value)
{
    Properties properties(ERROR_SCOPE_GENERIC, "TEST");
    const unsigned char data_2_vals[] = {4U, 2U};
    BinaryDataPtr binaty_data_property;
    BinaryData binary_data2{data_2_vals, 2};
    BinaryDataPtr binary_data_check_not_nullptr(
            make_clone(BinaryData{data_2_vals, 2}));

    EXPECT_ERROR(
            properties_set_binary_data_value(&properties, "v", &binary_data2));
    EXPECT_EQ(nullptr, binaty_data_property);

    properties.bind_property("v", &binaty_data_property);

    EXPECT_ERROR(
            properties_set_binary_data_value(
                    &properties, nullptr, &binary_data2));
    EXPECT_EQ(nullptr, binaty_data_property);

    EXPECT_ERROR(properties_set_binary_data_value(nullptr, "v", &binary_data2));
    EXPECT_EQ(nullptr, binaty_data_property);

    properties.bind_property("nv", &binary_data_check_not_nullptr);

    EXPECT_ERROR(properties_set_binary_data_value(&properties, "nv", nullptr));
    EXPECT_NE(nullptr, binary_data_check_not_nullptr);
}

GTEST_TEST(PropertiesTestInvalidArgs, properties_set_private_key_value)
{
    Properties properties(ERROR_SCOPE_GENERIC, "TEST");
    PrivateKeyPtr priv_key_property;
    PrivateKeyPtr priv_key_property1;

    AccountPtr account1;
    HANDLE_ERROR(make_account(
            BITCOIN_TEST_NET,
            ACCOUNT_TYPE_DEFAULT,
            "cScuLx5taDyuAfCnin5WWZz65yGCHMuuaFv6mgearmqAHC4p53sz",
            reset_sp(account1)));

    priv_key_property1 = account1->get_private_key();

    EXPECT_ERROR(
            properties_set_private_key_value(
                    &properties, "v", priv_key_property.get()));
    EXPECT_EQ(nullptr, priv_key_property);

    properties.bind_property("v", &priv_key_property);

    EXPECT_ERROR(
            properties_set_private_key_value(
                    &properties, nullptr, priv_key_property1.get()));
    EXPECT_EQ(nullptr, priv_key_property);

    EXPECT_ERROR(
            properties_set_private_key_value(
                    nullptr, "v", priv_key_property1.get()));
    EXPECT_EQ(nullptr, priv_key_property);

    properties.bind_property("vn", &priv_key_property1);

    EXPECT_ERROR(properties_set_private_key_value(&properties, "vn", nullptr));
    EXPECT_NE(nullptr, priv_key_property1);
}

GTEST_TEST(PropertiesTestInvalidArgs, properties_reset_value)
{
    Properties properties(ERROR_SCOPE_GENERIC, "TEST");
    int32_t int_property = 1;

    properties.bind_property("v", &int_property);

    EXPECT_ERROR(properties_reset_value(&properties, ""));
    EXPECT_EQ(1, int_property);

    EXPECT_ERROR(properties_reset_value(&properties, nullptr));
    EXPECT_EQ(1, int_property);

    EXPECT_ERROR(properties_reset_value(nullptr, "v"));
    EXPECT_EQ(1, int_property);
}

GTEST_TEST(PropertiesTestInvalidArgs, properties_validate)
{
    EXPECT_ERROR(properties_validate(nullptr));
}

GTEST_TEST(PropertiesTestInvalidArgs, properties_get_specification)
{
    Properties properties(ERROR_SCOPE_GENERIC, "TEST");
    ConstCharPtr specification;

    EXPECT_ERROR(properties_get_specification(&properties, nullptr));

    EXPECT_ERROR(properties_get_specification(nullptr, reset_sp(specification)));
}

GTEST_TEST(PropertiesTestInvalidValue, properties_validate)
{
    Properties properties(ERROR_SCOPE_GENERIC, "TEST");
    int32_t int_property = 1;

    properties.bind_property("v", &int_property);
    properties_reset_value(&properties, "v");

    EXPECT_ERROR(properties_validate(&properties));
}

GTEST_TEST(PropertiesTestInvalidType, int32_properties)
{
    Properties properties(ERROR_SCOPE_GENERIC, "TEST");
    int32_t int_value = 1;
    const std::string str = "0";
    const BigInt amount(0);

    const unsigned char data_4_vals[] = {1U, 2U, 3U, 4U};
    const BinaryDataPtr binaty_data_4_property(
            make_clone(BinaryData{data_4_vals, 4}));

    AccountPtr account;
    HANDLE_ERROR(make_account(
            BITCOIN_TEST_NET,
            ACCOUNT_TYPE_DEFAULT,
            "cQeGKosJjWPn9GkB7QmvmotmBbVg1hm8UjdN6yLXEWZ5HAcRwam7",
            reset_sp(account)));

    const PrivateKeyPtr priv_key_property = account->get_private_key();

    properties.bind_property("v", &int_value);

    EXPECT_ERROR(properties_set_string_value(&properties, "v", str.c_str()));
    EXPECT_EQ(1, int_value);

    EXPECT_ERROR(properties_set_big_int_value(&properties, "v", &amount));
    EXPECT_EQ(1, int_value);

    EXPECT_ERROR(
            properties_set_binary_data_value(
                    &properties, "v", binaty_data_4_property.get()));
    EXPECT_EQ(1, int_value);

    EXPECT_ERROR(
            properties_set_private_key_value(
                    &properties, "v", priv_key_property.get()));
    EXPECT_EQ(1, int_value);
}

GTEST_TEST(PropertiesTestInvalidType, String_properties)
{
    Properties properties(ERROR_SCOPE_GENERIC, "TEST");
    const int32_t int_value = 1;
    std::string str = "0";
    const BigInt amount(0);

    const unsigned char data_4_vals[] = {1U, 2U, 3U, 4U};
    const BinaryDataPtr binaty_data_4_property(
            make_clone(BinaryData{data_4_vals, 4}));

    AccountPtr account;
    HANDLE_ERROR(make_account(
            BITCOIN_TEST_NET,
            ACCOUNT_TYPE_DEFAULT,
            "cQeGKosJjWPn9GkB7QmvmotmBbVg1hm8UjdN6yLXEWZ5HAcRwam7",
            reset_sp(account)));

    const PrivateKeyPtr priv_key_property = account->get_private_key();

    properties.bind_property("v", &str);

    EXPECT_ERROR(properties_set_int32_value(&properties, "v", int_value));
    EXPECT_EQ("0", str);

    EXPECT_ERROR(properties_set_big_int_value(&properties, "v", &amount));
    EXPECT_EQ("0", str);

    EXPECT_ERROR(
            properties_set_binary_data_value(
                    &properties, "v", binaty_data_4_property.get()));
    EXPECT_EQ("0", str);

    EXPECT_ERROR(
            properties_set_private_key_value(
                    &properties, "v", priv_key_property.get()));
    EXPECT_EQ("0", str);
}

GTEST_TEST(PropertiesTestInvalidType, Amount_properties)
{
    Properties properties(ERROR_SCOPE_GENERIC, "TEST");
    const std::string str = "0";
    const int32_t int_value = 1;
    BigInt amount(2);

    const unsigned char data_4_vals[] = {1U, 2U, 3U, 4U};
    const BinaryDataPtr binaty_data_4_property(
            make_clone(BinaryData{data_4_vals, 4}));

    AccountPtr account;
    HANDLE_ERROR(make_account(
            BITCOIN_TEST_NET,
            ACCOUNT_TYPE_DEFAULT,
            "cQeGKosJjWPn9GkB7QmvmotmBbVg1hm8UjdN6yLXEWZ5HAcRwam7",
            reset_sp(account)));

    const PrivateKeyPtr priv_key_property = account->get_private_key();

    properties.bind_property("v", &amount);

    EXPECT_ERROR(properties_set_int32_value(&properties, "v", int_value));
    EXPECT_STREQ("2", amount.get_value().c_str());

    EXPECT_ERROR(properties_set_string_value(&properties, "v", str.c_str()));
    EXPECT_STREQ("2", amount.get_value().c_str());

    EXPECT_ERROR(
            properties_set_binary_data_value(
                    &properties, "v", binaty_data_4_property.get()));
    EXPECT_STREQ("2", amount.get_value().c_str());

    EXPECT_ERROR(
            properties_set_private_key_value(
                    &properties, "v", priv_key_property.get()));
    EXPECT_STREQ("2", amount.get_value().c_str());
}

GTEST_TEST(PropertiesTestInvalidType, BinaryData_properties)
{
    Properties properties(ERROR_SCOPE_GENERIC, "TEST");
    const std::string str = "0";
    const int32_t int_value = 1;
    const BigInt amount(2);

    const unsigned char data_4_vals[] = {1U, 2U, 3U, 4U};
    const BinaryData reference_value{data_4_vals, 4};
    BinaryDataPtr binaty_data_4_property(
            make_clone(BinaryData{data_4_vals, 4}));

    AccountPtr account;
    HANDLE_ERROR(make_account(
            BITCOIN_TEST_NET,
            ACCOUNT_TYPE_DEFAULT,
            "cQeGKosJjWPn9GkB7QmvmotmBbVg1hm8UjdN6yLXEWZ5HAcRwam7",
            reset_sp(account)));

    const PrivateKeyPtr priv_key_property = account->get_private_key();

    properties.bind_property("v", &binaty_data_4_property);

    EXPECT_ERROR(properties_set_int32_value(&properties, "v", int_value));
    EXPECT_EQ(reference_value, *binaty_data_4_property);

    EXPECT_ERROR(properties_set_string_value(&properties, "v", str.c_str()));
    EXPECT_EQ(reference_value, *binaty_data_4_property);

    EXPECT_ERROR(properties_set_big_int_value(&properties, "v", &amount));
    EXPECT_EQ(reference_value, *binaty_data_4_property);

    EXPECT_ERROR(
            properties_set_private_key_value(
                    &properties, "v", priv_key_property.get()));
    EXPECT_EQ(reference_value, *binaty_data_4_property);
}

GTEST_TEST(PropertiesTestInvalidType, PublicKey_properties)
{
    Properties properties(ERROR_SCOPE_GENERIC, "TEST");
    const std::string str = "0";
    const int32_t int_value = 1;
    const BigInt amount(2);

    const unsigned char data_4_vals[] = {1U, 2U, 3U, 4U};
    const BinaryDataPtr binaty_data_4_property(
            make_clone(BinaryData{data_4_vals, 4}));

    PrivateKeyPtr priv_key_property;

    properties.bind_property("v", &priv_key_property);

    EXPECT_ERROR(properties_set_int32_value(&properties, "v", int_value));
    EXPECT_EQ(nullptr, priv_key_property);

    EXPECT_ERROR(properties_set_string_value(&properties, "v", str.c_str()));
    EXPECT_EQ(nullptr, priv_key_property);

    EXPECT_ERROR(properties_set_big_int_value(&properties, "v", &amount));
    EXPECT_EQ(nullptr, priv_key_property);

    EXPECT_ERROR(
            properties_set_binary_data_value(
                    &properties, "v", binaty_data_4_property.get()));
    EXPECT_EQ(nullptr, priv_key_property);
}

GTEST_TEST(PropertiesTest, properties_set_int32_value)
{
    Properties properties(ERROR_SCOPE_GENERIC, "TEST");
    int32_t int_property = 0;

    properties.bind_property("v", &int_property);

    HANDLE_ERROR(properties_set_int32_value(&properties, "v", 42));
    EXPECT_EQ(42, int_property);
}

GTEST_TEST(PropertiesTest, properties_set_string_value)
{
    Properties properties(ERROR_SCOPE_GENERIC, "TEST");
    std::string str = "2";

    properties.bind_property("v", &str);

    HANDLE_ERROR(properties_set_string_value(&properties, "v", "42"));
    EXPECT_STREQ("42", str.c_str());
}

GTEST_TEST(PropertiesTest, properties_set_big_int_value)
{
    Properties properties(ERROR_SCOPE_GENERIC, "TEST");
    BigInt amount(0);
    const BigInt amount1(1);

    properties.bind_property("v", &amount);

    HANDLE_ERROR(properties_set_big_int_value(&properties, "v", &amount1));
    EXPECT_STREQ("1", amount.get_value().c_str());
}

GTEST_TEST(PropertiesTest, properties_set_binary_data_value)
{
    Properties properties(ERROR_SCOPE_GENERIC, "TEST");
    const unsigned char data_4_vals[] = {1U, 2U, 3U, 4U};
    const unsigned char data_2_vals[] = {4U, 2U};
    const BinaryData reference_data_2_value{data_2_vals, 2};
    const BinaryData reference_data_4_value{data_4_vals, 4};
    BinaryDataPtr binaty_data_property(make_clone(BinaryData{data_4_vals, 4}));
    const BinaryDataPtr binaty_data_2_property(
            make_clone(BinaryData{data_2_vals, 2}));

    properties.bind_property("v", &binaty_data_property);

    HANDLE_ERROR(
            properties_set_binary_data_value(
                    &properties, "v", binaty_data_2_property.get()));
    EXPECT_EQ(reference_data_2_value, *binaty_data_property);

    HANDLE_ERROR(
            properties_set_binary_data_value(
                    &properties, "v", binaty_data_property.get()));
    EXPECT_NE(reference_data_4_value, *binaty_data_property);
}

GTEST_TEST(PropertiesTest, properties_set_private_key_value)
{
    Properties properties(ERROR_SCOPE_GENERIC, "TEST");
    PrivateKeyPtr priv_key_property;

    AccountPtr account1;
    HANDLE_ERROR(make_account(
            BITCOIN_TEST_NET,
            ACCOUNT_TYPE_DEFAULT,
            "cScuLx5taDyuAfCnin5WWZz65yGCHMuuaFv6mgearmqAHC4p53sz",
            reset_sp(account1)));

    const PrivateKeyPtr priv_key_property1 = account1->get_private_key();

    properties.bind_property("v", &priv_key_property);

    HANDLE_ERROR(
            properties_set_private_key_value(
                    &properties, "v", priv_key_property1.get()));
    EXPECT_EQ(*priv_key_property, *priv_key_property1);
}

GTEST_TEST(PropertiesTest, properties_reset_value)
{
    Properties properties(ERROR_SCOPE_GENERIC, "TEST");
    int32_t int_property = 1;
    properties.bind_property("v", &int_property);

    HANDLE_ERROR(properties_reset_value(&properties, "v"));
    EXPECT_FALSE(properties.get_property("v").is_set());
}

GTEST_TEST(PropertiesTest, properties_validate)
{
    Properties properties(ERROR_SCOPE_GENERIC, "TEST");
    int32_t int_property = 1;

    HANDLE_ERROR(properties_validate(&properties));

    properties.bind_property("v", &int_property);
    properties.set_property_value("v", 5);

    HANDLE_ERROR(properties_validate(&properties));

    properties.reset_property("v");
    EXPECT_ERROR(properties_validate(&properties));
}

GTEST_TEST(PropertiesTest, properties_get_specification)
{
    Properties properties(ERROR_SCOPE_GENERIC, "TEST");
    ConstCharPtr specification;
    int int_property = 3;

    properties.bind_property("MANY_LONG_STRING_FOR_CHECKING", &int_property);
    properties.set_property_value("MANY_LONG_STRING_FOR_CHECKING", 5);

    HANDLE_ERROR(
            properties_get_specification(&properties, reset_sp(specification)));
    EXPECT_NE(
            nullptr,
            strstr(specification.get(), "MANY_LONG_STRING_FOR_CHECKING"));
}


template <typename T>
T get_test_data();

template <>
int32_t get_test_data<int32_t>()
{
    return 42;
}

template <>
std::string get_test_data<std::string>()
{
    return "std::string";
}

template <>
BigInt get_test_data<BigInt>()
{
    return BigInt(42*1000*1000)*(1000*1000*1000);
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
    return PrivateKeyPtr(new TestPrivateKey);
}

enum FunctionalPropertyValue
{
    PROPERTY_UNSET,
    PROPERTY_SET,
};

template <>
FunctionalPropertyValue get_test_data<FunctionalPropertyValue>()
{
    return PROPERTY_SET;
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
struct remove_unique_ptr_helper
{
    typedef T type;
};

template <typename T, typename D>
struct remove_unique_ptr_helper<std::unique_ptr<T,D>>
{
    typedef T type;
};

template <typename T>
struct remove_unique_ptr
{
    typedef typename ::remove_unique_ptr_helper<typename std::remove_cv<T>::type>::type type;
};


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

template <typename R, typename T>
R DefaultWriterFunc(const T&)
{
    return get_test_data<R>();
}

template <typename T>
struct PropertyT_TestP : public ::testing::Test
{
    const T INIT_VALUE = T();
    const T DATA = get_test_data<T>();
    const typename remove_unique_ptr<T>::type& NEW_VALUE = strip_unique_ptr(DATA);
    const std::string property_name = "p";

    PropertyT_TestP()
        : properties(),
          property()
    {}

    void SetUp() override
    {
        properties.reset(new Properties(ERROR_SCOPE_GENERIC, "Test"));
        property.reset(new PropertyT<T>(*properties, property_name, Property::REQUIRED));
    }

    void TearDown() override
    {
        property.reset();
        properties.reset();
    }

public:
    std::unique_ptr<Properties> properties;
    std::unique_ptr<PropertyT<T>> property;
};

TYPED_TEST_CASE_P(PropertyT_TestP);

TYPED_TEST_P(PropertyT_TestP, Properties_SmokeTest)
{
    const auto& INIT_VALUE = this->INIT_VALUE;
    const auto& NEW_VALUE = this->NEW_VALUE;
    const auto& property_name = this->property_name;
    auto& properties = *this->properties;

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
    // Testing that PropertyT unbinds itself when it gets out of scope.

    const auto& NEW_VALUE = this->NEW_VALUE;
    auto& property = *this->property;
    const auto& property_name = this->property_name;
    auto& properties = *this->properties;

    properties.set_property_value(property_name, NEW_VALUE);
    EXPECT_TRUE(property.is_set());
    EXPECT_EQ(NEW_VALUE, strip_unique_ptr(*property));
    void* value_ptr = &*property;

    // Reset unique_ptr, destroying PropertyT object and unbinding the name and value.
    this->property.reset();

    EXPECT_FALSE(properties.is_set(value_ptr));
    EXPECT_THROW(properties.get_property(property_name), Exception);
    EXPECT_THROW(properties.get_property_by_value(value_ptr), Exception);

    // property is unbound now, setting a value throws an exception
    // rather than corrupting some random piece of memory.
    EXPECT_THROW(properties.set_property_value(property_name, NEW_VALUE), Exception);
}

TYPED_TEST_P(PropertyT_TestP, is_dirty)
{
    const auto& NEW_VALUE = this->NEW_VALUE;
    const auto& property_name = this->property_name;
    auto& properties = *this->properties;

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
    const auto& NEW_VALUE = this->NEW_VALUE;
    auto& property = *this->property;

    EXPECT_FALSE(property.is_set());
    property.set_value(NEW_VALUE);

    EXPECT_TRUE(property.is_set());
    EXPECT_EQ(NEW_VALUE, strip_unique_ptr(property.get_value()));
}

TYPED_TEST_P(PropertyT_TestP, get_value)
{
    const auto& NEW_VALUE = this->NEW_VALUE;
    const auto& property_name = this->property_name;
    auto& property = *this->property;
    auto& properties = *this->properties;

    EXPECT_FALSE(property.is_set());
    EXPECT_THROW(property.get_value(), multy_core::internal::Exception);

    properties.set_property_value(property_name, NEW_VALUE);
    EXPECT_EQ(NEW_VALUE, strip_unique_ptr(property.get_value()));
}

TYPED_TEST_P(PropertyT_TestP, operator_star)
{
    // testing 'operator*' which is shorthand for 'get_value()'
    const auto& NEW_VALUE = this->NEW_VALUE;
    const auto& property_name = this->property_name;
    auto& property = *this->property;
    auto& properties = *this->properties;

    EXPECT_FALSE(property.is_set());
    EXPECT_THROW(*property, multy_core::internal::Exception);

    properties.set_property_value(property_name, NEW_VALUE);
    EXPECT_EQ(NEW_VALUE, strip_unique_ptr(*property));
}

TYPED_TEST_P(PropertyT_TestP, get_value_or_default)
{
    const auto& DATA = this->DATA;
    auto& property = *this->property;

    EXPECT_FALSE(property.is_set());
    EXPECT_EQ(DATA, property.get_value_or_default(DATA));
    EXPECT_FALSE(property.is_set());
}

REGISTER_TYPED_TEST_CASE_P(PropertyT_TestP,
        Properties_SmokeTest,
        DynamicProperty,
        is_dirty,
        set_value,
        get_value,
        operator_star,
        get_value_or_default);

typedef ::testing::Types<int32_t, std::string, BigInt, BinaryDataPtr, PrivateKeyPtr> PropertyT_Types;
INSTANTIATE_TYPED_TEST_CASE_P(PropertiesTest, PropertyT_TestP, PropertyT_Types);

template <typename T>
struct FunctionalPropertyT_TestP : public ::testing::Test
{
    const T DATA = get_test_data<T>();
    const typename remove_unique_ptr<T>::type& NEW_VALUE = strip_unique_ptr(DATA);
    const char* const property_name = "p";

    typedef FunctionalPropertyT<FunctionalPropertyValue, typename remove_unique_ptr<T>::type> FunctionalPropertyType;

    FunctionalPropertyT_TestP()
        : properties(),
          property()
    {}

    void SetUp() override
    {
        properties.reset(new Properties(ERROR_SCOPE_GENERIC, "Test"));

        auto writer = &DefaultWriterFunc<FunctionalPropertyValue, typename remove_unique_ptr<T>::type>;
        auto reader = &DefaultWriterFunc<T, FunctionalPropertyValue>;

        property.reset(new FunctionalPropertyType(*properties,
                property_name,
                writer,
                reader,
                Property::REQUIRED));
    }

    void TearDown() override
    {
        property.reset();
        properties.reset();
    }

public:
    std::unique_ptr<Properties> properties;
    std::unique_ptr<FunctionalPropertyType> property;
};

TYPED_TEST_CASE_P(FunctionalPropertyT_TestP);

TYPED_TEST_P(FunctionalPropertyT_TestP, Properties_SmokeTest)
{
    const auto& NEW_VALUE = this->NEW_VALUE;
    const auto& property_name = this->property_name;
    auto& property = *this->property;
    auto& properties = *this->properties;

    // Unlike PropertyT, output value is generated from stored value and it is safe to read it for unset property.
    TypeParam out;
    properties.get_property_value(property_name, &out);

    properties.set_property_value(property_name, NEW_VALUE);
    ASSERT_EQ(PROPERTY_SET, *property);

    properties.get_property_value(property_name, &out);
    EXPECT_EQ(NEW_VALUE, strip_unique_ptr(out));
}

TYPED_TEST_P(FunctionalPropertyT_TestP, FunctionalPropertyT_SmokeTest)
{
    const auto& NEW_VALUE = this->NEW_VALUE;
    const auto& property_name = this->property_name;
    auto& property = *this->property;

    ASSERT_EQ(Property::REQUIRED, property.get_trait());
    ASSERT_EQ(property_name, property.get_name());

    ASSERT_FALSE(property.is_set());
    ASSERT_THROW(property.get_value(), Exception);
    ASSERT_THROW(*property, Exception);

    ASSERT_EQ(PROPERTY_SET, property.get_value_or_default(PROPERTY_SET));

    ASSERT_NO_THROW(property.set_value(NEW_VALUE));
    ASSERT_TRUE(property.is_set());
    ASSERT_EQ(PROPERTY_SET, property.get_value());
    ASSERT_EQ(PROPERTY_SET, *property);

    // Actually returns stored value.
    ASSERT_EQ(&property.get_value(), &property.get_value_or_default(PROPERTY_SET));
}

REGISTER_TYPED_TEST_CASE_P(FunctionalPropertyT_TestP,
        Properties_SmokeTest, FunctionalPropertyT_SmokeTest);

INSTANTIATE_TYPED_TEST_CASE_P(PropertiesTest, FunctionalPropertyT_TestP, PropertyT_Types);
