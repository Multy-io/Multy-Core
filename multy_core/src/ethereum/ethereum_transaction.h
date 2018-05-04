/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_TRANSACTION_ETHEREUM_TRANSACTION_H
#define MULTY_TRANSACTION_ETHEREUM_TRANSACTION_H

#include "multy_core/ethereum.h"

#include "multy_core/src/api/properties_impl.h"
#include "multy_core/src/transaction_base.h"


namespace multy_core
{
namespace internal
{
struct EthereumTransactionSignature;
struct EthereumDataStream;
struct EthereumTransactionFee;
struct EthereumTransactionSource;
struct EthereumTransactionDestination;
class EthereumSmartContractPayload;

typedef std::unique_ptr<EthereumSmartContractPayload> EthereumSmartContractPayloadPtr;
typedef std::unique_ptr<EthereumTransactionFee> EthereumTransactionFeePtr;
typedef std::unique_ptr<EthereumTransactionSource> EthereumTransactionSourcePtr;
typedef std::unique_ptr<EthereumTransactionDestination> EthereumTransactionDestinationPtr;
typedef std::unique_ptr<EthereumTransactionSignature> EthereumTransactionSignaturePtr;

class EthereumTransaction : public TransactionBase
{
public:
    explicit EthereumTransaction(const Account& account);
    virtual ~EthereumTransaction();

    BinaryDataPtr serialize() override;
    BigInt get_total_spent() const override;
    BigInt get_total_fee() const override;

    void verify();
    void update() override;
    void sign();
    BigInt estimate_total_fee(size_t sources_count, size_t destinations_count) const override;

    Properties& add_source() override;
    Properties& add_destination() override;
    Properties& get_fee() override;
    void set_message(const BinaryData& value) override;

private:
    enum SerializationMode
    {
        SERIALIZE,
        SERIALIZE_WITH_SIGNATURE,
        SERIALIZE_WITH_CHAIN_ID,
    };
    void serialize_to_stream(EthereumDataStream& stream, SerializationMode mode) const;
    void on_token_transfer_set(const std::string& value);

private:
    const Account& m_account;

    PropertyT<BigInt> m_nonce;
    const EthereumChainId m_chain_id;
    PropertyT<std::string> m_token_transfer;

    EthereumTransactionFeePtr m_fee;
    EthereumTransactionSourcePtr m_source;
    // User-visible destination, where all values are explicitly set by user,
    // including token receiver address and token transfer amount.
    EthereumTransactionDestinationPtr m_destination;
    // A destination used only to simplify transaction serialization, can be generated
    // from token transfer info or just a receiver info (in case of plain Ether transfer).
    EthereumTransactionDestinationPtr m_internal_destination;
    EthereumSmartContractPayloadPtr m_token_transfer_data;
    EthereumTransactionSignaturePtr m_signature;
    BinaryDataPtr m_payload;
};

} // namespace internal
} // namespace multy_core

#endif // MULTY_TRANSACTION_ETHEREUM_TRANSACTION_H
