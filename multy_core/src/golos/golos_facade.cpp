/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/src/golos/golos_facade.h"

#include "multy_core/src/exception.h"
#include "multy_core/src/golos/golos_account.h"

namespace multy_core
{
namespace internal
{

GolosFacade::GolosFacade()
{
}

GolosFacade::~GolosFacade()
{
}

HDAccountPtr GolosFacade::make_hd_account(
        BlockchainType blockchain_type,
        const ExtendedKey& master_key,
        uint32_t index)
{
    return HDAccountPtr(new GolosHDAccount(blockchain_type, master_key, index));
}

AccountPtr GolosFacade::make_account(const char* serialized_private_key)
{
    return make_golos_account(serialized_private_key);
}

TransactionPtr GolosFacade::make_transaction(const Account& /*account*/)
{
    THROW_EXCEPTION2(ERROR_FEATURE_NOT_IMPLEMENTED_YET,
            "Creating Golos transactions is not supported yet.");
}

void GolosFacade::validate_address(
        BlockchainType, const char*)
{
    // We know nothing about golos address and can do perform basic verification,
    // like absence/presence of whitespace and special chars.

    THROW_EXCEPTION2(ERROR_FEATURE_NOT_IMPLEMENTED_YET,
            "Validating Golos addresses is not supported yet.");
}

} // namespace internal
} // namespace multy_core
