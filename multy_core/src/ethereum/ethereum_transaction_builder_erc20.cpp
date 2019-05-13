/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/src/ethereum/ethereum_transaction_builder_erc20.h"

#include "multy_core/error.h"

#include "multy_core/src/ethereum/ethereum_account.h"
#include "multy_core/src/ethereum/ethereum_address.h"
#include "multy_core/src/ethereum/ethereum_transaction.h"
#include "multy_core/src/ethereum/ethereum_stream.h"

#include "multy_core/src/api/big_int_impl.h"
#include "multy_core/src/api/properties_impl.h"
#include "multy_core/src/api/transaction_builder_impl.h"
#include "multy_core/src/exception.h"
#include "multy_core/src/exception_stream.h"
#include "multy_core/src/property_predicates.h"
#include "multy_core/src/utility.h"
#include "multy_core/transaction.h"

#include <string>
#include <unordered_map>

namespace
{
using namespace multy_core::internal;

class ERC20TransactionBuilderBase : public TransactionBuilder
{
protected:
    ERC20TransactionBuilderBase(const Account& account, const std::string& name)
        : m_properties(ERROR_SCOPE_TRANSACTION_BUILDER, name),
          m_account(account),
          m_eth_balance(m_properties,
              "balance_eth",
              Property::REQUIRED,
              &verify_bigger_than<BigInt, 0>),
          m_contract_address(m_properties,
              "contract_address",
              &EthereumAddress::from_string,
              &EthereumAddress::to_string,
              Property::REQUIRED)
    {}

    TransactionPtr make_transaction() const override
    {
        validate();

        TransactionPtr transaction(new EthereumTransaction(m_account));
        {
            Properties& source = transaction->add_source();
            source.set_property_value("amount", *m_eth_balance);
        }

        {
            Properties& destination = transaction->add_destination();
            destination.set_property_value("address", EthereumAddress::to_string(*m_contract_address));
            destination.set_property_value("amount", BigInt(0));
        }

        transaction->set_message(*make_message());

        return TransactionPtr(transaction.release());
    }

    Properties& get_properties() override
    {
        return m_properties;
    }

    ~ERC20TransactionBuilderBase()
    {}

    virtual BinaryDataPtr make_message() const = 0;

protected:
    Properties m_properties;
    const Account& m_account;
    PropertyT<BigInt> m_eth_balance;
    FunctionalPropertyT<EthereumAddress, std::string> m_contract_address;
};


class ERC20TransferTransactionBuilder : public ERC20TransactionBuilderBase
{
public:
    ERC20TransferTransactionBuilder(const Account& account, const std::string& name)
        : ERC20TransactionBuilderBase(account, name),
          m_balance_token(m_properties,
              "balance_token",
              Property::REQUIRED,
              &verify_bigger_than<BigInt, 0>),
          m_amount_token(m_properties,
              "transfer_amount_token",
              Property::REQUIRED,
              &verify_bigger_than<BigInt, 0>),
          m_destination_address(
              m_properties,
              "destination_address",
              &EthereumAddress::from_string,
              &EthereumAddress::to_string,
              Property::REQUIRED)
    {}

    void validate() const override
    {
        m_properties.validate(MULTY_CODE_LOCATION);
        if (*m_amount_token > *m_balance_token)
        {
            THROW_EXCEPTION2(ERROR_TRANSACTION_INSUFFICIENT_FUNDS,
                    "Trying to spend more tokens than available with ERC20/transfer")
                    << "/n balance token: " << *m_balance_token
                    << " transfer amount token: " << *m_amount_token;
        }
    }

    BinaryDataPtr make_message() const override
    {
        const static EthereumContractMethodHash method({0xa9, 0x05, 0x9c, 0xbb});

        EthereumPayloadDataStream result;
        EthereumPayloadAsUint256Stream variables;
        result << method;
        variables << m_destination_address.get_value();
        variables << *m_amount_token;
        result << variables;

        return make_clone(result.get_content());
    }

private:
    PropertyT<BigInt> m_balance_token;
    PropertyT<BigInt> m_amount_token;
    FunctionalPropertyT<EthereumAddress, std::string> m_destination_address;
};


class ERC20TransferFromTransactionBuilder : public ERC20TransactionBuilderBase
{
public:
    ERC20TransferFromTransactionBuilder(const Account& account, const std::string& name)
        : ERC20TransactionBuilderBase(account, name),
          m_available_token_from(m_properties,
              "available_token_from",
              Property::REQUIRED,
              &verify_bigger_than<BigInt, 0>),
          m_amount_token(m_properties,
              "transfer_amount_token",
              Property::REQUIRED,
              &verify_bigger_than<BigInt, 0>),
          m_from(
              m_properties,
              "from",
              &EthereumAddress::from_string,
              &EthereumAddress::to_string,
              Property::REQUIRED),
          m_to(
              m_properties,
              "to",
              &EthereumAddress::from_string,
              &EthereumAddress::to_string,
              Property::REQUIRED)
    {}

    void validate() const override
    {
        m_properties.validate(MULTY_CODE_LOCATION);
        if (*m_amount_token > *m_available_token_from)
        {
            THROW_EXCEPTION2(ERROR_TRANSACTION_INSUFFICIENT_FUNDS,
                    "Trying to spend more tokens than available with ERC20/transfer_from")
                    << "/n available token: " << *m_available_token_from
                    << " transfer amount token: " << *m_amount_token;
        }
    }

    BinaryDataPtr make_message() const override
    {
        const static EthereumContractMethodHash method({0x23, 0xb8, 0x72, 0xdd});

        EthereumPayloadDataStream result;
        EthereumPayloadAsUint256Stream variables;
        result << method;
        variables << m_from.get_value();
        variables << m_to.get_value();
        variables << *m_amount_token;
        result << variables;

        return make_clone(result.get_content());
    }

private:
    PropertyT<BigInt> m_available_token_from;
    PropertyT<BigInt> m_amount_token;
    FunctionalPropertyT<EthereumAddress, std::string> m_from;
    FunctionalPropertyT<EthereumAddress, std::string> m_to;
};

class ERC20ApproveTransactionBuilder : public ERC20TransactionBuilderBase
{
public:
    ERC20ApproveTransactionBuilder(const Account& account, const std::string& name)
        : ERC20TransactionBuilderBase(account, name),
          m_balance_token(m_properties,
              "balance_token",
              Property::REQUIRED,
              &verify_bigger_than<BigInt, 0>),
          m_approve_amount_token(m_properties,
              "approve_amount_token",
              Property::REQUIRED,
              &verify_bigger_than<BigInt, 0>),
          m_address_for_approve(
              m_properties,
              "address_for_approve",
              &EthereumAddress::from_string,
              &EthereumAddress::to_string,
              Property::REQUIRED)
    {}

    void validate() const override
    {
        m_properties.validate(MULTY_CODE_LOCATION);
        if (*m_approve_amount_token > *m_balance_token)
        {
            THROW_EXCEPTION2(ERROR_TRANSACTION_INSUFFICIENT_FUNDS,
                    "Trying to spend more tokens than available with ERC20/approve")
                    << "/n balance token: " << *m_balance_token
                    << " approve amount token: " << *m_approve_amount_token;
        }
    }

    BinaryDataPtr make_message() const override
    {
        const static EthereumContractMethodHash method({0x09, 0x5e, 0xa7, 0xb3});

        EthereumPayloadDataStream result;
        EthereumPayloadAsUint256Stream variables;
        result << method;
        variables << m_address_for_approve.get_value();
        variables << *m_approve_amount_token;
        result << variables;

        return make_clone(result.get_content());
    }

private:
    PropertyT<BigInt> m_balance_token;
    PropertyT<BigInt> m_approve_amount_token;
    FunctionalPropertyT<EthereumAddress, std::string> m_address_for_approve;
};

} // namespace

namespace multy_core
{
namespace internal
{

TransactionBuilderPtr make_ethereum_ERC20_transaction_builder(
        const EthereumAccount& account, const std::string& action)
{
    typedef TransactionBuilder* (*BuilderFunction)(const Account&, const std::string&);
    static const std::unordered_map<std::string, BuilderFunction> BUILDERS =
    {
        {"transfer",  &new_tx_builder<ERC20TransferTransactionBuilder>},
        {"approve",  &new_tx_builder<ERC20ApproveTransactionBuilder>},
        {"transfer_from",  &new_tx_builder<ERC20TransferFromTransactionBuilder>},
    };

    const auto builder = BUILDERS.find(action);
    if (builder == BUILDERS.end())
    {
        THROW_EXCEPTION2(ERROR_INVALID_ARGUMENT,
                "Invalid EthereumERC20TransactionBuilder action.")
                << " action: \"" << action << "\".";
    }

    return TransactionBuilderPtr(builder->second(account, action));
}

} // namespace internal
} // namespace multy_core
