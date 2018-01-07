/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_CORE_ACCOUNT_BASE_H
#define MULTY_CORE_ACCOUNT_BASE_H

#include "multy_core/api.h"
#include "multy_core/account.h"
#include "multy_core/keys.h"

#include "multy_core/internal/account.h"
#include "multy_core/internal/u_ptr.h"
#include "multy_core/internal/hd_path.h"

#include <array>
#include <memory>
#include <stddef.h>
#include <unordered_map>
#include <vector>

struct ExtendedKey;

namespace wallet_core
{
namespace internal
{
class AccountBase : public Account
{
public:
    AccountBase(
            Currency currency,
            const PrivateKey& private_key_ref,
            const HDPath& path = HDPath());
    virtual ~AccountBase();

    virtual Currency get_currency() const;
    virtual HDPath get_path() const;
    virtual PrivateKeyPtr get_private_key() const;
    virtual PublicKeyPtr get_public_key() const;

protected:
    const Currency m_currency;
    const HDPath m_path;

private:
    const PrivateKey& m_private_key_ref;
};

// Base class for coin-specific HD accounts.
struct HDAccountBase : public HDAccount
{
public:
    typedef wallet_core::internal::HDPath HDPath;

    virtual ~HDAccountBase();

    AccountPtr make_leaf_account(AddressType type, uint32_t index) const override;

protected:
    HDAccountBase(const ExtendedKey& bip44_master_key, Currency currency, uint32_t index);

    HDPath get_path() const override;
    Currency get_currency() const override;

    virtual AccountPtr make_account(
            const ExtendedKey& parent_key,
            AddressType type,
            uint32_t index) const = 0;

private:
    ExtendedKeyPtr m_account_key;
    const Currency m_currency;
    HDPath m_bip44_path;
};

} // namespace wallet_core
} // namespace internal

#endif // MULTY_CORE_ACCOUNT_BASE_H
