/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_CORE_ERROR_UTILITY_H
#define MULTY_CORE_ERROR_UTILITY_H

/** Set of utility functions/classes and macros for handling errors,
 *  throwing exceptions, etc.
 * Not part of the interface, and should not be used by clients.
 */

#include "multy_core/api.h"
#include "multy_core/error.h"
#include "multy_core/src/exception.h"

#include <cassert>
#include <string>

struct Error;

#ifndef MULTY_CODE_LOCATION
#define MULTY_CODE_LOCATION CodeLocation{__FILE__, __LINE__}
#endif

#define MAKE_ERROR(code, message) make_error((code), (message), MULTY_CODE_LOCATION)

#define ARG_CHECK(arg)                                                         \
    do                                                                         \
    {                                                                          \
        if (!(arg))                                                            \
        {                                                                      \
            return MAKE_ERROR(                                                 \
                    ERROR_API_INVALID_ARGUMENT,                                \
                    "Argument check failed: \"" #arg "\"");                    \
        }                                                                      \
    } while (false)

#define OUT_CHECK(arg)                                                         \
    do                                                                         \
    {                                                                          \
        if (!(arg))                                                            \
        {                                                                      \
            return MAKE_ERROR(                                                 \
                    ERROR_INVARIANT_FAILED,                                    \
                    "Failed to create output value: \"" #arg "\"");            \
        }                                                                      \
    } while (false)

#define CATCH_EXCEPTION_RETURN_ERROR(SCOPE)                                    \
    catch (...)                                                                \
    {                                                                          \
        return multy_core::internal::exception_to_error(                       \
                SCOPE, MULTY_CODE_LOCATION);                                   \
    }

#define CHECK_OBJECT(obj, error)                                               \
    do                                                                         \
    {                                                                          \
        if (!obj)                                                              \
        {                                                                      \
            return MAKE_ERROR(                                                 \
                    ERROR_API_NULL_OBJECT_ARGUMENT,                            \
                    "\"" #obj "\" is nullptr.");                               \
        }                                                                      \
        if (!(obj)->is_valid())                                                \
        {                                                                      \
            return MAKE_ERROR(                                                 \
                    (error),                                                   \
                    "\"" #obj "\" is not a valid object.");                    \
        }                                                                      \
    } while (false)

#define ARG_CHECK_OBJECT(obj)                                                  \
    do                                                                         \
    {                                                                          \
        CHECK_OBJECT((obj), ERROR_API_INVALID_OBJECT_ARGUMENT);                \
    } while (false)

#define OUT_CHECK_OBJECT(obj)                                                  \
    do                                                                         \
    {                                                                          \
        CHECK_OBJECT((obj), ERROR_INVARIANT_FAILED);                           \
    } while (false)

#define CHECK_OBJECT_BEFORE_FREE(obj)                                          \
    do                                                                         \
    {                                                                          \
        if ((obj))                                                             \
        {                                                                      \
            assert((obj)->is_valid() && "trying to free invalid object:" #obj);\
            (void)(0);                                                         \
        }                                                                      \
    } while(false)

#define THROW_IF_WALLY_ERROR(statement, message)                               \
    multy_core::internal::throw_if_wally_error(                                \
            MULTY_SIMULATE_ERROR(                                              \
                    (statement), #statement, MULTY_CODE_LOCATION),             \
            (message), MULTY_CODE_LOCATION)

#define THROW_IF_WALLY_ERROR2(statement, error, message)                       \
    multy_core::internal::throw_if_wally_error(                                \
            MULTY_SIMULATE_ERROR(                                              \
                    (statement), #statement, MULTY_CODE_LOCATION),             \
            error, (message), MULTY_CODE_LOCATION)

#ifndef MULTY_INVARIANT_DISABLED
    #if MULTY_INVARIANT_THROWS_EXCEPTION
        #define INVARIANT(statement)                                           \
            do                                                                 \
            {                                                                  \
                if (!(statement))                                              \
                {                                                              \
                    THROW_EXCEPTION2(ERROR_INVARIANT_FAILED,                   \
                            "Invariant failed: " #statement);                  \
                }                                                              \
            }                                                                  \
            while(false)

        #define INVARIANT2(statement, message)                                 \
            do {                                                               \
                if (!(statement))                                              \
                {                                                              \
                    THROW_EXCEPTION2(ERROR_INVARIANT_FAILED,                   \
                            "Invariant failed: " #statement) << " " << message;\
                }                                                              \
            }                                                                  \
            while(false)
    #else
        #define INVARIANT(statement)                                           \
                assert(("Failed invariant" && (statement)))
        #define INVARIANT2(statement, message)                                 \
                assert(("Failed invariant:" && (statement)))
    #endif
#endif

namespace multy_core
{
namespace internal
{
MULTY_CORE_API ErrorCode set_error_scope(ErrorScope error_scope, ErrorCode error_code);

// Can return empty string if taking error backtrace is disabled.
MULTY_CORE_API std::string get_error_backtrace(size_t ignore_frames);

/** Throw Exception if error is not nullptr.
 * @param error - error object, ownership IS transferred.
 * @throw Exception if error is non-nullptr, none otherwise.
 */
MULTY_CORE_API void throw_if_error(struct Error* error);

#ifndef MULTY_ENABLE_SIMULATE_ERROR
#define MULTY_SIMULATE_ERROR(err, statement, location) (err)
#else
#define MULTY_SIMULATE_ERROR simulate_error
MULTY_CORE_API int simulate_error(
        int err_code, const char* statement, const CodeLocation& location);
#endif

MULTY_CORE_API void throw_if_wally_error(
        int err_code, const char* message, const CodeLocation& location);

MULTY_CORE_API void throw_if_wally_error(
        int err_code,    // Value is discarded if it is an Error.
        ErrorCode error, // ErrorCode for the thrown Exception
        const char* message,
        const CodeLocation& location);

/// Converts exception to a Error*, to be used inside a catch(...) block.
MULTY_CORE_API Error* exception_to_error(ErrorScope scope, const CodeLocation& context);

} // namespace internal
} // namespace multy_core

#endif // MULTY_CORE_ERROR_UTILITY_H
