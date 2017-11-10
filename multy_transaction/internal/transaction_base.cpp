/* Copyright 2017 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_transaction/internal/transaction_base.h"

namespace multy_transaction
{
namespace internal
{

Amount TransactionFeeBase::get_total() const
{
    return m_total;
}

Properties& TransactionFeeBase::get_properties()
{
    return m_properties;
}

void TransactionFeeBase::set_total(const Amount& new_total)
{
    m_total = new_total;
}

Amount TransactionChangeBase::get_total() const
{
    return m_total;
}

Properties& TransactionChangeBase::get_properties()
{
    return m_properties;
}

void TransactionChangeBase::set_total(const Amount& new_total)
{
    m_total = new_total;
}

TransactionBase::TransactionBase(Currency currency, uint32_t traits)
    : m_currency(currency), m_traits(traits)
{
}

Currency TransactionBase::get_currency() const
{
    return m_currency;
}

uint32_t TransactionBase::get_traits() const
{
    return m_traits;
}

Properties& TransactionBase::get_transaction_properties()
{
    return m_properties;
}

} // namespace internal
} // namespaec multy_transaction
