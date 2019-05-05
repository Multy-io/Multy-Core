/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_CORE_ETHEREUM_ACCOUNT_H
#define MULTY_CORE_ETHEREUM_ACCOUNT_H

#include "multy_core/src/account_base.h"

#include <string>
#include <memory>

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

class EthereumAddress;

class EthereumAccount : public AccountBase
{
protected:
    using AccountBase::AccountBase;

public:
    ~EthereumAccount();

    virtual EthereumAddress get_ethereum_address() const = 0;
};

typedef std::unique_ptr<EthereumAccount> EthereumAccountPtr;

EthereumAccountPtr make_ethereum_account(BlockchainType blockchain_type,
        const char* serialized_private_key);

} // namespace internal
} // namespace multy_core

#endif // MULTY_CORE_ETHEREUM_ACCOUNT_H
