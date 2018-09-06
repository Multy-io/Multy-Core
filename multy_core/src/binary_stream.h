/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_CORE_SRC_BINARY_STREAM_BASE_H
#define MULTY_CORE_SRC_BINARY_STREAM_BASE_H

#include <vector>
#include <cstddef>
#include <stdint.h>

struct BinaryData;

namespace multy_core
{
namespace internal
{

class BinaryStream
{
public:
    BinaryStream();
    ~BinaryStream();

    void write_data(const uint8_t* data, size_t len);
    BinaryData get_content() const;

private:
    std::vector<uint8_t> m_data;
};

} // namespace internal
} // namespace multy_core

#endif // MULTY_CORE_SRC_BINARY_STREAM_BASE_H
