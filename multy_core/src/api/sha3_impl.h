/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_CORE_SRC_API_SHA3_IMPL_H
#define MULTY_CORE_SRC_API_SHA3_IMPL_H

#include "multy_core/api.h"

#include "multy_core/src/u_ptr.h"

struct BinaryData;

namespace multy_core
{
namespace internal
{

// These functions throw exceptions if anything goes wrong.

/** FIPS202 SHA-3 does hashing of the input producing hash of desired size.
 *
 * @param hash_size - size of the resulting hash in bits, one of [224, 256, 384, 512].
 * @param input - data to hash.
 * @return hash of the input, len is equal to hash_size/8.
 * @throws Exception if input.data is null or hash_size is not
 */
MULTY_CORE_API BinaryDataPtr sha3(size_t hash_bit_size, const BinaryData& input);

// inplace, size of hash is set explicitly with hash_size.
MULTY_CORE_API void sha3(size_t hash_bit_size, const BinaryData& input, BinaryData* output);

// inplace, size of the hash is dictated by the output->len,
// the maximum size that would fit in the output is taken,
// and then output->len is updated to reflect acutal size.
MULTY_CORE_API void sha3(const BinaryData& input, BinaryData* output);


/** Ethereum SHA-3 variant.
 *
 * 256-bits hash, since that is the only hash size we use in out Ethereum code.
 * @param input - data to hash;
 * @return - hash of the input, it's len is 256 bits (32 bytes).
 * @throws Exception if input->data is null or some other error occurred.
 */
MULTY_CORE_API BinaryDataPtr keccak_256(const BinaryData& input);

MULTY_CORE_API void keccak_256(const BinaryData& input, BinaryData* output);

} // namespace internale
} // namespace multy_core

#endif // MULTY_CORE_SRC_API_SHA3_IMPL_H
