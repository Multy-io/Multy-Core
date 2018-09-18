/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_CORE_BINARY_DATA_UTILITY_H
#define MULTY_CORE_BINARY_DATA_UTILITY_H

/** Set of utility functions/classes/etc for BinaryData.
 * Not part of the interface, and should not be used by clients.
 */

#include "multy_core/api.h"
#include "multy_core/binary_data.h"
#include "multy_core/src/u_ptr.h"

#include <array>
#include <string>
#include <vector>

#include <stdint.h>

namespace multy_core
{
namespace internal
{
/** Slice existing BinaryData into new BinaryData,
 *  starting from offset and with given size;
 *
 * Note that returned object points inside original one, so it MUST NOT
 * outlive it. Also, data pointed by returned object MUST NOT be freed.
 *
 * @param data - original binary data to perform a slice on.
 * @param offset - offset in data.data to start slice from.
 * @param size - size of slice.
 * @return BinaryData object that does not own the pointed data, DO NOT free
 *         neither the object nor it's. MUST NOT outlive data argument.
 * @throws Exception if offset or size is too big.
 */
MULTY_CORE_API BinaryData slice(const BinaryData& data, size_t offset, size_t size);

/** Slice existing BinaryData into new BinaryData with some added magic.
 *
 * Supports negative offset and size, megative values mean data.len + value.
 * That allows to get a slice without leading or trailing N bytes:
 *    power_slice(b, 1, -1) === slice(b, 1, b.len - 1); // skip first byte
 *    power_slice(b, 0, -1) === slice(b, 0, b.len - 1); // skip last byte
 *    power_slice(b, -1, 1) === slice(b, b.len - 1, 1); // get only last byte.
 *
 * @param data - original binary data to perform a slice on.
 * @param offset - offset in data.data to start slice from.
 * @param size - size of slice.
 * @return BinaryData object that does not own the pointed data, DO NOT free
 *         neither the object nor it's. MUST NOT outlive data argument.
 * @throws Exception if offset or size is too big.
 */
MULTY_CORE_API BinaryData power_slice(const BinaryData& data, int32_t offset, int32_t size);

namespace detail
{

template <typename T>
BinaryData do_convert_to_binary_data(const T* data, size_t len)
{
    static_assert(1 == sizeof(*data), "Only 1-byte wide types are supported.");
    return BinaryData{reinterpret_cast<const unsigned char*>(data), len};
}

} // namespace detail

/** as_binary_data() wraps given argument into BinaryData.
 *
 * Note that none of function overloads ever copies nor allocates any memory,
 * hence returned object MUST NOT outlive function argument.
 */
template <typename T, size_t N>
inline BinaryData as_binary_data(const T (&array)[N])
{
    return detail::do_convert_to_binary_data(array, N);
}

inline BinaryData as_binary_data(const std::string& str)
{
    return detail::do_convert_to_binary_data(str.c_str(), str.size());
}

template <typename T, typename A>
inline BinaryData as_binary_data(const std::vector<T, A>& vector)
{
    return detail::do_convert_to_binary_data(vector.data(), vector.size());
}

template <typename T, size_t N>
inline BinaryData as_binary_data(const std::array<T, N>& array)
{
    return detail::do_convert_to_binary_data(array.data(), array.size());
}

MULTY_CORE_API BinaryData as_binary_data(const char* str);

inline const BinaryData& as_binary_data(const BinaryData& binary_data)
{
    return binary_data;
}

BinaryDataPtr new_binary_data(size_t bytes);

MULTY_CORE_API bool operator==(const BinaryData& left, const BinaryData& right);
inline bool operator!=(const BinaryData& left, const BinaryData& right)
{
    return !(left == right);
}

template <typename T>
BinaryData slice(const T& data, size_t offset, size_t size)
{
    return slice(as_binary_data(data), offset, size);
}

template <typename T>
BinaryData power_slice(const T& data, int32_t offset, int32_t size)
{
    return power_slice(as_binary_data(data), offset, size);
}

} // namespace internal
} // namespace multy_core

namespace std
{

inline const unsigned char* begin(const BinaryData& data)
{
    return data.data;
}

inline const unsigned char* end(const BinaryData& data)
{
    return data.data + data.len;
}

inline const unsigned char* cbegin(const BinaryData& data)
{
    return data.data;
}

inline const unsigned char* cend(const BinaryData& data)
{
    return data.data + data.len;
}

} // namespace std

#endif // MULTY_CORE_BINARY_DATA_UTILITY_H
