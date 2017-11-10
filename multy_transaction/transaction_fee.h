/* Copyright 2017 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_TRANSACTION_FEE_H
#define MULTY_TRANSACTION_FEE_H

#include "multy_transaction/api.h"

#ifdef __cplusplus
extern "C" {
#endif

struct Amount;
struct Error;
struct Propeties;
struct TransactionFee;

MULTY_TRANSACTION_API struct Error* transaction_fee_get_total(
        struct TransactionFee* fee, struct Amount* amount);

MULTY_TRANSACTION_API struct Error* transaction_fee_get_properties(
        struct TransactionFee* fee, struct Properties** properties);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* MULTY_TRANSACTION_FEE_H */
