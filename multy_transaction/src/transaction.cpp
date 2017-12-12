/* Copyright 2017 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_transaction/transaction.h"

#include "multy_transaction/internal/bitcoin_transaction.h"
#include "multy_transaction/internal/transaction.h"
#include "multy_transaction/internal/u_ptr.h"

#include "multy_core/internal/account.h"
#include "multy_core/internal/utility.h"

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
        switch (account->get_currency())
        {
            case CURRENCY_BITCOIN:
                *new_transaction = new BitcoinTransaction(*account);
                break;
            default:
                return make_error(
                        ERROR_GENERAL_ERROR, "Currency not supported yet");
        }
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
            || trait == TRANSACTION_SUPPORTS_FEE);
    ARG_CHECK(out_has_capability);

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
    ARG_CHECK(out_currency);

    try
    {
        *out_currency = transaction->get_currency();
    }
    CATCH_EXCEPTION_RETURN_ERROR();

    return nullptr;
}

Error* transaction_add_source(Transaction* transaction, Properties** source)
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

Error* transaction_get_fee(Transaction* transaction, Properties** fee)
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
Error* transaction_estimate_fee(Transaction* transaction, Amount* out_fee_estimate)
{
    ARG_CHECK(transaction);
    ARG_CHECK(out_fee_estimate);

    try
    {
        *out_fee_estimate = transaction->estimate_fee();
    }
    CATCH_EXCEPTION_RETURN_ERROR();

    return nullptr;
}

Error* transaction_get_total_fee(Transaction* transaction, Amount* out_total_fee)
{
    ARG_CHECK(transaction);
    ARG_CHECK(out_total_fee);

    try
    {
        *out_total_fee = transaction->get_total_fee();
    }
    CATCH_EXCEPTION_RETURN_ERROR();

    return nullptr;
}

Error* transaction_update(Transaction* transaction)
{
    ARG_CHECK(transaction);

    try
    {
        transaction->update_state();
    }
    CATCH_EXCEPTION_RETURN_ERROR();

    return nullptr;
}

Error* transaction_sign(Transaction* transaction)
{
    ARG_CHECK(transaction);

    try
    {
        transaction->sign();
    }
    CATCH_EXCEPTION_RETURN_ERROR();

    return nullptr;
}

Error* transaction_serialize(
        const Transaction* transaction,
        BinaryData** const out_serialized_transaction)
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

// Error* transaction_serialize_raw(
//        const Transaction* transaction, const BinaryData**
//        out_raw_transaction)
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

//Error* transaction_get_hash(
//        Transaction* transaction, struct BinaryData** out_transaction_hash)
//{
//    ARG_CHECK(transaction);
//    ARG_CHECK(out_transaction_hash);

//    try
//    {
//        *out_transaction_hash = transaction->get_hash().release();
//    }
//    CATCH_EXCEPTION_RETURN_ERROR();

//    OUT_CHECK(*out_transaction_hash);

//    return nullptr;
//}

void free_transaction(Transaction* transaction)
{
    delete transaction;
}
