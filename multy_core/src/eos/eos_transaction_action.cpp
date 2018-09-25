/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/src/eos/eos_transaction_action.h"

#include "multy_core/src/error_utility.h"
#include "multy_core/src/enum_name_map.h"
#include "multy_core/src/api/big_int_impl.h"

#include <string>
#include <unordered_map>

namespace multy_core
{
namespace internal
{

EosTransactionAction::~EosTransactionAction()
{}

std::string EosTransactionAction::get_type_name() const
{
    static const EnumNameMap<ActionType> ACTION_NAMES =
    {
        "EosTransactionAction::ActionType",
        {
            {TRANSFER, "transfer"},
        }
    };

    return ACTION_NAMES.get_name(get_type());
}


EosAuthorization::EosAuthorization(const std::string& actor, const std::string& permission)
    : m_actor(actor), m_permission(permission)
{
}

EosName EosAuthorization::get_actor() const
{
    return m_actor;
}

EosName EosAuthorization::get_permission() const
{
    return m_permission;
}

} // namespace internal
} // namespace multy_core

