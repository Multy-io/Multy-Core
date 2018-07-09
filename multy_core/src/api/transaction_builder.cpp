/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/transaction_builder.h"

#include "multy_core/src/api/account_impl.h"
#include "multy_core/src/api/properties_impl.h"
#include "multy_core/src/api/transaction_impl.h"
#include "multy_core/src/api/transaction_builder_impl.h"
#include "multy_core/src/blockchain_facade_base.h"
#include "multy_core/src/error_utility.h"

namespace
{
using namespace multy_core::internal;
} // namespace

Error* make_transaction_builder(
        const Account* account, uint32_t type, const char* action,
        TransactionBuilder** new_transaction_builder)
{
    ARG_CHECK_OBJECT(account);
    ARG_CHECK(new_transaction_builder != nullptr);

    try
    {
        *new_transaction_builder = get_blockchain(*account)
                .make_transaction_builder(*account, type, action).release();
    }
    CATCH_EXCEPTION_RETURN_ERROR(ERROR_SCOPE_TRANSACTION_BUILDER);

    OUT_CHECK_OBJECT(*new_transaction_builder);

    return nullptr;
}

Error* transaction_builder_get_properties(
        TransactionBuilder* transaction_builder,
        Properties** transaction_builder_properties)
{
    ARG_CHECK_OBJECT(transaction_builder);
    ARG_CHECK(transaction_builder_properties != nullptr);

    try
    {
        *transaction_builder_properties = &transaction_builder->get_properties();
    }
    CATCH_EXCEPTION_RETURN_ERROR(ERROR_SCOPE_TRANSACTION_BUILDER);

    OUT_CHECK_OBJECT(*transaction_builder_properties);

    return nullptr;
}

Error* transaction_builder_make_transaction(
        const TransactionBuilder* transaction_builder,
        Transaction** new_transaction)
{
    ARG_CHECK_OBJECT(transaction_builder);
    ARG_CHECK(new_transaction != nullptr);

    try
    {
        *new_transaction = transaction_builder->make_transaction().release();
    }
    CATCH_EXCEPTION_RETURN_ERROR(ERROR_SCOPE_TRANSACTION_BUILDER);

    OUT_CHECK_OBJECT(*new_transaction);

    return nullptr;
}

void free_transaction_builder(TransactionBuilder* transaction_builder)
{
    CHECK_OBJECT_BEFORE_FREE(transaction_builder);
}
