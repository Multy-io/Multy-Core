/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_TEST_DEATH_TEST_H
#define MULTY_TEST_DEATH_TEST_H

#include <gtest/gtest.h>

// Disable death-test on non-Desktop platforms, since those might crash on mobile.
#if defined(MULTY_BUILD_FOR_DESKTOP) && MULTY_BUILD_FOR_DESKTOP
    #define MULTY_ASSERT_DEATH_IF_SUPPORTED(statement, message) \
            ASSERT_DEATH_IF_SUPPORTED(statement, message)
    #define MULTY_EXPECT_DEATH_IF_SUPPORTED(statement, message) \
            EXPECT_DEATH_IF_SUPPORTED(statement, message)
#else
    #define MULTY_ASSERT_DEATH_IF_SUPPORTED(statement, message)
    #define MULTY_EXPECT_DEATH_IF_SUPPORTED(statement, message)
#endif

#if defined(MULTY_TEST_DISABLE_DEATH_TESTS)
    #undef MULTY_ASSERT_DEATH_IF_SUPPORTED
    #undef MULTY_EXPECT_DEATH_IF_SUPPORTED

    #define MULTY_ON_DISABLED_DEATH_TEST(statement) \
do { std::cout << "DEATH TEST IS DISABLED FOR: " << #statement << " at " << __FILE__ << ":" << __LINE__ << std::endl; } while(false)

    #define MULTY_ASSERT_DEATH_IF_SUPPORTED(statement, message) \
            MULTY_ON_DISABLED_DEATH_TEST(statement)
    #define MULTY_EXPECT_DEATH_IF_SUPPORTED(statement, message) \
            MULTY_ON_DISABLED_DEATH_TEST(statement)

#endif

#endif // MULTY_TEST_DEATH_TEST_H
