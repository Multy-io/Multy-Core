/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_ETHEREUM_STREAM_H
#define MULTY_ETHEREUM_STREAM_H

#include "multy_core/src/u_ptr.h"

#include <vector>
#include <array>

namespace multy_core
{
namespace internal
{
class EthereumAddress;

const size_t ETH_METHOD_HASH_SIZE = 4;
typedef std::array<uint8_t, ETH_METHOD_HASH_SIZE> EthereumContractMethodHash;

EthereumContractMethodHash make_method_hash(const char* methodSignature);

struct EthereumPayloadDataStream
{
    EthereumPayloadDataStream();
    ~EthereumPayloadDataStream();
    void write_data(const void* data, size_t size);
    BinaryData get_content() const;

protected:
    std::vector<uint8_t> m_data;
};

struct EthereumPayloadAsUint256Stream
{
    EthereumPayloadAsUint256Stream();
    ~EthereumPayloadAsUint256Stream();
    void write_data(const void* data, size_t size);
    BinaryData get_content() const;

protected:
    std::vector<uint8_t> m_data;
};

EthereumPayloadDataStream& operator<<(EthereumPayloadDataStream& stream, const EthereumContractMethodHash& value);
EthereumPayloadDataStream& operator<<(EthereumPayloadDataStream& stream, const EthereumPayloadAsUint256Stream& value);
EthereumPayloadAsUint256Stream& operator<<(EthereumPayloadAsUint256Stream& stream, const BigInt& value);
EthereumPayloadAsUint256Stream& operator<<(EthereumPayloadAsUint256Stream& stream, const BinaryData& value);
EthereumPayloadAsUint256Stream& operator<<(EthereumPayloadAsUint256Stream& stream, const EthereumAddress& value);

} // interanl
} // namespace multy_core

#endif // MULTY_ETHEREUM_STREAM_H
