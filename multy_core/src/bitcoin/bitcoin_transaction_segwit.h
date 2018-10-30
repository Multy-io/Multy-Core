/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_CORE_BITCOIN_TRANSACTION_SEGWIT_H
#define MULTY_CORE_BITCOIN_TRANSACTION_SEGWIT_H

#include "multy_core/src/transaction_base.h"
#include "multy_core/src/bitcoin/bitcoin_account.h"

namespace multy_core
{
namespace internal
{


class BitcoinTransactionSegWit : public TransactionBase
{
public:
    BitcoinTransactionSegWit(BlockchainType blockchain_type, BitcoinAccountType account_type);

    //Transaction
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
    void sign();
    void verify();

private:
    int32_t m_version;
    int32_t m_is_segwit_transaction;
    uint32_t m_lock_time;

};

} // namespace internal
} // namespaec multy_core


#endif // MULTY_CORE_BITCOIN_TRANSACTION_SEGWIT_H
