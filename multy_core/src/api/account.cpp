/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/account.h"

#include "multy_core/common.h"
#include "multy_core/error.h"
#include "multy_core/key.h"

#include "multy_core/src/account_base.h"
#include "multy_core/src/api/key_impl.h"
#include "multy_core/src/blockchain_facade_base.h"
#include "multy_core/src/utility.h"

#include <memory>
#include <string>

namespace
{
using namespace multy_core::internal;
} // namespace

Error* make_hd_account(
        const ExtendedKey* master_key,
        BlockchainType blockchain_type,
        uint32_t index,
        HDAccount** new_account)
{
    ARG_CHECK(master_key != nullptr);
    ARG_CHECK(index < HARDENED_INDEX_BASE);
    ARG_CHECK(new_account != nullptr);

    try
    {
        *new_account = get_blockchain(blockchain_type.blockchain)
                .make_hd_account(blockchain_type, *master_key, index).release();
    }
    CATCH_EXCEPTION_RETURN_ERROR();

    OUT_CHECK_OBJECT(*new_account);

    return nullptr;
}

Error* make_hd_leaf_account(
        const HDAccount* base_account,
        AddressType address_type,
        uint32_t index,
        Account** new_account)
{
    ARG_CHECK_OBJECT(base_account);
    ARG_CHECK(address_type == ADDRESS_INTERNAL
            || address_type == ADDRESS_EXTERNAL);
    ARG_CHECK(index < HARDENED_INDEX_BASE);
    ARG_CHECK(new_account);

    try
    {
        *new_account = base_account->make_leaf_account(address_type, index)
                               .release();
    }
    CATCH_EXCEPTION_RETURN_ERROR();

    OUT_CHECK_OBJECT(*new_account);

    return nullptr;
}

Error* make_account(
        BlockchainType blockchain,
        const char* serialized_private_key,
        Account** new_account)
{
    ARG_CHECK(blockchain.blockchain == BLOCKCHAIN_BITCOIN
            || blockchain.blockchain == BLOCKCHAIN_ETHEREUM
            || blockchain.blockchain == BLOCKCHAIN_GOLOS);
    ARG_CHECK(serialized_private_key);
    ARG_CHECK(new_account);

    try
    {
        *new_account = get_blockchain(blockchain.blockchain)
                .make_account(blockchain, serialized_private_key).release();
    }
    CATCH_EXCEPTION_RETURN_ERROR();

    OUT_CHECK_OBJECT(*new_account);

    return nullptr;
}

Error* account_get_key(const Account* account, KeyType key_type, Key** out_key)
{
    ARG_CHECK_OBJECT(account);
    ARG_CHECK(key_type == KEY_TYPE_PRIVATE || key_type == KEY_TYPE_PUBLIC);
    ARG_CHECK(out_key != nullptr);

    try
    {
        if (key_type == KEY_TYPE_PRIVATE)
        {
            *out_key = account->get_private_key().release();
        }
        else
        {
            *out_key = account->get_public_key().release();
        }
    }
    CATCH_EXCEPTION_RETURN_ERROR();

    OUT_CHECK(*out_key);

    return nullptr;
}

Error* account_get_address_string(
        const Account* account, const char** out_address)
{
    ARG_CHECK_OBJECT(account);
    ARG_CHECK(out_address);

    try
    {
        *out_address = copy_string(account->get_address());
    }
    CATCH_EXCEPTION_RETURN_ERROR();

    OUT_CHECK(*out_address);

    return nullptr;
}

Error* account_get_address_path(
        const Account* account, const char** out_address_path)
{
    ARG_CHECK_OBJECT(account);
    ARG_CHECK(out_address_path);

    try
    {
        *out_address_path = copy_string(to_string(account->get_path()));
    }
    CATCH_EXCEPTION_RETURN_ERROR();

    OUT_CHECK(*out_address_path);

    return nullptr;
}

Error* account_get_blockchain_type(
        const Account* account,
        BlockchainType* out_blockchain_type)
{
    ARG_CHECK_OBJECT(account);
    ARG_CHECK(out_blockchain_type);

    try
    {
        *out_blockchain_type = account->get_blockchain_type();
    }
    CATCH_EXCEPTION_RETURN_ERROR();

    return nullptr;
}

void free_hd_account(HDAccount* account)
{
    CHECK_OBJECT_BEFORE_FREE(account);
    delete account;
}

void free_account(Account* account)
{
    CHECK_OBJECT_BEFORE_FREE(account);
    delete account;
}
