/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_CORE_INTERNAL_EXCEPTION_STREAM_H
#define MULTY_CORE_INTERNAL_EXCEPTION_STREAM_H

#include "multy_core/src/exception.h"

#include <sstream>
#include <string>

namespace multy_core
{
namespace internal
{

inline const Exception& operator<<(const Exception& e, const char* message)
{
    e.append_message(message);
    return e;
}

inline const Exception& operator<<(const Exception& e, const std::string& message)
{
    e.append_message(message.c_str());
    return e;
}

template <typename T>
inline const Exception& operator<<(const Exception& e, const T& value)
{
    std::stringstream sstr;
    sstr << value;
    return e << sstr.str();
}

} // namespace internal
} // namespace multy_core

#endif // MULTY_CORE_INTERNAL_EXCEPTION_STREAM_H
