/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_CORE_MNEMONIC_H
#define MULTY_CORE_MNEMONIC_H

#include "multy_core/api.h"
#include "multy_core/common.h"

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct Error;
struct BinayData;

/** Generate a mnemonic for given amount of entropy using default dictionary
 *(english).
 *
 * @param entropy_source - bytes of entropy.
 * @param [out]mnemonic - resulting mnemonic, a null-terminated, space-separated
 *	list of words from dictionary.
 */
MULTY_CORE_API struct Error* make_mnemonic(
        struct EntropySource entropy_source, const char** mnemonic);

/** Generates a pseudo-random seed from given mnemonic and password.
 * @param mnemonic - space-separated list of mnemonic words.
 * @param password - password, optional, can be null if not set.
 * @param [out]seed - resulting pseudo-random seed.
 */
MULTY_CORE_API struct Error* make_seed(
        const char* mnemonic, const char* password, struct BinaryData** seed);

MULTY_CORE_API struct Error* seed_to_string(
        const struct BinaryData* seed, const char** str);

MULTY_CORE_API struct Error* mnemonic_get_dictionary(
        const char** new_dictionary);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* MULTY_CORE_MNEMONIC_H */
