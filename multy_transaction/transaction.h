/* Copyright 2017 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_TRANSACTION_TRANSACTION_H
#define MULTY_TRANSACTION_TRANSACTION_H

#include "multy_transaction/api.h"

#include "multy_core/account.h"

#ifdef __cplusplus
extern "C" {
#endif

struct Account;
struct Amount;
struct BinaryData;
struct Error;
struct TransactionFee;
struct TransactionChange;
struct Transaction;

enum TransactionTrait
{
    TRANSACTION_REQUIRES_EXPLICIT_SOURCE,
    TRANSACTION_SUPPORTS_MULTIPLE_SOURCES,
    TRANSACTION_SUPPORTS_MULTIPLE_DESTINATIONS,
    TRANSACTION_SUPPORTS_CHANGE,
    TRANSACTION_SUPPORTS_FEE,
};

MULTY_TRANSACTION_API struct Error* make_transaction(
        const struct Account* account, struct Transaction** new_transaction);

MULTY_TRANSACTION_API struct Error* transaction_has_trait(
        const struct Transaction* transaction,
        TransactionTrait trait,
        bool* out_has_capability);

MULTY_TRANSACTION_API struct Error* transaction_get_currency(
        const struct Transaction* transaction, Currency* out_currency);

MULTY_TRANSACTION_API struct Error* transaction_add_source(
        struct Transaction* transaction, struct Properties** source);

/// @param destination - new destination, must NOT be freed by the caller.
MULTY_TRANSACTION_API struct Error* transaction_add_destination(
        struct Transaction* transaction,
        struct Properties** destination);

/// @param change - change destination, must NOT be freed by the caller.
MULTY_TRANSACTION_API struct Error* transaction_get_change(
        struct Transaction* transaction,
        struct TransactionChange** change);

/// @param fee - transaction fee, caller must NOT be freed by the caller.
MULTY_TRANSACTION_API struct Error* transaction_get_fee(
        struct Transaction* transaction, struct TransactionFee** fee);

MULTY_TRANSACTION_API struct Error* transaction_get_total(
        struct Transaction* transaction, struct Amount* out_total);

MULTY_TRANSACTION_API struct Error* transaction_serialize(
        const struct Transaction* transaction,
        struct BinaryData** out_transaction_str);

//MULTY_TRANSACTION_API struct Error* transaction_serialize_raw(
//        const struct Transaction* transaction,
//        const struct BinaryData** out_raw_transaction);

MULTY_TRANSACTION_API struct Error* transaction_get_hash(
        const struct Transaction* transaction,
        const struct BinaryData** out_transaction_hash);

MULTY_TRANSACTION_API void free_transaction(struct Transaction* transaction);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* MULTY_TRANSACTION_TRANSACTION_H */
