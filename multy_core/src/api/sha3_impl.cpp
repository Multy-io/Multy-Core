/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/src/api/sha3_impl.h"

#include "multy_core/common.h"
#include "multy_core/sha3.h"

#include "multy_core/src/exception.h"
#include "multy_core/src/exception_stream.h"
#include "multy_core/src/utility.h"

extern "C" {
#include "keccak-tiny/keccak-tiny.h"
} // extern "C"

#include <algorithm>
#include <iterator>
#include <unordered_map>

namespace
{
using namespace multy_core::internal;

// Byte-size.
static const size_t SHA3_HASH_BYTE_SIZES[] = {
    SHA3_224, SHA3_256, SHA3_384, SHA3_512
};

void do_sha3(size_t hash_bit_size, const BinaryData& input, BinaryData* output)
{
    static const std::unordered_map<size_t, decltype(&sha3_224)> SHA3_FUNCTIONS =
    {
        {224, &sha3_224},
        {256, &sha3_256},
        {384, &sha3_384},
        {512, &sha3_512},
    };

    const auto sha3_function = SHA3_FUNCTIONS.find(hash_bit_size);
    if (sha3_function == SHA3_FUNCTIONS.end())
    {
        THROW_EXCEPTION2(ERROR_INVALID_ARGUMENT,
                "Unsupported hash size for SHA3.")
                << " Size:" << hash_bit_size << ".";
    }

    THROW_IF_WALLY_ERROR(sha3_function->second(
                    const_cast<uint8_t*>(output->data), output->len,
                    input.data, input.len),
            "Failed to SHA3 input data.");
}

template <size_t N>
size_t get_biggest_of_supported_sizes(size_t size, const size_t (&supported_sizes)[N])
{
    const size_t default_value = std::numeric_limits<size_t>::max();
    const size_t hash_size = find_max_value(
            supported_sizes, default_value, size);
    if (hash_size == default_value)
    {
        THROW_EXCEPTION2(ERROR_INVALID_ARGUMENT,
                "Output BinaryData has not enough space available.")
                << " Need at least " << supported_sizes[0] << " bytes.";
    }

    return hash_size;
}

} // namespace

namespace multy_core
{
namespace internal
{

BinaryDataPtr sha3(const size_t hash_bit_size, const BinaryData& input)
{
    if (!contains(SHA3_HASH_BYTE_SIZES, hash_bit_size/8))
    {
        THROW_EXCEPTION2(ERROR_INVALID_ARGUMENT, "Unsupported hash_size.");
    }

    BinaryDataPtr result;

    throw_if_error(make_binary_data(hash_bit_size / 8, reset_sp(result)));
    do_sha3(hash_bit_size, input, result.get());

    return result;
}

BinaryDataPtr keccak_256(const BinaryData& input)
{
    BinaryDataPtr result;
    throw_if_error(make_binary_data(256 / 8, reset_sp(result)));
    keccak_256(input, result.get());

    return result;
}

void sha3(const BinaryData& input, BinaryData* output)
{
    INVARIANT(input.data != nullptr);
    INVARIANT(output != nullptr);

    const size_t hash_bit_size = get_biggest_of_supported_sizes(output->len , SHA3_HASH_BYTE_SIZES);
    do_sha3(hash_bit_size * 8, input, output);
    output->len = hash_bit_size;
}

void sha3(size_t hash_bit_size, const BinaryData& input, BinaryData* output)
{
    INVARIANT(input.data != nullptr);
    INVARIANT(output != nullptr);

    do_sha3(hash_bit_size, input, output);
}

void keccak_256(const BinaryData& input, BinaryData* output)
{
    INVARIANT(output != nullptr);
    INVARIANT(input.data != nullptr);

    const size_t HASH_SIZES[] = {256 / 8};
    const size_t hash_size = get_biggest_of_supported_sizes(output->len, HASH_SIZES);

    THROW_IF_WALLY_ERROR(
            ::keccak_256(const_cast<unsigned char*>(output->data), hash_size,
                    input.data, input.len),
            "Failed to hash input with keccack_256.");

    output->len = hash_size;
}

} // namespace internale
} // namespace multy_core
