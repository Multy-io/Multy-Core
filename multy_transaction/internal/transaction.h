/* Copyright 2018 by Multy.io
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

struct MULTY_TRANSACTION_API Transaction
{
    typedef wallet_core::internal::BinaryDataPtr BinaryDataPtr;

    Transaction();
    virtual ~Transaction();

    virtual Currency get_currency() const = 0;
    virtual uint32_t get_traits() const = 0;

    virtual BinaryDataPtr serialize() const = 0;
    virtual Amount get_total_fee() const = 0;
//    virtual Amount estimate_fee() const = 0;
    virtual Amount estimate_total_fee(size_t sources_count, size_t destinations_count) const = 0;

    /** Update transaction internal state, recalculate fee, change, sign inputs, etc.
     *
     * Must be called before serialize() or get_hash() to get valid results.
     */
    virtual void update_state() = 0;

    /** Sign transaction, i.e. make it ready to be serialized and send.
     */
    virtual void sign() = 0;

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
    virtual Properties& get_fee() = 0;

    /** Get transaction own properties.
     *
     * @return own properties of the transaction, DO NOT DELETE/FREE.
     */
    virtual Properties& get_transaction_properties() = 0;

    bool is_valid() const;

private:
    const void* m_magic;
};

#endif // MULTY_TRANSACTION_INTERNAL_TRANSACTION_H
