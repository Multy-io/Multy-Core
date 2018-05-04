/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/src/ec_key_utils.h"

#include "multy_core/src/exception.h"
#include "multy_core/src/utility.h"
#include "wally_crypto.h"

#include <array>
#include <string.h>

namespace
{

void copy_data(const BinaryData& source, BinaryData* dest)
{
    INVARIANT(source.len <= dest->len);

    memcpy(const_cast<uint8_t*>(dest->data), source.data, source.len);
    dest->len = source.len;
}

} // namespace

namespace multy_core
{
namespace internal
{

void ec_validate_private_key(const BinaryData& private_key_data)
{
    THROW_IF_WALLY_ERROR2(
            wally_ec_private_key_verify(private_key_data.data,
                    private_key_data.len),
            ERROR_KEY_CORRUPT,
            "Failed to verify private key.");
}

void ec_private_to_public_key(const BinaryData& private_key_data,
        PublicKeyFormat format,
        BinaryData* public_key_data)
{
    INVARIANT(public_key_data);

    std::array<uint8_t, EC_PUBLIC_KEY_LEN> key_data;
    THROW_IF_WALLY_ERROR2(
            wally_ec_public_key_from_private_key(
                    private_key_data.data, private_key_data.len,
                    key_data.data(), key_data.size()),
            ERROR_KEY_CANT_DERIVE_PUBLIC_KEY,
            "Failed to derive public key from private key.");

    if (format == EC_PUBLIC_KEY_COMPRESSED)
    {
        copy_data(as_binary_data(key_data), public_key_data);
        return;
    }

    std::array<uint8_t, EC_PUBLIC_KEY_UNCOMPRESSED_LEN> uncompressed;
    THROW_IF_WALLY_ERROR2(
            wally_ec_public_key_decompress(
                    key_data.data(), key_data.size(),
                    uncompressed.data(), uncompressed.size()),
            ERROR_KEY_CORRUPT,
            "Failed to uncompress public key from data.");

    copy_data(as_binary_data(uncompressed), public_key_data);
}

} // namespace internal
} // namespace multy_core
