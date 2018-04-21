/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_CORE_BIG_INT_H
#define MULTY_CORE_BIG_INT_H

#include "multy_core/api.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct BigInt;
struct Error;

MULTY_CORE_API struct Error* make_big_int(
        const char* value, struct BigInt** new_big_int);

MULTY_CORE_API struct Error* make_big_int_clone(
        const struct BigInt* original, struct BigInt** new_big_int);

MULTY_CORE_API struct Error* make_big_int_from_int64(
        int64_t value, struct BigInt** new_big_int);

MULTY_CORE_API struct Error* big_int_get_value(
        const struct BigInt* big_int, const char** out_string_value);

MULTY_CORE_API struct Error* big_int_set_value(
        struct BigInt* big_int, const char* value);

MULTY_CORE_API struct Error* big_int_get_int64_value(
        const struct BigInt* big_int, int64_t* out_value);

MULTY_CORE_API struct Error* big_int_set_int64_value(
        struct BigInt* big_int, int64_t value);

MULTY_CORE_API struct Error* big_int_add(struct BigInt* target, const struct BigInt* value);
MULTY_CORE_API struct Error* big_int_add_int64(struct BigInt* target, int64_t value);
MULTY_CORE_API struct Error* big_int_add_double(struct BigInt* target, double value);

MULTY_CORE_API struct Error* big_int_sub(struct BigInt* target, const struct BigInt* value);
MULTY_CORE_API struct Error* big_int_sub_int64(struct BigInt* target, int64_t value);
MULTY_CORE_API struct Error* big_int_sub_double(struct BigInt* target, double value);

MULTY_CORE_API struct Error* big_int_mul(struct BigInt* target, const struct BigInt* value);
MULTY_CORE_API struct Error* big_int_mul_int64(struct BigInt* target, int64_t value);
MULTY_CORE_API struct Error* big_int_mul_double(struct BigInt* target, double value);

MULTY_CORE_API struct Error* big_int_div(struct BigInt* target, const struct BigInt* value);
MULTY_CORE_API struct Error* big_int_div_int64(struct BigInt* target, int64_t value);
MULTY_CORE_API struct Error* big_int_div_double(struct BigInt* target, double value);

MULTY_CORE_API struct Error* big_int_cmp(struct BigInt* left, struct BigInt* right, int* out_result);
MULTY_CORE_API struct Error* big_int_cmp_int64(struct BigInt* left, int64_t right, int* out_result);
MULTY_CORE_API struct Error* big_int_cmp_double(struct BigInt* left, double right, int* out_result);

MULTY_CORE_API void free_big_int(struct BigInt*);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* MULTY_CORE_BIG_INT_H */
