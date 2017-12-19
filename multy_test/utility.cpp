/* Copyright 2017 by Multy.io
 * Licensed under Multy.io license
 *
 * See LICENSE for details
 */

#include "multy_test/utility.h"

#include "multy_core/common.h"
#include "multy_core/internal/key.h"
#include "wally_core.h"

#include <exception>

#include <memory.h>
#include <string.h>

namespace
{
using namespace wallet_core::internal;

size_t dummy_fill_entropy(void*, size_t size, void* dest)
{
    static const size_t entropy_max_size = 1024;
    unsigned char silly_entropy[entropy_max_size];

    if (size > entropy_max_size)
    {
        return 0;
    }

    memcpy(dest, silly_entropy, size);
    return size;
}

} // namespace

namespace test_utility
{

bytes from_hex(const char* hex_str)
{
    const size_t expected_size = strlen(hex_str) / 2;
    bytes result(expected_size);
    size_t bytes_written = 0;

    E(wally_hex_to_bytes(
            hex_str, result.data(), result.size(), &bytes_written));
    result.resize(bytes_written);

    return result;
}

std::string to_hex(const bytes& bytes)
{
    CharPtr hex_str;
    E(wally_hex_from_bytes(bytes.data(), bytes.size(), reset_sp(hex_str)));
    return std::string(hex_str.get());
}

std::string to_hex(const BinaryData& data)
{
    if (!data.data)
    {
        throw std::runtime_error("BinaryData::data is null");
    }

    CharPtr hex_str;
    E(wally_hex_from_bytes(data.data, data.len, reset_sp(hex_str)));
    return std::string(hex_str.get());
}

BinaryData to_binary_data(const bytes& data)
{
    return BinaryData{data.data(), data.size()};
}

BinaryData to_binary_data(const char* data)
{
    return BinaryData{
            reinterpret_cast<const unsigned char*>(data),
            data ? strlen(data) : 0
    };
}

ExtendedKey make_dummy_extended_key()
{
    ExtendedKey result;
    memset(&result.key, 0, sizeof(result.key));
    return result;
}

ExtendedKeyPtr make_dummy_extended_key_ptr()
{
    return ExtendedKeyPtr(new ExtendedKey(make_dummy_extended_key()));
}

EntropySource make_dummy_entropy_source()
{
    return EntropySource{nullptr, dummy_fill_entropy};
}

void throw_exception(const char* message)
{
    throw std::runtime_error(message);
}

} // namespace test_utility

bool operator==(const BinaryData& lhs, const BinaryData& rhs)
{
    return lhs.len == rhs.len && memcmp(lhs.data, rhs.data, lhs.len) == 0;
}

bool operator==(const PublicKey& lhs, const PublicKey& rhs)
{
    return lhs.get_content() == rhs.get_content();
}

bool operator==(const PrivateKey& lhs, const PrivateKey& rhs)
{
    return lhs.to_string() == rhs.to_string();
}
