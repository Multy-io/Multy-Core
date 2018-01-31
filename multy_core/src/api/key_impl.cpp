/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/src/api/key_impl.h"

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
