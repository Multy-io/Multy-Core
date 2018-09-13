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

/** Make transaction from JSON, return a JSON object that wraps a result and error code.
 *  @param json_string - JSON string with a transaction builder and transaction properties.
 *  @return
 */
MULTY_CORE_API const char* make_transaction_from_json(const char* json_string);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // MULTY_CORE_JSON_API_H
