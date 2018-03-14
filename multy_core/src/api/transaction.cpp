/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/transaction.h"

#include "multy_core/src/api/account_impl.h"
#include "multy_core/src/api/transaction_impl.h"
#include "multy_core/src/bitcoin/bitcoin_transaction.h"
#include "multy_core/src/ethereum/ethereum_transaction.h"
#include "multy_core/src/utility.h"

namespace
{
using namespace multy_core::internal;
} // namespace

Error* make_transaction(const Account* account, Transaction** new_transaction)
{
    ARG_CHECK_OBJECT(account);
    ARG_CHECK(new_transaction);

    try
    {
        const BlockchainType blockchain_type = account->get_blockchain_type();
        switch (blockchain_type.blockchain)
        {
            // TODO: use factory to create a transactions.
            case BLOCKCHAIN_BITCOIN:
                *new_transaction = new BitcoinTransaction(blockchain_type);
                break;
            case BLOCKCHAIN_ETHEREUM:
                *new_transaction = new EthereumTransaction(*account);
                break;
            default:
                return MAKE_ERROR(
                        ERROR_GENERAL_ERROR, "Blockchain not supported yet");
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

Error* transaction_get_blockchain_type(
        const Transaction* transaction, BlockchainType* out_blockchain_type)
{
    ARG_CHECK_OBJECT(transaction);
    ARG_CHECK(out_blockchain_type);

    try
    {
        *out_blockchain_type = transaction->get_blockchain_type();
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
Error* transaction_set_message(Transaction* transaction, const BinaryData* message)
{
    ARG_CHECK_OBJECT(transaction);
    ARG_CHECK(message);
    ARG_CHECK(message->data && message->len);

    try
    {
        transaction->set_message(*message);
    }
    CATCH_EXCEPTION_RETURN_ERROR();

    return nullptr;
}

Error* transaction_get_properties(Transaction* transaction,
        Properties** transaction_properties)
{
    ARG_CHECK_OBJECT(transaction);
    ARG_CHECK(transaction_properties);

    try
    {
        *transaction_properties = &transaction->get_transaction_properties();
    }
    CATCH_EXCEPTION_RETURN_ERROR();

    OUT_CHECK_OBJECT(*transaction_properties);

    return nullptr;
}


Error* transaction_estimate_total_fee(
        const Transaction* transaction,
        size_t sources_count,
        size_t destinations_count,
        BigInt** out_fee_estimate)
{
    ARG_CHECK_OBJECT(transaction);
    ARG_CHECK(out_fee_estimate);
    ARG_CHECK(sources_count>0);
    ARG_CHECK(destinations_count>0);

    try
    {
        BigIntPtr result(new BigInt);
        *result = transaction->estimate_total_fee(sources_count, destinations_count);
        *out_fee_estimate = result.release();
    }
    CATCH_EXCEPTION_RETURN_ERROR();
    OUT_CHECK_OBJECT(*out_fee_estimate);

    return nullptr;
}

Error* transaction_get_total_fee(const Transaction* transaction, BigInt** out_total_fee)
{
    ARG_CHECK_OBJECT(transaction);
    ARG_CHECK(out_total_fee);

    try
    {
        BigIntPtr result(new BigInt);
        *result = transaction->get_total_fee();
        *out_total_fee = result.release();
    }
    CATCH_EXCEPTION_RETURN_ERROR();
    OUT_CHECK_OBJECT(*out_total_fee);

    return nullptr;
}

Error* transaction_get_total_spent(
        const Transaction* transaction,
        BigInt** out_total_spent)
{
    ARG_CHECK_OBJECT(transaction);
    ARG_CHECK(out_total_spent);

    try
    {
        BigIntPtr result(new BigInt);
        *result = transaction->get_total_spent();
        *out_total_spent = result.release();
    }
    CATCH_EXCEPTION_RETURN_ERROR();
    OUT_CHECK_OBJECT(*out_total_spent);

    return nullptr;
}

Error* transaction_update(Transaction* transaction)
{
    ARG_CHECK_OBJECT(transaction);

    try
    {
        transaction->update();
    }
    CATCH_EXCEPTION_RETURN_ERROR();

    return nullptr;
}

Error* transaction_serialize(
        Transaction* transaction,
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

void free_transaction(Transaction* transaction)
{
    CHECK_OBJECT_BEFORE_FREE(transaction);
    delete transaction;
}
