/* Copyright 2017 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_CORE_BITCOIN_ACCOUNT_H
#define MULTY_CORE_BITCOIN_ACCOUNT_H

#include "account_base.h"

namespace wallet_core
{
namespace internal
{

class BitcoinHDAccount : public HDAccountBase
{
public:
    BitcoinHDAccount(const ExtendedKey& bip44_master_key, uint32_t index);
    ~BitcoinHDAccount();

    AccountPtr make_account(
            const ExtendedKey& parent_key,
            AddressType type,
            uint32_t index) const override;
};

AccountPtr make_bitcoin_account(const char* private_key);

} // namespace wallet_core
} // namespace internal

#endif // MULTY_CORE_BITCOIN_ACCOUNT_H
