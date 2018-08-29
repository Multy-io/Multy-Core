/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/src/EOS/eos_transaction_action.h"

#include "multy_core/src/error_utility.h"
#include "multy_core/src/enum_name_map.h"

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

} // namespace internal
} // namespace multy_core

