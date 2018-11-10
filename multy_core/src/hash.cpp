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

#include <array>
#include <initializer_list>
#include <string.h>

namespace
{
using namespace multy_core::internal;

class HasherBase : public Hasher
{
protected:
    explicit HasherBase(const char* hash_name, size_t hash_size, const std::initializer_list<size_t>& supported_sizes)
        : m_hash_size(hash_size)
    {
        if ( !contains(supported_sizes, hash_size) )
        {
            THROW_EXCEPTION("Unsupported hash size.")
                    << " For hash type: " << hash_name
                    << " Requested hash size in bytes: " << hash_size
                    << " Supported hash sizes: " << join(", ", supported_sizes) << ".";
        }
    }

    ~HasherBase() {}

    const size_t m_hash_size;
};

class Sha2Hasher : public HasherBase
{
    typedef int (*HashFunction)(const unsigned char *, size_t, unsigned char *, size_t);

public:
    Sha2Hasher(size_t hash_size)
        : HasherBase("SHA2", hash_size, {256, 512})
    {}

    void hash(const BinaryData& input, BinaryData* output) const override
    {
        INVARIANT(output);
        INVARIANT(output->len * 8 >= m_hash_size);

        const HashFunction hash_function = get_hash_function(m_hash_size);
        INVARIANT(hash_function != nullptr);

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

class KeccakHasher : public HasherBase
{
public:
    KeccakHasher(size_t hash_size)
        : HasherBase("Keccak", hash_size, {256})
    {}

    void hash(const BinaryData& input, BinaryData* output) const override
    {
        INVARIANT(output);
        INVARIANT(output->len * 8 >= m_hash_size);

        keccak_256(input, output);
    }
};

class Sha3Hasher : public HasherBase
{
public:
//    static const char* name = "Keccak";
//    static const std::array<size_t, 4> supported_sizes = {224, 256, 384, 512}

    explicit Sha3Hasher(size_t hash_size)
        : HasherBase("Keccak", hash_size, {224, 256, 384, 512})
    {}

    void hash(const BinaryData& input, BinaryData* output) const override
    {
        INVARIANT(output != nullptr);
        INVARIANT(output->len * 8 >= m_hash_size);

        sha3(input, output);
    }
};

class RipemdHasher : public HasherBase
{
public:
    explicit RipemdHasher(size_t hash_size)
        : HasherBase("RIPEMD", hash_size, {160})
    {}

    void hash(const BinaryData& input, BinaryData* output) const override
    {
        INVARIANT(output != nullptr);
        INVARIANT(output->len * 8 >= m_hash_size);

        struct ripemd160 ripemd;
        ripemd160(&ripemd, input.data, input.len);
        memcpy(const_cast<unsigned char*>(output->data), &ripemd, sizeof(ripemd));
    }
};

//template <typename HasherT>
//class DoubleHasher : public HasherBase
//{
//    using HasherBase::HasherBase;

//    void hash(const BinaryData& input, BinaryData* output) const override
//    {
//        INVARIANT(output != nullptr);
//        INVARIANT(output->len * 8 >= m_hash_size);

//        // Writing first hash into large enough tmp buffer
//        // to avoid modifying output if exception is thrown in the middle.

//        multy_core::internal::hash<1024> tmp_buffer;
//        BinaryData tmp_buffer_binary_data = as_binary_data(tmp_buffer);
//        INVARIANT(tmp_buffer_binary_data.len >= output->len);

//        tmp_buffer_binary_data.len = output->len;

//        HasherT hasher;
//        hasher.hash(input, &tmp_buffer_binary_data);
//        hasher.hash(tmp_buffer_binary_data, output);
//    }
//};

template <typename HasherT>
HasherPtr make_hasher(size_t hash_size)
{
    return HasherPtr(new HasherT(hash_size));
}

} // namespace

namespace multy_core
{
namespace internal
{

Hasher::~Hasher()
{}

HasherPtr make_hasher(HasherType hasher_type, size_t hash_size)
{
    switch(hasher_type)
    {
        case SHA2:
            return ::make_hasher<::Sha2Hasher>(hash_size);
//        case SHA2_DOUBLE:
//            return ::make_hasher<::DoubleHasher<::Sha2Hasher>>(hash_size);
        case SHA3:
            return ::make_hasher<::Sha3Hasher>(hash_size);
        case KECCAK:
            return ::make_hasher<::KeccakHasher>(hash_size);
        case RIPEMD:
            return ::make_hasher<::RipemdHasher>(hash_size);
        default:
            THROW_EXCEPTION("Unknown hasher type.")
                    << hasher_type;
    }
    return nullptr;
}

} // namespace internal
} // namespace multy_core
