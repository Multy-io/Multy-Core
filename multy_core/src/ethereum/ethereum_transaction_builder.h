/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_CORE_ETHEREUM_TRANSACTION_BUILDER_H
#define MULTY_CORE_ETHEREUM_TRANSACTION_BUILDER_H

#include "multy_core/src/u_ptr.h"
#include <string>

namespace multy_core
{
namespace internal
{

TransactionBuilderPtr make_ethereum_transaction_builder(
        const Account& account,
        const std::string& action);

} // namespace internal
} // namespace multy_core

#endif // MULTY_CORE_ETHEREUM_TRANSACTION_BUILDER_H
