/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/src/error_utility.h"

#include "multy_core/error.h"

#include "multy_core/src/backtrace.h"
#include "multy_core/src/exception.h"
#include "multy_core/src/exception_stream.h"
#include "multy_core/src/utility.h"
#include "multy_core/src/u_ptr.h"

#include "wally_core.h"

#include <cassert>
#include <string>
#include <string.h>

namespace
{
using namespace multy_core::internal;

const CodeLocation NULL_LOCATION {nullptr, 0};

class ErrorWrapperException : public Exception
{
public:
    explicit ErrorWrapperException(ErrorPtr error)
        : ErrorWrapperException(error.get())
    {
    }

    explicit ErrorWrapperException(Error* error)
        : Exception(
                (error ? error->code : ERROR_GENERAL_ERROR),
                nullptr,
                (error ? error->location : NULL_LOCATION),
                (error ? error->backtrace : "")),
          m_error(std::move(error))
    {
        assert(m_error.get());
    }

    const char* what() const noexcept override
    {
        return m_error->message;
    }

    Error* make_error() const override
    {
        return m_error.release();
    }

private:
    mutable ErrorPtr m_error;
};

Error* make_error_from_string(const char* message)
{
    // Copy a message from exception to a new buffer.
    // Buffer memory has to be allocated by libwally, since we want
    // to use free_string() consistently on all strings,
    // even if those were allocated internaly.
    const char* message_copy = copy_string(message);
    if (message && !message_copy)
    {
        // We have to guess here, most likely out of memory.
        return MAKE_ERROR(ERROR_OUT_OF_MEMORY, "Out of memory");
    }

    Error* error = MAKE_ERROR(ERROR_GENERAL_ERROR, message_copy);
    error->owns_message = true;
    return error;
}

ErrorCode convert_error_code(int code)
{
    switch (code)
    {
        case WALLY_EINVAL:
            return ERROR_INVARIANT_FAILED;
        case WALLY_ENOMEM:
            return ERROR_OUT_OF_MEMORY;
        case WALLY_OK:
        case WALLY_ERROR:
        default:
            return ERROR_GENERAL_ERROR;
    }
}

Error* set_error_scope(ErrorScope scope, Error* error)
{
    if (error && scope != ERROR_SCOPE_GENERIC)
    {
        error->code = multy_core::internal::set_error_scope(scope, error->code);
    }

    return error;
}

} // namespace

namespace multy_core
{
namespace internal
{

ErrorCode set_error_scope(ErrorScope error_scope, ErrorCode error_code)
{
    // Reset error scope if it is not set already.
    if ((error_code & MULTY_ERROR_SCOPE_MASK) == 0)
    {
        error_code = static_cast<ErrorCode>(
                (error_scope << MULTY_ERROR_SCOPE_SHIFT) | error_code);
    }

    return error_code;
}

void throw_if_error(Error* error)
{
    if (error)
    {
        throw ErrorWrapperException(error);
    }
}

#ifdef MULTY_ENABLE_SIMULATE_ERROR
int simulate_error(int err_code, const char* /*statement*/, const CodeLocation& /*location*/)
{
    // Here you can inject an error for some statement and\or code location,
    // simulating specific failure.
    if (false)
    {
        return INT32_MIN;
    }
    return err_code;
}
#endif

void throw_if_wally_error(int err_code, const char* message, const CodeLocation& location)
{
    if (err_code != 0)
    {
        throw_if_error(make_error(convert_error_code(err_code), message, location));
    }
}

void throw_if_wally_error(int err_code, ErrorCode error, const char* message, const CodeLocation& location)
{
    if (err_code != 0)
    {
        throw_if_error(make_error(error, message, location));
    }
}

Error* exception_to_error(ErrorScope scope, const CodeLocation& location)
{
    try
    {
        throw;
    }
    catch (const multy_core::internal::Exception& exception)
    {
        return ::set_error_scope(scope, exception.make_error());
    }
    catch (const std::exception& exception)
    {
        Error* result = make_error_from_string(exception.what());
        result->location = location;
        return ::set_error_scope(scope, result);
    }
    catch (...)
    {
        return ::set_error_scope(scope,
                make_error(ERROR_GENERAL_ERROR, "Unknown exception", location));
    }
}

std::string get_error_backtrace(size_t ignore_frames)
{
#ifdef MULTY_ENABLE_ERROR_BACKTRACE
    return get_backtrace(ignore_frames + 1);
#else
    (void)(ignore_frames);
    return std::string();
#endif
}

} // namespace internal
} // namespace multy_core
