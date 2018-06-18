/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/src/hash.h"

#include "multy_core/src/api/sha3_impl.h"
#include "multy_core/src/exception.h"
#include "multy_core/src/exception_stream.h"
#include "multy_core/src/utility.h"

extern "C" {
#include "ccan/ccan/crypto/ripemd160/ripemd160.h"
} // extern "C"

#include "wally_crypto.h"

#include <string.h>

namespace
{
using namespace multy_core::internal;

class Sha2Hasher : public Hasher
{
    typedef int (*HashFunction)(const unsigned char *, size_t, unsigned char *, size_t);

public:
    void hash(const BinaryData& input, BinaryData* output) const override
    {
        assert(output);
        const HashFunction hash_function = get_hash_function(output->len * 8);
        assert(hash_function != nullptr);

        THROW_IF_WALLY_ERROR(
                hash_function(input.data, input.len,
                const_cast<unsigned char*>(output->data), output->len),
                "Failed to sha2-256 the input.");
    }

    static HashFunction get_hash_function(size_t size)
    {
        switch (size)
        {
            case 256:
                return &wally_sha256;
            case 512:
                return &wally_sha512;
            default:
                THROW_EXCEPTION("Can't do a sha-2 with requested hash size, "
                        "only 256 and 512 are supported.")
                        << " Requested hash size in bytes: " << size;
        }
    }
};

class KeccakHasher : public Hasher
{
public:
    void hash(const BinaryData& input, BinaryData* output) const override
    {
        assert(output);
        keccak_256(input, output);
    }
};

class Sha3Hasher : public Hasher
{
public:
    void hash(const BinaryData& input, BinaryData* output) const override
    {
        assert(output);
        sha3(input, output);
    }
};

class RipemdHasher : public Hasher
{
public:
    void hash(const BinaryData& input, BinaryData* output) const override
    {
        assert(output);
        if (output->len != 160 / 8)
        {
            THROW_EXCEPTION("Unsupported Ripemd hash size.")
                    << " Requested hash size: " << output->len;
        }

        struct ripemd160 ripemd;
        ripemd160(&ripemd, input.data, input.len);
        memcpy(const_cast<unsigned char*>(output->data), &ripemd, sizeof(ripemd));
    }
};

class BitcoinHasher : public Hasher
{
    void hash(const BinaryData& input, BinaryData* output) const override
    {
        assert(output);
        if (output->len != 160 / 8)
        {
            THROW_EXCEPTION("Unsupported Bitcoin hash size.")
                    << " Requested hash size: " << output->len;
        }

        THROW_IF_WALLY_ERROR(
                wally_hash160(
                        input.data, input.len,
                        const_cast<unsigned char*>(output->data), output->len),
                "hash160 failed.");
    }
};

template <typename HasherT>
class DoubleHasher : public Hasher
{
    void hash(const BinaryData& input, BinaryData* output) const override
    {
        assert(output);
        // Writing first hash into large enough tmp buffer
        // to avoid modifying output if exception is thrown in the middle.

        multy_core::internal::hash<1024> tmp_buffer;
        BinaryData tmp_buffer_binary_data = as_binary_data(tmp_buffer);
        assert(tmp_buffer_binary_data.len >= output->len);
        tmp_buffer_binary_data.len = output->len;

        HasherT hasher;
        hasher.hash(input, &tmp_buffer_binary_data);
        hasher.hash(tmp_buffer_binary_data, output);
    }
};

template <typename HasherT>
HasherPtr make_hasher()
{
    return HasherPtr(new HasherT);
}

} // namespace

namespace multy_core
{
namespace internal
{

Hasher::~Hasher()
{}

HasherPtr make_hasher(HasherType hasher_type, size_t /*size*/)
{
    switch(hasher_type)
    {
        case SHA2:
            return ::make_hasher<::Sha2Hasher>();
        case SHA2_DOUBLE:
            return ::make_hasher<::DoubleHasher<::Sha2Hasher>>();
        case SHA3:
            return ::make_hasher<::Sha3Hasher>();
        case KECCAK:
            return ::make_hasher<::KeccakHasher>();
        case RIPEMD:
            return ::make_hasher<::RipemdHasher>();
        case BITCOIN_HASH:
            return ::make_hasher<::BitcoinHasher>();
        default:
            THROW_EXCEPTION("Unknown hasher type.")
                    << hasher_type;
    }
    return nullptr;
}

} // namespace internal
} // namespace multy_core
