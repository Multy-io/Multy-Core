/* Copyright 2018 by Multy.io
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
    ARG_CHECK_OBJECT(account);
    ARG_CHECK(new_transaction);

    try
    {
        switch (account->get_currency())
        {
            case CURRENCY_BITCOIN:
                *new_transaction = new BitcoinTransaction(*account);
                break;
            default:
                return MAKE_ERROR(
                        ERROR_GENERAL_ERROR, "Currency not supported yet");
        }
    }
    CATCH_EXCEPTION_RETURN_ERROR();
    OUT_CHECK_OBJECT(*new_transaction);

    return nullptr;
}

Error* transaction_has_trait(
        const Transaction* transaction,
        TransactionTrait trait,
        bool* out_has_capability)
{
    ARG_CHECK_OBJECT(transaction);
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
    ARG_CHECK_OBJECT(transaction);
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
    ARG_CHECK_OBJECT(transaction);
    ARG_CHECK(source);

    try
    {
        *source = &transaction->add_source();
    }
    CATCH_EXCEPTION_RETURN_ERROR();

    OUT_CHECK_OBJECT(*source);

    return nullptr;
}

Error* transaction_add_destination(
        Transaction* transaction, Properties** destination)
{
    ARG_CHECK_OBJECT(transaction);
    ARG_CHECK(destination);

    try
    {
        *destination = &transaction->add_destination();
    }
    CATCH_EXCEPTION_RETURN_ERROR();

    OUT_CHECK_OBJECT(*destination);

    return nullptr;
}

Error* transaction_get_fee(Transaction* transaction, Properties** fee)
{
    ARG_CHECK_OBJECT(transaction);
    ARG_CHECK(fee);

    try
    {
        *fee = &transaction->get_fee();
    }
    CATCH_EXCEPTION_RETURN_ERROR();

    OUT_CHECK_OBJECT(*fee);

    return nullptr;
}

Error* transaction_estimate_total_fee(
        const Transaction* transaction,
        size_t sources_count,
        size_t destinations_count,
        Amount** out_fee_estimate)
{
    ARG_CHECK_OBJECT(transaction);
    ARG_CHECK(out_fee_estimate);
    ARG_CHECK(sources_count>0);
    ARG_CHECK(destinations_count>0);

    try
    {
        AmountPtr result(new Amount);
        *result = transaction->estimate_total_fee(sources_count, destinations_count);
        *out_fee_estimate = result.release();
    }
    CATCH_EXCEPTION_RETURN_ERROR();
    OUT_CHECK_OBJECT(*out_fee_estimate);

    return nullptr;
}

Error* transaction_get_total_fee(const Transaction* transaction, Amount** out_total_fee)
{
    ARG_CHECK_OBJECT(transaction);
    ARG_CHECK(out_total_fee);

    try
    {
        AmountPtr result(new Amount);
        *result = transaction->get_total_fee();
        *out_total_fee = result.release();
    }
    CATCH_EXCEPTION_RETURN_ERROR();
    OUT_CHECK_OBJECT(*out_total_fee);

    return nullptr;
}

Error* transaction_update(Transaction* transaction)
{
    ARG_CHECK_OBJECT(transaction);

    try
    {
        transaction->update_state();
    }
    CATCH_EXCEPTION_RETURN_ERROR();

    return nullptr;
}

Error* transaction_sign(Transaction* transaction)
{
    ARG_CHECK_OBJECT(transaction);

    try
    {
        transaction->sign();
    }
    CATCH_EXCEPTION_RETURN_ERROR();

    return nullptr;
}

Error* transaction_serialize(
        const Transaction* transaction,
        BinaryData** out_serialized_transaction)
{
    ARG_CHECK_OBJECT(transaction);
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
