/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_CORE_ACCOUNT_BASE_H
#define MULTY_CORE_ACCOUNT_BASE_H

#include "multy_core/api.h"
#include "multy_core/account.h"
#include "multy_core/key.h"

#include "multy_core/src/api/account_impl.h"
#include "multy_core/src/u_ptr.h"
#include "multy_core/src/hd_path.h"

#include <array>
#include <memory>
#include <stddef.h>
#include <unordered_map>
#include <vector>

struct ExtendedKey;

namespace multy_core
{
namespace internal
{

const uint32_t CHAIN_INDEX_TEST = 0x01;

class AccountBase : public Account
{
public:
    AccountBase(
            BlockchainType blockchain_type,
            const PrivateKey& private_key_ref,
            const HDPath& path = HDPath());
    virtual ~AccountBase();

    virtual BlockchainType get_blockchain_type() const;
    virtual HDPath get_path() const;
    virtual PrivateKeyPtr get_private_key() const;
    virtual PublicKeyPtr get_public_key() const;

protected:
    const BlockchainType m_blockchain_type;
    const HDPath m_path;

private:
    const PrivateKey& m_private_key_ref;
};

// Base class for coin-specific HD accounts.
struct HDAccountBase : public HDAccount
{
public:
    typedef multy_core::internal::HDPath HDPath;

    virtual ~HDAccountBase();

    AccountPtr make_leaf_account(AddressType type, uint32_t index) const override;

protected:
    HDAccountBase(
            BlockchainType blockchain_type,
            uint32_t chain_index,
            const ExtendedKey& bip44_master_key,
            uint32_t index);

    HDPath get_path() const override;
    BlockchainType get_blockchain_type() const override;
    ExtendedKeyPtr get_account_key() const override;

    virtual AccountPtr make_account(
            const ExtendedKey& parent_key,
            AddressType type,
            uint32_t index) const = 0;

private:
    const BlockchainType m_blockchain_type;
    ExtendedKeyPtr m_account_key;
    HDPath m_bip44_path;
};

} // namespace multy_core
} // namespace internal

#endif // MULTY_CORE_ACCOUNT_BASE_H
