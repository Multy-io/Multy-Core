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
#include <sstream>

namespace
{
using namespace multy_core::internal;

// Byte-size.
static const size_t HASH_SIZES[] = {
    SHA3_224, SHA3_256, SHA3_384, SHA3_512
};

#define DO_SHA3(size, in, out)                                                 \
    THROW_IF_WALLY_ERROR(                                                      \
            ::sha3_##size(                                                     \
                    const_cast<uint8_t*>(output->data), output->len,           \
                    input.data, input.len),                                    \
            "Failed to SHA3-" #size " input data.");

void do_sha3(const BinaryData& input, BinaryData* output)
{
    switch (output->len * 8)
    {
        case 224:
        {
            DO_SHA3(224, input, output);
            break;
        }
        case 256:
        {
            DO_SHA3(256, input, output);
            break;
        }
        case 384:
        {
            DO_SHA3(384, input, output);
            break;
        }
        case 512:
        {
            DO_SHA3(512, input, output);
            break;
        }
        default:
        {
            THROW_EXCEPTION2(ERROR_INVALID_ARGUMENT,
                    "Unsupported hash size for SHA3.")
                    << " Size:" << output->len;
        }
    }
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

BinaryDataPtr sha3(const size_t hash_size, const BinaryData& input)
{
    if (std::find(std::begin(HASH_SIZES), std::end(HASH_SIZES), hash_size/8)
                    == std::end(HASH_SIZES))
    {
        THROW_EXCEPTION2(ERROR_INVALID_ARGUMENT, "Unsupported hash_size.");
    }

    BinaryDataPtr result;

    throw_if_error(make_binary_data(hash_size / 8, reset_sp(result)));
    do_sha3(input, result.get());

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
    INVARIANT(output != nullptr);
    INVARIANT(input.data != nullptr);

    // Copy to keep output intact in case of exception.
    BinaryData tmp = *output;
    tmp.len = get_biggest_of_supported_sizes(output->len , HASH_SIZES);

    do_sha3(input, &tmp);
    *output = tmp;
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
