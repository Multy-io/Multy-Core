/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_CORE_BITCOIN_FACADE_H
#define MULTY_CORE_BITCOIN_FACADE_H

#include "multy_core/src/blockchain_facade_base.h"

namespace multy_core
{
namespace internal
{

class BitcoinFacade : public BlockchainFacadeBase
{
public:
    BitcoinFacade();
    ~BitcoinFacade();

    HDAccountPtr make_hd_account(BlockchainType blockchain_type,
            const ExtendedKey& master_key, uint32_t index) override;
    AccountPtr make_account(BlockchainType blockchain_type, const char* serialized_private_key) override;
    TransactionPtr make_transaction(const Account&) override;
    void validate_address(BlockchainType blockchain_type, const char*) override;
};

} // internal
} // multy_core

#endif // MULTY_CORE_BITCOIN_FACADE_H
