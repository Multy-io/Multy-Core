/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/src/utility.h"

#include "multy_core/blockchain.h"
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

    INVARIANT(result == WALLY_OK);
    INVARIANT(wally_ops.malloc_fn != nullptr);

    char* new_message = static_cast<char*>(wally_ops.malloc_fn(len + 1));
    if (!new_message)
    {
        // TODO: any better way for handling out of memory,
        // cause throwing an exception, in fact, allocates more memory.
        THROW_EXCEPTION2(ERROR_OUT_OF_MEMORY, "Failed to allocate memory.")
                << " Requested: " << len + 1;
    }

    memcpy(new_message, str, len);
    new_message[len] = '\0';
    return new_message;
}

bool operator==(const BlockchainType& left, const BlockchainType& right)
{
    return left.blockchain == right.blockchain
            && left.net_type == right.net_type;
}

void trim_excess_trailing_null(std::string* str)
{
    auto pos = str->find_last_not_of('\0');
    if (pos != std::string::npos)
    {
        str->erase(pos + 1);
    }
}

} // namespace internal
} // namespace multy_core
