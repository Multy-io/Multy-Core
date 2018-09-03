/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/src/eos/eos_transaction_builder_updateauth.h"

#include "multy_core/error.h"

#include "multy_core/src/eos/eos_name.h"

#include "multy_core/src/api/transaction_builder_impl.h"
#include "multy_core/src/api/account_impl.h"
#include "multy_core/src/api/properties_impl.h"
#include "multy_core/src/api/key_impl.h"
#include "multy_core/src/error_utility.h"
#include "multy_core/src/u_ptr.h"

#include <string>

namespace
{
using namespace multy_core::internal;

// Makes a transaction that replaces user keys in blockchain.
// takes in new private key,
struct EosTransactionBuilderUpdateAuth : public TransactionBuilder
{
    EosTransactionBuilderUpdateAuth(const Account& account)
        : m_account(account),
          m_properties(
                ERROR_SCOPE_TRANSACTION_BUILDER,
                "EOS AuthUpdate Tx Builder"),
          m_address(
                m_properties,
                "address",
                &EosAddress::from_string,
                &EosAddress::to_string,
                Property::REQUIRED),
          m_new_private_key(m_properties, "new_active_key", Property::REQUIRED)
    {}

    TransactionPtr make_transaction() const
    {
        m_properties.validate(MULTY_CODE_LOCATION);
        return TransactionPtr{nullptr};
    }

    Properties& get_properties()
    {
        return m_properties;
    }

public:
    const Account& m_account;

    Properties m_properties;
    FunctionalPropertyT<EosAddress, std::string> m_address;
    PropertyT<PrivateKeyPtr> m_new_private_key;
};

} // namespace

namespace multy_core
{
namespace internal
{

TransactionBuilderPtr make_eos_transaction_builder_updateauth(
        const Account& account,
        const std::string& /*action*/)
{
    return TransactionBuilderPtr{new EosTransactionBuilderUpdateAuth(account)};
}

} // namespace internal
} // namespace multy_core
