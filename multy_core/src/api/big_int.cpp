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
    CATCH_EXCEPTION_RETURN_ERROR(ERROR_SCOPE_BIG_INT);

    OUT_CHECK_OBJECT(*new_big_int);

    return nullptr;
}

Error* make_big_int_clone(const BigInt* original, BigInt** new_big_int)
{
    ARG_CHECK_OBJECT(original);
    ARG_CHECK(new_big_int);

    try
    {
        *new_big_int = new BigInt(*original);
    }
    CATCH_EXCEPTION_RETURN_ERROR(ERROR_SCOPE_BIG_INT);

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
    CATCH_EXCEPTION_RETURN_ERROR(ERROR_SCOPE_BIG_INT);

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
    CATCH_EXCEPTION_RETURN_ERROR(ERROR_SCOPE_BIG_INT);

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
    CATCH_EXCEPTION_RETURN_ERROR(ERROR_SCOPE_BIG_INT);

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
    CATCH_EXCEPTION_RETURN_ERROR(ERROR_SCOPE_BIG_INT);

    return nullptr;
}

Error* big_int_set_int64_value(BigInt* big_int, int64_t value)
{
    ARG_CHECK_OBJECT(big_int);

    try
    {
        *big_int = BigInt(value);
    }
    CATCH_EXCEPTION_RETURN_ERROR(ERROR_SCOPE_BIG_INT);

    return nullptr;
}

#define BIG_INT_OP(statement) \
    try \
    { \
        statement; \
    } \
    CATCH_EXCEPTION_RETURN_ERROR(ERROR_SCOPE_BIG_INT)

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

Error* big_int_add_double(BigInt* target, double value)
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

Error* big_int_sub_double(BigInt* target, double value)
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

Error* big_int_mul_double(BigInt* target, double value)
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

Error* big_int_div_double(BigInt* target, double value)
{
    ARG_CHECK_OBJECT(target);

    BIG_INT_OP(*target /= value);

    return nullptr;
}

Error* big_int_cmp(struct BigInt* left, struct BigInt* right, int* out_result)
{
    ARG_CHECK_OBJECT(left);
    ARG_CHECK_OBJECT(right);
    ARG_CHECK(out_result);

    BIG_INT_OP(*out_result = left->compare(*right));

    return nullptr;
}

Error* big_int_cmp_int64(struct BigInt* left, int64_t right, int* out_result)
{
    ARG_CHECK_OBJECT(left);
    ARG_CHECK(out_result);

    BIG_INT_OP(*out_result = left->compare(right));

    return nullptr;
}

Error* big_int_cmp_double(struct BigInt* left, double right, int* out_result)
{
    ARG_CHECK_OBJECT(left);
    ARG_CHECK(out_result);

    BIG_INT_OP(*out_result = left->compare(right));

    return nullptr;
}

void free_big_int(BigInt* big_int)
{
    CHECK_OBJECT_BEFORE_FREE(big_int);
    delete big_int;
}
