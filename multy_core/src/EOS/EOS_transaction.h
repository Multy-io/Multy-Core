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

class EOSAccount;
class EOSTransactionSource;
class EOSTransactionDestination;
class EOSTransactionAction;
class EOSBinaryStream;

typedef std::unique_ptr<EOSTransactionSource> EOSTransactionSourcePtr;
typedef std::unique_ptr<EOSTransactionDestination> EOSTransactionDestinationPtr;
typedef std::unique_ptr<EOSTransactionAction> EOSTransactionActionPtr;

class EOSTransaction : public TransactionBase
{
public:
    EOSTransaction(const Account& account);
    ~EOSTransaction();

    void sign();
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
    enum SerializationMode
    {
        SERIALIZE,
        SERIALIZE_FOR_SIGN
    };
    void verify();
    void set_expiration(const std::string&);
    void serialize_to_stream(EOSBinaryStream& stream, SerializationMode mode) const;

private:
    const Account& m_account;

    BinaryDataPtr m_message;
    EOSTransactionSourcePtr m_source;
    EOSTransactionDestinationPtr m_destination;
    std::vector<EOSTransactionActionPtr> m_actions;

    PropertyT<std::string> m_explicit_expiration;
    PropertyT<int32_t> m_ref_block_num;
    PropertyT<BigInt> m_ref_block_prefix;

    std::time_t m_expiration;
    BinaryDataPtr m_signature;
};

} // namespace internal
} // namespace multy_core

#endif // MULTY_CORE_EOS_TRANSACTION_H
