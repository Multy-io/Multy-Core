/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_CORE_SRC_EOS_TRANSACTION_BUILDER_TRANSFER_H
#define MULTY_CORE_SRC_EOS_TRANSACTION_BUILDER_TRANSFER_H

#include "multy_core/src/u_ptr.h"

#include <string>

struct Account;

namespace multy_core
{
namespace internal
{

TransactionBuilderPtr make_eos_transaction_builder_transfer(
        const Account& account,
        const std::string& action);

} // namespace internal
} // namespace multy_core


#endif // MULTY_CORE_SRC_EOS_TRANSACTION_BUILDER_TRANSFER_H
