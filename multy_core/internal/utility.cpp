/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/internal/utility.h"

#include "multy_core/error.h"

#include "multy_core/internal/exception.h"

#include "wally_core.h"

#include <cassert>
#include <string.h>
#include <string>

namespace
{
using namespace wallet_core::internal;

class ErrorWrapperException : public Exception
{
public:
    ErrorWrapperException(ErrorPtr error)
        : Exception("", error ? error->location : CodeLocation{nullptr, 0}),
          m_error(std::move(error))
    {
        assert(m_error.get());
    }

    ErrorWrapperException(Error* error)
        : Exception("", error ? error->location : CodeLocation{nullptr, 0}),
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

Error* make_error_from_string(const std::string& message)
{
    return make_error_from_string(message.c_str());
}

} // namespace

namespace wallet_core
{

namespace internal
{
char* copy_string(const std::string& str)
{
    return copy_string(str.c_str());
}

char* copy_string(const char* str)
{
    if (!str)
    {
        return nullptr;
    }

    const size_t len = strlen(str);

    wally_operations wally_ops;
    int result = wally_get_operations(&wally_ops);
    if (result != WALLY_OK || !wally_ops.malloc_fn)
    {
        THROW_EXCEPTION("Failed to copy string.");
    }

    char* new_message = static_cast<char*>(wally_ops.malloc_fn(len + 1));
    if (!new_message)
    {
        THROW_EXCEPTION("Failed to allocate memory.");
    }

    memcpy(new_message, str, len);
    new_message[len] = '\0';
    return new_message;
}

void throw_if_error(Error* error)
{
    if (error)
    {
        throw ErrorWrapperException(error);
    }
}

#ifndef NO_MULTY_SIMULATE_ERROR
int simulate_error(int err_code, const char* statement, const CodeLocation& location)
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
        throw_if_error(internal_make_error(err_code, message, location));
    }
}

Error* exception_to_error(const CodeLocation& location)
{
    try
    {
        throw;
    }
    catch (const Exception* exception)
    {
        return exception->make_error();
    }
    catch (const wallet_core::internal::Exception& exception)
    {
        return exception.make_error();
    }
    catch (const std::exception& exception)
    {
        Error* result = make_error_from_string(exception.what());
        result->location = location;
        return result;
    }
    catch (...)
    {
        return make_error(ERROR_GENERAL_ERROR, "Unknown exception", location);
    }
}

} // namespace internal

} // namespace wallet_core
