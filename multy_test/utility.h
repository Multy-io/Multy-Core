/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_TEST_UTILITY_H
#define MULTY_TEST_UTILITY_H

#include "multy_core/account.h"
#include "multy_core/common.h"
#include "multy_core/src/utility.h"
#include "multy_core/src/hash.h"

#include "multy_test/value_printers.h"

#include "gtest/gtest.h"

#include <string>
#include <vector>
#include <iostream>

#define HANDLE_ERROR(statement)                                                \
    do                                                                         \
    {                                                                          \
        multy_core::internal::ErrorPtr error(statement);                       \
        ASSERT_EQ(nullptr, error);                                             \
    } while (0)

#define EXPECT_ERROR(statement)                                                \
    do                                                                         \
    {                                                                          \
        multy_core::internal::ErrorPtr error(statement);                       \
        EXPECT_NE(nullptr, error);                                             \
    } while (0)

#define ASSERT_ERROR(statement)                                                \
    do                                                                         \
    {                                                                          \
        multy_core::internal::ErrorPtr error(statement);                                             \
        ASSERT_NE(nullptr, error);                                             \
    } while (false)

#define EXPECT_ERROR_WITH_CODE(statement, error_code)                          \
    do                                                                         \
    {                                                                          \
        multy_core::internal::ErrorPtr error(statement);                       \
        EXPECT_NE(nullptr, error);                                             \
        EXPECT_EQ(error_code, error->code);                                    \
    } while (0)

#define EXPECT_ERROR_WITH_SCOPE(statement, error_scope)                        \
    do                                                                         \
    {                                                                          \
        multy_core::internal::ErrorPtr error(statement);                       \
        EXPECT_NE(nullptr, error);                                             \
        EXPECT_EQ(error_scope, error_get_scope(error->code));                  \
    } while (0)

#define E(statement)                                                           \
    if ((statement) != 0)                                                      \
    {                                                                          \
        test_utility::throw_exception(#statement);                             \
    }

struct BinaryData;
struct ExtendedKey;
struct BlockchainType;
struct Error;

namespace test_utility
{
typedef std::vector<unsigned char> bytes;

bytes from_hex(const char* hex_str);
std::string to_hex(const bytes& bytes);
std::string to_hex(const BinaryData& data);
ExtendedKey make_dummy_extended_key();
multy_core::internal::ExtendedKeyPtr make_dummy_extended_key_ptr();
EntropySource make_dummy_entropy_source();

void throw_exception(const char* message);
void throw_exception_if_error(Error* error);
bool blockchain_can_derive_address_from_private_key(Blockchain blockchain);

std::string minify_json(const std::string &input_json);

BigInt operator"" _BTC(const long double btc);
BigInt operator"" _SATOSHI(const unsigned long long int btc);
BigInt operator "" _ETH(const long double eth);
BigInt operator "" _GWEI(const long double gwei);
BigInt operator "" _WEI(const unsigned long long int wei);
} // test_utility

bool operator==(const PrivateKey& lhs, const PrivateKey& rhs);
bool operator==(const PublicKey& lhs, const PublicKey& rhs);

using multy_core::internal::operator==;
using multy_core::internal::operator!=;

inline bool operator!=(const PublicKey& lhs, const PublicKey& rhs)
{
    return !(lhs == rhs);
}

inline bool operator!=(const PrivateKey& lhs, const PrivateKey& rhs)
{
    return !(lhs == rhs);
}

using multy_core::internal::operator==;
using multy_core::internal::operator!=;

#endif // MULTY_TEST_UTILITY_H
