/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_CORE_INTERNAL_SHA3_H
#define MULTY_CORE_INTERNAL_SHA3_H

#include "multy_core/api.h"

#include "multy_core/internal/u_ptr.h"

struct BinaryData;

namespace wallet_core
{
namespace internal
{

// These functions throw exceptions if anything goes wrong.

MULTY_CORE_API BinaryDataPtr sha3(size_t hash_size, const BinaryData& input);

// inplace, size of the hash is dictated by the output->len,
// the maximum size that would fit in the output is taken,
// and then output->len is updated to reflect acutal size.
MULTY_CORE_API void sha3(const BinaryData& input, BinaryData* output);

} // namespace internale
} // namespace wallet_core

#endif // MULTY_CORE_INTERNAL_SHA3_H
