/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_CORE_INTERNAL_KEY_IMPL_H
#define MULTY_CORE_INTERNAL_KEY_IMPL_H

#include "multy_core/api.h"
#include "multy_core/src/u_ptr.h"
#include "multy_core/src/object.h"

#include "wally_bip32.h"

#include <string>

struct BinaryData;
struct PrivateKey;
struct PublicKey;

struct MULTY_CORE_API ExtendedKey : public ::multy_core::internal::ObjectBase<ExtendedKey>
{
    ExtendedKey();

    std::string to_string() const;

    static const void* get_object_magic();
public:
    ext_key key;
};

struct MULTY_CORE_API Key : public ::multy_core::internal::ObjectBase<Key>
{
    Key();
    virtual ~Key();

    virtual std::string to_string() const = 0;

    static const void* get_object_magic();
};

struct MULTY_CORE_API PrivateKey : public Key
{
    virtual ~PrivateKey();

    virtual multy_core::internal::PublicKeyPtr make_public_key() const = 0;
    virtual multy_core::internal::PrivateKeyPtr clone() const = 0;
    virtual multy_core::internal::BinaryDataPtr sign(const BinaryData& data) const = 0;
};

struct MULTY_CORE_API PublicKey : public Key
{
    virtual ~PublicKey();
    virtual multy_core::internal::PublicKeyPtr clone() const = 0;
    virtual const BinaryData get_content() const = 0;
};

#endif // MULTY_CORE_INTERNAL_KEY_IMPL_H
