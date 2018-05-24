#include "multy_core/src/codec.h"

#include "multy_core/src/exception.h"
#include "multy_core/src/exception_stream.h"
#include "multy_core/src/utility.h"

#include "wally_core.h"

extern "C" {
#include "libwally-core/src/ccan/ccan/str/hex/hex.h"
#include "libwally-core/src/base58.h"
#include "ccan/str/base32/base32.h"
} // extern "C"

namespace
{
using namespace multy_core::internal;

struct Codec
{
    std::string (*encode)(const BinaryData& binary);
    BinaryDataPtr (*decode)(const char* hex_str, size_t len);
};

std::string encodeHex(const BinaryData& binary)
{
    const size_t len = binary.len * 2;

    std::string result(len + 1, '\0');
    if (!hex_encode(binary.data, binary.len,
            const_cast<char*>(result.data()), result.size()))
    {
        THROW_EXCEPTION("Failed to encode binary to hex string.")
                << " Input binary size: " << binary.len << ".";
    }
    // Remove the trailing zero.
    result.resize(result.size() - 1);

    return result;
}

BinaryDataPtr decodeHex(const char* hex_str, size_t len)
{
    BinaryDataPtr result;

    if (len & 1)
    {
        THROW_EXCEPTION("Failed to decode hex-encoded string:"
                "invalid input length.")
                << " Expected length to be even, actual length: "
                << len << ".";
    }
    throw_if_error(make_binary_data(len / 2, reset_sp(result)));
    if (len > 0 && !hex_decode(hex_str, len,
            const_cast<unsigned char*>(result->data), result->len))
    {
        THROW_EXCEPTION("Failed to decode hex-encoded string.");
    }

    return result;
}

std::string encodeBase58(const BinaryData& binary)
{
    if (binary.len == 0)
    {
        return std::string();
    }

    CharPtr out_str;
    THROW_IF_WALLY_ERROR(
            wally_base58_from_bytes(
                    binary.data, binary.len, 0, reset_sp(out_str)),
            "Failed encode binary to base58 string.");

    return std::string(out_str.get());
}

BinaryDataPtr decodeBase58(const char* str, size_t len)
{
    BinaryDataPtr result;

    throw_if_error(make_binary_data(len, reset_sp(result)));
    if (len == 0)
    {
        return result;
    }

    size_t resulting_size = len;
    THROW_IF_WALLY_ERROR(
            base58_decode(str, len,
                    const_cast<unsigned char*>(result->data),
                    &resulting_size),
            "Failed to decode base58-encoded string.");
    result->len = resulting_size;

    return result;
}

std::string encodeBase32(const BinaryData& binary)
{
    INVARIANT(binary.data != nullptr);

    std::string result(base32_str_size(binary.len), '\0');

    if (!base32_encode(
            binary.data, binary.len,
            const_cast<char*>(result.data()), result.length()))
    {
            THROW_EXCEPTION("Failed encode binary to base32 string.");
    }
    // Remove the trailing zero.
    result.resize(result.size() - 1);

    return result;
}

BinaryDataPtr decodeBase32(const char* str, size_t len)
{
    BinaryDataPtr result;
    throw_if_error(make_binary_data(base32_data_size(str, len),
            reset_sp(result)));

    if (!base32_decode(str, len,
            const_cast<unsigned char*>(result->data),
            result->len))
    {
        THROW_EXCEPTION("Failed to decode base32-encoded string.");
    }

    return result;
}

Codec make_codec(CodecType codec_type)
{
    switch(codec_type)
    {
        case CODEC_HEX:
            return Codec{&encodeHex, &decodeHex};
        case CODEC_BASE58:
            return Codec{&encodeBase58, &decodeBase58};
        case CODEC_BASE32:
            return Codec{&encodeBase32, &decodeBase32};
        default:
            INVARIANT2(false && "Unknown codec type.", codec_type);
    }

    return Codec{nullptr, nullptr};
}

} // namespace

namespace multy_core
{
namespace internal
{

std::string encode(const BinaryData& data, CodecType codec_type)
{
    INVARIANT(data.data != nullptr);

    return make_codec(codec_type).encode(data);
}

BinaryDataPtr decode(const std::string& string, CodecType codec_type)
{
    return make_codec(codec_type).decode(string.c_str(), string.length());
}

BinaryDataPtr decode(const char* string, size_t len, CodecType codec_type)
{
    INVARIANT(string != nullptr);

    return make_codec(codec_type).decode(string, len);
}

} // namespace internal
} // namespace multy_core
