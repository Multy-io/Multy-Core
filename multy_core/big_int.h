/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_CORE_BIG_INT_H
#define MULTY_CORE_BIG_INT_H

#include "multy_core/api.h"

#ifdef __cplusplus
extern "C" {
#endif

struct BigInt;
struct Error;

MULTY_CORE_API struct Error* make_big_int(
        const char* value, struct BigInt** new_big_int);

MULTY_CORE_API struct Error* big_int_to_string(
        const struct BigInt* big_int, const char** out_string_value);

MULTY_CORE_API struct Error* big_int_set_value(
        struct BigInt* big_int, const char* value);

MULTY_CORE_API void free_big_int(struct BigInt*);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* MULTY_CORE_BIG_INT_H */
