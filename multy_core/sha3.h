/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_CORE_SHA3_H
#define MULTY_CORE_SHA3_H

#include "multy_core/api.h"

#ifdef __cplusplus
extern "C" {
#endif

struct Error;
struct BinaryData;

// These are size in bytes to use with BinaryData.
enum Sha3HashSize
{
    SHA3_224 = 224 / 8,
    SHA3_256 = 256 / 8,
    SHA3_384 = 384 / 8,
    SHA3_512 = 512 / 8,
};

/** SHA3-Hash input and write result into output.
 *
 * Please note that hash size is determined by the output->len,
 * which is adjusted to reflect actual bytes written.
 *
 * If output->len >= 64 (which is 512/8), then SHA3-512 is done over input,
 * and output->len is set to 64.
 * @param input - input data to be hashed;
 * @param output - where to put hash, must not be nullptr;
 * @result nullptr if Ok, Error otherwise.
**/
MULTY_CORE_API struct Error* sha3(
        const struct BinaryData* input, struct BinaryData* output);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // MULTY_CORE_SHA3_H
