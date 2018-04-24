/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_CORE_BLOCKCHAIN_FACADE_BASE_H
#define MULTY_CORE_BLOCKCHAIN_FACADE_BASE_H

#include "multy_core/api.h"
#include "multy_core/blockchain.h"

#include "multy_core/src/u_ptr.h"

#include <functional>
#include <memory>
#include <unordered_map>

namespace multy_core
{
namespace internal
{

class BlockchainFacadeBase;
typedef std::unique_ptr<BlockchainFacadeBase> BlockchainBasePtr;

class BlockchainFacadeBase
{
public:
    virtual ~BlockchainFacadeBase();

    virtual HDAccountPtr make_hd_account(
            BlockchainType blockchain_type,
            const ExtendedKey& master_key,
            uint32_t index) = 0;

    virtual AccountPtr make_account(BlockchainType,
            const char* serialized_private_key) = 0;

    virtual TransactionPtr make_transaction(const Account&) = 0;

    virtual void validate_address(BlockchainType, const char* ) = 0;
};

BlockchainFacadeBase& get_blockchain(Blockchain blockchain_type);

class BlockchainFacadeRegistry
{
public:
    static BlockchainFacadeRegistry& get_instance();

    BlockchainFacadeBase& get_blockchain(Blockchain blockchain_type);

    typedef std::function<BlockchainFacadeBase*()> FactoryFunction;
    void register_blockchain(Blockchain blockchain_type,
            FactoryFunction factory_function);

    template <typename T>
    void register_blockchain(Blockchain blockchain_type)
    {
        register_blockchain(blockchain_type, []() { return new T; });
    }

private:
    BlockchainFacadeRegistry();
    ~BlockchainFacadeRegistry();

private:
    // using Blockchain as a key
    std::unordered_map<size_t, BlockchainBasePtr> m_instances;
    std::unordered_map<size_t, FactoryFunction> m_factory_functions;
};

} // namespace internal
} // namespace multy_core

#endif // MULTY_CORE_BLOCKCHAIN_FACADE_BASE_H
