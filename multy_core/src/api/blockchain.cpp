/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/blockchain.h"

#include "multy_core/common.h"
#include "multy_core/error.h"

#include "multy_core/src/blockchain_facade_base.h"
#include "multy_core/src/utility.h"

namespace
{
using namespace multy_core::internal;
} // namespace

Error* validate_address(BlockchainType blockchain_type, const char* address)
{
    ARG_CHECK(address);

    try
    {
        get_blockchain(blockchain_type.blockchain)
                .validate_address(blockchain_type, address);
    }
    CATCH_EXCEPTION_RETURN_ERROR();

    return nullptr;
}
