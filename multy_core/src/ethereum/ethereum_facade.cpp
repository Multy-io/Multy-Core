/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/src/ethereum/ethereum_facade.h"

#include "multy_core/ethereum.h"
#include "multy_core/src/ethereum/ethereum_account.h"
#include "multy_core/src/ethereum/ethereum_address.h"
#include "multy_core/src/ethereum/ethereum_transaction.h"
#include "multy_core/src/ethereum/ethereum_transaction_builder_multisig.h"
#include "multy_core/src/ethereum/ethereum_transaction_builder_erc20.h"

#include "multy_core/src/exception.h"
#include "multy_core/src/exception_stream.h"

#include "multy_core/src/codec.h"

#include <unordered_map>

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
        uint32_t index) const
{
    validate_ethereum_account_type(account_type);

    return HDAccountPtr(new EthereumHDAccount(blockchain_type, master_key, index));
}

AccountPtr EthereumFacade::make_account(
        BlockchainType blockchain_type,
        uint32_t account_type,
        const char* serialized_private_key) const
{
    validate_ethereum_account_type(account_type);

    return make_ethereum_account(blockchain_type, serialized_private_key);
}

TransactionPtr EthereumFacade::make_transaction(const Account& account) const
{
    return TransactionPtr(new EthereumTransaction(account));
}

TransactionBuilderPtr EthereumFacade::make_transaction_builder(
        const Account& account, uint32_t type, const char* action) const
{
    typedef TransactionBuilderPtr (*BuilderFactoryFunction)(const Account&, const std::string&);
    static const std::unordered_map<size_t, BuilderFactoryFunction> BUILDERS =
    {
        {
            ETHEREUM_TRANSACTION_BUILDER_MULTISIG,
            &make_ethereum_multisig_transaction_builder
        },
        {
            ETHEREUM_TRANSACTION_BUILDER_ERC20,
            &make_ethereum_ERC20_transaction_builder
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

void EthereumFacade::validate_address(
        BlockchainType, const char* address) const
{
    EthereumAddress::from_string(address);
}

std::string EthereumFacade::encode_serialized_transaction(
        Transaction* transaction) const
{
    INVARIANT(transaction != nullptr);
    const BinaryDataPtr serialized_transaction = transaction->serialize();
    return "0x" + encode(*serialized_transaction, CODEC_HEX);
}

} // namespace internal
} // namespace multy_core
