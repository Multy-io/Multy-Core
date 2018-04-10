/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/src/api/key_impl.h"

#include "multy_core/src/exception.h"
#include "multy_core/src/hash.h"
#include "multy_core/src/u_ptr.h"
#include "multy_core/src/utility.h"

#include "wally_bip32.h"
#include "wally_core.h"

#include <cstring>

namespace
{
ext_key get_default_ext_key()
{
    ext_key result;
    memset(&result, 0, sizeof(result));
    return result;
}
} // namespace

ExtendedKey::ExtendedKey()
    : key(get_default_ext_key())
{}

std::string ExtendedKey::to_string() const
{
    using namespace multy_core::internal;

    unsigned char serialized_key[BIP32_SERIALIZED_LEN] = {'\0'};
    THROW_IF_WALLY_ERROR(
            bip32_key_serialize(
                    &key, 0, serialized_key, sizeof(serialized_key)),
            "Failed to searialize ExtendedKey");
    CharPtr out_str;
    THROW_IF_WALLY_ERROR(
            wally_base58_from_bytes(
                    serialized_key, sizeof(serialized_key),
                    BASE58_FLAG_CHECKSUM, reset_sp(out_str)),
            "Failed to convert serialized ExtendedKey to string");
    return std::string(out_str.get());
}

const void* ExtendedKey::get_object_magic()
{
    RETURN_MAGIC();
}

Key::Key()
{}

Key::~Key()
{
}

const void* Key::get_object_magic()
{
    RETURN_MAGIC();
}

PrivateKey::~PrivateKey()
{
}

PublicKey::~PublicKey()
{
}

namespace multy_core
{
namespace internal
{

ExtendedKeyPtr make_master_key(const BinaryData& seed)
{
    ExtendedKeyPtr key(new ExtendedKey);
    const int result = bip32_key_from_seed(
            seed.data, seed.len, BIP32_VER_MAIN_PRIVATE, 0, &key->key);

    if (result == WALLY_ERROR)
    {
        THROW_EXCEPTION2(
                ERROR_BAD_ENTROPY,
                "Can't generate master key with given entropy.");
    }
    THROW_IF_WALLY_ERROR(result, "Failed to generate master key.");

    return key;
}

ExtendedKeyPtr make_child_key(
        const ExtendedKey& parent_key,
        uint32_t chain_code)
{
    ExtendedKeyPtr child_key(new ExtendedKey);
    THROW_IF_WALLY_ERROR(
            bip32_key_from_parent(
                    &parent_key.key, chain_code, BIP32_FLAG_KEY_PRIVATE,
                    &child_key->key),
            "Failed to make child key");

    return child_key;
}

CharPtr make_user_id_from_master_key(const ExtendedKey& master_key)
{
    if (!(master_key.key.depth == 0 && master_key.key.child_num == 0))
    {
        THROW_EXCEPTION("Can't create a user id from non-master key.");
    }

    const ExtendedKeyPtr multy_user_id_key = make_child_key(master_key,
            MULTY_USER_ID_PURPOSE);

    const auto& pub_key = multy_user_id_key->key.pub_key;
    const auto hash = do_hash<SHA3, 256>(do_hash<SHA3, 256>(pub_key));

    std::array<uint8_t, sizeof(hash) + 1> user_id;
    memcpy(user_id.data() + 1, hash.data(), hash.size());
    user_id[0] = MULTY_USER_ID_VERSION;

    CharPtr out_id;
    THROW_IF_WALLY_ERROR(wally_hex_from_bytes(user_id.data(), user_id.size(),
            reset_sp(out_id)),
            "Failed to create user id.");

    return out_id;
}

} // namespace internal
} // namespace multy_core
