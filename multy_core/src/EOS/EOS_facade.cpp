/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/src/EOS/EOS_facade.h"

#include "multy_core/EOS.h"

#include "multy_core/src/exception.h"
#include "multy_core/src/exception_stream.h"
#include "multy_core/src/EOS/EOS_account.h"
#include "multy_core/src/EOS/EOS_transaction.h"

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
        BlockchainType /*blockchain_type*/, const char* /*address*/) const
{
    //  TODO: add logic to validate address
}

std::string EOSFacade::encode_serialized_transaction(
        const BinaryData& /*serialized_transaction*/) const
{
    THROW_EXCEPTION("Not implimented yet");
}

} // namespace internal
} // namespace multy_core
