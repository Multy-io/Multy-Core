/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/src/ethereum/ethereum_facade.h"

#include "multy_core/src/exception.h"
#include "multy_core/src/ethereum/ethereum_account.h"
#include "multy_core/src/ethereum/ethereum_transaction.h"

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
        const ExtendedKey& master_key,
        uint32_t index)
{
    return HDAccountPtr(new EthereumHDAccount(blockchain_type, master_key, index));
}

AccountPtr EthereumFacade::make_account(const char* serialized_private_key)
{
    return make_ethereum_account(serialized_private_key);
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
