/* Copyright 2017 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_TRANSACTION_INTERNAL_TRANSACTION_H
#define MULTY_TRANSACTION_INTERNAL_TRANSACTION_H

#include "multy_transaction/api.h"
#include "multy_transaction/internal/amount.h"
#include "multy_transaction/internal/u_ptr.h"
#include "multy_transaction/transaction.h"

#include "multy_core/account.h"

#include <string>

struct Amount;
struct PublicKey;

struct MULTY_TRANSACTION_API TransactionChange
{
    virtual ~TransactionChange();

    virtual Amount get_total() const = 0;
    virtual Properties& get_properties() = 0;
};

struct MULTY_TRANSACTION_API TransactionFee
{
    virtual ~TransactionFee();

    virtual Amount get_total() const = 0;
    virtual Properties& get_properties() = 0;
};

struct MULTY_TRANSACTION_API Transaction
{
    typedef wallet_core::internal::BinaryDataPtr BinaryDataPtr;

    virtual ~Transaction();

    virtual Currency get_currency() const = 0;
    virtual uint32_t get_traits() const = 0;

    // Methods that implicitly update state of the transaction.
    virtual BinaryDataPtr serialize() = 0;
    virtual BinaryDataPtr get_hash() = 0;
    virtual Amount get_total() = 0;

    /** Update transaction internal state, recalculate fee, change, sign inputs,
     * etc.
     *
     * Must be called before serialize() or get_hash() to get valid results.
     */
    virtual void update_state() = 0;

    /** Add a source to a transaction.
     *
     * New object is returned every time, adding new source to the
     * transaction.
     * @return properties of the new source, DO NOT DELETE/FREE.
     * @exception may be thrown if transaction doesn't support more
     * sources.
     */
    virtual Properties& add_source() = 0;

    /** Add a destination to a transaction.
     *
     * New object is returned every time, adding new destination to the
     * transaction.
     * @return properties of the new destination, DO NOT DELETE/FREE.
     * @exception may be thrown if transaction doesn't support more
     * destinations.
     */
    virtual Properties& add_destination() = 0;

    /** Get transaction change.
     *
     * Same object is returned every time.
     * @return transaction fee object, DO NOT DELETE/FREE.
     * @exception may be thrown if transaction doesn't support fee.
     */
    virtual TransactionFee& get_fee() = 0;

    /** Get transaction change.
     *
     * Same object is returned every time.
     * @return transaction change object, DO NOT DELETE/FREE.
     * @exception may be thrown if transaction doesn't support change.
     */
    virtual TransactionChange& get_change() = 0;

    /** Get transaction own properties.
     *
     * @return own properties of the transaction, DO NOT DELETE/FREE.
     */
    virtual Properties& get_transaction_properties() = 0;
};

#endif // MULTY_TRANSACTION_INTERNAL_TRANSACTION_H
