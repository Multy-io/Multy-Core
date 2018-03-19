/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_CORE_TRANSACTION_H
#define MULTY_CORE_TRANSACTION_H

#include "multy_core/api.h"

#include "multy_core/account.h"

#ifdef __cplusplus
extern "C" {
#endif

struct Account;
struct BigInt;
struct BinaryData;
struct Error;
struct Transaction;

MULTY_CORE_API struct Error* make_transaction(
        const struct Account* account, struct Transaction** new_transaction);

MULTY_CORE_API struct Error* transaction_get_blockchain_type(
        const struct Transaction* transaction, struct BlockchainType* out_blockchain_type);

/// @param source - new source, must NOT be freed by the caller.
MULTY_CORE_API struct Error* transaction_add_source(
        struct Transaction* transaction, struct Properties** source);

/// @param destination - new destination, must NOT be freed by the caller.
MULTY_CORE_API struct Error* transaction_add_destination(
        struct Transaction* transaction, struct Properties** destination);

/// @param fee - transaction fee, caller must NOT be freed by the caller.
MULTY_CORE_API struct Error* transaction_get_fee(
        struct Transaction* transaction, struct Properties** fee);

/// @param message - message which user wants to set.
MULTY_CORE_API struct Error* transaction_set_message(
        struct Transaction* transaction,
        const struct BinaryData* message);

/// @param transaction_properties - transaction-level properties, must NOT be freed by the caller.
MULTY_CORE_API struct Error* transaction_get_properties(
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
MULTY_CORE_API struct Error* transaction_estimate_total_fee(
        const struct Transaction* transaction,
        size_t sources_count,
        size_t destinations_count,
        struct BigInt** out_fee_estimation);

/** Get resulting fee value.
 * Call transaction_update() first to get up to date data.
 *
 * @param transaction - transaction to get fee from.
 * @param out_fee_total - fee value in transaction currency (BTC, ETH, etc.).
 */
MULTY_CORE_API struct Error* transaction_get_total_fee(
        const struct Transaction* transaction, struct BigInt** out_fee_total);

/** Get resulting total spent value.
 * Call transaction_update() first to get up to date data.
 *
 * @param transaction - transaction to get total spent value from.
 * @param out_fee_total - total value spent by this transaction.
 */
MULTY_CORE_API struct Error* transaction_get_total_spent(
        const struct Transaction* transaction, struct BigInt** out_total_spent);

/** Update transaction state.
 *
 * Invoke after modifying transaction internal state (by adding sources\destinations
 * and\or setting propertie* of any of those and or fee), to get up to date
 * info from transaction.
 * @param transaction - transaction to update.
 */
MULTY_CORE_API struct Error* transaction_update(
        struct Transaction* transaction);

MULTY_CORE_API struct Error* transaction_serialize(
        struct Transaction* transaction,
        struct BinaryData** out_serialized_transaction);

MULTY_CORE_API void free_transaction(struct Transaction* transaction);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* MULTY_CORE_TRANSACTION_H */
