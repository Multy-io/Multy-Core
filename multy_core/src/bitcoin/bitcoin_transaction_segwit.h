/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_CORE_BITCOIN_TRANSACTION_SEGWIT_H
#define MULTY_CORE_BITCOIN_TRANSACTION_SEGWIT_H

#include "multy_core/src/transaction_base.h"
#include "multy_core/src/bitcoin/bitcoin_account.h"
#include "multy_core/src/api/properties_impl.h"
#include "multy_core/src/bitcoin/bitcoin_transaction_base.h"
#include "multy_core/src/hash.h"

#include <vector>

namespace multy_core
{
namespace internal
{

class BitcoinTransactionSegWitDestination : public BitcoinTransactionDestinationBase
{
public:
    explicit BitcoinTransactionSegWitDestination(BitcoinNetType net_type);
};

class BitcoinTransactionSegWitSource : public BitcoinTransactionSourceBase
{
public:
    BitcoinTransactionSegWitSource();

    ~BitcoinTransactionSegWitSource();

    void sign(const hash<256>& hash_prevouts, const hash<256>& hash_sequence, const hash<256>& hash_outputs, const BinaryDataPtr& lock_time);

    BinaryDataPtr serialize_with_script_pub_key() const;
    BinaryDataPtr preview_transaction() const;

private:
    BinaryDataPtr make_script_sig() const;

public:
    BinaryDataPtr script_witness;
};

class BitcoinTransactionFeeBase;

typedef std::unique_ptr<BitcoinTransactionSegWitDestination> BitcoinTransactionSegWitDestinationPtr;
typedef std::unique_ptr<BitcoinTransactionSegWitSource> BitcoinTransactionSegWitSourcePtr;
typedef std::unique_ptr<BitcoinTransactionFeeBase> BitcoinTransactionFeeBasePtr;

class BitcoinTransactionSegWit : public BitcoinTransactionBase// TransactionBase
{
public:
    explicit BitcoinTransactionSegWit(BlockchainType blockchain_type);

    // Transaction API
    Properties& add_source() override;
    Properties& add_destination() override;
    void set_message(const BinaryData& value) override;
    BigInt estimate_total_fee(size_t sources_count, size_t destinations_count) const override;
    BigInt get_total_spent() const override;

    void update() override;
    BinaryDataPtr serialize() override;

private:
    void sign();
    void verify() const override;

    BitcoinNetType get_net_type() const;

private:
    int32_t m_version;

    PropertyT<BinaryDataPtr> lock_time;
};

} // namespace internal
} // namespaec multy_core


#endif // MULTY_CORE_BITCOIN_TRANSACTION_SEGWIT_H
