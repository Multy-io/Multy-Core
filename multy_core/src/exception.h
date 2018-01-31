/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_CORE_SRC_EXCEPTION_H
#define MULTY_CORE_SRC_EXCEPTION_H

#include "multy_core/api.h"
#include "multy_core/error.h"

#include "multy_core/error.h"

#include <exception>
#include <string>

namespace multy_core
{
namespace internal
{

/** Exception class used in multy_core.
 *
 * Message can be modified with append_message() and\or overloaded operator<<().
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
    Exception(const char* message, CodeLocation location);
    virtual ~Exception();

    virtual Error* make_error() const;

    const char* what() const noexcept override;

    void append_message(const char* message) const;

private:
    mutable std::string m_message;
    const CodeLocation m_location;
};

#define THROW_EXCEPTION(msg) \
    throw multy_core::internal::Exception(msg, MULTY_CODE_LOCATION)

} // namespace internal
} // namespace multy_core

#endif // MULTY_CORE_SRC_EXCEPTION_H
