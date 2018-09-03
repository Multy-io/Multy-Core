/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/src/eos/eos_facade.h"

#include "multy_core/eos.h"
#include "multy_core/src/eos/eos_account.h"
#include "multy_core/src/eos/eos_name.h"
#include "multy_core/src/eos/eos_transaction.h"

#include "multy_core/src/exception.h"
#include "multy_core/src/exception_stream.h"

#include "multy_core/src/eos/eos_account.h"
#include "multy_core/src/eos/eos_transaction.h"
#include "multy_core/src/eos/eos_transaction_builder_updateauth.h"

#include <cstring>

namespace
{

void validate_EOS_account_type(uint32_t account_type)
{
    if (account_type != 0)
    {
        THROW_EXCEPTION2(ERROR_INVALID_ARGUMENT,
                "Unknown EOS account type.")
                << " Value: " << account_type << ".";
    }
}

} // namespace

namespace multy_core
{
namespace internal
{

EosFacade::EosFacade()
{
}

EosFacade::~EosFacade()
{
}

HDAccountPtr EosFacade::make_hd_account(
        BlockchainType blockchain_type,
        uint32_t account_type,
        const ExtendedKey& master_key,
        uint32_t index) const
{
    validate_EOS_account_type(account_type);

    return HDAccountPtr(new EosHDAccount(blockchain_type, master_key, index));
}

AccountPtr EosFacade::make_account(
        BlockchainType blockchain_type,
        uint32_t account_type,
        const char* serialized_private_key) const
{
    validate_EOS_account_type(account_type);

    return make_EOS_account(blockchain_type, serialized_private_key);
}

TransactionPtr EosFacade::make_transaction(const Account& account) const
{
    return TransactionPtr(new EosTransaction(account));
}

TransactionBuilderPtr EosFacade::make_transaction_builder(
        const Account& account,
        uint32_t type,
        const char* action) const
{
    typedef TransactionBuilderPtr (*BuilderFactoryFunction)(const Account&, const std::string&);
    static const std::unordered_map<size_t, BuilderFactoryFunction> BUILDERS =
    {
        {
            EOS_TRANSACTION_BUILDER_UPDATEAUTH,
            &make_eos_transaction_builder_updateauth
        },
    };

    const auto builder = BUILDERS.find(type);
    if (builder == BUILDERS.end())
    {
        THROW_EXCEPTION2(ERROR_INVALID_ARGUMENT,
                "Invalid TransactionBuilder type.")
                << " Type: " << type
                << ", action: \"" << (action ? action : "") << "\".";
    }

    return builder->second(account, std::string(action ? action : ""));
}

void EosFacade::validate_address(
        BlockchainType blockchain_type, const char* address) const
{
    INVARIANT(address != nullptr);
    INVARIANT(blockchain_type.blockchain == BLOCKCHAIN_EOS);

    if (strlen(address) == 0)
    {
        THROW_EXCEPTION2(ERROR_INVALID_ADDRESS, "EOS Address is to short.")
                << " Address expected to be at least 1 character long, got 0.";
    }

    try
    {
        EosName::from_string(address);
    }
    catch (const std::exception& e)
    {
        THROW_EXCEPTION2(ERROR_INVALID_ADDRESS, "Invalid EOS address.")
                << " " << e.what() << ".";
    }
}

std::string EosFacade::encode_serialized_transaction(
        Transaction* transaction) const
{
    INVARIANT(transaction != nullptr);
    return transaction->encode_serialized();
}

} // namespace internal
} // namespace multy_core
