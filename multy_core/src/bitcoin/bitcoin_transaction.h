/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_CORE_BITCOIN_TRANSACTION_H
#define MULTY_CORE_BITCOIN_TRANSACTION_H

#include "multy_core/src/transaction_base.h"
#include "multy_core/src/bitcoin/bitcoin_account.h"
#include "multy_core/src/api/properties_impl.h"
#include "multy_core/src/bitcoin/bitcoin_transaction_base.h"

#include <memory>
#include <vector>

namespace multy_core
{
namespace internal
{
enum DestinationsToUse
{
    WITH_POSITIVE_CHANGE_AMOUNT,
    WITH_NONPOSITIVE_CHANGE_AMOUNT
};

class BitcoinTransactionSource : public BitcoinTransactionSourceBase
{
public:
    BitcoinTransactionSource();
    ~BitcoinTransactionSource();

    void serializeToStream(BitcoinStream*) const override;
};

class BitcoinAccount;
class BitcoinTransactionFee;
class BitcoinStream;

typedef std::unique_ptr<BitcoinTransactionFee> BitcoinTransactionFeePtr;
typedef std::unique_ptr<BitcoinTransactionSource> BitcoinTransactionSourcePtr;

class BitcoinTransaction : public BitcoinTransactionBase
{
public:
    explicit BitcoinTransaction(BlockchainType blockchain_type);

    // Transaction
    void update() override;
    BigInt get_total_spent() const override;
    BigInt estimate_total_fee(size_t sources_count, size_t destinations_count) const override;
    BinaryDataPtr serialize() override;
    Properties& add_source() override;
    Properties& add_destination() override;
    void set_message(const BinaryData& value) override;

private:
    uint64_t get_transaction_serialized_size(DestinationsToUse destinations_to_use);
    void verify() const override;
    void sign();

    template <typename T>
    void serialize_to_stream(T* stream, DestinationsToUse destinations_to_use) const;
    bool is_segwit() const;

    typedef std::vector<const BitcoinTransactionDestinationBase*> Destinations;
    Destinations get_non_zero_destinations(DestinationsToUse destinations_to_use) const;

    size_t estimate_transaction_size() const;

private:
    int32_t m_version;
    int32_t m_is_segwit_transaction;
    uint32_t m_lock_time;
    PropertyT<int32_t> m_is_replaceable;

    BitcoinTransactionDestinationBasePtr m_message;
};

} // namespace internal
} // namespaec multy_core

#endif // MULTY_CORE_BITCOIN_TRANSACTION_H
