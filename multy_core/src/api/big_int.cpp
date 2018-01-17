/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/big_int.h"

#include "multy_core/src/api/big_int_impl.h"

#include "multy_core/src/utility.h"

Error* make_big_int(const char* value, BigInt** new_big_int)
{
    ARG_CHECK(value);
    ARG_CHECK(new_big_int);

    try
    {
        *new_big_int = new BigInt(value);
    }
    CATCH_EXCEPTION_RETURN_ERROR();

    OUT_CHECK_OBJECT(*new_big_int);

    return nullptr;
}

Error* big_int_to_string(const BigInt* big_int, const char** out_string_value)
{
    ARG_CHECK(big_int);
    ARG_CHECK(out_string_value);

    try
    {
        *out_string_value
                = multy_core::internal::copy_string(big_int->get_value());
    }
    CATCH_EXCEPTION_RETURN_ERROR();

    OUT_CHECK(*out_string_value);

    return nullptr;
}

Error* big_int_set_value(BigInt* big_int, const char* value)
{
    ARG_CHECK(big_int);
    ARG_CHECK(value);

    try
    {
        big_int->set_value(value);
    }
    CATCH_EXCEPTION_RETURN_ERROR();

    return nullptr;
}

void free_big_int(BigInt* big_int)
{
    CHECK_OBJECT_BEFORE_FREE(big_int);
    delete big_int;
}
