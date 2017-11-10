/* Copyright 2017 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_transaction/transaction_fee.h"

#include "multy_transaction/internal/amount.h"
#include "multy_transaction/internal/transaction.h"

#include "multy_core/internal/utility.h"

Error* transaction_fee_get_total(
        TransactionFee* fee, Amount* total)
{
    ARG_CHECK(fee);
    ARG_CHECK(total);

    try
    {
        *total = fee->get_total();
    }
    CATCH_EXCEPTION_RETURN_ERROR();
    OUT_CHECK(total);

    return nullptr;
}

Error* transaction_fee_get_properties(
        TransactionFee* fee, Properties** properties)
{
    ARG_CHECK(fee);
    ARG_CHECK(properties);

    try
    {
        *properties = &fee->get_properties();
    }
    CATCH_EXCEPTION_RETURN_ERROR();
    OUT_CHECK(properties);

    return nullptr;
}
