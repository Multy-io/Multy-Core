/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_CORE_GOLOS_H
#define MULTY_CORE_GOLOS_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

enum GolosNetType
{
    GOLOS_NET_TYPE_MAINNET = 0,
    GOLOS_NET_TYPE_TESTNET = 1,
};

extern const char* GOLOS_TOKEN_NAME;
const size_t GOLOS_EXPIRE_MIN_SECONDS = 10;
const size_t GOLOS_VALUE_DECIMAL_PLACES = 3; // NUMBER OF digits right to decimal point.

const size_t GOLOS_ACCOUNT_MIN_LENGTH = 3;
const size_t GOLOS_ACCOUNT_MAX_LENGTH = 16;

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* MULTY_CORE_GOLOS_H */
