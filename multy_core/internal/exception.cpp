/* Copyright 2017 by Multy.io
 * Licensed under Multy.io license
 *
 * See LICENSE for details
 */

#include "exception.h"

namespace wallet_core
{
namespace internal
{

Exception::Exception(const std::string& message)
    : m_message(message)
{
}

Exception::~Exception()
{
}

const char* Exception::what() const noexcept
{
    return m_message.c_str();
}

ExceptionBuilder::ExceptionBuilder(const std::string& message)
    : m_exception_thrown(false),
      m_message(message)
{}

ExceptionBuilder::~ExceptionBuilder()
{
    if (!std::uncaught_exception() && !m_exception_thrown)
    {
        throw_exception();
    }
}

void ExceptionBuilder::throw_exception()
{
    m_exception_thrown = true;
    throw Exception(m_message);
}

void ExceptionBuilder::append_message(const std::string& message) const
{
    m_message += message;
}

} // namespace internal
} // namespace wallet_core
