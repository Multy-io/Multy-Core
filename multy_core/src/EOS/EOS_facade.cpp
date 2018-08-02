/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/src/EOS/EOS_facade.h"

#include "multy_core/EOS.h"
#include "multy_core/src/EOS/EOS_account.h"
#include "multy_core/src/EOS/eos_name.h"
#include "multy_core/src/EOS/EOS_transaction.h"

#include "multy_core/src/exception.h"
#include "multy_core/src/exception_stream.h"

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

EOSFacade::EOSFacade()
{
}

EOSFacade::~EOSFacade()
{
}

HDAccountPtr EOSFacade::make_hd_account(
        BlockchainType blockchain_type,
        uint32_t account_type,
        const ExtendedKey& master_key,
        uint32_t index) const
{
    validate_EOS_account_type(account_type);

    return HDAccountPtr(new EOSHDAccount(blockchain_type, master_key, index));
}

AccountPtr EOSFacade::make_account(
        BlockchainType blockchain_type,
        uint32_t account_type,
        const char* serialized_private_key) const
{
    validate_EOS_account_type(account_type);

    return make_EOS_account(blockchain_type, serialized_private_key);
}

TransactionPtr EOSFacade::make_transaction(const Account& account) const
{
    return TransactionPtr(new EOSTransaction(account));
}

void EOSFacade::validate_address(
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

std::string EOSFacade::encode_serialized_transaction(
        Transaction* transaction) const
{
    INVARIANT(transaction != nullptr);
    return transaction->encode_serialized();
}

} // namespace internal
} // namespace multy_core
