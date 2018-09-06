/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_CORE_SRC_PROPERTY_PREDICATES_H
#define MULTY_CORE_SRC_PROPERTY_PREDICATES_H

#include "multy_core/error.h"

#include "multy_core/src/api/big_int_impl.h"

#include "multy_core/src/exception.h"
#include "multy_core/src/exception_stream.h"

namespace multy_core
{
namespace internal
{

/** Collection of common property predicates, to be used with PropertyT.
*/

template <typename T, int64_t L, ErrorCode error_code = ERROR_INVALID_ARGUMENT>
void verify_bigger_than(const T& new_value)
{
    if (new_value < L)
    {
        THROW_EXCEPTION2(error_code, "Value is too low.")
                << " Expected to be >" << L << ", "
                << "actual : " << new_value << ".";
    }
}

template <typename T, int64_t L, ErrorCode error_code = ERROR_INVALID_ARGUMENT>
void verify_smaller_than(const T& new_value)
{
    if (new_value > L)
    {
        THROW_EXCEPTION2(error_code, "Value is too big.")
                << " Expected to be >" << L << ", "
                << "actual : " << new_value << ".";
    }
}
} // namespace internal
} // namespace multy_core

#endif // MULTY_CORE_SRC_PROPERTY_PREDICATES_H
