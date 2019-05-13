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

struct ValueRangeChecker
{
    static constexpr auto NoBound = nullptr;
};

template <typename MinValueType, typename MaxValueType>
struct ValueRangeCheckerImpl
{
    MinValueType min_value;
    MaxValueType max_value;
    ErrorCode error_code;

    template <typename T>
    void operator()(const T& new_value) const {
        if (new_value < min_value || new_value > max_value)
        {
            THROW_EXCEPTION2(error_code, "Value is outside of range.")
                    << " Expected to be in [" << min_value << ", " << max_value << "] range, "
                    << " actual: " << new_value << ".";
        }
    }
};

template <typename MaxValueType>
struct ValueRangeCheckerImpl<std::nullptr_t, MaxValueType>
{
    std::nullptr_t min_value;
    MaxValueType max_value;
    ErrorCode error_code;

    template <typename T>
    void operator()(const T& new_value) const {
        if (new_value > max_value)
        {
            THROW_EXCEPTION2(error_code, "Value is outside of range.")
                    << " Expected to be less than " << max_value << ", "
                    << " actual: " << new_value << ".";
        }
    }
};

template <typename MinValueType>
struct ValueRangeCheckerImpl<MinValueType, std::nullptr_t>
{
    MinValueType min_value;
    std::nullptr_t max_value;
    ErrorCode error_code;

    template <typename T>
    void operator()(const T& new_value) const {
        if (new_value < min_value)
        {
            THROW_EXCEPTION2(error_code, "Value is outside of range.")
                    << " Expected to be bigger than " << min_value << ", "
                    << " actual: " << new_value << ".";
        }
    }
};

template <typename MinValueType, typename MaxValueType>
ValueRangeCheckerImpl<MinValueType, MaxValueType> verify_in_range(
        MinValueType min_value,
        MaxValueType max_value,
        ErrorCode error_code = ERROR_INVALID_ARGUMENT)
{
    return ValueRangeCheckerImpl<MinValueType, MaxValueType>{min_value, max_value, error_code};
}

} // namespace internal
} // namespace multy_core

#endif // MULTY_CORE_SRC_PROPERTY_PREDICATES_H
