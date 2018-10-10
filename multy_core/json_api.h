/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_CORE_JSON_API_H
#define MULTY_CORE_JSON_API_H

#include "multy_core/api.h"

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

struct Error;
struct Transaction;

/** Make transaction from JSON, return a JSON object that has a serialized transaction.
 *  @param json_string - JSON string with a transaction builder and transaction properties.
 *  @param out_transaction_json - JSON object with serialized transaction.
 *  @return Error non null if something went wrong, null otherwise.
 */
MULTY_CORE_API struct Error* make_transaction_from_json(const char* json_string, const char** out_transaction_json);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // MULTY_CORE_JSON_API_H
