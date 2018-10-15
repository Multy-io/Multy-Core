/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/json_api.h"

#include "multy_core/src/error_utility.h"

namespace
{
using namespace multy_core::internal;
} // namespace

Error* make_transaction_from_json(
        const char* json,
        Transaction** new_transaction)
{
    ARG_CHECK(json != nullptr);
    ARG_CHECK(new_transaction != nullptr);

    try
    {
        *new_transaction = make_transaction_from_json(std::string(json)).release();
    }
    CATCH_EXCEPTION_RETURN_ERROR(ERROR_SCOPE_TRANSACTION_BUILDER);

    OUT_CHECK_OBJECT(*new_transaction);

    return nullptr;
}
