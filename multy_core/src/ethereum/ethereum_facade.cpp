/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/src/ethereum/ethereum_facade.h"

#include "multy_core/src/ethereum/ethereum_account.h"
#include "multy_core/src/ethereum/ethereum_transaction.h"

#include "multy_core/src/exception.h"
#include "multy_core/src/exception_stream.h"

namespace
{

void validate_ethereum_account_type(uint32_t account_type)
{
    if (account_type != 0)
    {
        THROW_EXCEPTION2(ERROR_INVALID_ARGUMENT,
                "Unknown Ethereum account type.")
                << " Value: " << account_type << ".";
    }
}

} // namespace

namespace multy_core
{
namespace internal
{

EthereumFacade::EthereumFacade()
{
}

EthereumFacade::~EthereumFacade()
{
}

HDAccountPtr EthereumFacade::make_hd_account(
        BlockchainType blockchain_type,
        uint32_t account_type,
        const ExtendedKey& master_key,
        uint32_t index)
{
    validate_ethereum_account_type(account_type);

    return HDAccountPtr(new EthereumHDAccount(blockchain_type, master_key, index));
}

AccountPtr EthereumFacade::make_account(
        BlockchainType blockchain_type,
        uint32_t account_type,
        const char* serialized_private_key)
{
    validate_ethereum_account_type(account_type);

    return make_ethereum_account(blockchain_type, serialized_private_key);
}

TransactionPtr EthereumFacade::make_transaction(const Account& account)
{
    return TransactionPtr(new EthereumTransaction(account));
}

void EthereumFacade::validate_address(
        BlockchainType, const char* address)
{
    ethereum_parse_address(address);
}

} // namespace internal
} // namespace multy_core
