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
    ERROR_FEATURE_NOT_SUPPORTED,
    ERROR_FEATURE_NOT_IMPLEMENTED_YET
};

struct CodeLocation
{
    const char* file;
    int line;
};

/** Error
 * Holds information about error occured inside library.
 */
struct Error
{
    ErrorCode code;
    const char* message;
    bool owns_message;

    // Points to the location in code where error occured.
    struct CodeLocation location;
};

/** Allocates Error object, assumes that message is satic and shouldn't be copied. **/
MULTY_CORE_API struct Error* make_error(enum ErrorCode code, const char* message, struct CodeLocation location);

/** Frees Error object, can take nullptr. **/
MULTY_CORE_API void free_error(struct Error* error);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* MULTY_CORE_ERROR_H */
