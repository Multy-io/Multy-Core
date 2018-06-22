/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/src/EOS/EOS_facade.h"

#include "multy_core/EOS.h"

#include "multy_core/src/exception.h"
#include "multy_core/src/exception_stream.h"

namespace multy_core
{
namespace internal
{

EOSFacade::EOSFacade()
{
}

EOSFacade::~EOSFacade()
{
}

HDAccountPtr EOSFacade::make_hd_account(
        BlockchainType /*blockchain_type*/,
        uint32_t /*account_type*/,
        const ExtendedKey& /*master_key*/,
        uint32_t /*index*/) const
{
    THROW_EXCEPTION("Not implimented yet");
}

AccountPtr EOSFacade::make_account(
        BlockchainType /*blockchain_type*/,
        uint32_t /*account_type*/,
        const char* /*serialized_private_key*/) const
{
    THROW_EXCEPTION("Not implimented yet");
}

TransactionPtr EOSFacade::make_transaction(const Account& /*account*/) const
{
    THROW_EXCEPTION("Not implimented yet");
}

void EOSFacade::validate_address(
        BlockchainType /*blockchain_type*/, const char* /*address*/) const
{
    THROW_EXCEPTION("Not implimented yet");
}

std::string EOSFacade::encode_serialized_transaction(
        const BinaryData& /*serialized_transaction*/) const
{
    THROW_EXCEPTION("Not implimented yet");
}

} // namespace internal
} // namespace multy_core
