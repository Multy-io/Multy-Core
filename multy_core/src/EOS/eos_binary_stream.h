/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_CORE_SRC_EOS_BINARY_STREAM_H
#define MULTY_CORE_SRC_EOS_BINARY_STREAM_H

#include "multy_core/src/binary_stream.h"

namespace multy_core
{
namespace internal
{

class EosBinaryStream : public BinaryStream
{
public:
    EosBinaryStream();
    ~EosBinaryStream();
};

} // namespace internal
} // namespace multy_core

#endif // MULTY_CORE_SRC_EOS_BINARY_STREAM_H
