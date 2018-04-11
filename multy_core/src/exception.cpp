/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license
 *
 * See LICENSE for details
 */

#include "multy_core/src/exception.h"

#include "multy_core/error.h"
#include "multy_core/src/backtrace.h"
#include "multy_core/src/utility.h"
#include "multy_core/src/error_utility.h"

#include <iostream>

namespace multy_core
{
namespace internal
{

Exception::Exception(
        ErrorCode error_code,
        const char* message,
        CodeLocation location,
        const char* backtrace)
    : m_error_code(error_code),
      m_message((message ? message : "")),
      m_location(location),
      m_backtrace(backtrace ? backtrace : get_error_backtrace(2))
{
}

Exception::Exception(
        const char* message,
        CodeLocation location,
        const char* backtrace)
    : Exception(ERROR_GENERAL_ERROR, message, location, backtrace)
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
    Error* result = ::make_error_with_backtrace(
            m_error_code, message.get(), m_location, m_backtrace.c_str());

    result->owns_message = true;
    message.release();

    return result;
}

void Exception::append_message(const char* message) const
{
    m_message += message;
}

} // namespace internal
} // namespace multy_core
