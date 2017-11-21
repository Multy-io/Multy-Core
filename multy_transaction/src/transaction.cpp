/* Copyright 2017 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_transaction/transaction.h"

#include "multy_transaction/internal/transaction.h"
#include "multy_transaction/internal/u_ptr.h"

#include "multy_core/internal/utility.h"
#include "multy_core/internal/account.h"

namespace
{
using namespace wallet_core::internal;
using namespace multy_transaction::internal;
} // namespace

Error* make_transaction(const Account* account, Transaction** new_transaction)
{
    ARG_CHECK(account);
    ARG_CHECK(new_transaction);

    try
    {
//        switch (account->get_currency())
//        {
//            case CURRENCY_BITCOIN:
//                *new_transaction =
//        }
    }
    CATCH_EXCEPTION_RETURN_ERROR();

    OUT_CHECK(*new_transaction);

    return nullptr;
}

Error* transaction_has_trait(
        const Transaction* transaction,
        TransactionTrait trait,
        bool* out_has_capability)
{
    ARG_CHECK(transaction);
    ARG_CHECK(trait == TRANSACTION_REQUIRES_EXPLICIT_SOURCE
            || trait == TRANSACTION_SUPPORTS_MULTIPLE_SOURCES
            || trait == TRANSACTION_SUPPORTS_MULTIPLE_DESTINATIONS
            || trait == TRANSACTION_SUPPORTS_CHANGE
            || trait == TRANSACTION_SUPPORTS_FEE);

    try
    {
        *out_has_capability = transaction->get_traits() & (1 << trait);
    }
    CATCH_EXCEPTION_RETURN_ERROR();

    return nullptr;
}

Error* transaction_get_currency(
        const Transaction* transaction, Currency* out_currency)
{
    ARG_CHECK(transaction);

    try
    {
        *out_currency = transaction->get_currency();
    }
    CATCH_EXCEPTION_RETURN_ERROR();

    return nullptr;
}

Error* transaction_add_source(
        Transaction* transaction, Properties** source)
{
    ARG_CHECK(transaction);
    ARG_CHECK(source);

    try
    {
        *source = &transaction->add_source();
    }
    CATCH_EXCEPTION_RETURN_ERROR();

    OUT_CHECK(*source);

    return nullptr;
}

Error* transaction_add_destination(
        Transaction* transaction, Properties** destination)
{
    ARG_CHECK(transaction);
    ARG_CHECK(destination);

    try
    {
        *destination = &transaction->add_destination();
    }
    CATCH_EXCEPTION_RETURN_ERROR();

    OUT_CHECK(*destination);

    return nullptr;
}

Error* transaction_get_change(
        Transaction* transaction, TransactionChange** change)
{
    ARG_CHECK(transaction);
    ARG_CHECK(change);

    try
    {
        *change = &transaction->get_change();
    }
    CATCH_EXCEPTION_RETURN_ERROR();

    OUT_CHECK(*change);

    return nullptr;
}

Error* transaction_get_fee(Transaction* transaction, TransactionFee** fee)
{
    ARG_CHECK(transaction);
    ARG_CHECK(fee);

    try
    {
        *fee = &transaction->get_fee();
    }
    CATCH_EXCEPTION_RETURN_ERROR();

    OUT_CHECK(*fee);

    return nullptr;
}

Error* transaction_get_total(Transaction* transaction, Amount** out_total)
{
    ARG_CHECK(transaction);
    ARG_CHECK(out_total);

    try
    {
        *out_total = AmountPtr(new Amount(transaction->get_total())).release();
    }
    CATCH_EXCEPTION_RETURN_ERROR();

    OUT_CHECK(*out_total);

    return nullptr;
}

Error* transaction_serialize(
        Transaction* transaction, BinaryData** out_serialized_transaction)
{
    ARG_CHECK(transaction);
    ARG_CHECK(out_serialized_transaction);

    try
    {
        *out_serialized_transaction = transaction->serialize().release();
    }
    CATCH_EXCEPTION_RETURN_ERROR();

    OUT_CHECK(*out_serialized_transaction);

    return nullptr;
}

//Error* transaction_serialize_raw(
//        const Transaction* transaction, const BinaryData** out_raw_transaction)
//{
//    ARG_CHECK(transaction);
//    ARG_CHECK(out_raw_transaction);

//    try
//    {
//    }
//    CATCH_EXCEPTION_RETURN_ERROR();

//    OUT_CHECK(*out_raw_transaction);

//    return nullptr;
//}

Error* transaction_get_hash(
        Transaction* transaction,
        struct BinaryData** out_transaction_hash)
{
    ARG_CHECK(transaction);
    ARG_CHECK(out_transaction_hash);

    try
    {
        *out_transaction_hash = transaction->get_hash().release();
    }
    CATCH_EXCEPTION_RETURN_ERROR();

    OUT_CHECK(*out_transaction_hash);

    return nullptr;
}

void free_transaction(Transaction* transaction)
{
    delete transaction;
}
