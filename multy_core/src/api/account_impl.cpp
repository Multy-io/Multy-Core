/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/src/api/account_impl.h"

#include "multy_core/src/exception.h"
#include "multy_core/src/exception_stream.h"
#include "multy_core/src/utility.h"

Account::Account()
{}

Account::~Account()
{}

void Account::change_private_key(int /*position*/, unsigned char /*byte*/)
{
    THROW_EXCEPTION2(ERROR_FEATURE_NOT_SUPPORTED, "Changing private key is not supported for Accounts of this blockchain.")
            << " Blockchain: " << multy_core::internal::to_string(get_blockchain_type()) << ".";
}

const void* Account::get_object_magic()
{
    RETURN_MAGIC();
}

HDAccount::HDAccount()
{}

HDAccount::~HDAccount()
{}

const void* HDAccount::get_object_magic()
{
    RETURN_MAGIC();
}
