/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/sha3.h"

#include "multy_core/src/api/sha3_impl.h"
#include "multy_core/src/utility.h"

Error* sha3(const BinaryData* input, BinaryData* output)
{
    ARG_CHECK(input);
    ARG_CHECK(output);
    ARG_CHECK(output->len);
    try
    {
        multy_core::internal::sha3(*input, output);
    }
    CATCH_EXCEPTION_RETURN_ERROR();

    return nullptr;
}
