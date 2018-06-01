/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_CORE_ERROR_H
#define MULTY_CORE_ERROR_H

#include "multy_core/api.h"

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Multy Error
 * multy_core API functions reports errors by returning Error* object
 * that gives some information on what went wrong and why.
 *
 * Error message, location and backtrace (if present) are mostly for the
 * logging and post-mortem analysis of the case.
 * The most important part for the clients is the ErrorCode, that points
 * to a specific error condition.
 *
 * ErrorCode is an 32-bit integer, disguised as an enum and consists of
 * two parts: Scope and Code.
 *
 * Scope occupies 'top' half (0xFFFF0000) of the ErrorCode and defines a
 * context of error.
 *
 * Code occupies 'bottom' half (0x0000FFFF) of the ErrorCode and defines
 * specific error condition. It's values are split in two 'regions':
 * Generic and Specific codes.
 *
 * Generic codes are 0xFF and below (+ scope offset).
 * Represent errors that can happen in any scope,
 * like invariant violation or out of memory.
 *
 * Specific codes are 0xFF and above ( + scope offset).
 * Represent errors that could happen only in certain scope,
 * like invalid prev tx hash for transaction source.
 */

enum {
    MULTY_ERROR_SCOPE_MASK = 0xFFFF0000,
    MULTY_ERROR_SCOPE_SHIFT = 16,

    MULTY_ERROR_CODE_MASK = 0x0000FFFF,
    MULTY_ERROR_CODE_SHIFT = 0,

    MULTY_ERROR_GENERIC_ERROR_MAX = 0x000000FF,
};

enum ErrorScope
{
    ERROR_SCOPE_GENERIC                     = 0,
    ERROR_SCOPE_API                         = 1,
    ERROR_SCOPE_BIG_INT                     = 2,
    ERROR_SCOPE_MNEMONIC                    = 3,
    ERROR_SCOPE_KEY                         = 4,
//    ERROR_SCOPE_PRIVATE_KEY                 = 5,
//    ERROR_SCOPE_PUBLIC_KEY                  = 6,
    ERROR_SCOPE_EXTENDED_KEY                = 7,
    ERROR_SCOPE_ACCOUNT                     = 8,
    ERROR_SCOPE_TRANSACTION                 = 16,
    ERROR_SCOPE_TRANSACTION_DESTINATION     = 17,
    ERROR_SCOPE_TRANSACTION_SOURCE          = 18,
    ERROR_SCOPE_TRANSACTION_FEE             = 19,
    ERROR_SCOPE_TRANSACTION_PAYLOAD         = 20,
};

#define MULTY_SCOPE_ERROR_BASE(scope) ((scope) << MULTY_ERROR_SCOPE_SHIFT)
#define MULTY_SCOPE_SPECIFIC_ERROR_BASE(scope) (MULTY_SCOPE_ERROR_BASE(scope) + MULTY_ERROR_GENERIC_ERROR_MAX)

enum ErrorCode
{
    // Generic error codes
    ERROR_INVARIANT_FAILED = 0xFF,              // something gone really wrong
    ERROR_OUT_OF_MEMORY = 0xFE,                 //
    ERROR_FEATURE_NOT_SUPPORTED = 0xFD,         // that specific feature is not supported.
    ERROR_FEATURE_NOT_IMPLEMENTED_YET = 0xFC,   // special case for silencing unit tests

    ERROR_GENERAL_ERROR = 1,

    ERROR_INVALID_ARGUMENT,
    ERROR_INVALID_ADDRESS,                      // blockhain account address is invalid

    // Scope-specific error codes

    // API-level errors, reported when client uses API in erroneous way.
    ERROR_API_INVALID_ARGUMENT = MULTY_SCOPE_ERROR_BASE(ERROR_SCOPE_API) | ERROR_INVALID_ARGUMENT,
    ERROR_API_SPECIFIC_ERROR_BASE = MULTY_SCOPE_SPECIFIC_ERROR_BASE(ERROR_SCOPE_API),
    ERROR_API_NULL_OBJECT_ARGUMENT,
    ERROR_API_INVALID_OBJECT_ARGUMENT,

    ERROR_BIG_INT_SPECIFIC_ERROR_BASE = MULTY_SCOPE_ERROR_BASE(ERROR_SCOPE_BIG_INT),
    ERROR_BIG_INT_MATH_WITH_INFINITY,
    ERROR_BIG_INT_MATH_WITH_NAN,
    ERROR_BIG_INT_ZERO_DIVISION,
    ERROR_BIG_INT_INVALID_STRING,
    ERROR_BIG_INT_TOO_BIG_FOR_INT64,
    ERROR_BIG_INT_TOO_BIG_FOR_UINT64,

    ERROR_MNEMONIC_SPECIFIC_ERROR_BASE = MULTY_SCOPE_ERROR_BASE(ERROR_SCOPE_MNEMONIC),
    ERROR_MNEMONIC_BAD_ENTROPY,
    ERROR_MNEMONIC_INVALID,

    ERROR_KEY_SPECIFIC_ERROR_BASE = MULTY_SCOPE_ERROR_BASE(ERROR_SCOPE_KEY),
    ERROR_KEY_BAD_ENTROPY,
    ERROR_KEY_CANT_DERIVE_CHILD_KEY,
    ERROR_KEY_INVALID_SERIALIZED_STRING,
    ERROR_KEY_CANT_SERIALIZE,
    ERROR_KEY_CORRUPT,
    ERROR_KEY_CANT_DERIVE_PUBLIC_KEY,
    ERROR_KEY_CANT_SIGN_WITH_PRIVATE_KEY,
    ERROR_KEY_CANT_MAKE_USER_ID_FROM_NON_ROOT_KEY,

    // Transaction-level errors, reported by transaction object.
    ERROR_TRANSACTION_SPECIFIC_ERROR_BASE = MULTY_SCOPE_SPECIFIC_ERROR_BASE(ERROR_SCOPE_TRANSACTION),
    ERROR_TRANSACTION_NOT_ALL_REQUIRED_PROPERTIES_SET,
    ERROR_TRANSACTION_NO_SOURCES,
    ERROR_TRANSACTION_NO_DESTINATIONS,
    ERROR_TRANSACTION_CHANGE_IS_TOO_SMALL_AND_NO_OTHER_DESTINATIONS,
    ERROR_TRANSACTION_TOO_MANY_SOURCES,
    ERROR_TRANSACTION_TOO_MANY_DESTINATIONS,
    ERROR_TRANSACTION_TOO_MANY_CHANGE_DESTINATIONS,
    ERROR_TRANSACTION_INSUFFICIENT_FUNDS,
    ERROR_TRANSACTION_TRANSFER_AMOUNT_TOO_SMALL,
    ERROR_TRANSACTION_INVALID_SIGNATURE,
    ERROR_TRANSACTION_INVALID_PRIVATE_KEY,
    ERROR_TRANSACTION_TOKEN_TRANSFER_MISSING_ADDRESS,
    ERROR_TRANSACTION_TOKEN_TRANSFER_MISSING_STANDARD,
    ERROR_TRANSACTION_TOKEN_TRANSFER_MISSING_METHOD,
    ERROR_TRANSACTION_TOKEN_TRANSFER_INVALID_STANDARD,
    ERROR_TRANSACTION_TOKEN_TRANSFER_INVALID_METHOD,

    ERROR_TRANSACTION_SOURCE_SPECIFIC_ERROR_BASE = MULTY_SCOPE_SPECIFIC_ERROR_BASE(ERROR_SCOPE_TRANSACTION_SOURCE),
    ERROR_TRANSACTION_SOURCE_INVALID_PREV_TX_SCRIPT_PUBKEY,
    ERROR_TRANSACTION_SOURCE_INVALID_PREV_TX_HASH,

    ERROR_TRANSACTION_FEE_SPECIFIC_ERROR_BASE = MULTY_SCOPE_ERROR_BASE(ERROR_SCOPE_TRANSACTION_FEE),
    ERROR_TRANSACTION_FEE_TOO_LOW,

    ERROR_TRANSACTION_PAYLOAD_SPECIFIC_ERROR_BASE = MULTY_SCOPE_ERROR_BASE(ERROR_SCOPE_TRANSACTION_PAYLOAD),
    ERROR_TRANSACTION_PAYLOAD_TO_BIG,
};

struct CodeLocation
{
    const char* file;
    int line;
};

/** Error
 * Holds information about error occured inside library.
 */
struct Error
{
    enum ErrorCode code;
    const char* message;
    bool owns_message;

    // Points to the location in code where error occured.
    struct CodeLocation location;

    const char* backtrace;
};

/** Allocates Error object, assumes that message is satic and shouldn't be copied. **/
MULTY_CORE_API struct Error* make_error(
        enum ErrorCode code,
        const char* message,
        struct CodeLocation location);

/** Makes a new Error object with given backtrace;
 * @param code - ErrorCode of the new Error;
 * @param message - message text of the new error, ownership is transferred;
 * @param location - error location;
 * @param backtrace - backtrace info, ownership is NOT transferred.
 * @return Error object, must be freed with free_error().
 */
MULTY_CORE_API struct Error* make_error_with_backtrace(
        enum ErrorCode code,
        const char* message,
        struct CodeLocation location,
        const char* backtrace);

MULTY_CORE_API enum ErrorScope error_get_scope(enum ErrorCode code);

/** Frees Error object, can take nullptr. **/
MULTY_CORE_API void free_error(struct Error* error);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* MULTY_CORE_ERROR_H */
