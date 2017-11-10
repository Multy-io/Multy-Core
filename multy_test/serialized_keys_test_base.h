/* Copyright 2017 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_TEST_SERIALIZED_KEYS_TEST_BASE_H
#define MULTY_TEST_SERIALIZED_KEYS_TEST_BASE_H

#include "multy_core/account.h"

#include "gtest/gtest.h"

#include <iosfwd>

struct SerializedKeyTestCase
{
    const char* private_key;
    const char* public_key;
    const char* address;
};

void PrintTo(const SerializedKeyTestCase& c, std::ostream* out);

class SerializedKeyTestP : public ::testing::TestWithParam<
        ::testing::tuple<Currency, SerializedKeyTestCase>>
{
};



#endif // MULTY_TEST_SERIALIZED_KEYS_TEST_BASE_H
