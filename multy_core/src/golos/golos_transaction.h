/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_CORE_GOLOS_TRANSACTION_H
#define MULTY_CORE_GOLOS_TRANSACTION_H

#include "multy_core/src/transaction_base.h"

#include "multy_core/big_int.h"
#include "multy_core/binary_data.h"

#include "multy_core/src/api/properties_impl.h"

#include <ctime>
#include <memory>

#include <stdint.h>

namespace multy_core
{
namespace internal
{

class GolosAccount;
class GolosTransactionSource;
class GolosTransactionDestination;
class GolosTransactionOperation;

typedef std::unique_ptr<GolosTransactionSource> GolosTransactionSourcePtr;
typedef std::unique_ptr<GolosTransactionDestination> GolosTransactionDestinationPtr;
typedef std::unique_ptr<GolosTransactionOperation> GolosTransactionOperationPtr;

class GolosTransaction : public TransactionBase
{
public:
    GolosTransaction(BlockchainType blockchain_type);
    ~GolosTransaction();

    void update() override;
    BinaryDataPtr serialize() override;
    BigInt get_total_fee() const override;
    BigInt get_total_spent() const override;
    BigInt estimate_total_fee(size_t sources_count, size_t destinations_count) const override;
    Properties& add_source() override;
    Properties& add_destination() override;
    Properties& get_fee() override;
    void set_message(const BinaryData& value) override;

private:
    void verify();
    void set_expiration(const std::string&);

private:
    BinaryDataPtr m_message;
    GolosTransactionSourcePtr m_source;
    GolosTransactionDestinationPtr m_destination;
    GolosTransactionOperationPtr m_operation;

    PropertyT<int32_t> m_expires_in_seconds;
    PropertyT<std::string> m_explicit_expiration;
    PropertyT<int32_t> m_ref_block_num;
    PropertyT<BinaryDataPtr> m_ref_block_hash;

    std::time_t m_expiration;
    BinaryDataPtr m_signature;
};

} // namespace internal
} // namespace multy_core

#endif // MULTY_CORE_GOLOS_TRANSACTION_H
