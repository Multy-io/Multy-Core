/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_CORE_SRC_API_BIG_INT_IMPL_H
#define MULTY_CORE_SRC_API_BIG_INT_IMPL_H

#include "multy_core/api.h"

#include "multy_core/src/object.h"
#include "multy_core/src/u_ptr.h"

#include "third-party/mini-gmp/mini-gmp.h"

#include <stdint.h>
#include <string>

/** BigInt: class to deal with integers that may be too big for int64.
 *
 * Handle really large integers, bot signed and unsigned.
 * Can convert internal value to string, int64, uin64, however later may fail
 * if value is too big.
 *
 * Supports + , - , * operations as well as assignment version of those.
 * Division is explicitly not supported to simplify implementation.
 *
 * Supports full set of comparision operators.
 */
struct MULTY_CORE_API BigInt : public ::multy_core::internal::ObjectBase<BigInt>
{
    explicit BigInt(const char* value);
    explicit BigInt(int32_t value = 0);
    explicit BigInt(int64_t value);
    explicit BigInt(uint64_t value);
    // TODO: implement template constructor that would choose appropriate overload based on signess and size of the int argument.

    BigInt(const BigInt& other);
    BigInt& operator=(const BigInt& other);

    BigInt(BigInt&& other);
    BigInt& operator=(BigInt&& other);

    ~BigInt();

    void set_value(const char* value);
    void set_value_int64(int64_t value);

    std::string get_value() const;

    // Returns value as int64_t, throws exception if value is too big.
    int64_t get_value_as_int64() const;

    // Returns value as uint64_t, throws exception if value is too big.
    uint64_t get_value_as_uint64() const;

    template <typename T>
    T get() const;

    size_t get_exported_size_in_bytes() const;

    typedef multy_core::internal::BinaryDataPtr BinaryDataPtr;
    enum ExportFormat {EXPORT_BIG_ENDIAN, EXPORT_LITTLE_ENDIAN};
    BinaryDataPtr export_as_binary_data(ExportFormat format) const;

    BigInt& operator+=(const BigInt& other);
    BigInt& operator-=(const BigInt& other);
    BigInt& operator*=(const BigInt& other);
    BigInt& operator/=(const BigInt& other);
    BigInt operator-() const;

    template <typename T>
    BigInt& operator+=(const T& value)
    {
        return *this += BigInt(value);
    }

    template <typename T>
    BigInt& operator-=(const T& value)
    {
        return *this -= BigInt(value);
    }

    template <typename T>
    BigInt& operator*=(const T& value)
    {
        return *this *= BigInt(value);
    }
    template <typename T>
    BigInt& operator/=(const T& value)
    {
        return *this /= BigInt(value);
    }

    bool operator==(const BigInt& other) const;
    bool operator!=(const BigInt& other) const;
    bool operator<(const BigInt& other) const;
    bool operator>(const BigInt& other) const;
    bool operator<=(const BigInt& other) const;
    bool operator>=(const BigInt& other) const;

    static const void* get_object_magic();

private:
    mpz_t m_value;
};

template <>
inline uint64_t BigInt::get<uint64_t>() const
{
    return get_value_as_uint64();
}

template <>
inline int64_t BigInt::get<int64_t>() const
{
    return get_value_as_int64();
}

template <>
inline std::string BigInt::get<std::string>() const
{
    return get_value();
}

// TODO: Those perform not-so-obvious conversion to BigInt, get rid of it.
#define MULTY_BIG_INT_DEFINE_OP(op)                                                   \
    inline BigInt operator op(BigInt left, const BigInt& right)                \
    {                                                                          \
        left op## = right;                                                     \
        return left;                                                           \
    }                                                                          \
    template <typename T>                                                      \
    inline BigInt operator op(BigInt left, const T& right)                     \
    {                                                                          \
        left op## = right;                                                     \
        return left;                                                           \
    }                                                                          \
    template <typename T>                                                      \
    inline BigInt operator op(T left, const BigInt& right)                     \
    {                                                                          \
        BigInt result(left);                                                   \
        result op## = right;                                                   \
        return result;                                                         \
    }

#define MULTY_BIG_INT_DEFINE_CMP_OP(op)                                               \
    template <typename T>                                                      \
    inline bool operator op(const BigInt& left, const T& right)                \
    {                                                                          \
        return left op BigInt(right);                                          \
    }                                                                          \
    template <typename T>                                                      \
    inline bool operator op(const T& left, const BigInt& right)                \
    {                                                                          \
        return BigInt(left) op right;                                          \
    }

MULTY_BIG_INT_DEFINE_OP(+)
MULTY_BIG_INT_DEFINE_OP(-)
MULTY_BIG_INT_DEFINE_OP(*)
MULTY_BIG_INT_DEFINE_OP(/)

#undef MULTY_BIG_INT_DEFINE_OP

MULTY_BIG_INT_DEFINE_CMP_OP(==)
MULTY_BIG_INT_DEFINE_CMP_OP(!=)
MULTY_BIG_INT_DEFINE_CMP_OP(<=)
MULTY_BIG_INT_DEFINE_CMP_OP(>=)
MULTY_BIG_INT_DEFINE_CMP_OP(<)
MULTY_BIG_INT_DEFINE_CMP_OP(>)

#undef MULTY_BIG_INT_DEFINE_CMP_OP

#endif // MULTY_CORE_SRC_API_BIG_INT_IMPL_H
