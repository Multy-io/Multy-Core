/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/binary_data.h"

#include "multy_core/src/u_ptr.h"
#include "multy_core/src/utility.h"

#include "wally_core.h"

#include <string.h>

namespace
{
using namespace multy_core::internal;
} // namespace

Error* make_binary_data_clone(const BinaryData* source, BinaryData** new_binary_data)
{
    ARG_CHECK(source);
    return make_binary_data_from_bytes(
            source->data, source->len, new_binary_data);
}

Error* make_binary_data(size_t size, BinaryData** new_binary_data)
{
    ARG_CHECK(new_binary_data);
    try
    {
        std::unique_ptr<unsigned char[]> data(new unsigned char[size]);
        wally_bzero(data.get(), size);
        *new_binary_data = new BinaryData{data.get(), size};

        data.release();
    }
    CATCH_EXCEPTION_RETURN_ERROR();

    OUT_CHECK(*new_binary_data);

    return nullptr;
}

Error* make_binary_data_from_bytes(
        const unsigned char* data, size_t size, BinaryData** new_binary_data)
{
    ARG_CHECK(size == 0 || data);
    ARG_CHECK(new_binary_data);
    try
    {
        BinaryDataPtr result;
        throw_if_error(make_binary_data(size, reset_sp(result)));
        if (result->data && size != 0)
        {
            memcpy(const_cast<unsigned char*>(result->data), data, size);
        }
        *new_binary_data = result.release();
    }
    CATCH_EXCEPTION_RETURN_ERROR();

    OUT_CHECK(*new_binary_data);

    return nullptr;
}

Error* make_binary_data_from_hex(
        const char* hex_str, BinaryData** new_binary_data)
{
    ARG_CHECK(hex_str);
    ARG_CHECK(new_binary_data);
    try
    {
        const size_t hex_str_len = strlen(hex_str);
        if (hex_str_len & 1)
        {
            return MAKE_ERROR(ERROR_INVALID_ARGUMENT, "Input string length must be even.");
        }

        const size_t data_len = hex_str_len / 2;
        BinaryDataPtr result;
        throw_if_error(make_binary_data(data_len, reset_sp(result)));

        if (data_len != 0)
        {
            size_t real_size = 0;
            THROW_IF_WALLY_ERROR(
                    wally_hex_to_bytes(
                            hex_str, const_cast<unsigned char*>(result->data),
                            result->len, &real_size),
                    "Failed to convert hex string to binary data.");

            result->len = real_size;
        }
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
