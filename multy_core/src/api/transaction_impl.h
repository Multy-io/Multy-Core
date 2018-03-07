/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_CORE_SRC_API_TRANSACTION_IMPL_H
#define MULTY_CORE_SRC_API_TRANSACTION_IMPL_H

#include "multy_core/transaction.h"

#include "multy_core/api.h"
#include "multy_core/account.h"

#include "multy_core/src/api/big_int_impl.h"
#include "multy_core/src/object.h"
#include "multy_core/src/u_ptr.h"

#include <string>

struct BigInt;
struct PublicKey;

struct MULTY_CORE_API Transaction : public ::multy_core::internal::ObjectBase<Transaction>
{
    typedef multy_core::internal::BinaryDataPtr BinaryDataPtr;

    Transaction();
    virtual ~Transaction();

    virtual BlockchainType get_blockchain_type() const = 0;
    virtual uint32_t get_traits() const = 0;

    virtual void update() = 0;
    /** Serialize transaction to binary form.
     *
     * May verify/update internal state and throw exception if something is wrong.
     */
    virtual BinaryDataPtr serialize() = 0;

    virtual BigInt get_total_fee() const = 0;

    virtual BigInt get_total_spent() const = 0;

    virtual BigInt estimate_total_fee(size_t sources_count, size_t destinations_count) const = 0;

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

    virtual void set_message(const BinaryData& value) = 0;
    static const void* get_object_magic();
};

#endif // MULTY_CORE_SRC_API_TRANSACTION_IMPL_H
