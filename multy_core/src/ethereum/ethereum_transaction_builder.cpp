/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/src/ethereum/ethereum_transaction_builder.h"

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
#include "multy_core/transaction.h"

#include <string>
#include <unordered_map>

namespace
{
using namespace multy_core::internal;

class EthereumTransactionBuilder : public TransactionBuilder
{
public:
    EthereumTransactionBuilder(const Account& account, const std::string& name)
        : m_properties(ERROR_SCOPE_TRANSACTION_BUILDER, name),
          m_account(account),
          m_balance(m_properties,
                  "balance",
                  Property::REQUIRED,
                  &verify_bigger_than<BigInt, 0>),
          m_destination_address(m_properties,
                  "destination_address",
                  &EthereumAddress::from_string,
                  &EthereumAddress::to_string,
                  Property::REQUIRED),
          m_destination_amount(m_properties,
                  "destination_amount",
                  Property::REQUIRED,
                  &verify_bigger_than<BigInt, -1>),
          m_payload(m_properties,
                  "payload",
                  Property::OPTIONAL)
    {}

    TransactionPtr make_transaction() const override
    {
        validate();

        TransactionPtr transaction(new EthereumTransaction(m_account));
        {
            Properties& source = transaction->add_source();
            source.set_property_value("amount", *m_balance);
        }

        {
            Properties& destination = transaction->add_destination();
            destination.set_property_value("address", EthereumAddress::to_string(*m_destination_address));
            destination.set_property_value("amount", *m_destination_amount);
        }

        if (m_payload.is_set())
        {
            transaction->set_message(*m_payload.get_value());
        }

        return TransactionPtr(transaction.release());
    }

    void validate() const override
    {
        m_properties.validate(MULTY_CODE_LOCATION);
    }

    ~EthereumTransactionBuilder()
    {}

private:
    Properties& get_properties() override
    {
        return m_properties;
    }

    void validate(const CodeLocation& location) const
    {
        m_properties.validate(location);
    }

private:
    Properties m_properties;
    const Account& m_account;
    PropertyT<BigInt> m_balance;
    FunctionalPropertyT<EthereumAddress, std::string> m_destination_address;
    PropertyT<BigInt> m_destination_amount;
    PropertyT<BinaryDataPtr> m_payload;
};

} // namespace

namespace multy_core
{
namespace internal
{

TransactionBuilderPtr make_ethereum_transaction_builder(
        const Account& account, const std::string& action)
{
    if (action != "normal_transfer")
    {
        THROW_EXCEPTION2(ERROR_INVALID_ARGUMENT,
                "Invalid EthereumTransactionBuilderMultisig action.")
                << " action: \"" << action << "\".";
    }

    return TransactionBuilderPtr(new EthereumTransactionBuilder(account, action));
}

} // namespace internal
} // namespace multy_core
