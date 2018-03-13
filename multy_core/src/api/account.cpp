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
#include "multy_core/src/bitcoin/bitcoin_account.h"
#include "multy_core/src/ethereum/ethereum_account.h"
#include "multy_core/src/golos/golos_account.h"
#include "multy_core/src/exception.h"
#include "multy_core/src/exception_stream.h"
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
        switch (blockchain_type.blockchain)
        {
            case BLOCKCHAIN_BITCOIN:
            {
                *new_account = new BitcoinHDAccount(blockchain_type, *master_key, index);
                break;
            }
            case BLOCKCHAIN_ETHEREUM:
            {
                *new_account = new EthereumHDAccount(blockchain_type, *master_key, index);
                break;
            }
            case BLOCKCHAIN_GOLOS:
            {
                *new_account = new GolosHDAccount(blockchain_type, *master_key, index);
                break;
            }
            default:
            {
                return MAKE_ERROR(
                        ERROR_GENERAL_ERROR, "Blockchain not supported yet");
            }
        }
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
        Blockchain blockchain,
        const char* serialized_private_key,
        Account** new_account)
{
    ARG_CHECK(blockchain == BLOCKCHAIN_BITCOIN
            || blockchain == BLOCKCHAIN_ETHEREUM
            || blockchain == BLOCKCHAIN_GOLOS);
    ARG_CHECK(serialized_private_key);
    ARG_CHECK(new_account);

    try
    {
        // TODO: make a factory
        switch (blockchain)
        {
            case BLOCKCHAIN_BITCOIN:
            {
                *new_account = make_bitcoin_account(serialized_private_key)
                                       .release();
                break;
            }
            case BLOCKCHAIN_ETHEREUM:
            {
                *new_account = make_ethereum_account(serialized_private_key)
                        .release();
                break;
            }
            case BLOCKCHAIN_GOLOS:
            {
                *new_account = make_golos_account(serialized_private_key)
                        .release();
                break;
            }
            default:
            {
                THROW_EXCEPTION("Blockchain not supported yet.");
            }
        }
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

Error* validate_address(BlockchainType blockchain_type, const char* address)
{
    ARG_CHECK(address);
    ARG_CHECK(blockchain_type.blockchain == BLOCKCHAIN_BITCOIN
            || blockchain_type.blockchain == BLOCKCHAIN_ETHEREUM);

    try
    {
        switch (blockchain_type.blockchain)
        {
            case BLOCKCHAIN_BITCOIN:
            {
                BitcoinAddressType address_type;
                BitcoinNetType net_type;
                bitcoin_parse_address(address, &net_type, &address_type);
                if (address_type != BITCOIN_ADDRESS_P2PKH)
                {
                    THROW_EXCEPTION("BTC: Only P2PKH addresses are supported for now.");
                }
                if (net_type != blockchain_type.net_type)
                {
                    THROW_EXCEPTION("Incompatitable net_type.")
                            << " Requested: " << blockchain_type.net_type
                            << ", address net type:" << net_type;
                }
                break;
            }
            case BLOCKCHAIN_ETHEREUM:
            {
                THROW_EXCEPTION("ETH addresses are not supported yet.");
            }
            default:
            {
                return MAKE_ERROR(
                        ERROR_GENERAL_ERROR, "Blockchain not supported yet");
            }
        }
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
