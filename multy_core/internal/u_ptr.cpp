/* Copyright 2017 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/internal/u_ptr.h"

#include "multy_core/error.h"
#include "multy_core/common.h"
#include "multy_core/account.h"
#include "multy_core/keys.h"
#include "multy_core/mnemonic.h"

namespace wallet_core
{
namespace internal
{

void UniversalDeleter::operator()(Error* error) const
{
    free_error(error);
}

void UniversalDeleter::operator()(Account* account) const
{
    free_account(account);
}
void UniversalDeleter::operator()(BinaryData* binary_data) const
{
    free_binarydata(binary_data);
}

void UniversalDeleter::operator()(Key* key) const
{
    free_key(key);
}

void UniversalDeleter::operator()(ExtendedKey* key) const
{
    free_extended_key(key);
}

void UniversalDeleter::operator()(HDAccount* account) const
{
    free_hdaccount(account);
}

void UniversalDeleter::operator()(const char* str) const
{
    free_string(str);
}

void UniversalDeleter::operator()(char* str) const
{
    free_string(str);
}

} // namespace wallet_core
} // namespace internal
