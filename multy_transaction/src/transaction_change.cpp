/* Copyright 2017 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_transaction/transaction_change.h"

#include "multy_transaction/internal/transaction.h"

#include "multy_core/internal/utility.h"

namespace
{
using namespace multy_transaction::internal;
} // namespace

Error* transaction_change_get_total(
        TransactionChange* change, Amount* total)
{
    ARG_CHECK(change);
    ARG_CHECK(total);

    try
    {
        *total = change->get_total();
    }
    CATCH_EXCEPTION_RETURN_ERROR();
    OUT_CHECK(total);

    return nullptr;
}

Error* transaction_change_get_properties(
        TransactionChange* change, Properties** properties)
{
    ARG_CHECK(change);
    ARG_CHECK(properties);

    try
    {
        *properties = &change->get_properties();
    }
    CATCH_EXCEPTION_RETURN_ERROR();
    OUT_CHECK(properties);

    return nullptr;
}
