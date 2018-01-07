/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_TRANSACTION_INTERNAL_BITCOIN_TRANSACTION_H
#define MULTY_TRANSACTION_INTERNAL_BITCOIN_TRANSACTION_H

#include "multy_transaction/internal/transaction_base.h"

#include <memory>
#include <vector>

namespace wallet_core
{
namespace internal
{
class BitcoinAccount;
} // namespace internal
} // namespace wallet_core

namespace multy_transaction
{
namespace internal
{
class BitcoinTransactionChange;
class BitcoinTransactionDestination;
class BitcoinTransactionFee;
class BitcoinTransactionSource;
class BitcoinStream;

typedef std::unique_ptr<BitcoinTransactionChange> BitcoinTransactionChangePtr;
typedef std::unique_ptr<BitcoinTransactionDestination> BitcoinTransactionDestinationPtr;
typedef std::unique_ptr<BitcoinTransactionFee> BitcoinTransactionFeePtr;
typedef std::unique_ptr<BitcoinTransactionSource> BitcoinTransactionSourcePtr;

class BitcoinTransaction : public TransactionBase
{
public:
    BitcoinTransaction(const Account& account);

    Amount get_total_fee() const override;
    Amount estimate_total_fee(size_t sources_count, size_t destinations_count) const override;
    BinaryDataPtr serialize() const override;

    void update_state() override;
    void sign() override;

    Properties& add_source() override;
    Properties& add_destination() override;
    Properties& get_fee() override;

private:
    template <typename T>
    void serialize_to_stream(T* stream) const;
    bool is_segwit() const;

    typedef std::vector<const BitcoinTransactionDestination*> Destinations;
    Destinations get_non_zero_destinations() const;

    size_t estimate_transaction_size() const;

private:
    const wallet_core::internal::BitcoinAccount& m_account;
    int32_t m_version;
    int32_t m_is_segwit_transaction;
    uint32_t m_lock_time;
    Amount m_total_fee;

    BitcoinTransactionFeePtr m_fee;
    std::vector<BitcoinTransactionSourcePtr> m_sources;
    std::vector<BitcoinTransactionDestinationPtr> m_destinations;
};

} // namespace internal
} // namespaec multy_transaction

#endif // MULTY_TRANSACTION_INTERNAL_BITCOIN_TRANSACTION_H
