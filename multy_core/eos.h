/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_CORE_EOS_H
#define MULTY_CORE_EOS_H

#include <stddef.h>
#include <array>

#ifdef __cplusplus
extern "C" {
#endif

enum EosNetType
{
    EOS_NET_TYPE_MAINNET = 0,
    EOS_NET_TYPE_TESTNET = 1,
};

const size_t EOS_ADDRESS_MAX_SIZE = 12;
const uint8_t EOS_PRECISION = 4;
const std::array<uint8_t, 7> EOS_TOKEN_NAME = {0x45, 0x4f, 0x53, 0x00, 0x00, 0x00, 0x00};

enum EosTransactionBuilderType
{
    EOS_TRANSACTION_BUILDER_UPDATEAUTH,
    EOS_TRANSACTION_BUILDER_TRANSFER,
};

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* MULTY_CORE_EOS_H */
