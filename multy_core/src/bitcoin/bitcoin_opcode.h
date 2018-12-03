/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_CORE_BITCOIN_OP_CODE_H
#define MULTY_CORE_BITCOIN_OP_CODE_H

#include <stdint.h>

namespace multy_core
{
namespace internal
{
enum OP_CODE : uint8_t
{
    OP_FALSE = 0x00,
    OP_0 = 0x00,
    OP_RETURN = 0x6A,
    OP_DUP = 0x76,
    OP_HASH160 = 0xA9,
    OP_EQUALVERIFY = 0x88,
    OP_CHECKSIG = 0xAC,
    OP_EQUAL = 0x87
};
} // namespace internal
} // namespace multy_core

#endif // MULTY_CORE_BITCOIN_OP_CODE_H
