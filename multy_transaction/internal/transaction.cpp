/* Copyright 2017 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "transaction.h"

namespace
{
const int32_t TRANSACTION_MAGIC = __LINE__;
} // namespace

Transaction::Transaction()
    : m_magic(&TRANSACTION_MAGIC)
{

}

Transaction::~Transaction()
{
}

bool Transaction::is_valid() const
{
    return m_magic == &TRANSACTION_MAGIC;
}
