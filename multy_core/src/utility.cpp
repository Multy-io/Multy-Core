/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/src/utility.h"

#include "multy_core/error.h"

#include "multy_core/src/exception.h"
#include "multy_core/src/exception_stream.h"

#include "wally_core.h"

#include <cassert>
#include <string>
#include <string.h>

namespace
{
using namespace multy_core::internal;
} // namespace

namespace multy_core
{
namespace internal
{

char* copy_string(const std::string& str)
{
    return copy_string(str.c_str());
}

char* copy_string(const char* str)
{
    if (!str)
    {
        return nullptr;
    }

    const size_t len = strlen(str);

    wally_operations wally_ops;
    int result = wally_get_operations(&wally_ops);
    if (result != WALLY_OK || !wally_ops.malloc_fn)
    {
        THROW_EXCEPTION("Failed to copy string.");
    }

    char* new_message = static_cast<char*>(wally_ops.malloc_fn(len + 1));
    if (!new_message)
    {
        THROW_EXCEPTION("Failed to allocate memory.")
                << " Requested: " << len + 1;
    }

    memcpy(new_message, str, len);
    new_message[len] = '\0';
    return new_message;
}

} // namespace internal
} // namespace multy_core
