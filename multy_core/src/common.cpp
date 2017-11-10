/* Copyright 2017 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/common.h"

#include "wally_core.h"

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
