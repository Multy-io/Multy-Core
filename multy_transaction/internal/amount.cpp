/* Copyright 2017 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_transaction/internal/amount.h"

#include "multy_core/internal/exception.h"
#include "multy_core/internal/utility.h"

#include "multy_test/value_printers.h"

namespace
{
using namespace wallet_core::internal;
void throw_exception(const std::string& message)
{
    throw Exception(message);
}
const int32_t AMOUNT_MAGIC = __LINE__;

} // namespace

Amount::Amount(const char* value)
    : m_magic(&AMOUNT_MAGIC)
{
    throw_if_wally_error(
            mpz_init_set_str(m_value, value, 10),
            "Failed to initialize Amount from string.");
}

Amount::Amount(int32_t value)
    : m_magic(&AMOUNT_MAGIC)
{
    mpz_init_set_si(m_value, value);
}

Amount::Amount(const Amount& other)
    : m_magic(&AMOUNT_MAGIC)
{
    mpz_init_set(m_value, other.m_value);
}

Amount& Amount::operator=(const Amount& other)
{
    if (&other == this)
    {
        return *this;
    }

    mpz_clear(m_value);
    mpz_init_set(m_value, other.m_value);
    return *this;
}

Amount::Amount(int64_t value)
    : m_magic(&AMOUNT_MAGIC)
{
    mpz_init(m_value);
    mpz_import(m_value, 1, -1, sizeof(value), 0, 0, &value);
    if (value < 0)
    {
        mpz_mul_si(m_value, m_value, -1);
    }
}

Amount::Amount(uint64_t value)
    : m_magic(&AMOUNT_MAGIC)
{
    mpz_init(m_value);
    mpz_import(m_value, 1, -1, sizeof(value), 0, 0, &value);
}

Amount::~Amount()
{
    mpz_clear(m_value);
}

void Amount::set_value(const char* value)
{
    Amount tmp(value);
    std::swap(m_value, tmp.m_value);
}

std::string Amount::get_value() const
{
    // +1 for null-terminator, +1 for sign, +1 for decimal mark.
    std::string result(mpz_sizeinbase(m_value, 10) + 3, '\0');
    mpz_get_str(const_cast<char*>(result.data()), 10, m_value);

    // remove excess '\0' chars at end of string.
    auto pos = result.find_last_not_of('\0');
    if (pos != std::string::npos)
    {
        result.erase(pos + 1);
    }
    return result;
}

uint64_t Amount::get_value_as_uint64() const
{
    if (mpz_sizeinbase(m_value, 2) > sizeof(uint64_t) * 8)
    {
        throw_exception("Amount value is not representable as int64_t");
    }
    uint64_t result = 0;
    mpz_export(&result, 0, -1, sizeof(result), 0, 0, m_value);
    return result;
}

int64_t Amount::get_value_as_int64() const
{
    // >= due to the sign bit
    if (mpz_sizeinbase(m_value, 2) >= sizeof(int64_t) * 8)
    {
        throw_exception("Amount value is not representable as int64_t");
    }
    int64_t result = 0;
    mpz_export(&result, 0, -1, sizeof(result), 0, 0, m_value);
    return result;
}

Amount& Amount::operator+=(const Amount& other)
{
    mpz_add(m_value, m_value, other.m_value);
    return *this;
}

Amount& Amount::operator-=(const Amount& other)
{
    mpz_sub(m_value, m_value, other.m_value);
    return *this;
}

Amount& Amount::operator*=(const Amount& other)
{
    mpz_mul(m_value, m_value, other.m_value);
    return *this;
}

bool Amount::operator==(const Amount& other) const
{
    return mpz_cmp(m_value, other.m_value) == 0;
}

bool Amount::operator!=(const Amount& other) const
{
    return !(*this == other);
}

bool Amount::operator<(const Amount& other) const
{
    return mpz_cmp(m_value, other.m_value) < 0;
}

bool Amount::operator>(const Amount& other) const
{
    return mpz_cmp(m_value, other.m_value) > 0;
}

bool Amount::operator<=(const Amount& other) const
{
    return mpz_cmp(m_value, other.m_value) <= 0;
}

bool Amount::operator>=(const Amount& other) const
{
    return mpz_cmp(m_value, other.m_value) >= 0;
}

bool Amount::is_valid() const
{
    return m_magic == &AMOUNT_MAGIC;
}
