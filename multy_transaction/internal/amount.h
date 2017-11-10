/* Copyright 2017 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_TRANSACTION_INTERNAL_AMOUNT_H
#define MULTY_TRANSACTION_INTERNAL_AMOUNT_H

#include "multy_transaction/api.h"

#include "third-party/mini-gmp/mini-gmp.h"

#include <string>
#include <stdint.h>

struct MULTY_TRANSACTION_API Amount
{
    explicit Amount(const char* value);
    explicit Amount(int32_t value = 0);
    Amount(const Amount& other);

    ~Amount();

    void set_value(const char* value);
    std::string get_value() const;

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
        return *this += Amount(value);
    }

    template <typename T>
    Amount& operator*=(const T& value)
    {
        return *this *= Amount(value);
    }

private:
    mpz_t m_value;
};

// TODO: Those perform not-so-obvious conversion to Amount, get rid of it.
#define AMOUNT_DEFINE_OP(op)                                               \
inline Amount operator op(Amount left, const Amount& right)                \
{                                                                          \
    Amount result(std::move(left));                                        \
    result op## = right;                                                   \
    return result;                                                         \
}                                                                          \
template <typename T>                                                      \
inline Amount operator op(Amount left, const T& right)                     \
{                                                                          \
    Amount result(std::move(left));                                        \
    result op## = right;                                                   \
    return result;                                                         \
}                                                                          \
template <typename T>                                                      \
inline Amount operator op(T left, const Amount& right)                     \
{                                                                          \
    Amount result(std::move(left));                                        \
    result op## = right;                                                   \
    return result;                                                         \
}

AMOUNT_DEFINE_OP(+)
AMOUNT_DEFINE_OP(-)
AMOUNT_DEFINE_OP(*)

#endif // MULTY_TRANSACTION_INTERNAL_AMOUNT_H
