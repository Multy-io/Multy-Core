/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_CORE_SRC_API_ACCOUNT_IMPL_H
#define MULTY_CORE_SRC_API_ACCOUNT_IMPL_H

#include "multy_core/account.h"
#include "multy_core/api.h"

#include "multy_core/src/object.h"

#include "multy_core/src/hd_path.h"
#include "multy_core/src/u_ptr.h"

struct PublicKey;
struct PrivateKey;

// Declared a struct (and out of multy_core::internal namespace)
// for consitency with a C-like interface.
// Exported only to make testing easier.
struct MULTY_CORE_API Account : public ::multy_core::internal::ObjectBase<Account>
{
public:
    typedef multy_core::internal::HDPath HDPath;
    typedef multy_core::internal::PrivateKeyPtr PrivateKeyPtr;
    typedef multy_core::internal::PublicKeyPtr PublicKeyPtr;

    Account();
    virtual ~Account();

    virtual HDPath get_path() const = 0;
    virtual BlockchainType get_blockchain_type() const = 0;
    virtual std::string get_address() const = 0;
    virtual PrivateKeyPtr get_private_key() const = 0;
    virtual PublicKeyPtr get_public_key() const = 0;

    static const void* get_object_magic();
};

// Exported only to make testing easier.
struct MULTY_CORE_API HDAccount : public ::multy_core::internal::ObjectBase<HDAccount>
{
public:
    typedef multy_core::internal::AccountPtr AccountPtr;
    typedef multy_core::internal::HDPath HDPath;

    HDAccount();
    virtual ~HDAccount();

    virtual HDPath get_path() const = 0;
    virtual BlockchainType get_blockchain_type() const = 0;

    virtual AccountPtr make_leaf_account(AddressType type, uint32_t index) const = 0;

    static const void* get_object_magic();
};

#endif // MULTY_CORE_SRC_API_ACCOUNT_IMPL_H
