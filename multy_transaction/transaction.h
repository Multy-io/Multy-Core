/* Copyright 2018 by Multy.io
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
struct Transaction;

enum TransactionTrait
{
    TRANSACTION_REQUIRES_EXPLICIT_SOURCE,
    TRANSACTION_SUPPORTS_MULTIPLE_SOURCES,
    TRANSACTION_SUPPORTS_MULTIPLE_DESTINATIONS,
    TRANSACTION_SUPPORTS_FEE,
};

MULTY_TRANSACTION_API struct Error* make_transaction(
        const struct Account* account, struct Transaction** new_transaction);

MULTY_TRANSACTION_API struct Error* transaction_has_trait(
        const struct Transaction* transaction,
        enum TransactionTrait trait,
        bool* out_has_capability);

MULTY_TRANSACTION_API struct Error* transaction_get_currency(
        const struct Transaction* transaction, enum Currency* out_currency);

/// @param source - new source, must NOT be freed by the caller.
MULTY_TRANSACTION_API struct Error* transaction_add_source(
        struct Transaction* transaction, struct Properties** source);

/// @param destination - new destination, must NOT be freed by the caller.
MULTY_TRANSACTION_API struct Error* transaction_add_destination(
        struct Transaction* transaction, struct Properties** destination);

/// @param fee - transaction fee, must NOT be freed by the caller.
MULTY_TRANSACTION_API struct Error* transaction_get_fee(
        struct Transaction* transaction, struct Properties** fee);

/// @param transaction_properties - transaction-level properties, must NOT be freed by the caller.
MULTY_TRANSACTION_API struct Error* transaction_get_properties(
        struct Transaction* transaction, struct Properties** transaction_properties);

/** Estimate a fee value without serializing and signing the transaction.
 * Transaction must have sources, destinations and fee properly set up
 * in order to provide an estimation.
 * @param transaction - transaction to performa estimate on;
 * @param sources_count - number of sources transaction is going to use;
 * @param destinations_count - number of destinations transaction is going to use;
 * @param out_fee_estimation - fee estimation in transaction currency (BTC, ETHC, etc.).
 *
 * Please note that final fee value might differ from this estimation.
 * @throw Exception if any of the required transaction (or fee) properties are not set.
 */
MULTY_TRANSACTION_API struct Error* transaction_estimate_total_fee(
        const struct Transaction* transaction,
        size_t sources_count,
        size_t destinations_count,
        struct Amount** out_fee_estimation);

/** Get resulting fee value.
 * In order to update, invoke transaction_update().
 *
 * @param transaction - transaction to get fee from.
 * @param out_fee_total - fee value in transaction currency (BTC, ETH, etc.).
 */
MULTY_TRANSACTION_API struct Error* transaction_get_total_fee(
        const struct Transaction* transaction, struct Amount** out_fee_total);

/** Validate and update transaction internal state.
 *
 * Validate sources, destinations, compute change, fee, sign etc.
 * Call this before serializing transaction or computing it's hash.
 */
MULTY_TRANSACTION_API struct Error* transaction_update(
        struct Transaction* transaction);

/** Sign transaction, making it ready to be serialized and sent.
 * Call this before serializing
 */
MULTY_TRANSACTION_API struct Error* transaction_sign(
        struct Transaction* transaction);

MULTY_TRANSACTION_API struct Error* transaction_serialize(
        const struct Transaction* transaction,
        struct BinaryData** out_serialized_transaction);

MULTY_TRANSACTION_API void free_transaction(struct Transaction* transaction);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* MULTY_TRANSACTION_TRANSACTION_H */
