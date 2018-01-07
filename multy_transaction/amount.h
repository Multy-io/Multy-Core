/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_TRANSACTION_AMOUNT_H
#define MULTY_TRANSACTION_AMOUNT_H

#include "multy_transaction/api.h"

#ifdef __cplusplus
extern "C" {
#endif

struct Amount;
struct Error;

MULTY_TRANSACTION_API struct Error* make_amount(
        const char* amount_str, struct Amount** new_amount);

MULTY_TRANSACTION_API struct Error* amount_to_string(
        const struct Amount* amount, const char** out_amount_str);

MULTY_TRANSACTION_API struct Error* amount_set_value(
        struct Amount* amount, const char* value);

MULTY_TRANSACTION_API void free_amount(struct Amount*);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* MULTY_TRANSACTION_AMOUNT_H */
