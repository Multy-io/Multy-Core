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

Error* make_big_int_from_int64(int64_t value, BigInt** new_big_int)
{
    ARG_CHECK(new_big_int);

    try
    {
        *new_big_int = new BigInt(value);
    }
    CATCH_EXCEPTION_RETURN_ERROR();

    OUT_CHECK_OBJECT(*new_big_int);

    return nullptr;
}

Error* big_int_get_value(const BigInt* big_int, const char** out_string_value)
{
    ARG_CHECK_OBJECT(big_int);
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
    ARG_CHECK_OBJECT(big_int);
    ARG_CHECK(value);

    try
    {
        big_int->set_value(value);
    }
    CATCH_EXCEPTION_RETURN_ERROR();

    return nullptr;
}

Error* big_int_get_int64_value(const BigInt* big_int, int64_t* out_value)
{
    ARG_CHECK_OBJECT(big_int);
    ARG_CHECK(out_value);

    try
    {
        *out_value = big_int->get_value_as_int64();
    }
    CATCH_EXCEPTION_RETURN_ERROR();

    return nullptr;
}

Error* big_int_set_int64_value(BigInt* big_int, int64_t value)
{
    ARG_CHECK_OBJECT(big_int);

    try
    {
        big_int->set_value_int64(value);
    }
    CATCH_EXCEPTION_RETURN_ERROR();

    return nullptr;
}

#define BIG_INT_OP(statement) \
    try \
    { \
        statement; \
    } \
    CATCH_EXCEPTION_RETURN_ERROR()

Error* big_int_add(BigInt* target, const BigInt* value)
{
    ARG_CHECK_OBJECT(target);
    ARG_CHECK_OBJECT(value);

    BIG_INT_OP(*target += *value);

    return nullptr;
}

Error* big_int_add_int64(BigInt* target, int64_t value)
{
    ARG_CHECK_OBJECT(target);

    BIG_INT_OP(*target += value);

    return nullptr;
}

Error* big_int_sub(BigInt* target, const BigInt* value)
{
    ARG_CHECK_OBJECT(target);
    ARG_CHECK_OBJECT(value);

    BIG_INT_OP(*target -= *value);

    return nullptr;
}

Error* big_int_sub_int64(BigInt* target, int64_t value)
{
    ARG_CHECK_OBJECT(target);

    BIG_INT_OP(*target -= value);

    return nullptr;
}

Error* big_int_mul(BigInt* target, const BigInt* value)
{
    ARG_CHECK_OBJECT(target);
    ARG_CHECK_OBJECT(value);

    BIG_INT_OP(*target *= *value);

    return nullptr;
}

Error* big_int_mul_int64(BigInt* target, int64_t value)
{
    ARG_CHECK_OBJECT(target);

    BIG_INT_OP(*target *= value);

    return nullptr;
}

Error* big_int_div(BigInt* target, const BigInt* value)
{
    ARG_CHECK_OBJECT(target);
    ARG_CHECK_OBJECT(value);

    BIG_INT_OP(*target /= *value);

    return nullptr;
}

Error* big_int_div_int64(BigInt* target, int64_t value)
{
    ARG_CHECK_OBJECT(target);

    BIG_INT_OP(*target /= value);

    return nullptr;
}

void free_big_int(BigInt* big_int)
{
    CHECK_OBJECT_BEFORE_FREE(big_int);
    delete big_int;
}
