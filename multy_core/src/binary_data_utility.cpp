/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/src/utility.h"

#include "multy_core/error.h"

#include "multy_core/src/exception.h"
#include "multy_core/src/exception_stream.h"

#include <string.h>

namespace multy_core
{
namespace internal
{

BinaryData power_slice(const BinaryData& data, int32_t offset, int32_t size)
{
    if (offset < 0)
    {
        offset += data.len;
    }
    if (size < 0)
    {
        size += data.len;
    }
    if (offset < 0 || size < 0)
    {
        THROW_EXCEPTION("Can't power_slice BinaryData: either offset or size "
                "is too small.")
                << " data length: " << data.len
                << " offset: " << offset
                << " size: " << size;
    }

    return slice(data, offset, size);
}

BinaryData slice(const BinaryData& data, size_t offset, size_t size)
{
    if (!data.data)
    {
        THROW_EXCEPTION("Can't slice BinaryData: data is nullptr.");
    }
    if (offset > data.len)
    {
        THROW_EXCEPTION("Can't slice BinaryData: offset > data length.")
                << " data length: " << data.len
                << " offset: " << offset;
    }
    if (offset + size > data.len)
    {
        THROW_EXCEPTION("Can't slice BinaryData: offset + size > data length.")
                << " data length: " << data.len
                << " offset: " << offset
                << " size: " << size;
    }

    return BinaryData{data.data + offset, size};
}

BinaryData as_binary_data(const char* str)
{
    const size_t len = str ? strlen(str) : 0;
    return BinaryData{reinterpret_cast<const unsigned char*>(str), len};
}

bool operator==(const BinaryData& left, const BinaryData& right)
{
    return left.len == right.len
            && memcmp(left.data, right.data, left.len) == 0;
}

} // namespace internal

} // namespace multy_core
