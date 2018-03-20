/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/blockchain.h"

#include "multy_core/common.h"
#include "multy_core/error.h"

#include "multy_core/src/bitcoin/bitcoin_account.h"
#include "multy_core/src/exception.h"
#include "multy_core/src/exception_stream.h"
#include "multy_core/src/utility.h"

namespace
{
using namespace multy_core::internal;
} // namespace

Error* validate_address(BlockchainType blockchain_type, const char* address)
{
    ARG_CHECK(address);
    ARG_CHECK(blockchain_type.blockchain == BLOCKCHAIN_BITCOIN
            || blockchain_type.blockchain == BLOCKCHAIN_ETHEREUM);

    try
    {
        switch (blockchain_type.blockchain)
        {
            case BLOCKCHAIN_BITCOIN:
            {
                BitcoinAddressType address_type;
                BitcoinNetType net_type;
                bitcoin_parse_address(address, &net_type, &address_type);
                if (address_type != BITCOIN_ADDRESS_P2PKH)
                {
                    THROW_EXCEPTION("Not supported address type, only P2PKH and P2SH are supported.")
                                   << " Address type: " << address_type;
                }
                if (net_type != blockchain_type.net_type)
                {
                    THROW_EXCEPTION("Incompatitable net_type.")
                            << " Requested: " << blockchain_type.net_type
                            << ", address net type:" << net_type;
                }
                break;
            }
            case BLOCKCHAIN_ETHEREUM:
            {
                THROW_EXCEPTION("ETH addresses are not supported yet.");
            }
            default:
            {
                return MAKE_ERROR(
                        ERROR_GENERAL_ERROR, "Blockchain not supported yet");
            }
        }
    }
    CATCH_EXCEPTION_RETURN_ERROR();

    return nullptr;
}
