/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/json_api.h"

#include "multy_core/src/api/json_api_impl.h"
#include "multy_core/src/utility.h"


Error* make_transaction_from_json(const char* json_string, const char** out_transaction_json)
{
    ARG_CHECK(json_string != nullptr);
    ARG_CHECK(out_transaction_json != nullptr);

    try
    {
        *out_transaction_json = multy_core::internal::copy_string(
                multy_core::internal::make_transaction_from_json(json_string));
    }
    CATCH_EXCEPTION_RETURN_ERROR(ERROR_SCOPE_JSON_API_TRANSACTION);

    OUT_CHECK(out_transaction_json);

    return nullptr;
}

