#ifndef MULTY_TRANSACTION_ETHEREUM_TRANSACTION_H
#define MULTY_TRANSACTION_ETHEREUM_TRANSACTION_H

#include "multy_core/src/transaction_base.h"
#include "multy_core/src/api/properties_impl.h"

namespace multy_core
{
namespace internal
{
struct EthereumTransactionFee;
struct EthereumTransactionSource;
struct EthereumTransactionDestination;
struct EthereumTransactionSignature;
struct EthereumDataStream;

typedef std::unique_ptr<EthereumTransactionFee> EthereumTransactionFeePtr;
typedef std::unique_ptr<EthereumTransactionSource> EthereumTransactionSourcePtr;
typedef std::unique_ptr<EthereumTransactionDestination> EthereumTransactionDestinationPtr;
typedef std::unique_ptr<EthereumTransactionSignature> EthereumTransactionSignaturePtr;

enum EthereumChainId
{
    // Default chain id value from Ethereum sources.
    ETHEREUM_CHAIN_ID_PRE_EIP155 = -4,

    // https://github.com/ethereum/EIPs/blob/master/EIPS/eip-155.md
    ETHEREUM_CHAIN_ID_MAINNET = 1, // Ethereum mainnet
    ETHEREUM_CHAIN_ID_MORDEN = 2, // 	Morden (disused), Expanse mainnet
    ETHEREUM_CHAIN_ID_ROPSTEN = 3, // 	Ropsten
    ETHEREUM_CHAIN_ID_RINKEBY = 4, // 	Rinkeby
    ETHEREUM_CHAIN_ID_ROOTSTOCK_MAINNET = 30, // 	Rootstock mainnet
    ETHEREUM_CHAIN_ID_ROOTSTOCK_TESTNET = 31, // 	Rootstock testnet
    ETHEREUM_CHAIN_ID_KOVAN = 42, // 	Kovan
    ETHEREUM_CHAIN_ID_ETC_MAINNET = 61, // 	Ethereum Classic mainnet
    ETHEREUM_CHAIN_ID_ETC_TESTNET = 62, //	Ethereum Classic testnet
};

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

private:
    const Account& m_account;

    PropertyT<BigInt> m_nonce;
    PropertyT<BinaryDataPtr> m_payload;
    PropertyT<int32_t> m_chain_id;

    EthereumTransactionFeePtr m_fee;
    EthereumTransactionSourcePtr m_source;
    EthereumTransactionDestinationPtr m_destination;
    EthereumTransactionSignaturePtr m_signature;

    BigInt m_gas;
};

} // namespace internal
} // namespace multy_core

#endif // MULTY_TRANSACTION_ETHEREUM_TRANSACTION_H
