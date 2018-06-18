/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/src/bitcoin/bitcoin_stream.h"

#include "multy_core/binary_data.h"
#include "multy_core/src/error_utility.h"
#include "multy_core/src/hash.h"
#include "multy_core/src/api/big_int_impl.h"
#include "multy_core/src/api/key_impl.h"
#include "multy_core/src/bitcoin/bitcoin_transaction_base.h"
#include "multy_core/src/bitcoin/bitcoin_transaction.h"

#include "wally_crypto.h"

#include "third-party/portable_endian.h"

#include <vector>
#include <string>


namespace multy_core
{
namespace internal
{
BitcoinStream::~BitcoinStream()
{
}

BitcoinDataStream::BitcoinDataStream()
    : m_data()
{}

BitcoinDataStream& BitcoinDataStream::write_data(const uint8_t* data, uint32_t len)
{
    INVARIANT(data != nullptr);

    m_data.insert(m_data.end(), data, data + len);

    return *this;
}

BinaryData BitcoinDataStream::get_content() const
{
    return BinaryData{m_data.data(), m_data.size()};
}

BitcoinStream& BitcoinBytesCountStream::write_data(const uint8_t* /*data*/, uint32_t len)
{
    bytes_count += len;
    return *this;
}

size_t BitcoinBytesCountStream::get_bytes_count() const
{
    return bytes_count;
}

void write_compact_size(uint64_t size, BitcoinStream* stream)
{
    if (size < 253)
    {
        *stream << static_cast<uint8_t>(size);
    }
    else if (size <= std::numeric_limits<uint16_t>::max())
    {
        *stream << static_cast<uint8_t>(253);
        *stream << static_cast<uint16_t>(size);
    }
    else if (size <= std::numeric_limits<uint32_t>::max())
    {
        *stream << static_cast<uint8_t>(254);
        *stream << static_cast<uint32_t>(size);
    }
    else
    {
        *stream << static_cast<uint8_t>(255);
        *stream << size;
    }
}

template <typename T>
BitcoinStream& write_as_data(const T& data, BitcoinStream& stream)
{
    return stream.write_data(
            reinterpret_cast<const uint8_t*>(&data), sizeof(data));
}

BitcoinStream& operator<<(BitcoinStream& stream, uint8_t data)
{
    return write_as_data(data, stream);
}

BitcoinStream& operator<<(BitcoinStream& stream, OP_CODE data)
{
    return write_as_data(data, stream);
}

BitcoinStream& operator<<(BitcoinStream& stream, uint16_t data)
{
    return write_as_data(htole16(data), stream);
}

BitcoinStream& operator<<(BitcoinStream& stream, uint32_t data)
{
    return write_as_data(htole32(data), stream);
}

BitcoinStream& operator<<(BitcoinStream& stream, uint64_t data)
{
    return write_as_data(htole64(data), stream);
}

BitcoinStream& operator<<(BitcoinStream& stream, int8_t data)
{
    return write_as_data(data, stream);
}

BitcoinStream& operator<<(BitcoinStream& stream, int16_t data)
{
    return write_as_data(htole16(data), stream);
}

BitcoinStream& operator<<(BitcoinStream& stream, int32_t data)
{
    return write_as_data(htole32(data), stream);
}

BitcoinStream& operator<<(BitcoinStream& stream, int64_t data)
{
    return write_as_data(htole64(data), stream);
}

BitcoinStream& operator<<(BitcoinStream& stream, const BigInt& data)
{
    // Any BigInt we are going to serialize to transaction is non-negative.
    return stream << data.get_value_as_uint64();
}

BitcoinStream& operator<<(BitcoinStream& stream, const BinaryData& data)
{
    return stream.write_data(
            reinterpret_cast<const uint8_t*>(data.data), data.len);
}

BitcoinStream& operator<<(BitcoinStream& stream, const PublicKey& key)
{
    const BinaryData& key_data = key.get_content();
    return stream << as_compact_size(key_data.len) << key_data;
}

ReversedBinaryData reverse(const BinaryData& data)
{
    return ReversedBinaryData{data};
}

BitcoinStream& operator<<(BitcoinStream& stream, const ReversedBinaryData& data)
{
    for (int i = data.data.len - 1; i >= 0; --i)
    {
        stream << data.data.data[i];
    }
    return stream;
}

BitcoinStream& operator<<(BitcoinStream& stream, const BitcoinTransactionSource& source)
{
    stream << reverse(**source.prev_transaction_hash);
    stream << source.prev_transaction_out_index;

    if (source.script_signature)
    {
        stream << as_compact_size(source.script_signature->len);
        stream << *source.script_signature;
    }
    else
    {
        stream << as_compact_size(0);
    }

    stream << *source.sequence;

    return stream;
}

BitcoinStream& operator<<(BitcoinStream& stream, const BitcoinTransactionDestinationBase& destination)
{
    INVARIANT(destination.sig_script != nullptr);

    stream << destination.amount;
    stream << as_compact_size(destination.sig_script->len);
    stream << *destination.sig_script;

    return stream;
}

//BitcoinStream& operator<<(BitcoinStream& stream, const BitcoinTransactionSegWitDestination& destination)
//{
//    INVARIANT(destination.sig_script != nullptr);

//    stream << destination.amount;
//    stream << as_compact_size(destination.sig_script->len);
//    stream << *destination.sig_script;

//    return stream;
//}

}
}
