/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/src/api/big_int_impl.h"

#include "multy_core/src/exception.h"
#include "multy_core/src/utility.h"

#include "multy_test/value_printers.h"

#include <cmath>

namespace
{
using namespace multy_core::internal;

template <typename T>
void unsigned_import(T value, mpz_t gmp_value)
{
    mpz_import(gmp_value, 1, -1, sizeof(value), 0, 0, &value);
}

template <typename T>
void signed_import(T value, mpz_t gmp_value)
{
    if (value < 0)
    {
        value = std::abs(value);
        unsigned_import(value, gmp_value);
        mpz_mul_si(gmp_value, gmp_value, -1);
    }
    else
    {
        unsigned_import(value, gmp_value);
    }
}

} // namespace

BigInt::BigInt(const char* value)
{
    THROW_IF_WALLY_ERROR(
            mpz_init_set_str(m_value, value, 10),
            "Failed to initialize BigInt from string.");
}

BigInt::BigInt(int32_t value)
{
    mpz_init_set_si(m_value, value);
}

BigInt::BigInt(const BigInt& other)
{
    mpz_init_set(m_value, other.m_value);
}

BigInt::BigInt(int64_t value)
{
    mpz_init(m_value);
    signed_import(value, m_value);
}

BigInt::BigInt(uint64_t value)
{
    mpz_init(m_value);
    unsigned_import(value, m_value);
}

BigInt& BigInt::operator=(const BigInt& other)
{
    if (&other == this)
    {
        return *this;
    }

    mpz_clear(m_value);
    mpz_init_set(m_value, other.m_value);
    return *this;
}

BigInt::~BigInt()
{
    mpz_clear(m_value);
}

void BigInt::set_value(const char* value)
{
    BigInt tmp(value);
    std::swap(m_value, tmp.m_value);
}

std::string BigInt::get_value() const
{
    // +1 for null-terminator, +1 for sign.
    std::string result(mpz_sizeinbase(m_value, 10) + 2, '\0');
    mpz_get_str(const_cast<char*>(result.data()), 10, m_value);
    trim_excess_trailing_null(&result);
    return result;
}

uint64_t BigInt::get_value_as_uint64() const
{
    if (mpz_sgn(m_value) < 0 || mpz_sizeinbase(m_value, 2) > sizeof(uint64_t) * 8)
    {
        THROW_EXCEPTION("BigInt value is not representable as int64_t");
    }
    uint64_t result = 0;
    mpz_export(&result, 0, -1, sizeof(result), 0, 0, m_value);
    return result;
}

int64_t BigInt::get_value_as_int64() const
{
    // >= due to the sign bit
    if (mpz_sizeinbase(m_value, 2) >= sizeof(int64_t) * 8)
    {
        THROW_EXCEPTION("BigInt value is too big for int64_t");
    }
    int64_t result = 0;
    mpz_export(&result, 0, -1, sizeof(result), 0, 0, m_value);
    if (mpz_sgn(m_value) < 0)
    {
        result *= -1;
    }
    return result;
}

size_t BigInt::get_exported_size_in_bytes() const
{
    const size_t limbs_count = mpz_size(m_value);
    const mp_limb_t* limbs = mpz_limbs_read(m_value);

    size_t result = 0;
    for (size_t i = 0; i < limbs_count; ++i)
    {
        result += get_bytes_len(limbs[i]);
    }
    return result;
}

BinaryDataPtr BigInt::export_as_binary_data(BigInt::ExportFormat format) const
{
    const size_t size = get_exported_size_in_bytes();

    BinaryDataPtr result;
    throw_if_error(make_binary_data(size, reset_sp(result)));

    const int word_order = (format == EXPORT_BIG_ENDIAN) ? 1 : -1;
    const int byte_order = word_order;
    size_t result_size = 0;
    mpz_export(const_cast<uint8_t*>(result->data), &result_size, word_order,
            result->len, byte_order, 0, m_value);

    return result;
}

BigInt& BigInt::operator+=(const BigInt& other)
{
    mpz_add(m_value, m_value, other.m_value);
    return *this;
}

BigInt& BigInt::operator-=(const BigInt& other)
{
    mpz_sub(m_value, m_value, other.m_value);
    return *this;
}

BigInt& BigInt::operator*=(const BigInt& other)
{
    mpz_mul(m_value, m_value, other.m_value);
    return *this;
}

bool BigInt::operator==(const BigInt& other) const
{
    return mpz_cmp(m_value, other.m_value) == 0;
}

bool BigInt::operator!=(const BigInt& other) const
{
    return !(*this == other);
}

bool BigInt::operator<(const BigInt& other) const
{
    return mpz_cmp(m_value, other.m_value) < 0;
}

bool BigInt::operator>(const BigInt& other) const
{
    return mpz_cmp(m_value, other.m_value) > 0;
}

bool BigInt::operator<=(const BigInt& other) const
{
    return mpz_cmp(m_value, other.m_value) <= 0;
}

bool BigInt::operator>=(const BigInt& other) const
{
    return mpz_cmp(m_value, other.m_value) >= 0;
}

const void* BigInt::get_object_magic()
{
    RETURN_MAGIC();
}
