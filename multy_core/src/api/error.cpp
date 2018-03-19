/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/error.h"

#include "multy_core/common.h"

#include "multy_core/src/error_utility.h"

#include <stdlib.h>

namespace
{
// Can't be const because make_error returns non-const Error.
Error OutOfMemoryError;

// Just in case, resetting OutOfMemory before passing it to the user.
Error* get_out_of_memory_error()
{
    OutOfMemoryError = Error
    {
        ERROR_OUT_OF_MEMORY,
        "Failed to allocate Error object.",
        false,
        MULTY_CODE_LOCATION
    };
    return &OutOfMemoryError;
}

} // namespace

Error* make_error(ErrorCode code, const char* message, CodeLocation location)
{
    message = message ? message : "";
    try
    {
        return new Error{code, message, false, location};
    }
    catch(...)
    {
        return get_out_of_memory_error();
    }
    return nullptr;
}

void free_error(Error* error)
{
    if (!error || error == &OutOfMemoryError)
    {
        return;
    }
    if (error->owns_message)
    {
        free_string(error->message);
    }
    delete error;
}
