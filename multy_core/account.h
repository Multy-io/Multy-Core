/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_CORE_ACCOUNT_H
#define MULTY_CORE_ACCOUNT_H

#include "multy_core/api.h"

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct HDAccount;
struct Account;
struct Error;
struct ExtendedKey;
struct Key;

enum Currency
{
    CURRENCY_BITCOIN,
    CURRENCY_ETHEREUM,
};

enum AddressType
{
    ADDRESS_EXTERNAL,
    ADDRESS_INTERNAL /** change-address **/
};

enum KeyType
{
    KEY_TYPE_PRIVATE,
    KEY_TYPE_PUBLIC,
};

/** Make an account of given currency with given id.
 *
 * @param master_key - master key, generated from seed.
 * @param currency - currency to use account for.
 * @param index - acccount index
 * @param account - (out) new account
 */
MULTY_CORE_API struct Error* make_hd_account(
        const struct ExtendedKey* master_key,
        enum Currency currency,
        uint32_t index,
        struct HDAccount** new_account);

/** Make a leaf HD account - the one that has an address and can be paid from/to.
 * @param base_account - base account, for which leaf is generated.
 * @param address_type - type of address for account: internal or external.
 * @param index - index of account within base account.
 * @param new_account - newly created account, must be freed by caller with
 * free_account().
 */
MULTY_CORE_API struct Error* make_hd_leaf_account(
        const struct HDAccount* base_account,
        enum AddressType address_type,
        uint32_t index,
        struct Account** new_account);

/** Make regular account from private key and currency.
 * @param currency - currency to use account for.
 * @param serialized_private_key - private key for account.
 * @param new_account - newly created account, must be freed by caller with
 * free_account().
 */
MULTY_CORE_API struct Error* make_account(
        enum Currency currency,
        const char* serialized_private_key,
        struct Account** new_account);

/** Get a key from account.
 *
 * See keys.h for key manipulation API.
 * @param account - the account to get the key from.
 * @param key_type - either KEY_TYPE_PUBLIC or KEY_TYPE_PRIVTAE.
 * @param out_key - resulting key, must be freed by caller with free_key().
 */
MULTY_CORE_API struct Error* get_account_key(
        const struct Account* account,
        enum KeyType key_type,
        struct Key** out_key);

/** Get account address as a string.
 * @param account - account.
 * @param out_address - address string, must be feed by caller with free_string().
 */
MULTY_CORE_API struct Error* get_account_address_string(
        const struct Account* account,
        const char** out_address);

/** Get account HD path as a string.
 *
 * The path might be empty if account is not an HD-account, but an imported one.
 * @param account - account.
 * @param out_address_path - HD path string, must be feed by caller with free_string().
 */
MULTY_CORE_API struct Error* get_account_address_path(
        const struct Account* account,
        const char** out_address_path);

/** Get account currency.
 * @param account - account.
 * @param out_currency - where to store the currency value.
 */
MULTY_CORE_API struct Error* get_account_currency(
        const struct Account* account,
        enum Currency* out_currency);

/** Frees HDAccount instance, can accept nullptr. **/
MULTY_CORE_API void free_hdaccount(struct HDAccount*);

/** Frees Account instance, can accept nullptr. **/
MULTY_CORE_API void free_account(struct Account*);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* MULTY_CORE_ACCOUNT_H */
