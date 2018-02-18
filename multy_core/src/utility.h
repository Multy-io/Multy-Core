/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_CORE_UTILITY_H
#define MULTY_CORE_UTILITY_H

/** Set of utility functions/classes/etc.
 * Not part of the interface, and should not be used by clients.
 */

#include "multy_core/api.h"
#include "multy_core/common.h"
#include "multy_core/error.h"

#include "multy_core/src/u_ptr.h"

#include <cassert>
#include <iterator>
#include <limits>
#include <memory>

struct Error;

#define ARG_CHECK(arg)                                                         \
    do                                                                         \
    {                                                                          \
        if (!(arg))                                                            \
        {                                                                      \
            return MAKE_ERROR(                                                 \
                    ERROR_INVALID_ARGUMENT,                                    \
                    "Argument check failed: \"" #arg "\"");                    \
        }                                                                      \
    } while (false)

#define OUT_CHECK(arg)                                                         \
    do                                                                         \
    {                                                                          \
        if (!(arg))                                                            \
        {                                                                      \
            return MAKE_ERROR(                                                 \
                    ERROR_INTERNAL,                                            \
                    "Failed to create output value: \"" #arg "\"");            \
        }                                                                      \
    } while (false)

#define CATCH_EXCEPTION_RETURN_ERROR()                                         \
    catch (...)                                                                \
    {                                                                          \
        return multy_core::internal::exception_to_error(MULTY_CODE_LOCATION);  \
    }

#define CHECK_OBJECT(obj)                                                      \
    do                                                                         \
    {                                                                          \
        if (!(obj)->is_valid())                                                \
        {                                                                      \
            return MAKE_ERROR(                                                 \
                    ERROR_INVALID_ARGUMENT,                                    \
                    "\"" #obj "\" is not a valid object.");                    \
        }                                                                      \
    } while (false)

#define ARG_CHECK_OBJECT(obj)                                                  \
    do                                                                         \
    {                                                                          \
        ARG_CHECK((obj) != nullptr);                                           \
        CHECK_OBJECT((obj));                                                   \
    } while (false)

#define OUT_CHECK_OBJECT(obj)                                                  \
    do                                                                         \
    {                                                                          \
        OUT_CHECK((obj) != nullptr);                                           \
        CHECK_OBJECT((obj));                                                   \
    } while (false)

#define CHECK_OBJECT_BEFORE_FREE(obj)                                          \
    do                                                                         \
    {                                                                          \
        if ((obj))                                                             \
        {                                                                      \
            assert((obj)->is_valid() && "trying to free invalid object:" #obj);\
            return;                                                            \
        }                                                                      \
    } while(false)

namespace multy_core
{

namespace internal
{
MULTY_CORE_API void throw_if_error(struct Error* err);

#define THROW_IF_WALLY_ERROR(statement, message)                               \
    multy_core::internal::throw_if_wally_error(                                \
            MULTY_SIMULATE_ERROR(                                              \
                    (statement), #statement, MULTY_CODE_LOCATION),             \
            (message), MULTY_CODE_LOCATION)

#ifndef MULTY_ENABLE_SIMULATE_ERROR
#define MULTY_SIMULATE_ERROR(err, statement, location) (err)
#else
#define MULTY_SIMULATE_ERROR simulate_error
MULTY_CORE_API int simulate_error(
        int err_code, const char* statement, const CodeLocation& location);
#endif

MULTY_CORE_API void throw_if_wally_error(
        int err_code, const char* message, const CodeLocation& location);

template <typename T, size_t N>
constexpr size_t array_size(T (&)[N])
{
    return N;
}

template <typename T, size_t N>
constexpr size_t array_size(const std::array<T, N>&)
{
    return N;
}

/** Finds a max item in values that is smaller than value;
 * If there is no such value, returns default_value.
 * Relies on the fact that values are sorted in ascending order.
 * @param values - orderes array of value to choose from.
 * @param default_value - returned if value is less than any in values.
 * @param value - initial value, the one being compared.
 * @return reference either to matching item in values or to default_value.
 */
template <typename T, size_t N>
T find_max_value(const T (&values)[N], const T& default_value, const T& value)
{
    T result = default_value;
    for (int i = array_size(values) - 1; i >= 0; --i)
    {
        if (value >= values[i])
        {
            result = values[i];
            break;
        }
    }
    return result;
}

/// Converts exception to a Error*, to be used inside a catch(...) block.
MULTY_CORE_API Error* exception_to_error(const CodeLocation& context);

/** Convenience function to copy a string.
 * @param str - string to copy, must not be null.
 * @return - copy of a string, must be freed with free_string(), can be null on
 * error.
 */
MULTY_CORE_API char* copy_string(const char* str);

MULTY_CORE_API char* copy_string(const std::string& str);

/** Slice existing BinaryData into new BinaryData,
 *  starting from offset and with given size;
 *
 * Note that returned object points inside original one, so it must not outlive it,
 * also, data pointed by returned object should not be freed.
 *
 * Throws exceptions if offset or size (or offset + size) is too big.
 */
MULTY_CORE_API BinaryData slice(const BinaryData& data, size_t offset, size_t size);

/** Convenience to simplify passing C++ smart_pointers (like std::unique_ptr<T>)
 * to C-like functions than take T** and store address of new object there.
 * Should be used in conjunction with reset_sp() function.
 * Resets value of smart pointer if it was requested to be converted to
 * pointer-to-pointer (T**)
 * and that value was modified during lifetime of the object.
 */
template <typename SP>
class UniquePointerUpdater
{
    typedef typename SP::pointer Pointer;
    SP& sp;
    mutable Pointer p;

public:
    // TODO: change from reference to pointer
    explicit UniquePointerUpdater(SP& sp) : sp(sp), p(sp.get())
    {
    }

    ~UniquePointerUpdater()
    {
        if (p != sp.get())
        {
            sp.reset(p);
        }
    }

    operator Pointer*() const
    {
        p = sp.get();
        return &p;
    }

    UniquePointerUpdater(const UniquePointerUpdater&) = delete;
    UniquePointerUpdater& operator=(const UniquePointerUpdater&) = delete;
    UniquePointerUpdater(UniquePointerUpdater&&) = default;
    UniquePointerUpdater& operator=(UniquePointerUpdater&&) = default;
};

/// See UniquePointerUpdater.
template <typename SP>
inline UniquePointerUpdater<SP> reset_sp(SP& sp)
{
    return UniquePointerUpdater<SP>(sp);
}

template <typename T>
UPtr<T> make_clone(const T& original)
{
    return UPtr<T>(new T(original));
}

inline BinaryDataPtr make_clone(const BinaryData& other)
{
    BinaryDataPtr result;
    throw_if_error(make_binary_data_clone(&other, reset_sp(result)));
    return result;
}

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

bool operator==(const BinaryData& left, const BinaryData& right);
inline bool operator!=(const BinaryData& left, const BinaryData& right)
{
    return !(left == right);
}

// Gets minimum number of bytes required to represent integer value.
template <typename T>
size_t get_bytes_len(T value)
{
    static_assert(!std::numeric_limits<T>::is_signed, "Only unsigned types supported");
    size_t i = 0;
    uint64_t tmp_value = value; // suppress warning about >>= below when T is uint8_t.
    for (; i < sizeof(value) && tmp_value; ++i)
    {
        tmp_value >>= 8;
    }

    return i;
}

} // namespace internal
} // namespace multy_core

#endif // MULTY_CORE_UTILITY_H
