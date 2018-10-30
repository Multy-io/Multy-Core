/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_CORE_BITCOIN_STREAM_H
#define MULTY_CORE_BITCOIN_STREAM_H

#include "multy_core/src/bitcoin/bitcoin_opcode.h"
#include "multy_core/src/api/properties_impl.h"

#include <string>
#include <vector>

struct BinaryData;
struct PublicKey;
struct BigInt;

namespace multy_core
{
namespace internal
{
class BitcoinTransactionSource;
class BitcoinTransactionDestination;

class BitcoinStream
{
public:
    virtual ~BitcoinStream()
    {
    }

    virtual BitcoinStream& write_data(const uint8_t* data, uint32_t len) = 0;
};

class BitcoinDataStream : public BitcoinStream
{
public:
    BitcoinDataStream();
    BitcoinDataStream& write_data(const uint8_t* data, uint32_t len) override;
    BinaryData get_content() const;
private:
    std::vector<uint8_t> m_data;
};

// Does no writing, only counting how many bytes would have been written.
class BitcoinBytesCountStream : public BitcoinStream
{
public:
    BitcoinStream& write_data(const uint8_t* /*data*/, uint32_t len);
    size_t get_bytes_count() const;
private:
    size_t bytes_count = 0;
};

struct ReversedBinaryData
{
    const BinaryData& data;
};

template <typename T>
struct CompactSizeWrapper
{
    const T& value;
};


ReversedBinaryData reverse(const BinaryData& data);

template <typename T>
CompactSizeWrapper<T> as_compact_size(const T& value);

template <typename T>
BitcoinStream& operator<<(BitcoinStream& stream, const PropertyT<T>& value);

template <typename T>
BitcoinStream& operator<<(
        BitcoinStream& stream, const CompactSizeWrapper<T>& value);
BitcoinStream& operator<<(BitcoinStream& stream, const ReversedBinaryData& data);
BitcoinStream& operator<<(BitcoinStream& stream, const PublicKey& key);
BitcoinStream& operator<<(BitcoinStream& stream, const std::string& str);
BitcoinStream& operator<<(BitcoinStream& stream, const BinaryData& data);
BitcoinStream& operator<<(BitcoinStream& stream, const BigInt& data);
BitcoinStream& operator<<(BitcoinStream& stream, OP_CODE data);
BitcoinStream& operator<<(BitcoinStream& stream, int8_t data);
BitcoinStream& operator<<(BitcoinStream& stream, int16_t data);
BitcoinStream& operator<<(BitcoinStream& stream, int32_t data);
BitcoinStream& operator<<(BitcoinStream& stream, int64_t data);
BitcoinStream& operator<<(BitcoinStream& stream, uint8_t data);
BitcoinStream& operator<<(BitcoinStream& stream, uint16_t data);
BitcoinStream& operator<<(BitcoinStream& stream, uint32_t data);
BitcoinStream& operator<<(BitcoinStream& stream, uint64_t data);
BitcoinStream& operator<<(
        BitcoinStream& stream, const BitcoinTransactionSource& source);
BitcoinStream& operator <<(BitcoinStream& stream, const BitcoinTransactionDestination& destination);

} // internal
} // multy_core
#endif // MULTY_CORE_BITCOIN_STREAM_H
