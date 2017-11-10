/* Copyright 2017 by Multy.io
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

namespace wallet_core
{
namespace internal
{

typedef UPtr<BinaryData> BinaryDataPtr;
typedef UPtr<PrivateKey> PrivateKeyPtr;
typedef UPtr<PublicKey> PublicKeyPtr;

} // namespace wallet_core
} // namespace internal

struct MULTY_CORE_API ExtendedKey
{
    ext_key key;

    std::string to_string() const;
};

struct MULTY_CORE_API Key
{
    virtual ~Key();

    virtual std::string to_string() const = 0;
    virtual const BinaryData get_content() const = 0;

//    virtual wallet_core::internal::BinaryDataPtr sign(const BinaryData* data) const = 0;
//    virtual wallet_core::internal::BinaryDataPtr encrypt(const BinaryData* data) const = 0;
//    virtual wallet_core::internal::BinaryDataPtr decrypt(const BinaryData* data) const = 0;
};

struct MULTY_CORE_API PrivateKey : public Key
{
    virtual ~PrivateKey();

    virtual wallet_core::internal::PublicKeyPtr make_public_key() const = 0;
    virtual wallet_core::internal::PrivateKeyPtr clone() const = 0;
};

struct MULTY_CORE_API PublicKey : public Key
{
    virtual ~PublicKey();
    virtual wallet_core::internal::PublicKeyPtr clone() const = 0;
};

#endif // WALLET_CORE_INTERNAL_KEY_H
