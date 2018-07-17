/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/src/utility.h"

#include "multy_core/bitcoin.h"
#include "multy_core/ethereum.h"
#include "multy_core/golos.h"

#include "multy_core/blockchain.h"
#include "multy_core/error.h"

#include "multy_core/src/enum_name_map.h"
#include "multy_core/src/exception.h"
#include "multy_core/src/exception_stream.h"

#include "wally_core.h"

#include <cassert>
#include <string>
#include <string.h>

namespace
{
using namespace multy_core::internal;
} // namespace

namespace multy_core
{
namespace internal
{

char* copy_string(const std::string& str)
{
    return copy_string(str.c_str());
}

char* copy_string(const char* str)
{
    if (!str)
    {
        return nullptr;
    }

    const size_t len = strlen(str);

    wally_operations wally_ops;
    int result = wally_get_operations(&wally_ops);

    INVARIANT(result == WALLY_OK);
    INVARIANT(wally_ops.malloc_fn != nullptr);

    char* new_message = static_cast<char*>(wally_ops.malloc_fn(len + 1));
    if (!new_message)
    {
        // TODO: any better way for handling out of memory,
        // cause throwing an exception, in fact, allocates more memory.
        THROW_EXCEPTION2(ERROR_OUT_OF_MEMORY, "Failed to allocate memory.")
                << " Requested: " << len + 1;
    }

    memcpy(new_message, str, len);
    new_message[len] = '\0';
    return new_message;
}

bool operator==(const BlockchainType& left, const BlockchainType& right)
{
    return left.blockchain == right.blockchain
            && left.net_type == right.net_type;
}

std::string to_string(const BlockchainType& blockchain_type)
{
    std::string net_type;
    switch(blockchain_type.blockchain)
    {
        case BLOCKCHAIN_BITCOIN:
            net_type = to_string(static_cast<BitcoinNetType>(blockchain_type.net_type));
            break;
        case BLOCKCHAIN_ETHEREUM:
            net_type = to_string(static_cast<EthereumChainId>(blockchain_type.net_type));
            break;
#if MULTY_WITH_GOLOS
        case BLOCKCHAIN_GOLOS:
            net_type = to_string(static_cast<GolosNetType>(blockchain_type.net_type));
            break;
#endif // MULTY_WITH_GOLOS
        default:
            net_type = "UNKNOWN";
    }

    return to_string(blockchain_type.blockchain) + '/' + net_type;
}

std::string to_string(Blockchain blockchain)
{
    static const EnumNameMap<Blockchain> BLOCKCHAIN_NAMES =
    {
        "BlockchainNames",
        {
            {BLOCKCHAIN_BITCOIN, "Bitcoin"},
            {BLOCKCHAIN_ETHEREUM, "Ethereum"},
#if MULTY_WITH_GOLOS
            {BLOCKCHAIN_GOLOS, "Golos"}
#endif // MULTY_WITH_GOLOS
        }
    };

    return BLOCKCHAIN_NAMES.get_name_or_dummy(blockchain);
}

std::string to_string(BitcoinNetType net_type)
{
    static const EnumNameMap<BitcoinNetType> BITCOIN_NET_TYPE_NAMES =
    {
        "BitcoinNetType",
        {
            {BITCOIN_NET_TYPE_MAINNET, "MainNet"},
            {BITCOIN_NET_TYPE_TESTNET, "TestNet"},
        }
    };

    return BITCOIN_NET_TYPE_NAMES.get_name_or_dummy(net_type);
}

std::string to_string(EthereumChainId net_type)
{
    static const EnumNameMap<EthereumChainId> ETHEREUM_NET_TYPE_NAMES =
    {
        "EthereumChainId",
        {
            {ETHEREUM_CHAIN_ID_PRE_EIP155, "PRE-EIP115_DEFAULT"},
            {ETHEREUM_CHAIN_ID_MAINNET, "MainNet"},
            {ETHEREUM_CHAIN_ID_MORDEN, "Morden"},
            {ETHEREUM_CHAIN_ID_ROPSTEN, "Ropsten"},
            {ETHEREUM_CHAIN_ID_RINKEBY, "Rinkeby"},
            {ETHEREUM_CHAIN_ID_ROOTSTOCK_MAINNET, "RootstockMainNet"},
            {ETHEREUM_CHAIN_ID_ROOTSTOCK_TESTNET, "RootstockTestNet"},
            {ETHEREUM_CHAIN_ID_KOVAN, "Kovan"},
            {ETHEREUM_CHAIN_ID_ETC_MAINNET, "ETCMainNet"},
            {ETHEREUM_CHAIN_ID_ETC_TESTNET, "ETCTestNet"},
        }
    };

    return ETHEREUM_NET_TYPE_NAMES.get_name_or_dummy(net_type);
}

std::string to_string(GolosNetType net_type)
{
    static const EnumNameMap<GolosNetType> GOLOS_NET_TYPE_NAMES =
    {
        "GolosNetType",
        {
            {GOLOS_NET_TYPE_MAINNET, "MainNet"},
            {GOLOS_NET_TYPE_TESTNET, "TestNet"},
        }
    };

    return GOLOS_NET_TYPE_NAMES.get_name_or_dummy(net_type);
}

void trim_excess_trailing_null(std::string* str)
{
    auto pos = str->find_last_not_of('\0');
    if (pos != std::string::npos)
    {
        str->erase(pos + 1);
    }
}

} // namespace internal
} // namespace multy_core
