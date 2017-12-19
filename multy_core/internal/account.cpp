/* Copyright 2017 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/internal/account.h"

namespace
{
const int32_t ACCOUNT_MAGIC = __LINE__;
const int32_t HD_ACCOUNT_MAGIC = __LINE__;
} // namespace

Account::Account()
    : m_magic(&ACCOUNT_MAGIC)
{}

Account::~Account()
{}

bool Account::is_valid() const
{
    return m_magic == &ACCOUNT_MAGIC;
}

HDAccount::HDAccount()
    : m_magic(&HD_ACCOUNT_MAGIC)
{}

HDAccount::~HDAccount()
{}

bool HDAccount::is_valid() const
{
    return m_magic == &HD_ACCOUNT_MAGIC;
}
