/* Copyright 2017 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_transaction/internal/transaction_base.h"

#include <sstream>

namespace multy_transaction
{
namespace internal
{

TransactionBase::TransactionBase(Currency currency, uint32_t traits)
    : m_properties("Transaction"), m_currency(currency), m_traits(traits)
{
    register_properties("", m_properties);
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

Properties& TransactionBase::register_properties(
        std::string name, Properties& properties)
{
    m_all_properties.push_back(std::make_pair(std::move(name), &properties));
    return properties;
}

bool TransactionBase::validate_all_properties(
        std::string* not_set_properties) const
{
    std::ostringstream sstr;
    std::vector<std::string> missing_props_raport;
    for (const auto& p : m_all_properties)
    {
        if (!p.second->validate(&missing_props_raport))
        {
            sstr << p.second->get_name() << p.first << ":\n";
            for (const auto& s : missing_props_raport)
            {
                sstr << "\t" << s << "\n";
            }
            missing_props_raport.clear();
        }
    }
    if (sstr.tellp() > 0)
    {
        *not_set_properties
                = "Not all required properties were set:\n" + sstr.str();
        return false;
    }
    return true;
}

} // namespace internal
} // namespaec multy_transaction
