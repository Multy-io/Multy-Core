/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#define BUILDING_MULTY_TESTS 1
#include "multy_test/run_tests.h"

#include "gtest/gtest.h"

int run_tests(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
