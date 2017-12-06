/* Copyright 2017 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "serialized_keys_test_base.h"

#include "multy_core/account.h"
#include "multy_core/keys.h"

#include "multy_core/internal/u_ptr.h"
#include "multy_core/internal/utility.h"

#include "multy_test/value_printers.h"

namespace
{
using namespace wallet_core::internal;
} // namespace

void PrintTo(const SerializedKeyTestCase& c, std::ostream* out)
{
    *out << "SerializedKeyTestCase {\n"
         << "\tprivate key: " << c.private_key << ",\n"
         << "\tpublic key: " << c.public_key << ",\n"
         << "\taddress: " << c.address << "\n"
         << "}";
}

TEST_P(SerializedKeyTestP, private_key_to_address)
{
    const auto& param = GetParam();
    const Currency currency = ::testing::get<0>(param);
    const SerializedKeyTestCase& test_data = ::testing::get<1>(param);
    AccountPtr account;
    ErrorPtr error;
    error.reset(
            make_account(
                    currency, test_data.private_key, reset_sp(account)));
    EXPECT_EQ(nullptr, error);
    ASSERT_NE(nullptr, account);

//    KeyPtr private_key;
//    error.reset(
//            get_account_key(
//                    account.get(), KEY_TYPE_PRIVATE, reset_sp(private_key)));
//    EXPECT_EQ(nullptr, error);
//    ASSERT_NE(nullptr, private_key);

//    ConstCharPtr serialized_private_key;
//    error.reset(
//            key_to_string(private_key.get(), reset_sp(serialized_private_key)));
//    EXPECT_EQ(nullptr, error);
//    ASSERT_NE(nullptr, serialized_private_key);
//    ASSERT_STREQ(param.private_key, serialized_private_key.get());
    ConstCharPtr public_key_string;
    KeyPtr public_key;
    error.reset(
            get_account_key(account.get(), KEY_TYPE_PUBLIC, reset_sp(public_key)));
    EXPECT_EQ(nullptr, error);
    ASSERT_NE(nullptr, public_key);

    if (test_data.public_key && strlen(test_data.public_key))
    {
        error.reset(key_to_string(public_key.get(), reset_sp(public_key_string)));
        EXPECT_EQ(nullptr, error);
        ASSERT_NE(nullptr, public_key_string);
        ASSERT_STREQ(test_data.public_key, public_key_string.get());
    }

    ConstCharPtr address;
    error.reset(get_account_address_string(account.get(), reset_sp(address)));
    EXPECT_EQ(nullptr, error);
    ASSERT_NE(nullptr, address);
    ASSERT_STREQ(test_data.address, address.get());
}
