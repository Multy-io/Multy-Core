/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license
 *
 * See LICENSE for details
 */

#include "multy_core/src/object.h"

#include "gtest/gtest.h"

namespace
{
using namespace multy_core::internal;
struct TestObject : public ObjectBase<TestObject>
{
    static const void* get_object_magic()
    {
        RETURN_MAGIC();
    }
};

// Test basic detection of use-after-free.
GTEST_TEST(ObjectTest, is_valid)
{
    TestObject* test = new TestObject;
    ASSERT_TRUE(test->is_valid());

    delete test;
    ASSERT_FALSE(test->is_valid());

    test = nullptr;
    ASSERT_FALSE(test->is_valid());
}

} // namespace
