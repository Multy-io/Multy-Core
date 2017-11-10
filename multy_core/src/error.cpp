/* Copyright 2017 by Multy.io
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

Error* make_error(ErrorCode code, const char* message)
{
    Error* result = new Error;
    result->message = message;
    result->code = code;
    result->owns_message = false;
    return result;
}

Error* internal_make_error(int code, const char* message)
{
    return make_error(convert_error_code(code), message);
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
