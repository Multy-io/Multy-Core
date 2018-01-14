/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/common.h"

#include "multy_core/internal/u_ptr.h"
#include "multy_core/internal/utility.h"

#include "wally_core.h"

#include <string.h>
#include <sstream>

#ifndef MULTY_CORE_VERSION_MAJOR
#define MULTY_CORE_VERSION_MAJOR 0
#endif // MULTY_CORE_VERSION_MAJOR

#ifndef MULTY_CORE_VERSION_MINOR
#define MULTY_CORE_VERSION_MINOR 0
#endif // MULTY_CORE_VERSION_MINOR

#ifndef MULTY_CORE_VERSION_BUILD
#define MULTY_CORE_VERSION_BUILD 0
#endif // MULTY_CORE_VERSION_BUILD

#ifndef MULTY_CORE_VERSION_NOTE
#define MULTY_CORE_VERSION_NOTE ""
#endif // MULTY_CORE_VERSION_NOTE

#ifndef MULTY_CORE_VERSION_COMMIT
#define MULTY_CORE_VERSION_COMMIT ""
#endif // MULTY_CORE_VERSION_COMMIT

namespace
{
using namespace wallet_core::internal;

const Version CURRENT_VERSION = {
    MULTY_CORE_VERSION_MAJOR,
    MULTY_CORE_VERSION_MINOR,
    MULTY_CORE_VERSION_BUILD,
    MULTY_CORE_VERSION_NOTE,
    MULTY_CORE_VERSION_COMMIT
};

std::string format_version(const Version& version)
{
    std::stringstream sstr;
    sstr << version.major << "." << version.minor << "." << version.build;
    if (version.note && strlen(version.note))
    {
        sstr << "-" << version.note;
    }
    if (version.commit && strlen(version.commit))
    {
        sstr << "(" << version.commit << ")";
    }
    return sstr.str();
}

} // namespace

Error* get_version(Version* version)
{
    ARG_CHECK(version);
    *version = CURRENT_VERSION;
    return nullptr;
}

Error* make_version_string(const char** out_version_string)
{
    ARG_CHECK(out_version_string);
    try
    {
        *out_version_string = copy_string(format_version(CURRENT_VERSION));
    }
    CATCH_EXCEPTION_RETURN_ERROR();

    OUT_CHECK(*out_version_string);
    return nullptr;
}

Error* binary_data_clone(const BinaryData* source, BinaryData** new_binary_data)
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

void free_string(const char* str)
{
    if (!str)
    {
        return;
    }
    wally_free_string(const_cast<char*>(str));
}
