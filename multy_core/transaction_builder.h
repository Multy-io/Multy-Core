/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_CORE_TRANSACTION_BUILDER_H
#define MULTY_CORE_TRANSACTION_BUILDER_H

#include "multy_core/api.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct Error;
struct Transaction;
struct TransactionBuilder;

/** Make a new transaction builder object.
 *
 * @param account: account that is used to sign transaction.
 * @param type: blockchain-specific transaction builder type.
 * @param action: type-specific action, could be nullptr.
 * @param new_transaction_buidler: new TransactionBuilder object,
 *      which has to be freed by caller with free_transaction_buidler().
 */
MULTY_CORE_API struct Error* make_transaction_builder(
        const struct Account* account, uint32_t type, const char* action,
        struct TransactionBuilder** new_transaction_builder);

/** Get properties of TransactionBuilder
 */
MULTY_CORE_API struct Error* transaction_builder_get_properties(
        struct TransactionBuilder* transaction_builder,
        struct Properties** transaction_builder_properties);

/** Make Transaction from TransactionBuilder
 *
 * @param new_transaction: new Transaction object, caller MUST free it with
 *      free_transaction().
 */
MULTY_CORE_API struct Error* transaction_builder_make_transaction(
        const struct TransactionBuilder* transaction_builder,
        struct Transaction** new_transaction);

MULTY_CORE_API void free_transaction_builder(
        struct TransactionBuilder* transaction_builder);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // MULTY_CORE_TRANSACTION_BUILDER_H
