/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/src/ethereum/ethereum_transaction_builder_multisig.h"

#include "multy_core/error.h"

#include "multy_core/src/ethereum/ethereum_account.h"
#include "multy_core/src/ethereum/ethereum_address.h"
#include "multy_core/src/ethereum/ethereum_transaction.h"

#include "multy_core/src/api/big_int_impl.h"
#include "multy_core/src/api/properties_impl.h"
#include "multy_core/src/api/transaction_builder_impl.h"
#include "multy_core/src/enum_name_map.h"
#include "multy_core/src/exception.h"
#include "multy_core/src/exception_stream.h"
#include "multy_core/src/property_predicates.h"
#include "multy_core/src/utility.h"

#include <string>
#include <regex>
#include <unordered_map>

namespace
{
using namespace multy_core::internal;

class MultisigTransactionBuilderBase : public TransactionBuilder
{
protected:
    MultisigTransactionBuilderBase(const Account& account, const std::string& name)
        : m_properties(ERROR_SCOPE_TRANSACTION_BUILDER, name),
          m_account(account)
    {}

    Properties& get_properties() override
    {
        return m_properties;
    }

    ~MultisigTransactionBuilderBase()
    {}

    void validate(const CodeLocation& location) const
    {
        m_properties.validate(location);
    }

protected:
    Properties m_properties;
    const Account& m_account;
};

class MultisigNewWaletTransactionBuilder : public MultisigTransactionBuilderBase
{
public:
    MultisigNewWaletTransactionBuilder(const Account& account, const std::string& name)
        : MultisigTransactionBuilderBase(account, name),
          m_factory_address(
                m_properties,
                "factory_address",
                &EthereumAddress::from_string,
                &EthereumAddress::to_string,
                Property::REQUIRED),
          m_owners(
                m_properties,
                "owners",
                &addresses_from_string,
                &addresses_to_string,
                Property::REQUIRED),
          m_confirmations(
                m_properties,
                "confirmations",
                Property::REQUIRED,
                &verify_bigger_than<int32_t, 1>)
    {}

    TransactionPtr make_transaction() const override
    {
        validate(MULTY_CODE_LOCATION);

        // TODO: make a transaction
        return nullptr;
    }
private:
    static std::vector<EthereumAddress> addresses_from_string(const std::string& addresses)
    {
        static const std::regex SPLIT_TOKEN_RE(",\\s*");

        if (addresses.size() < 2 || addresses.front() != '[' || addresses.back() != ']')
        {
            THROW_EXCEPTION2(ERROR_INVALID_ARGUMENT,
                    "Expected comma-separated list of Ethereum addresses.");
        }

        std::vector<EthereumAddress> result;

        // skip surrounding '[' and ']'.
        std::sregex_token_iterator iterator(addresses.begin() + 1, addresses.end() - 1, SPLIT_TOKEN_RE, -1);
        size_t index = 1;
        while (iterator != std::sregex_token_iterator())
        {
            try
            {
                result.emplace_back(iterator->str());
            }
            catch (const Exception& e)
            {
                // Additional context for error message.
                e << " For address #" << index << ".";
                throw;
            }

            ++index;
            ++iterator;
        }

        return result;
    }

    static std::string addresses_to_string(const std::vector<EthereumAddress>& addresses)
    {
        return "[" + join(",", addresses, &EthereumAddress::to_string) + "]";
    }

private:
    FunctionalPropertyT<EthereumAddress, std::string> m_factory_address;
    FunctionalPropertyT<std::vector<EthereumAddress>, std::string> m_owners;
    PropertyT<int32_t> m_confirmations;
};

/** New payment request.
 *
 * Properties:
 *      wallet_address: Ethereum Multisig Wallet SmartContract address.
 *      dest_address: Ethereum account address of receiving party.
 *      amount: amount of Ether to transfer.
 */
class MultisigNewPaymentRequestTransactionBuilder : public MultisigTransactionBuilderBase
{
public:
    MultisigNewPaymentRequestTransactionBuilder(const Account& account, const std::string& name)
        : MultisigTransactionBuilderBase(account, name),
          m_wallet_address(
                m_properties,
                "wallet_address",
                &EthereumAddress::from_string,
                &EthereumAddress::to_string,
                Property::REQUIRED),
          m_dest_address(m_properties,
                "dest_address",
                &EthereumAddress::from_string,
                &EthereumAddress::to_string,
                Property::REQUIRED),
          m_amount(m_properties,
                "amount",
                Property::REQUIRED,
                &verify_bigger_than<BigInt, 0>)
    {}

    TransactionPtr make_transaction() const override
    {
        validate(MULTY_CODE_LOCATION);

        // TODO: make a transaction
        return nullptr;
    }

private:
    FunctionalPropertyT<EthereumAddress, std::string> m_wallet_address;
    FunctionalPropertyT<EthereumAddress, std::string> m_dest_address;
    PropertyT<BigInt> m_amount;
};

enum RequestAction
{
    CONFIRM,
    REJECT,
    SEND
};

static const EnumNameMap<RequestAction> ACTIONS_MAP =
{
    "RequestAction",
    {
        {CONFIRM, "confirm"},
        {REJECT,  "reject"},
        {SEND,    "send"},
    }
};

static RequestAction action_from_string(const std::string& action_name)
{
    return ACTIONS_MAP.get_value(action_name);
}
static std::string string_from_action(RequestAction action)
{
    return ACTIONS_MAP.get_name(action);
}

/** Different payment request actions: confirm, reject, send.
 *
 * Properties:
 *      wallet_address: Ethereum Multisig Wallet SmartContract address.
 *      action: either 'confirm' or 'reject' or 'send'.
 *      request_id: id of request.
 */
class MultisigPaymentRequestActionTransactionBuilder : public MultisigTransactionBuilderBase
{
public:
    MultisigPaymentRequestActionTransactionBuilder(const Account& account, const std::string& name)
        : MultisigTransactionBuilderBase(account, name),
          m_wallet_address(
                m_properties,
                "wallet_address",
                &EthereumAddress::from_string,
                &EthereumAddress::to_string,
                Property::REQUIRED),
          m_action(
                m_properties,
                "action",
                &action_from_string,
                &string_from_action,
                Property::REQUIRED),
          m_request_id(
                m_properties,
                "request_id",
                Property::REQUIRED,
                &verify_bigger_than<BigInt, 0>)
    {}

    TransactionPtr make_transaction() const override
    {
        validate(MULTY_CODE_LOCATION);

        // TODO: make a transaction
        return nullptr;
    }

private:
    FunctionalPropertyT<EthereumAddress, std::string> m_wallet_address;
    FunctionalPropertyT<RequestAction, std::string> m_action;
    PropertyT<BigInt> m_request_id;
};

template <typename T>
TransactionBuilder* tx_builder(const Account& account, const std::string& name)
{
    return new T(account, name);
}

} // namespace

namespace multy_core
{
namespace internal
{

TransactionBuilderPtr make_ethereum_multisig_transaction_builder(
        const Account& account, const std::string& action)
{
    typedef TransactionBuilder* (*BuilderFunction)(const Account&, const std::string&);
    static const std::unordered_map<std::string, BuilderFunction> BUILDERS =
    {
        {"new_wallet",  &tx_builder<MultisigNewWaletTransactionBuilder>},
        {"new_request", &tx_builder<MultisigNewPaymentRequestTransactionBuilder>},
        {"request",     &tx_builder<MultisigPaymentRequestActionTransactionBuilder>},
    };

    const auto builder = BUILDERS.find(action);
    if (builder == BUILDERS.end())
    {
        THROW_EXCEPTION2(ERROR_INVALID_ARGUMENT,
                "Invalid EthereumTransactionBuilderMultisig action.")
                << " action: \"" << action << "\".";
    }

    return TransactionBuilderPtr(builder->second(account, action));
}

} // namespace internal
} // namespace multy_core
