#include "multy_core/src/blockchain_facade_base.h"

#include "multy_core/src/hash.h"
#include "multy_core/src/utility.h"

#include "multy_core/src/exception.h"
#include "multy_core/src/exception_stream.h"

#include "multy_core/src/bitcoin/bitcoin_facade.h"
#include "multy_core/src/ethereum/ethereum_facade.h"
#include "multy_core/src/golos/golos_facade.h"

namespace
{
using namespace multy_core::internal;
} // namespace

namespace multy_core
{
namespace internal
{

BlockchainFacadeBase& get_blockchain(Blockchain blockchain_type)
{
    return BlockchainFacadeRegistry::get_instance().get_blockchain(blockchain_type);
}

BlockchainFacadeBase::~BlockchainFacadeBase()
{
}

BlockchainFacadeRegistry::BlockchainFacadeRegistry()
    : m_instances(),
      m_factory_functions()
{
    // Exiplictly registering blockchains, since when multy_core
    // is compiled as static library, registrar-based approach,
    // based on static constructors does not work:
    // Static initializers are not invoked and blockchains stay unregistered.
    register_blockchain<BitcoinFacade>(BLOCKCHAIN_BITCOIN);
    register_blockchain<EthereumFacade>(BLOCKCHAIN_ETHEREUM);
    register_blockchain<GolosFacade>(BLOCKCHAIN_GOLOS);
}

BlockchainFacadeRegistry::~BlockchainFacadeRegistry()
{
}

BlockchainFacadeRegistry& BlockchainFacadeRegistry::get_instance()
{
    static BlockchainFacadeRegistry g_factory;
    return g_factory;
}

BlockchainFacadeBase& BlockchainFacadeRegistry::get_blockchain(
        Blockchain blockchain_type)
{
    auto instance = m_instances.find(blockchain_type);
    if (instance == m_instances.end())
    {
        const auto& factory = m_factory_functions.find(blockchain_type);
        if (factory == m_factory_functions.end())
        {
            THROW_EXCEPTION2(ERROR_INVALID_ARGUMENT,
                    "Not supported blockchain type.")
                    << " Requested type: " << blockchain_type;
        }

        BlockchainBasePtr new_instance{factory->second()};
        const auto inserted = m_instances.emplace(blockchain_type,
                std::move(new_instance));

        INVARIANT2(inserted.second == true, blockchain_type);

        instance = inserted.first;
    }

    return *instance->second;
}

void BlockchainFacadeRegistry::register_blockchain(
        Blockchain blockchain_type,
        BlockchainFacadeRegistry::FactoryFunction factory_function)
{
    const auto p = m_factory_functions.emplace(blockchain_type,
            std::move(factory_function));

    INVARIANT2(p.second == true, blockchain_type);
}

} // namespace internal
} // namespace multy_core
