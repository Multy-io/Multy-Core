/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_test/serialized_keys_test_base.h"
#include "multy_core/account.h"

#include "multy_core/src/EOS/EOS_account.h"
#include "multy_core/src/api/key_impl.h"
#include "multy_core/EOS.h"

#include "multy_test/supported_blockchains.h"
#include "multy_test/utility.h"

#include "gtest/gtest.h"

namespace
{
using namespace multy_core::internal;
using namespace test_utility;

const SerializedKeyTestCase EOS_KEYS[] =
{
    {
        "5KJdX2hHqfgJhSf2TJjdgbYg4b4JLCRkKoyF2DSn2Dj5mvink7J",
        "EOS6WkssWjMkfrfMUZhKvzoEbEHfRRnm6k7trvrMZsefkF6ZAKdGK",
        ""
    },
    {
        "5Jte92DsHfdQJigfZCk4tGPA1evbfN38zniftNHqcFyg9mLxbJp",
        "EOS56Eq8xXW9J3CUHgckWprxBozYisaDMm3VFZdQZdWDbdNN6ehf3",
        ""
    },
    {
        "5JanB6wZj4k8wNqExKQ2aSCPdEVRHMgmDiwx2Veu5ffa4pHyvMT",
        "EOS72KPaQyEoZQo1FHFs8bNEppECDq8Y5f8XQFMyg8p4PKyWXFSB4",
        ""
    },
    {
        "5Jy2y2AaqnH6RMEZbs5dz1ap2ZXroXWqkEZ9iYTABFK6y946p8i",
        "EOS6y7jp8U18p7ZtgLg8NLaqpof1jb5W9ZewAsxegRnP381F1fbsr",
        ""
    },
    {
        "5KNcnmwteGFjSysLEGYx9Uq1GNWGNMvYgQTk8x2eDCPnBVYhjvq",
        "EOS8Z9wYkVdfdjkZpeJUBFQt6oMxoR9pNzyFfG1rUFpctmYerTVf3",
        ""
    }
};


INSTANTIATE_TEST_CASE_P(
        eos_account,
        SerializedKeyTestP,
        ::testing::Combine(
                ::testing::Values(EOS_MAIN_NET),
                ::testing::Values(ACCOUNT_TYPE_DEFAULT),
                ::testing::ValuesIn(EOS_KEYS)));

GTEST_TEST(EOSAccountTest, TestPrivateKey)
{
    AccountPtr account;
    EXPECT_ERROR(
            make_account(
                    EOS_MAIN_NET,
                    ACCOUNT_TYPE_DEFAULT,
                    "",
                    reset_sp(account)));
}

} // namespace
