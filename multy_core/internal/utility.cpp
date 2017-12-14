/* Copyright 2017 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/internal/utility.h"

#include "multy_core/error.h"

#include "multy_core/internal/exception.h"

#include "wally_core.h"

#include <string.h>
#include <string>

namespace
{
using namespace wallet_core::internal;

class ErrorWrapperException : public std::exception
{
public:
    explicit ErrorWrapperException(ErrorPtr error) : m_error(std::move(error))
    {
    }

    explicit ErrorWrapperException(Error* error) : m_error(error)
    {
    }

    const char* what() const noexcept override
    {
        return m_error->message;
    }

    Error* steal() const
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
        return make_error(ERROR_OUT_OF_MEMORY, "Out of memory");
    }

    Error* error = make_error(ERROR_GENERAL_ERROR, message_copy);
    error->owns_message = true;
    return error;
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
        throw ErrorWrapperException(make_error(ERROR_INTERNAL,
                "Failed to copy string."));
    }

    char* new_message = static_cast<char*>(wally_ops.malloc_fn(len + 1));
    if (!new_message)
    {
        throw ErrorWrapperException(make_error(ERROR_INTERNAL,
                "Failed to allocate memory."));
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

void throw_if_wally_error(int err_code, const char* message)
{
    if (err_code != 0)
    {
        ErrorPtr error(internal_make_error(err_code, message));
        if (error)
        {
            throw ErrorWrapperException(std::move(error));
        }
    }
}

Error* exception_to_error()
{
    try
    {
        throw;
    }
    catch (const Error* error)
    {
        return const_cast<Error*>(error);
    }
    catch (const Exception& exception)
    {
        return make_error_from_string(exception.what());
    }
    catch (const ErrorWrapperException& exception)
    {
        return exception.steal();
    }
    catch (const std::exception& exception)
    {
        return make_error_from_string(exception.what());
    }
    catch (...)
    {
        return make_error(ERROR_GENERAL_ERROR, "Unknown exception");
    }
}

} // namespace internal

} // namespace wallet_core
