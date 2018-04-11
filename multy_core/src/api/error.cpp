/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/error.h"

#include "multy_core/common.h"

#include "multy_core/src/backtrace.h"
#include "multy_core/src/error_utility.h"
#include "multy_core/src/utility.h"
#include "multy_core/src/u_ptr.h"

#include <stdlib.h>

namespace
{
using namespace multy_core::internal;

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
        MULTY_CODE_LOCATION,
        nullptr
    };
    return &OutOfMemoryError;
}

} // namespace

Error* make_error(ErrorCode code, const char* message, CodeLocation location)
{
    return make_error_with_backtrace(
            code, message, location, get_error_backtrace(1).c_str());
}

Error* make_error_with_backtrace(
        ErrorCode code,
        const char* message,
        CodeLocation location,
        const char* backtrace)
{
    message = message ? message : "";
    try
    {
        CharPtr backtrace_ptr{copy_string(backtrace)};
        Error* result = new Error{
                code, message, false, location, backtrace_ptr.get()};
        backtrace_ptr.release();

        return result;
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
    free_string(error->backtrace);

    delete error;
}
