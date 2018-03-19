/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_CORE_BINARY_DATA_H
#define MULTY_CORE_BINARY_DATA_H

#include "multy_core/api.h"

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

struct Error;

/** Binary data, just a pointer and a size in bytes. */
struct BinaryData
{
    const unsigned char* data;
    size_t len;
};

/** Frees BinaryData, can take null. **/
MULTY_CORE_API void free_binarydata(struct BinaryData*);

/** Create new BinaryData with data of given size, data is zeroed. **/
MULTY_CORE_API struct Error* make_binary_data(
        size_t size, struct BinaryData** new_binary_data);

MULTY_CORE_API struct Error* make_binary_data_from_bytes(
        const unsigned char* data, size_t size,
        struct BinaryData** new_binary_data);

MULTY_CORE_API struct Error* make_binary_data_from_hex(
        const char* hex_str, struct BinaryData** new_binary_data);

/** Copies BinaryData. **/
MULTY_CORE_API struct Error* make_binary_data(
        size_t size, struct BinaryData** new_binary_data);

MULTY_CORE_API struct Error* make_binary_data_from_bytes(
        const unsigned char* data, size_t size,
        struct BinaryData** new_binary_data);

MULTY_CORE_API struct Error* make_binary_data_from_hex(
        const char* hex_str, struct BinaryData** new_binary_data);

/** Copies BinaryData. **/
MULTY_CORE_API struct Error* make_binary_data_clone(
        const struct BinaryData* source, struct BinaryData** new_binary_data);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // MULTY_CORE_BINARY_DATA_H
