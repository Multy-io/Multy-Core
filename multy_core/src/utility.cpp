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
#include "multy_core/src/u_ptr.h"

#include "wally_core.h"

#include <cassert>
#include <chrono>
#include <ctime>
#include <string>
#include <string.h>
#include <sstream>
#include <iomanip>

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
        case BLOCKCHAIN_GOLOS:
            net_type = to_string(static_cast<GolosNetType>(blockchain_type.net_type));
            break;
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
            {BLOCKCHAIN_GOLOS, "Golos"}
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

std::string to_hex_string(const BinaryData& data)
{
    multy_core::internal::CharPtr hex_str;
    wally_hex_from_bytes(data.data, data.len, reset_sp(hex_str));
    return  std::string(hex_str.get());
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

namespace
{
const char* ISO8601_TIME_FORMAT="%FT%T";

std::time_t get_timezone_offset()
{
    // local time
    static const auto t0 = std::time(nullptr);
    // global time (UTC)
    static const auto t1 = std::mktime(std::gmtime(&t0));

    return t0 - t1;
}

struct ExpectedSymbol
{
    const char symbol;
};

std::istream& operator>>(std::istream& istr, const ExpectedSymbol& expected)
{
    char c = '\0';
    istr >> c;
    if (c != expected.symbol)
    {
        THROW_EXCEPTION("Unexpected symbol.")
                << " At position: " << istr.tellg()
                << " expected: '" << expected.symbol
                << "' got: '" << c << "'";
    }

    return istr;
}
} // namespace

std::string format_iso8601_string(const std::time_t& time)
{
    std::ostringstream ss;
    ss << std::put_time(std::gmtime(&time), ISO8601_TIME_FORMAT);

    return ss.str();
}

std::time_t parse_iso8601_string(const std::string& str)
{
    std::string time_string = str;

    bool global_time = false;
    // UTC (Zulu-time)
    if (time_string.length() > 0 && time_string.back() == 'Z')
    {
        global_time = true;
        time_string.pop_back();
    }

    std::tm tm;
    memset(&tm, 0, sizeof(tm));

    std::istringstream ss(time_string);
    try
    {
        ss.exceptions(std::ios::failbit | std::ios::badbit);

        ss >> tm.tm_year >> ExpectedSymbol{'-'}
            >> tm.tm_mon >> ExpectedSymbol{'-'}
            >> tm.tm_mday >> ExpectedSymbol{'T'}
            >> tm.tm_hour >> ExpectedSymbol{':'}
            >> tm.tm_min >> ExpectedSymbol{':'}
            >> tm.tm_sec;
        if (!ss.eof())
        {
            THROW_EXCEPTION2(ERROR_INVALID_TIME_STRING,
                    "Failed to parse ISO8601 time.")
                    << " Leftovers after parsing: \"" << ss.str() << "\".";
        }

        tm.tm_year -= 1900;
        tm.tm_mon -= 1;
    } catch (const std::exception& e) {
        THROW_EXCEPTION2(ERROR_INVALID_TIME_STRING,
                "Failed to parse ISO8601-time.")
                << " " << e.what();
    }
    if (tm.tm_year < 0
        || tm.tm_mon < 0 || tm.tm_mon > 11
        || tm.tm_mday < 1 || tm.tm_mday > 31
        || tm.tm_hour < 0 || tm.tm_hour > 23
        || tm.tm_min < 0 || tm.tm_min > 59
        || tm.tm_sec < 0 || tm.tm_sec > 60)
    {
        THROW_EXCEPTION2(ERROR_INVALID_TIME_STRING,
                "Failed to parse ISO8601-time.")
                << " Value is out of range: " << str;
    }

    const auto result = std::mktime(&tm);
    if (result < 0)
    {
        THROW_EXCEPTION2(ERROR_INVALID_TIME_STRING,
                "Invalid ISO8601 date/time value.");
    }

    // mktime() converts to localtime, adding timezone offset to make global time.
    return result + (global_time ? get_timezone_offset() : 0);
}

std::time_t to_system_seconds(size_t seconds)
{
    return std::chrono::system_clock::to_time_t(
            std::chrono::system_clock::from_time_t(0)
            + std::chrono::seconds(seconds));
}

std::time_t get_system_time_now()
{
    return std::chrono::system_clock::to_time_t(
            std::chrono::system_clock::now());
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
