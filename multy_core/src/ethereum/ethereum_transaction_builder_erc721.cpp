/* Copyright 2019 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/src/ethereum/ethereum_transaction_builder_erc721.h"

#include "multy_core/src/ethereum/ethereum_account.h"
#include "multy_core/src/ethereum/ethereum_address.h"
#include "multy_core/src/ethereum/ethereum_stream.h"
#include "multy_core/src/ethereum/ethereum_transaction.h"
#include "multy_core/src/ethereum/ethereum_transaction_builder.h"

#include "multy_core/src/api/transaction_builder_impl.h"
#include "multy_core/src/api/properties_impl.h"
#include "multy_core/src/api/big_int_impl.h"
#include "multy_core/src/property_predicates.h"
#include "multy_core/src/utility.h"

#include <string>
#include <unordered_map>

namespace
{

using namespace multy_core::internal;

class ERC721TransactionBuilderBase : public TransactionBuilder
{
protected:
    const EthereumAccount& m_account;
    Properties m_properties;
    PropertyT<BigInt> m_sender_eth_balance;
    FunctionalPropertyT<EthereumAddress, std::string> m_contract_address;
    FunctionalPropertyT<EthereumAddress, std::string> m_receiver_address;
    PropertyT<BigInt> m_token_id;

    ERC721TransactionBuilderBase(const EthereumAccount& account, const std::string& name)
        : m_account(account),
          m_properties(ERROR_SCOPE_TRANSACTION_BUILDER, "ERC721/" + name),
          m_sender_eth_balance(
                  m_properties,
                  "balance_eth",
                  Property::REQUIRED,
                  verify_in_range(0, ValueRangeChecker::NoBound)),
          m_contract_address(
                  m_properties,
                  "contract_address",
                  &EthereumAddress::from_string,
                  &EthereumAddress::to_string),
          m_receiver_address(m_properties,
                  "to",
                  &EthereumAddress::from_string,
                  &EthereumAddress::to_string),
          m_token_id(m_properties, "token_id", Property::REQUIRED,
                [](const BigInt& new_value){
                    if (new_value.get_exported_size_in_bytes() > 256/8)
                    {
                        THROW_EXCEPTION2(ERROR_BIG_INT_TOO_BIG_FOR_UINT256, "Too big for uint256")
                                << " Expected to be at most " << 256/8 << "bytes,"
                                << " Actual byte-size:" << new_value.get_exported_size_in_bytes();
                    }
          })
    {}
    ~ERC721TransactionBuilderBase()
    {}

    TransactionPtr make_transaction() const override
    {
        validate();

        TransactionPtr transaction(new EthereumTransaction(m_account));
        {
            Properties& source = transaction->add_source();
            source.set_property_value("amount", *m_sender_eth_balance);
        }

        {
            Properties& destination = transaction->add_destination();
            destination.set_property_value("address", EthereumAddress::to_string(*m_contract_address));
            destination.set_property_value("amount", BigInt(0));
        }

        transaction->set_message(*make_message());

        return transaction;
    }
    Properties& get_properties() override
    {
        return m_properties;
    }
    void validate() const override
    {
        m_properties.validate(MULTY_CODE_LOCATION);
    }

    virtual BinaryDataPtr make_message() const = 0;
};

// transferFrom(address _from, address _to, uint256 _tokenId)
class ERC721TransferFromTransactionBuilder : public ERC721TransactionBuilderBase
{
    FunctionalPropertyT<EthereumAddress, std::string> m_sender_address;

public:
    ERC721TransferFromTransactionBuilder(const EthereumAccount& account, const std::string& name)
        : ERC721TransactionBuilderBase(account, name),
          m_sender_address(m_properties,
                  "from",
                  &EthereumAddress::from_string,
                  &EthereumAddress::to_string,
                  Property::OPTIONAL)
    {}

    BinaryDataPtr make_message() const override
    {
        const static auto methodSig = make_method_hash("transferFrom(address,address,uint256)");

        EthereumPayloadDataStream result;
        EthereumPayloadAsUint256Stream variables;
        result << methodSig;
        variables << m_sender_address.get_value_or_default(
                m_account.get_ethereum_address());
        variables << m_receiver_address.get_value();
        variables << *m_token_id;
        result << variables;

        return make_clone(result.get_content());
    }
};

// Not part of official ERC721 standard, but common enough, an ERC20-style transfer:
// transfer(address _to, uint256 _tokenId)
class ERC721TransferTransactionBuilder : public ERC721TransactionBuilderBase
{
public:
    ERC721TransferTransactionBuilder(const EthereumAccount& account, const std::string& name)
        : ERC721TransactionBuilderBase(account, name)
    {}

    BinaryDataPtr make_message() const override
    {
        const static auto methodSig = make_method_hash("transfer(address,uint256)");

        EthereumPayloadDataStream result;
        EthereumPayloadAsUint256Stream variables;
        result << methodSig;
        variables << m_receiver_address.get_value();
        variables << *m_token_id;
        result << variables;

        return make_clone(result.get_content());
    }
};

class ERC721ApproveTransactionBuilder : public ERC721TransactionBuilderBase
{
public:
    ERC721ApproveTransactionBuilder(const EthereumAccount& account, const std::string& name)
        : ERC721TransactionBuilderBase(account, name)
    {}

    BinaryDataPtr make_message() const override
    {
        const static auto methodSig = make_method_hash("approve(address,uint256)");

        EthereumPayloadDataStream result;
        EthereumPayloadAsUint256Stream variables;
        result << methodSig;
        variables << m_receiver_address.get_value();
        variables << *m_token_id;
        result << variables;

        return make_clone(result.get_content());
    }
};

} // namespace

namespace multy_core
{
namespace internal
{

TransactionBuilderPtr make_ethereum_ERC721_transaction_builder(
        const EthereumAccount& account,
        const std::string& action)
{
    typedef TransactionBuilder* (*BuilderFunction)(const EthereumAccount&, const std::string&);
    static const std::unordered_map<std::string, BuilderFunction> BUILDERS =
    {
        {"transfer",  &new_tx_builder<ERC721TransferTransactionBuilder>},
        {"approve",  &new_tx_builder<ERC721ApproveTransactionBuilder>},
        {"transfer_from",  &new_tx_builder<ERC721TransferFromTransactionBuilder>},
    };

    const auto builder = BUILDERS.find(action);
    if (builder == BUILDERS.end())
    {
        THROW_EXCEPTION2(ERROR_INVALID_ARGUMENT,
                "Invalid Ethereum ERC721 TransactionBuilder.")
                << " action: \"" << action << "\".";
    }

    return TransactionBuilderPtr(builder->second(account, action));
}

} // namespace internal
} // namespace multy_core
