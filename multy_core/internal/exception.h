/* Copyright 2017 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_CORE_INTERNAL_EXCEPTION_H
#define MULTY_CORE_INTERNAL_EXCEPTION_H

#include "multy_core/api.h"

#include <exception>
#include <string>

namespace wallet_core
{
namespace internal
{

class MULTY_CORE_API Exception : public std::exception
{
public:
    Exception(const std::string& message);
    virtual ~Exception();

    const char* what() const noexcept override;

private:
    std::string m_message;
};

class MULTY_CORE_API ExceptionBuilder
{
public:
    ExceptionBuilder(const std::string& message);

    // Throws an exception
    ~ExceptionBuilder();

    void throw_exception();
    void append_message(const std::string& message) const;

public:
    bool m_exception_thrown;
    mutable std::string m_message;
};

inline const ExceptionBuilder& operator<<(
        const ExceptionBuilder& builder, const std::string& message)
{
    builder.append_message(message);
    return builder;
}

} // namespace internal
} // namespace wallet_core

#endif // MULTY_CORE_INTERNAL_EXCEPTION_H
