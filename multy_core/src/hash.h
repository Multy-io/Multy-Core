/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_CORE_SRC_HASH_H
#define MULTY_CORE_SRC_HASH_H

#include "multy_core/api.h"

#include "multy_core/common.h"
#include "multy_core/src/utility.h"

#include <array>
#include <memory>

struct BinaryData;

namespace multy_core
{
namespace internal
{

/// Stores product of hashing. Size in bits, like 256 or 512.
template <size_t N>
using hash = std::array<std::uint8_t, N/8>;

enum HasherType
{
    SHA2, // 256, 512 only
    SHA2_DOUBLE, // double hashing: sha2(sha2(data, size), size), same sizes as for SHA2
    SHA3, // 224, 256, 384, 512
    KECCAK, // 256 only
    RIPEMD, // 160 only
    BITCOIN_HASH, // 160 only
};

class Hasher;
typedef std::unique_ptr<Hasher> HasherPtr;

// TODO: implemenet progressive hashing, when not all data available at once.
class Hasher
{
public:
    virtual ~Hasher();
    virtual void hash(const BinaryData& input, BinaryData* output) const = 0;
};

MULTY_CORE_API HasherPtr make_hasher(HasherType hasher_type, size_t hash_bit_size);

// Hash given data with sepecific hasher and hash size.
template <HasherType HasherT, size_t hash_bit_size, typename T>
inline hash<hash_bit_size> do_hash(const T& input)
{
    hash<hash_bit_size> result;
    BinaryData result_data = as_binary_data(result);

    make_hasher(HasherT, hash_bit_size)->hash(as_binary_data(input), &result_data);

    return result;
}

// Hash given data and write a result into provided buffer starting from given offset.
// Please note that that buffer should be bigger than offset + N.
template <HasherType HasherT, size_t hash_bit_size, typename T, typename U>
inline void do_hash_inplace(const T& input, U* destination)
{
    INVARIANT(destination != nullptr);

    BinaryData dest = as_binary_data(*destination);
    make_hasher(HasherT, hash_bit_size)->hash(as_binary_data(input), &dest);
}

} // namespace internal
} // namespace multy_core

#endif // MULTY_CORE_SRC_HASH_H
