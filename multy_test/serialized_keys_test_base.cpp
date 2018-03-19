/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "serialized_keys_test_base.h"

#include "multy_core/account.h"
#include "multy_core/blockchain.h"
#include "multy_core/key.h"

#include "multy_core/src/u_ptr.h"
#include "multy_core/src/utility.h"

#include "multy_test/utility.h"
#include "multy_test/value_printers.h"

namespace
{
using namespace multy_core::internal;
using namespace test_utility;
} // namespace

void PrintTo(const SerializedKeyTestCase& c, std::ostream* out)
{
    *out << "SerializedKeyTestCase {\n"
         << "\tprivate key: " << c.private_key << ",\n"
         << "\tpublic key: " << c.public_key << ",\n"
         << "\taddress: " << c.address << "\n"
         << "}";
}

void SerializedKeyTestP::SetUp()
{
    const BlockchainType blockchain_type = ::testing::get<0>(GetParam());
    const SerializedKeyTestCase& test_data = ::testing::get<1>(GetParam());

    HANDLE_ERROR(
            make_account(
                    blockchain_type.blockchain,
                    test_data.private_key, reset_sp(account)));
    ASSERT_NE(nullptr, account);
}

TEST_P(SerializedKeyTestP, public_key)
{
    const SerializedKeyTestCase& test_data = ::testing::get<1>(GetParam());

    ConstCharPtr public_key_string;
    KeyPtr public_key;
    HANDLE_ERROR(
            account_get_key(account.get(), KEY_TYPE_PUBLIC, reset_sp(public_key)));
    ASSERT_NE(nullptr, public_key);

    if (test_data.public_key && strlen(test_data.public_key))
    {
        HANDLE_ERROR(key_to_string(public_key.get(), reset_sp(public_key_string)));
        ASSERT_NE(nullptr, public_key_string);
        ASSERT_STREQ(test_data.public_key, public_key_string.get());
    }
}

TEST_P(SerializedKeyTestP, address)
{
    const SerializedKeyTestCase& test_data = ::testing::get<1>(GetParam());
    if (!test_data.address || strlen(test_data.address) == 0)
    {
        ASSERT_FALSE(blockchain_can_derive_address_from_private_key(
                ::testing::get<0>(GetParam()).blockchain));
        return;
    }

    ConstCharPtr address;
    HANDLE_ERROR(account_get_address_string(account.get(), reset_sp(address)));
    ASSERT_NE(nullptr, address);
    ASSERT_STREQ(test_data.address, address.get());
}

TEST_P(SerializedKeyTestP, private_key)
{
    const SerializedKeyTestCase& test_data = ::testing::get<1>(GetParam());

    KeyPtr private_key;
    HANDLE_ERROR(
            account_get_key(
                    account.get(), KEY_TYPE_PRIVATE, reset_sp(private_key)));
    ASSERT_NE(nullptr, private_key);

    ConstCharPtr serialized_private_key;
    HANDLE_ERROR(
            key_to_string(private_key.get(), reset_sp(serialized_private_key)));
    ASSERT_NE(nullptr, serialized_private_key);
    ASSERT_STREQ(test_data.private_key, serialized_private_key.get());
}

TEST_P(CheckAddressTestP, validate_address)
{
    const auto& param = GetParam();
    const BlockchainType blockchain_type = ::testing::get<0>(param);
    const char* test_data = ::testing::get<1>(param);
    HANDLE_ERROR(validate_address(blockchain_type, test_data));
}
