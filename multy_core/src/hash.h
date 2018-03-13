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

// TODO: implemenet progressive hashing, when not all data available at once.
class Hasher
{
public:
    virtual ~Hasher();
    virtual void hash(const BinaryData& input, BinaryData* output) const = 0;
};

typedef std::unique_ptr<Hasher> HasherPtr;
enum HasherType
{
    SHA2, // 256, 512 only
    SHA2_DOUBLE, // double hashing: sha2(sha2(data, size), size), same sizes as for SHA2
    SHA3, // 224, 256, 384, 512
    KECCAK, // 256 only
    RIPEMD, // 160 only
};

MULTY_CORE_API HasherPtr make_hasher(HasherType hasher_type, size_t size);

// Hash given data with sepecific hasher and hash size.
template <HasherType HasherT, size_t N, typename T>
inline hash<N> do_hash(const T& input)
{
    HasherPtr hashser = make_hasher(HasherT, N);
    hash<N> result;
    BinaryData result_data = as_binary_data(result);
    hashser->hash(as_binary_data(input), &result_data);

    return result;
}

} // namespace internal
} // namespace multy_core

#endif // MULTY_CORE_SRC_HASH_H
