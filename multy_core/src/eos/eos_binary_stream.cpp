/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/src/eos/eos_binary_stream.h"

#include "multy_core/src/error_utility.h"
#include "multy_core/binary_data.h"
#include "multy_core/src/eos/eos_transaction_action.h"
#include "multy_core/src/eos/eos_name.h"
#include "multy_core/src/eos/eos_transaction.h"
#include "multy_core/src/eos/eos_transaction.h"


#include "third-party/portable_endian.h"

#include <array>

namespace multy_core
{
namespace internal
{

EosBinaryStream::EosBinaryStream()
{}

EosBinaryStream::~EosBinaryStream()
{}

EosBinaryStream& operator<<(EosBinaryStream& stream, const EosTransactionAction& op)
{
    op.write_to_stream(&stream);

    return stream;
}

EosBinaryStream& operator<<(EosBinaryStream& stream, const BinaryData& value)
{
    INVARIANT(value.data !=  nullptr);
    stream.write_data(value.data, value.len);

    return stream;
}

EosBinaryStream& operator<<(EosBinaryStream& stream, const uint8_t& value)
{
    stream.write_data(&value, 1);

    return stream;
}

EosBinaryStream& operator<<(EosBinaryStream& stream, const uint16_t& value)
{
    return write_as_data(htole16(value), stream);
}

EosBinaryStream& operator<<(EosBinaryStream& stream, const uint32_t& value)
{
    return write_as_data(htole32(value), stream);
}


EosBinaryStream& operator<<(EosBinaryStream& stream, const uint64_t& value)
{
    return write_as_data(htole64(value), stream);
}

EosBinaryStream& operator<<(EosBinaryStream& stream, const EosName& value)
{
    stream << value.get_data();

    return stream;
}

EosBinaryStream& operator<<(EosBinaryStream& stream, const EosAuthorization& value)
{
    stream << value.get_actor();
    stream << value.get_permission();

    return stream;
}

EosBinaryStream& operator<<(EosBinaryStream& stream, const EosChainId& value)
{
    stream.write_data(value.data(), value.size());

    return stream;
}
} // namespace internal
} // namespace multy_core

