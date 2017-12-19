/* Copyright 2017 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_transaction/amount.h"

#include "multy_transaction/internal/amount.h"

#include "multy_core/internal/utility.h"

Error* make_amount(const char* amount_str, Amount** new_amount)
{
    ARG_CHECK(amount_str);
    ARG_CHECK(new_amount);

    try
    {
        *new_amount = new Amount(amount_str);
    }
    CATCH_EXCEPTION_RETURN_ERROR();

    OUT_CHECK_OBJECT(*new_amount);

    return nullptr;
}

Error* amount_to_string(const Amount* amount, const char** out_amount_str)
{
    ARG_CHECK(amount);
    ARG_CHECK(out_amount_str);

    try
    {
        *out_amount_str
                = wallet_core::internal::copy_string(amount->get_value());
    }
    CATCH_EXCEPTION_RETURN_ERROR();

    OUT_CHECK(*out_amount_str);

    return nullptr;
}

Error* amount_set_value(Amount* amount, const char* value)
{
    ARG_CHECK(amount);
    ARG_CHECK(value);

    try
    {
        amount->set_value(value);
    }
    CATCH_EXCEPTION_RETURN_ERROR();

    return nullptr;
}

void free_amount(Amount* amount)
{
    delete amount;
}
