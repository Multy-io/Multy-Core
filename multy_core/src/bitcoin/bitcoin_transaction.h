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

class BitcoinTransactionDestination : public TransactionDestinationBase
{
public:
    explicit BitcoinTransactionDestination(BitcoinNetType net_type);
    void on_change_set(bool new_value);

public:
    PropertyT<BigInt> amount;
    PropertyT<std::string> address;
    PropertyT<int32_t> is_change;

    BinaryDataPtr sig_script;
    const BitcoinNetType m_net_type;
    bool m_is_segwit_destination;
};

class BitcoinTransactionSource : public TransactionSourceBase
{
public:
    BitcoinTransactionSource();

    ~BitcoinTransactionSource()
    {
    }

public:
    PropertyT<BinaryDataPtr> prev_transaction_hash;
    PropertyT<int32_t> prev_transaction_out_index;
    PropertyT<BinaryDataPtr> prev_transaction_out_script_pubkey;
    PropertyT<PrivateKeyPtr> private_key;

    // not a property since set by Transaction or Source itself.
    int32_t segwit = 0;
    uint32_t seq;
    BinaryDataPtr script_signature;
    BinaryDataPtr script_witness; // serialized separately.

    PropertyT<BigInt> amount; // Not serialized:

};

class BitcoinAccount;
class BitcoinTransactionFee;
class BitcoinStream;

typedef std::unique_ptr<BitcoinTransactionDestination> BitcoinTransactionDestinationPtr;
typedef std::unique_ptr<BitcoinTransactionFee> BitcoinTransactionFeePtr;
typedef std::unique_ptr<BitcoinTransactionSource> BitcoinTransactionSourcePtr;

class BitcoinTransaction : public TransactionBase
{
public:
    explicit BitcoinTransaction(BlockchainType blockchain_type);

    // Transaction
    void update() override;
    BigInt get_total_fee() const override;
    BigInt get_total_spent() const override;
    BigInt estimate_total_fee(size_t sources_count, size_t destinations_count) const override;
    BinaryDataPtr serialize() override;
    Properties& add_source() override;
    Properties& add_destination() override;
    Properties& get_fee() override;
    void set_message(const BinaryData& value) override;

private:
    uint64_t get_transaction_serialized_size(DestinationsToUse destinations_to_use);
    BigInt calculate_diff() const;
    void verify() const;
    void sign();

    template <typename T>
    void serialize_to_stream(T* stream, DestinationsToUse destinations_to_use) const;
    bool is_segwit() const;

    typedef std::vector<const BitcoinTransactionDestination*> Destinations;
    Destinations get_non_zero_destinations(DestinationsToUse destinations_to_use) const;

    size_t estimate_transaction_size() const;
    BitcoinNetType get_net_type() const;

private:
    int32_t m_version;
    int32_t m_is_segwit_transaction;
    uint32_t m_lock_time;
    PropertyT<int32_t> m_is_replaceable;

    BitcoinTransactionFeePtr m_fee;
    std::vector<BitcoinTransactionSourcePtr> m_sources;
    std::vector<BitcoinTransactionDestinationPtr> m_destinations;
    BitcoinTransactionDestinationPtr m_message;
};

} // namespace internal
} // namespaec multy_core

#endif // MULTY_CORE_BITCOIN_TRANSACTION_H
