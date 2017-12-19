/* Copyright 2017 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_CORE_INTERNAL_ACCOUNT_H
#define MULTY_CORE_INTERNAL_ACCOUNT_H

#include "multy_core/account.h"
#include "multy_core/api.h"

#include "multy_core/internal/hd_path.h"
#include "multy_core/internal/u_ptr.h"

struct PublicKey;
struct PrivateKey;

// Declared a struct (and out of wallet_core::internal namespace)
// for consitency with a C-like interface.
// Exported only to make testing easier.
struct MULTY_CORE_API Account
{
public:
    typedef wallet_core::internal::HDPath HDPath;
    typedef wallet_core::internal::PrivateKeyPtr PrivateKeyPtr;
    typedef wallet_core::internal::PublicKeyPtr PublicKeyPtr;

    Account();
    virtual ~Account();

    virtual HDPath get_path() const = 0;
    virtual Currency get_currency() const = 0;
    virtual std::string get_address() const = 0;
    virtual PrivateKeyPtr get_private_key() const = 0;
    virtual PublicKeyPtr get_public_key() const = 0;

    bool is_valid() const;

private:
    const void* m_magic;
};

// Exported only to make testing easier.
struct MULTY_CORE_API HDAccount
{
public:
    typedef wallet_core::internal::AccountPtr AccountPtr;
    typedef wallet_core::internal::HDPath HDPath;

    HDAccount();

    virtual HDPath get_path() const = 0;
    virtual Currency get_currency() const = 0;

    virtual ~HDAccount();
    virtual AccountPtr make_leaf_account(AddressType type, uint32_t index) const = 0;

    bool is_valid() const;
private:
    const void* m_magic;
};

#endif // MULTY_CORE_INTERNAL_ACCOUNT_H
