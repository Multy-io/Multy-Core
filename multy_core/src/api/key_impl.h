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

const unsigned char MULTY_USER_ID_VERSION = 0;
const uint32_t HARDENED_INDEX_BASE = 0x80000000;
inline constexpr uint32_t hardened_index(uint32_t index)
{
    return index | HARDENED_INDEX_BASE;
}

// 0x4d554c5459 is hex-encoded MULTY, but it doesn't fit into 32 bits,
// hence trimming the last byte.
const uint32_t MULTY_USER_ID_PURPOSE = hardened_index(0x4d554c54);

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
    typedef multy_core::internal::BinaryDataPtr BinaryDataPtr;
    typedef multy_core::internal::PrivateKeyPtr PrivateKeyPtr;
    typedef multy_core::internal::PublicKeyPtr PublicKeyPtr;

    virtual ~PrivateKey();
    virtual PublicKeyPtr make_public_key() const = 0;
    virtual PrivateKeyPtr clone() const = 0;
    virtual BinaryDataPtr sign(const BinaryData& data) const = 0;
};

struct MULTY_CORE_API PublicKey : public Key
{
    typedef multy_core::internal::PublicKeyPtr PublicKeyPtr;

    virtual ~PublicKey();
    virtual multy_core::internal::PublicKeyPtr clone() const = 0;
    virtual const BinaryData get_content() const = 0;
};

namespace multy_core
{
namespace internal
{

ExtendedKeyPtr make_master_key(const BinaryData& seed);

ExtendedKeyPtr make_child_key(const ExtendedKey& parent_key,
        uint32_t chain_code);

/** Makes a user id string from master key.
 *
 * User id is a string that uniquely identifies user without giving away
 * any details about master key.
 * @param master_key - key that was created from binary seed.
 * @throws Exception if master_key is not a root of the HD key tree or
 *          something else gone wrong.
 */
CharPtr make_user_id_from_master_key(const ExtendedKey& master_key);

} // namespace internal
} // namespace multy_core

#endif // MULTY_CORE_INTERNAL_KEY_IMPL_H
