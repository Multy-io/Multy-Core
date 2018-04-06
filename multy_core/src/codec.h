/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_CORE_CODEC_H
#define MULTY_CORE_CODEC_H

#include "multy_core/api.h"

#include "multy_core/binary_data.h"
#include "multy_core/src/u_ptr.h"

#include <string>

namespace multy_core
{
namespace internal
{

enum CodecType
{
    CODEC_BASE58,
    CODEC_HEX
};

MULTY_CORE_API std::string encode(const BinaryData& data, CodecType);
MULTY_CORE_API BinaryDataPtr decode(const std::string& string, CodecType);
MULTY_CORE_API BinaryDataPtr decode(const char* string, size_t len, CodecType);

} // namespace internal
} // namespace multy_core

#endif // MULTY_CORE_CODEC_H
