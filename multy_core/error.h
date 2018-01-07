/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_CORE_ERROR_H
#define MULTY_CORE_ERROR_H

#include "multy_core/api.h"

#ifdef __cplusplus
extern "C" {
#endif

enum ErrorCode
{
    ERROR_INTERNAL,
    ERROR_INVALID_ARGUMENT,
    ERROR_OUT_OF_MEMORY,
    ERROR_GENERAL_ERROR,
    ERROR_BAD_ENTROPY,
};

/** Error
 * Holds information about error occured inside library.
 */
struct Error
{
    ErrorCode code;
    const char* message;
    bool owns_message;
};

/** Allocates Error object.**/
MULTY_CORE_API struct Error* make_error(enum ErrorCode code, const char* message);

/** Frees Error object, can take nullptr. **/
MULTY_CORE_API void free_error(struct Error* error);

MULTY_CORE_API struct Error* internal_make_error(int code, const char* message);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* MULTY_CORE_ERROR_H */
