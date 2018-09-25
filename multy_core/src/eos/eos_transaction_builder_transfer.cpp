/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/src/eos/eos_transaction_builder_transfer.h"

#include "multy_core/error.h"

#include "multy_core/src/eos/eos_name.h"
#include "multy_core/src/eos/eos_transaction_transfer_action.h"
#include "multy_core/src/eos/eos_transaction.h"

#include "multy_core/src/api/transaction_builder_impl.h"
#include "multy_core/src/api/account_impl.h"
#include "multy_core/src/api/properties_impl.h"
#include "multy_core/src/api/key_impl.h"
#include "multy_core/src/error_utility.h"
#include "multy_core/src/u_ptr.h"
#include "multy_core/src/utility.h"

#include <string>
#include <memory>

namespace
{
using namespace multy_core::internal;

// Makes a transaction that replaces user keys in blockchain.
// takes in new private key,
struct EosTransactionBuilderTransfer : public TransactionBuilder
{
    typedef std::unique_ptr<EosTransaction> EosTransactionPtr;

    EosTransactionBuilderTransfer(const Account& account)
        : m_account(account),
          m_properties(
                ERROR_SCOPE_TRANSACTION_BUILDER,
                "EOS transfer Tx builder"),
          m_from(
                get_properties(),
                "from",
                &EosAddress::from_string,
                &EosAddress::to_string,
                Property::REQUIRED),
          m_to(
                get_properties(),
                "to",
                &EosAddress::from_string,
                &EosAddress::to_string,
                Property::REQUIRED),
          m_explicit_expiration(
              get_properties(),
              "expiration",
              Property::OPTIONAL
              ),
          m_ref_block_num(
              get_properties(),
              "block_num",
              Property::REQUIRED),
          m_ref_block_prefix(
              get_properties(),
              "ref_block_prefix",
              Property::REQUIRED),
          m_balance(
              get_properties(),
              "balance",
              Property::REQUIRED),
          m_amount(
              get_properties(),
              "amount",
              Property::REQUIRED),
          m_memo(
              BinaryDataPtr(new BinaryData{nullptr, 0}),
              get_properties(),
              "memo",
              Property::OPTIONAL)
          //m_new_private_key(m_properties, "", Property::REQUIRED)
    {}

    TransactionPtr make_transaction() const override
    {
        EosTransactionPtr transaction(new EosTransaction(m_account));
        transaction->set_expiration(m_explicit_expiration.get_value());

        const u_int16_t ref_block_num = static_cast<uint16_t>(m_ref_block_num.get_value());
        transaction->set_ref_block_num(ref_block_num);
        transaction->set_ref_block_prefix(m_ref_block_prefix.get_value());

        transaction->set_max_net_usage(0);
        transaction->set_max_cpu_usage(0);
        transaction->set_delay_seconds(0);


        {
            transaction->set_action(EosTransactionActionPtr(new EosTransactionTransferAction(
                                         EosAddress::to_string(*m_from),
                                         EosAddress::to_string(*m_to),
                                         m_amount.get_value(),
                                         *(m_memo.get_value()))));
        }
        return TransactionPtr(transaction.release());
    }

    void validate() const override
    {
        m_properties.validate(MULTY_CODE_LOCATION);
    }

    Properties& get_properties() override
    {
        return m_properties;
    }

public:
    const Account& m_account;
    
    Properties m_properties;
    FunctionalPropertyT<EosAddress, std::string> m_from;
    FunctionalPropertyT<EosAddress, std::string> m_to;
    PropertyT<std::string> m_explicit_expiration;
    PropertyT<int32_t> m_ref_block_num;
    PropertyT<BigInt> m_ref_block_prefix;
    PropertyT<BigInt> m_balance;
    PropertyT<BigInt> m_amount;
    PropertyT<BinaryDataPtr> m_memo;

};

} // namespace

namespace multy_core
{
namespace internal
{

TransactionBuilderPtr make_eos_transaction_builder_transfer(
        const Account& account,
        const std::string& /*action*/)
{
    return TransactionBuilderPtr{new EosTransactionBuilderTransfer(account)};
}

} // namespace internal
} // namespace multy_core
