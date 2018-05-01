/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/src/golos/golos_facade.h"

#include "multy_core/golos.h"
#include "multy_core/src/golos/golos_account.h"
#include "multy_core/src/golos/golos_transaction.h"

#include "multy_core/src/exception.h"
#include "multy_core/src/exception_stream.h"

#include <algorithm>
#include <cstring>
#include <regex>

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

TransactionPtr GolosFacade::make_transaction(const Account& account)
{
    return TransactionPtr(new GolosTransaction(account.get_blockchain_type()));
}

void GolosFacade::validate_address(
        BlockchainType, const char* address)
{
    INVARIANT(address != nullptr);

    const size_t address_length = strlen(address);
    if (address_length < GOLOS_ACCOUNT_MIN_LENGTH
        || address_length > GOLOS_ACCOUNT_MAX_LENGTH)
    {
        THROW_EXCEPTION2(ERROR_INVALID_ADDRESS, "Invalid Golos account length.")
                << " Expected: between " << GOLOS_ACCOUNT_MIN_LENGTH
                << " and " << GOLOS_ACCOUNT_MAX_LENGTH
                << ", actual: " << address_length << ".";
    }

    static const char* GOLOS_ACCOUNT_NAME_RE = "[a-z][a-z0-9]+";
    static const std::regex golos_account_name_re(GOLOS_ACCOUNT_NAME_RE);
    if (!std::regex_match(address, golos_account_name_re))
    {
        THROW_EXCEPTION2(ERROR_INVALID_ADDRESS, "Invalid Golos account address.")
                << "Should match pattern: \"" << GOLOS_ACCOUNT_NAME_RE << "\".";
    }
}

} // namespace internal
} // namespace multy_core
