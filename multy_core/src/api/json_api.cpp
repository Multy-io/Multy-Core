/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/json_api.h"

#include "multy_core/src/api/json_api_impl.h"

#include "multy_core/src/utility.h"
#include "multy_core/src/u_ptr.h"

namespace
{
using namespace multy_core::internal;

// A shim-function to bridge JSON-api and regular error-emitting API.
Error* make_transaction_from_json_internal(const char* json_string, CharPtr* transaction)
{
    INVARIANT(transaction != nullptr);

    ARG_CHECK(json_string != nullptr);
    ARG_CHECK(transaction != nullptr);

    try
    {
        transaction->reset(copy_string(
                multy_core::internal::make_transaction_from_json(
                        std::string(json_string))));
    }
    CATCH_EXCEPTION_RETURN_ERROR(ERROR_SCOPE_JSON_API_TRANSACTION);

    return nullptr;
}

char* make_error_json_result(const std::string& error_message)
{
    return copy_string(std::string("{\"error\":{\"message\":\"") + error_message + "\"}}");
}

char* make_error_json_result(const Error& error)
{
    return copy_string(std::string("{\"error\":") + error_to_json(error) + "}");
}

} // namespace

/** Make transaction from JSON, return a JSON object that wraps a result and error code.
 *  @param json_string - JSON string with a transaction builder and transaction properties.
 *  @return
 */
const char* make_transaction_from_json(const char* json_string)
{
    try
    {
        CharPtr transaction;
        ErrorPtr error(make_transaction_from_json_internal(json_string, &transaction));
        if (error)
        {
            return make_error_json_result(*error);
        }

        return transaction.release();
    }
    catch(const Exception& e)
    {
        try
        {
            ErrorPtr error(e.make_error());
            return make_error_json_result(*error);
        }
        catch(...)
        {
            return make_error_json_result(e.what());
        }
    }
    catch(const std::exception& e)
    {
        return make_error_json_result(e.what());
    }
    catch(...)
    {
        return make_error_json_result("Unknown error.");
    }
}

