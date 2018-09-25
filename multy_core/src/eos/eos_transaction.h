/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_CORE_EOS_TRANSACTION_H
#define MULTY_CORE_EOS_TRANSACTION_H

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

class EosAccount;
class EosTransactionSource;
class EosTransactionDestination;
class EosTransactionAction;
class EosBinaryStream;

typedef std::unique_ptr<EosTransactionSource> EosTransactionSourcePtr;
typedef std::unique_ptr<EosTransactionDestination> EosTransactionDestinationPtr;
typedef std::unique_ptr<EosTransactionAction> EosTransactionActionPtr;

class EosTransaction : public TransactionBase
{
public:
    EosTransaction(const Account& account);
    ~EosTransaction();

    void sign();
    void update() override;
    BinaryDataPtr serialize() override;
    std::string encode_serialized() override;
    BigInt get_total_fee() const override;
    BigInt get_total_spent() const override;
    BigInt estimate_total_fee(size_t sources_count, size_t destinations_count) const override;
    Properties& add_source() override;
    Properties& add_destination() override;
    Properties& get_fee() override;
    void set_message(const BinaryData& payload) override;

    // Ownership is transferred from caller to EosTransaction.
    void set_action(EosTransactionActionPtr action);

private:
    enum SerializationMode
    {
        SERIALIZE,
        SERIALIZE_FOR_SIGN
    };
    void verify();
    void set_expiration(const std::string&);
    void serialize_to_stream(EosBinaryStream& stream, SerializationMode mode) const;

private:
    const Account& m_account;

    BinaryDataPtr m_message;
    EosTransactionSourcePtr m_source;
    EosTransactionDestinationPtr m_destination;
    std::vector<EosTransactionActionPtr> m_external_actions;
    // TODO: make a TxBuilder for transfer operation and get rid of this,
    // since it is going to be set from TX builder as external_action.
    std::vector<EosTransactionActionPtr> m_actions;

    PropertyT<std::string> m_explicit_expiration;
    PropertyT<int32_t> m_ref_block_num;
    PropertyT<BigInt> m_ref_block_prefix;

    std::time_t m_expiration;
    BinaryDataPtr m_signature;
};

} // namespace internal
} // namespace multy_core

#endif // MULTY_CORE_EOS_TRANSACTION_H
