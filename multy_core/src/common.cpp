/* Copyright 2017 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/common.h"

#include "multy_core/internal/utility.h"
#include "multy_core/internal/u_ptr.h"

#include "wally_core.h"

#include <string.h>

namespace
{
using namespace wallet_core::internal;
}

Error* binary_data_clone(const BinaryData* source, BinaryData** new_binary_data)
{
    ARG_CHECK(source);
    ARG_CHECK(new_binary_data);

    try
    {
        std::unique_ptr<unsigned char[]> data(new unsigned char[source->len]);
        memcpy(data.get(), source->data, source->len);

        BinaryDataPtr result(new BinaryData{data.get(), source->len});
        data.release();

        *new_binary_data = result.release();
    }
    CATCH_EXCEPTION_RETURN_ERROR();

    OUT_CHECK(*new_binary_data);

    return nullptr;
}

void free_binarydata(BinaryData* data)
{
    if (!data)
    {
        return;
    }
    delete[] data->data;
    delete data;
}

void free_string(const char* str)
{
    if (!str)
    {
        return;
    }
    wally_free_string(const_cast<char*>(str));
}
