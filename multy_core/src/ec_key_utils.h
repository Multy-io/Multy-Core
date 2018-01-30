/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_CORE_EC_KEY_UTILS_H
#define MULTY_CORE_EC_KEY_UTILS_H

#include "multy_core/src/u_ptr.h"

struct BinaryData;

namespace multy_core
{
namespace internal
{

enum PublicKeyFormat
{
    EC_PUBLIC_KEY_UNCOMPRESSED,
    EC_PUBLIC_KEY_COMPRESSED
};

void ec_validate_private_key(const BinaryData& data);
void ec_private_to_public_key(const BinaryData& private_key_data,
        PublicKeyFormat format,
        BinaryData* public_key_data);

} // namespace internal
} // namespace multy_core

#endif // MULTY_CORE_EC_KEY_UTILS_H
