/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/src/api/account_impl.h"

Account::Account()
{}

Account::~Account()
{}

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
