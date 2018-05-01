/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/src/bitcoin/bitcoin_facade.h"

#include "multy_core/bitcoin.h"

#include "multy_core/src/error_utility.h"
#include "multy_core/src/exception.h"
#include "multy_core/src/exception_stream.h"
#include "multy_core/src/bitcoin/bitcoin_account.h"
#include "multy_core/src/bitcoin/bitcoin_transaction.h"

namespace
{
using namespace multy_core::internal;
} // namespace

namespace multy_core
{
namespace internal
{

BitcoinFacade::BitcoinFacade()
{
}

BitcoinFacade::~BitcoinFacade()
{
}

HDAccountPtr BitcoinFacade::make_hd_account(BlockchainType blockchain_type,
        const ExtendedKey& master_key, uint32_t index)
{
    return HDAccountPtr(new BitcoinHDAccount(blockchain_type, master_key, index));
}

AccountPtr BitcoinFacade::make_account(const char* serialized_private_key)
{
    return make_bitcoin_account(serialized_private_key);
}

TransactionPtr BitcoinFacade::make_transaction(const Account& account)
{
    return TransactionPtr(new BitcoinTransaction(account.get_blockchain_type()));
}

void BitcoinFacade::validate_address(BlockchainType blockchain_type, const char* address)
{
    INVARIANT(address);

    BitcoinAddressType address_type;
    BitcoinNetType net_type;
    bitcoin_parse_address(address, &net_type, &address_type);
    if (address_type != BITCOIN_ADDRESS_P2PKH && address_type != BITCOIN_ADDRESS_P2SH)
    {
        THROW_EXCEPTION2(ERROR_INVALID_ADDRESS, "Not supported address type.")
                << " Only P2PKH and P2SH are supported, "
                << " given address type: " << address_type;
    }
    if (net_type != blockchain_type.net_type)
    {
        THROW_EXCEPTION2(ERROR_INVALID_ADDRESS, "Incompatitable net_type.")
                << " Requested: " << blockchain_type.net_type
                << ", address net type:" << net_type;
    }
}

} // internal
} // multy_core
