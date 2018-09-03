/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_CORE_SRC_EOS_BINARY_STREAM_H
#define MULTY_CORE_SRC_EOS_BINARY_STREAM_H

#include "multy_core/src/binary_stream.h"

#include <array>

namespace multy_core
{
namespace internal
{

class EosTransactionAction;
class EosName;
class EosAuthorization;
typedef std::array<uint8_t, 32> EosChainId;

class EosBinaryStream : public BinaryStream
{
public:
    EosBinaryStream();
    ~EosBinaryStream();
};

template <typename T>
EosBinaryStream& write_as_data(const T& data, EosBinaryStream& stream)
{
    stream.write_data(
                reinterpret_cast<const uint8_t*>(&data), sizeof(data));
    return stream;
}

EosBinaryStream& operator<<(EosBinaryStream& stream, const EosTransactionAction& op);
EosBinaryStream& operator<<(EosBinaryStream& stream, const BinaryData& value);
EosBinaryStream& operator<<(EosBinaryStream& stream, const uint8_t& value);
EosBinaryStream& operator<<(EosBinaryStream& stream, const uint16_t& value);
EosBinaryStream& operator<<(EosBinaryStream& stream, const uint32_t& value);
EosBinaryStream& operator<<(EosBinaryStream& stream, const uint64_t& value);
EosBinaryStream& operator<<(EosBinaryStream& stream, const EosName& value);
EosBinaryStream& operator<<(EosBinaryStream& stream, const EosAuthorization& value);
EosBinaryStream& operator<<(EosBinaryStream& stream, const EosChainId& value);

} // namespace internal
} // namespace multy_core

#endif // MULTY_CORE_SRC_EOS_BINARY_STREAM_H
