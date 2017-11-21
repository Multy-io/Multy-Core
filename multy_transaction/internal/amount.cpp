/* Copyright 2017 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_transaction/internal/amount.h"

#include "multy_core/internal/utility.h"

namespace
{
using namespace wallet_core::internal;
} // namespace

Amount::Amount(const char* value)
{
    throw_if_wally_error(mpz_init_set_str(m_value, value, 10),
            "Failed to initialize Amount from string.");
}

Amount::Amount(int32_t value)
{
    mpz_init_set_si(m_value, value);
}

Amount::Amount(const Amount& other)
{
    mpz_init_set(m_value, other.m_value);
}

Amount::~Amount()
{
    mpz_clear(m_value);
}

void Amount::set_value(const char* value)
{
    throw_if_wally_error(mpz_set_str(m_value, value, 10),
            "Failed to set Amount value from string.");
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
