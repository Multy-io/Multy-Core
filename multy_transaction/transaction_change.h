/* Copyright 2017 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_TRANSACTION_CHANGE_H
#define MULTY_TRANSACTION_CHANGE_H

#include "multy_transaction/api.h"

#ifdef __cplusplus
extern "C" {
#endif

struct TransactionChange;
struct Properties;

MULTY_TRANSACTION_API struct Error* transaction_change_get_total(
        struct TransactionChange* change,
        struct Amount* total);

MULTY_TRANSACTION_API struct Error* transaction_change_get_properties(
        struct TransactionChange* change,
        struct Properties** properties);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* MULTY_TRANSACTION_CHANGE_H */
