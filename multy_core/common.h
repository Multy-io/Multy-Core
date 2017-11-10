/* Copyright 2017 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_CORE_COMMON_H
#define MULTY_CORE_COMMON_H

#include "multy_core/api.h"

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Entropy generator interface.
 * Fill `dest` with `size` random bytes.
 * Caller ensures that `dest` has enough space.
 * Implementation should return 0 on error, or size of generated entropy.
 */
struct EntropySource
{
    void* data; /** Opaque caller-supplied pointer, passed as first argument to
                 fill_entropy(). **/
    size_t (*fill_entropy)(void* data, size_t size, void* dest);
};

/** Binary data, just a pointer and a size in bytes. */
struct BinaryData
{
    const unsigned char* data;
    size_t len;
};

/** Frees BinaryData, can take null. **/
MULTY_CORE_API void free_binarydata(struct BinaryData*);

/** Frees a string, can take null. **/
MULTY_CORE_API void free_string(const char* str);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* MULTY_CORE_COMMON_H */
