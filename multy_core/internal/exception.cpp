/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license
 *
 * See LICENSE for details
 */

#include "multy_core/internal/exception.h"

#include "multy_core/error.h"
#include "multy_core/internal/utility.h"

#include <iostream>

namespace wallet_core
{
namespace internal
{

Exception::Exception(const char* message, CodeLocation location)
    : m_message(message),
      m_location(location)
{
}

Exception::~Exception()
{
}

const char* Exception::what() const noexcept
{
    return m_message.c_str();
}

Error* Exception::make_error() const
{
    CharPtr message(copy_string(m_message));
    Error* result = ::make_error(ERROR_GENERAL_ERROR, message.get(), m_location);
    result->owns_message = true;
    message.release();

    return result;
}

void Exception::append_message(const char* message) const
{
    m_message += message;
}

} // namespace internal
} // namespace wallet_core
