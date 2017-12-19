/* Copyright 2017 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_TRANSACTION_INTERNAL_AMOUNT_H
#define MULTY_TRANSACTION_INTERNAL_AMOUNT_H

#include "multy_transaction/api.h"

#include "third-party/mini-gmp/mini-gmp.h"

#include <stdint.h>
#include <string>

struct MULTY_TRANSACTION_API Amount
{
    explicit Amount(const char* value);
    explicit Amount(int32_t value = 0);
    explicit Amount(int64_t value);
    explicit Amount(uint64_t value);
    // TODO: implement template constructor that would choose appropriate overload based on signess and size of the int argument.

    Amount(const Amount& other);
    Amount& operator=(const Amount& other);

    ~Amount();

    void set_value(const char* value);
    std::string get_value() const;
    int64_t get_value_as_int64() const;
    uint64_t get_value_as_uint64() const;

    Amount& operator+=(const Amount& other);
    Amount& operator-=(const Amount& other);
    Amount& operator*=(const Amount& other);

    template <typename T>
    Amount& operator+=(const T& value)
    {
        return *this += Amount(value);
    }

    template <typename T>
    Amount& operator-=(const T& value)
    {
        return *this -= Amount(value);
    }

    template <typename T>
    Amount& operator*=(const T& value)
    {
        return *this *= Amount(value);
    }

    bool operator==(const Amount& other) const;
    bool operator!=(const Amount& other) const;
    bool operator<(const Amount& other) const;
    bool operator>(const Amount& other) const;
    bool operator<=(const Amount& other) const;
    bool operator>=(const Amount& other) const;

    bool is_valid() const;

private:
    mpz_t m_value;
    const void* m_magic;
};

// TODO: Those perform not-so-obvious conversion to Amount, get rid of it.
#define AMOUNT_DEFINE_OP(op)                                                   \
    inline Amount operator op(Amount left, const Amount& right)                \
    {                                                                          \
        left op## = right;                                                     \
        return left;                                                           \
    }                                                                          \
    template <typename T>                                                      \
    inline Amount operator op(Amount left, const T& right)                     \
    {                                                                          \
        left op## = right;                                                     \
        return left;                                                           \
    }                                                                          \
    template <typename T>                                                      \
    inline Amount operator op(T left, const Amount& right)                     \
    {                                                                          \
        Amount result(left);                                                   \
        result op## = right;                                                   \
        return result;                                                         \
    }

#define AMOUNT_DEFINE_CMP_OP(op)                                               \
    template <typename T>                                                      \
    inline bool operator op(const Amount& left, const T& right)                \
    {                                                                          \
        return left op Amount(right);                                          \
    }                                                                          \
    template <typename T>                                                      \
    inline bool operator op(const T& left, const Amount& right)                \
    {                                                                          \
        return Amount(left) op right;                                          \
    }

AMOUNT_DEFINE_OP(+)
AMOUNT_DEFINE_OP(-)
AMOUNT_DEFINE_OP(*)

#undef AMOUNT_DEFINE_OP

AMOUNT_DEFINE_CMP_OP(==)
AMOUNT_DEFINE_CMP_OP(!=)
AMOUNT_DEFINE_CMP_OP(<=)
AMOUNT_DEFINE_CMP_OP(>=)
AMOUNT_DEFINE_CMP_OP(<)
AMOUNT_DEFINE_CMP_OP(>)

#undef AMOUNT_DEFINE_OP

#endif // MULTY_TRANSACTION_INTERNAL_AMOUNT_H
