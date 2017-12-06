/* Copyright 2017 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_TRANSACTION_PROPERTIES_H
#define MULTY_TRANSACTION_PROPERTIES_H

#include "multy_transaction/api.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct Amount;
struct BinaryData;
struct PublicKey;
struct Properties;

MULTY_TRANSACTION_API struct Error* properties_set_int32_value(
        struct Properties* properties, const char* name, int32_t value);

MULTY_TRANSACTION_API struct Error* properties_set_string_value(
        struct Properties* properties, const char* name, const char* value);

MULTY_TRANSACTION_API struct Error* properties_set_amount_value(
        struct Properties* properties,
        const char* name,
        const struct Amount* value);

MULTY_TRANSACTION_API struct Error* properties_set_binary_data_value(
        struct Properties* properties,
        const char* name,
        const struct BinaryData* value);

MULTY_TRANSACTION_API struct Error* properties_set_public_key_value(
        struct Properties* properties,
        const char* name,
        const struct PublicKey* value);

MULTY_TRANSACTION_API struct Error* properties_reset_value(
        struct Properties* properties, const char* name);

MULTY_TRANSACTION_API struct Error* properties_validate(
        const struct Properties* properties);

MULTY_TRANSACTION_API struct Error* properties_get_specification(
        const struct Properties* properties, const char** out_specification);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* MULTY_TRANSACTION_PROPERTIES_H */
