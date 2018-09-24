/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_CORE_SRC_EXCEPTION_H
#define MULTY_CORE_SRC_EXCEPTION_H

#include "multy_core/api.h"
#include "multy_core/error.h"

#include "multy_core/src/error_utility.h"

#include <exception>
#include <string>

namespace multy_core
{
namespace internal
{

/** Exception class used in multy_core.
 *
 * Message can be modified with append_message() and/or overloaded operator<<().
 *
 * Please use THROW_EXCEPTION() to throw Exception with proper CodeLocation set.
 *
 * Note that it is a BAD IDEA to subclass this outside of multy_core library.
 * Since that could cause a havoc at run-time if a subclassed exception
 * is thrown from other module and caught in multy_core.
 */
class MULTY_CORE_API Exception : public std::exception
{
public:
    Exception(ErrorCode error_code,
            const char* message,
            CodeLocation location,
            const char* backtrace = nullptr);

    Exception(const char* message,
            CodeLocation location,
            const char* backtrace = nullptr);

    virtual ~Exception();

    virtual Error* make_error() const;

    const char* what() const noexcept override;

    void append_message(const char* message) const;

    ErrorCode get_error_code() const;
    std::string get_message() const;
    CodeLocation get_location() const;
    std::string get_backtrace() const;

private:
    const ErrorCode m_error_code;
    mutable std::string m_message;
    const CodeLocation m_location;
    const std::string m_backtrace;
};

#define THROW_EXCEPTION(msg) \
    throw multy_core::internal::Exception(msg, MULTY_CODE_LOCATION)

#define THROW_EXCEPTION2(error_code, msg) \
    throw multy_core::internal::Exception(error_code, msg, MULTY_CODE_LOCATION)

} // namespace internal
} // namespace multy_core

#endif // MULTY_CORE_SRC_EXCEPTION_H
