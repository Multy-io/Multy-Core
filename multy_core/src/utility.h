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
#include "multy_core/binary_data.h"
#include "multy_core/common.h"
#include "multy_core/blockchain.h"

#include "multy_core/bitcoin.h"
#include "multy_core/ethereum.h"
#include "multy_core/golos.h"

#include "multy_core/src/binary_data_utility.h"
#include "multy_core/src/error_utility.h"
#include "multy_core/src/u_ptr.h"

#include <algorithm>
#include <array>
#include <iterator>
#include <limits>
#include <memory>
#include <string>
#include <sstream>
#include <ctime>

extern "C" struct BlockchainType;

namespace multy_core
{
namespace internal
{

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

/** Convenience function to copy a string.
 * @param str - string to copy, must not be null.
 * @return - copy of a string, must be freed with free_string(), can be null on
 * error.
 */
MULTY_CORE_API char* copy_string(const char* str);
MULTY_CORE_API char* copy_string(const std::string& str);

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

template <typename Container, typename Value>
bool contains(const Container& container, const Value& v)
{
    return std::find(std::begin(container), std::end(container), v)
            != std::end(container);
}

template <typename Container, typename Sequence>
bool contains_sequence(const Container& container, const Sequence& s)
{
    return std::search(std::begin(container), std::end(container),
            std::begin(s), std::end(s)) != std::end(container);
}

struct NoOpPredicate
{
    template <typename T>
    inline const T& operator()(const T& value) const
    {
        return value;
    }
};

template <typename T, typename Container, typename Predicate = NoOpPredicate>
std::string join(const T& separator, const Container& container, Predicate predicate = Predicate())
{
    std::stringstream sstr;

    const auto end = std::end(container);
    for (auto i = std::begin(container); i != end; ++i)
    {
        sstr << predicate(*i);
        if (i != end)
        {
            sstr << separator;
        }
    }

    return sstr.str();
}

MULTY_CORE_API bool operator==(const BlockchainType& left, const BlockchainType& right);
inline bool operator!=(const BlockchainType& left, const BlockchainType& right)
{
    return !(left == right);
}

MULTY_CORE_API std::string to_string(const BlockchainType& blockchain_type);
MULTY_CORE_API std::string to_string(Blockchain blockchain);
MULTY_CORE_API std::string to_string(BitcoinNetType net_type);
MULTY_CORE_API std::string to_string(EthereumChainId net_type);
MULTY_CORE_API std::string to_string(GolosNetType net_type);

MULTY_CORE_API std::string format_iso8601_string(const std::time_t& time);
MULTY_CORE_API std::time_t parse_iso8601_string(const std::string& str);

std::time_t to_system_seconds(size_t seconds);
std::time_t get_system_time_now();

// remove excess '\0' chars at end of string.
void trim_excess_trailing_null(std::string* str);

} // namespace internal
} // namespace multy_core

#endif // MULTY_CORE_UTILITY_H
