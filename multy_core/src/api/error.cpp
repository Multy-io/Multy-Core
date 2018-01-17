/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/error.h"

#include "multy_core/common.h"

#include "wally_core.h"

#include <stdlib.h>

namespace
{

ErrorCode convert_error_code(int code)
{
    switch (code)
    {
        case WALLY_EINVAL:
            return ERROR_INTERNAL;
        case WALLY_ENOMEM:
            return ERROR_OUT_OF_MEMORY;
        case WALLY_OK:
        case WALLY_ERROR:
        default:
            return ERROR_GENERAL_ERROR;
    }
}

} // namespace

Error* make_error(ErrorCode code, const char* message, CodeLocation location)
{
    return new Error{code, message, false, location};
}

Error* internal_make_error(int code, const char* message, CodeLocation location)
{
    return make_error(convert_error_code(code), message, location);
}

void free_error(Error* error)
{
    if (!error)
    {
        return;
    }
    if (error->owns_message)
    {
        free_string(error->message);
    }
    // no need to delete error message, since it is always a static string.
    delete error;
}
