/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef WALLET_CORE_INTERNAL_KEY_H
#define WALLET_CORE_INTERNAL_KEY_H

#include "multy_core/api.h"
#include "multy_core/internal/u_ptr.h"

#include "wally_bip32.h"

#include <string>

struct BinaryData;
struct PrivateKey;
struct PublicKey;

struct MULTY_CORE_API ExtendedKey
{
    ExtendedKey();

    std::string to_string() const;

    bool is_valid() const;

public:
    ext_key key;

private:
    const void* m_magic;
};

struct MULTY_CORE_API Key
{
    Key();
    virtual ~Key();

    virtual std::string to_string() const = 0;

//    virtual wallet_core::internal::BinaryDataPtr encrypt(const BinaryData* data) const = 0;
//    virtual wallet_core::internal::BinaryDataPtr decrypt(const BinaryData* data) const = 0;

    bool is_valid() const;

private:
    const void* m_magic;
};

struct MULTY_CORE_API PrivateKey : public Key
{
    virtual ~PrivateKey();

    virtual wallet_core::internal::PublicKeyPtr make_public_key() const = 0;
    virtual wallet_core::internal::PrivateKeyPtr clone() const = 0;
    virtual wallet_core::internal::BinaryDataPtr sign(const BinaryData& data) const = 0;
};

struct MULTY_CORE_API PublicKey : public Key
{
    virtual ~PublicKey();
    virtual wallet_core::internal::PublicKeyPtr clone() const = 0;
    virtual const BinaryData get_content() const = 0;
};

#endif // WALLET_CORE_INTERNAL_KEY_H
