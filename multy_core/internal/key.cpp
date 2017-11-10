/* Copyright 2017 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/internal/key.h"

#include "multy_core/internal/u_ptr.h"
#include "multy_core/internal/utility.h"

#include "wally_bip32.h"
#include "wally_core.h"

std::string ExtendedKey::to_string() const
{
    using namespace wallet_core::internal;

    unsigned char serialized_key[BIP32_SERIALIZED_LEN] = {'\0'};
    throw_if_wally_error(
            bip32_key_serialize(
                    &key, 0, serialized_key, sizeof(serialized_key)),
            "Failed to searialize ExtendedKey");
    CharPtr out_str;
    throw_if_wally_error(
            wally_base58_from_bytes(
                    serialized_key, sizeof(serialized_key),
                    BASE58_FLAG_CHECKSUM, reset_sp(out_str)),
            "Failed to convert serialized ExtendedKey to string");
    return std::string(out_str.get());
}

Key::~Key()
{
}

PrivateKey::~PrivateKey()
{
}

PublicKey::~PublicKey()
{
}
