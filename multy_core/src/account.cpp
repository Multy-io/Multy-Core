/* Copyright 2017 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/account.h"

#include "multy_core/common.h"
#include "multy_core/error.h"
#include "multy_core/keys.h"

#include "multy_core/internal/account_base.h"
#include "multy_core/internal/bitcoin_account.h"
#include "multy_core/internal/ethereum_account.h"
#include "multy_core/internal/key.h"
#include "multy_core/internal/utility.h"

#include <memory>
#include <string>

namespace
{
using namespace wallet_core::internal;
} // namespace

Error* make_hd_account(
        const ExtendedKey* master_key,
        Currency currency,
        uint32_t index,
        HDAccount** new_account)
{
    ARG_CHECK(master_key != nullptr);
    ARG_CHECK(index < HARDENED_INDEX_BASE);
    ARG_CHECK(new_account != nullptr);

    try
    {
        switch (currency)
        {
            case CURRENCY_BITCOIN:
            {
                *new_account = new BitcoinHDAccount(*master_key, index);
                break;
            }
            case CURRENCY_ETHEREUM:
            {
                *new_account = new EthereumHDAccount(*master_key, index);
                break;
            }
            default:
            {
                return make_error(
                        ERROR_GENERAL_ERROR, "Currency not supported yet");
            }
        }
    }
    catch (...)
    {
        return exception_to_error();
    }
    OUT_CHECK(*new_account);

    return nullptr;
}

Error* make_hd_leaf_account(
        const HDAccount* base_account,
        AddressType address_type,
        uint32_t index,
        Account** new_account)
{
    ARG_CHECK(base_account);
    ARG_CHECK(address_type == ADDRESS_INTERNAL
            || address_type == ADDRESS_EXTERNAL);
    ARG_CHECK(index < HARDENED_INDEX_BASE);
    ARG_CHECK(new_account);

    try
    {
        *new_account = base_account->make_leaf_account(address_type, index)
                               .release();
    }
    catch (...)
    {
        return exception_to_error();
    }
    OUT_CHECK(*new_account);

    return nullptr;
}

Error* make_account(
        Currency currency,
        const char* serialized_private_key,
        Account** new_account)
{
    ARG_CHECK(currency == CURRENCY_BITCOIN || currency == CURRENCY_ETHEREUM);
    ARG_CHECK(serialized_private_key);
    ARG_CHECK(new_account);

    try
    {
        // TODO: make a factory
        switch (currency)
        {
            case CURRENCY_BITCOIN:
            {
                *new_account = make_bitcoin_account(serialized_private_key)
                                       .release();
                break;
            }
            case CURRENCY_ETHEREUM:
            {
                *new_account = make_ethereum_account(serialized_private_key)
                        .release();
                break;
            }
            default:
            {
                return make_error(
                        ERROR_GENERAL_ERROR, "Currency not supported yet");
            }
        }
    }
    catch (...)
    {
        return exception_to_error();
    }
    OUT_CHECK(*new_account);

    return nullptr;
}

Error* get_account_key(const Account* account, KeyType key_type, Key** out_key)
{
    ARG_CHECK(account != nullptr);
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
    catch (...)
    {
        return exception_to_error();
    }
    OUT_CHECK(*out_key);

    return nullptr;
}

Error* get_account_address_string(
        const Account* account, const char** out_address)
{
    ARG_CHECK(account);
    ARG_CHECK(out_address);

    try
    {
        *out_address = copy_string(account->get_address());
    }
    catch (...)
    {
        return exception_to_error();
    }
    OUT_CHECK(*out_address);

    return nullptr;
}

Error* get_account_address_path(
        const Account* account, const char** out_address_path)
{
    ARG_CHECK(account);
    ARG_CHECK(out_address_path);

    try
    {
        *out_address_path = copy_string(to_string(account->get_path()));
    }
    catch (...)
    {
        return exception_to_error();
    }
    OUT_CHECK(*out_address_path);

    return nullptr;
}

Error* get_account_currency(const Account* account, Currency* out_currency)
{
    ARG_CHECK(account);
    ARG_CHECK(out_currency);

    try
    {
        *out_currency = account->get_currency();
    }
    catch (...)
    {
        return exception_to_error();
    }

    return nullptr;
}

void free_hdaccount(HDAccount* account)
{
    delete account;
}

void free_account(Account* account)
{
    delete account;
}
