/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef BITCOIN_TRANSACTION_BASE_H
#define BITCOIN_TRANSACTION_BASE_H

#include "multy_core/src/transaction_base.h"
#include "multy_core/bitcoin.h"

#include <sstream>

namespace multy_core
{
namespace internal
{

BinaryDataPtr make_script_pub_key(const BinaryData& public_key_hash, BitcoinAddressType address_type);

template <typename T>
std::string make_id(const std::string& base, const T& suffix)
{
    std::ostringstream ostr;
    ostr << base << suffix;
    return ostr.str();
}

class BitcoinTransactionDestinationBase : public TransactionDestinationBase
{
public:
    explicit BitcoinTransactionDestinationBase(BitcoinNetType net_type);
    ~BitcoinTransactionDestinationBase();
    void on_change_set(bool new_value);

public:
    PropertyT<BigInt> amount;
    PropertyT<std::string> address;
    PropertyT<int32_t> is_change;

    BinaryDataPtr sig_script;
    const BitcoinNetType m_net_type;

};

class BitcoinTransactionSourceBase : public TransactionSourceBase
{
public:
    BitcoinTransactionSourceBase();
    ~BitcoinTransactionSourceBase();

public:
    PropertyT<BinaryDataPtr> prev_transaction_hash;
    PropertyT<int32_t> prev_transaction_out_index;
    PropertyT<BinaryDataPtr> prev_transaction_out_script_pubkey;
    PropertyT<PrivateKeyPtr> private_key;
    PropertyT<int32_t> sequence;
    PropertyT<BigInt> amount; // Not serialized

    BinaryDataPtr script_signature;
};

class BitcoinTransactionFeeBase : public TransactionFeeBase
{
public:
    BitcoinTransactionFeeBase();

    const BigInt& get_amount_per_byte() const;
    void validate_fee(const BigInt& leftover, uint64_t transaction_size) const;

public:
    PropertyT<BigInt> amount_per_byte;
};

typedef std::unique_ptr<BitcoinTransactionDestinationBase> BitcoinTransactionDestinationBasePtr;
typedef std::unique_ptr<BitcoinTransactionSourceBase> BitcoinTransactionSourceBasePtr;
typedef std::unique_ptr<BitcoinTransactionFeeBase> BitcoinTransactionFeeBasePtr;
class BitcoinTransactionBase : public TransactionBase
{
public:
    explicit BitcoinTransactionBase(BlockchainType);
    ~BitcoinTransactionBase();

    Properties& get_fee() override;
    BigInt get_total_fee() const override;

protected:
    BigInt calculate_diff() const;
    BitcoinNetType get_net_type() const;
    virtual void verify() const;

protected:
    std::vector<BitcoinTransactionSourceBasePtr> m_sources;
    std::vector<BitcoinTransactionDestinationBasePtr> m_destinations;
    BitcoinTransactionFeeBasePtr m_fee;
};

} // namespace internal
} // namespace multy_core

#endif // BITCOIN_TRANSACTION_BASE_H
