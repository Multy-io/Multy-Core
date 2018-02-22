/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_CORE_ETHEREUM_ACCOUNT_H
#define MULTY_CORE_ETHEREUM_ACCOUNT_H

#include "multy_core/src/account_base.h"

namespace multy_core
{
namespace internal
{

class EthereumHDAccount : public HDAccountBase
{
public:
    EthereumHDAccount(BlockchainType blockchain_type, const ExtendedKey& bip44_master_key, uint32_t index);

protected:
    AccountPtr make_account(
            const ExtendedKey& parent_key, AddressType type, uint32_t index) const override;
};

AccountPtr make_ethereum_account(const char* serialized_private_key);

} // namespace internal
} // namespace multy_core

#endif // MULTY_CORE_ETHEREUM_ACCOUNT_H
