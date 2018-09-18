/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/key.h"

#include "multy_core/common.h"
#include "multy_core/error.h"

#include "multy_core/src/api/key_impl.h"
#include "multy_core/src/utility.h"

namespace
{
using namespace multy_core::internal;
} // namespace

Error* make_master_key(const BinaryData* seed, ExtendedKey** new_master_key)
{
    ARG_CHECK(seed);
    ARG_CHECK(new_master_key);

    try
    {
        *new_master_key = make_master_key(*seed).release();
    }
    CATCH_EXCEPTION_RETURN_ERROR(ERROR_SCOPE_KEY);

    OUT_CHECK(*new_master_key);

    return nullptr;
}

Error* make_user_id_from_master_key(
        const ExtendedKey* master_key,
        const char** out_user_id)
{
    ARG_CHECK_OBJECT(master_key);
    ARG_CHECK(out_user_id);
    try
    {
        *out_user_id = make_user_id_from_master_key(*master_key).release();
    }
    CATCH_EXCEPTION_RETURN_ERROR(ERROR_SCOPE_KEY);

    OUT_CHECK(*out_user_id);

    return nullptr;
}

Error* make_child_key(
        const ExtendedKey* parent_key,
        uint32_t chain_code,
        ExtendedKey** new_child_key)
{
    ARG_CHECK_OBJECT(parent_key);
    ARG_CHECK(new_child_key);

    try
    {
        *new_child_key = make_child_key(*parent_key, chain_code).release();
    }
    CATCH_EXCEPTION_RETURN_ERROR(ERROR_SCOPE_KEY);

    OUT_CHECK(*new_child_key);

    return nullptr;
}

Error* extended_key_to_string(
        const ExtendedKey* extended_key, const char** new_str)
{
    ARG_CHECK_OBJECT(extended_key);
    ARG_CHECK(new_str);

    try
    {
        *new_str = copy_string(extended_key->to_string());
    }
    CATCH_EXCEPTION_RETURN_ERROR(ERROR_SCOPE_KEY);
    OUT_CHECK(*new_str);

    return nullptr;
}

Error* key_to_string(const Key* key, const char** new_str)
{
    ARG_CHECK_OBJECT(key);
    ARG_CHECK(new_str);
    try
    {
        *new_str = copy_string(key->to_string());
    }
    CATCH_EXCEPTION_RETURN_ERROR(ERROR_SCOPE_KEY);
    OUT_CHECK(*new_str);

    return nullptr;
}

Error* sign_with_key(const Key* key, const BinaryData* data,
        BinaryData** new_signature)
{
    ARG_CHECK_OBJECT(key);
    ARG_CHECK(data != nullptr);
    ARG_CHECK(new_signature != nullptr);

    try
    {
        const PrivateKey* pk = dynamic_cast<const PrivateKey*>(key);
        if (!pk)
        {
            THROW_EXCEPTION2(ERROR_INVALID_ARGUMENT, "Key is not a PrivateKey.");
        }

        *new_signature = pk->sign(*data).release();
    }
    CATCH_EXCEPTION_RETURN_ERROR(ERROR_SCOPE_KEY);
    OUT_CHECK(*new_signature);

    return nullptr;
}

//Error* sign_with_key(
//        const Key* key, const BinaryData* data, BinaryData** new_signature)
//{
//    ARG_CHECK(key);
//    ARG_CHECK(data);
//    ARG_CHECK(new_signature);
//    try
//    {
//        *new_signature = key->sign(data).release();
//    }
//    catch (...)
//    {
//        return exception_to_error();
//    }
//    return nullptr;
//}

//Error* encrypt_with_key(
//        const Key* key, const BinaryData* data, BinaryData** new_encrypted_data)
//{
//    ARG_CHECK(key);
//    ARG_CHECK(data);
//    ARG_CHECK(new_encrypted_data);
//    try
//    {
//        *new_encrypted_data = key->encrypt(data).release();
//    }
//    catch (...)
//    {
//        return exception_to_error();
//    }
//    return nullptr;
//}

//Error* decrypt_with_key(
//        const Key* key, const BinaryData* data, BinaryData** new_decrypted_data)
//{
//    ARG_CHECK(key);
//    ARG_CHECK(data);
//    ARG_CHECK(new_decrypted_data);
//    try
//    {
//        *new_decrypted_data = key->decrypt(data).release();
//    }
//    catch (...)
//    {
//        return exception_to_error();
//    }
//    return nullptr;
//}

void free_extended_key(ExtendedKey* key)
{
    CHECK_OBJECT_BEFORE_FREE(key);
    delete key;
}

void free_key(Key* key)
{
    CHECK_OBJECT_BEFORE_FREE(key);
    delete key;
}
