/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/src/binary_stream.h"

#include "multy_core/binary_data.h"

#include "multy_core/src/exception.h"
#include "multy_core/src/exception_stream.h"
#include "multy_core/src/utility.h"

#include <vector>

namespace multy_core
{
namespace internal
{

BinaryStream::BinaryStream()
    : m_data()
{
    m_data.reserve(256);
}

BinaryStream::~BinaryStream()
{}

void BinaryStream::write_data(const uint8_t* data, const size_t len)
{
    INVARIANT(data != nullptr);
    m_data.insert(m_data.end(), data, data + len);
}

BinaryData BinaryStream::get_content() const
{
    return as_binary_data(m_data);
}

} // namespace internal
} // namespace multy_core
