/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/src/u_ptr.h"

#include "multy_core/account.h"
#include "multy_core/big_int.h"
#include "multy_core/binary_data.h"
#include "multy_core/common.h"
#include "multy_core/error.h"
#include "multy_core/key.h"
#include "multy_core/mnemonic.h"
#include "multy_core/transaction.h"

#include "multy_core/src/api/key_impl.h"

namespace multy_core
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

void UniversalDeleter::operator()(BigInt* amount) const
{
    free_big_int(amount);
}

void UniversalDeleter::operator()(BinaryData* binary_data) const
{
    free_binarydata(binary_data);
}

void UniversalDeleter::operator()(Key* key) const
{
    free_key(key);
}

void UniversalDeleter::operator()(PublicKey* key) const
{
    free_key(key);
}

void UniversalDeleter::operator()(PrivateKey* key) const
{
    free_key(key);
}

void UniversalDeleter::operator()(ExtendedKey* key) const
{
    free_extended_key(key);
}

void UniversalDeleter::operator()(HDAccount* account) const
{
    free_hd_account(account);
}

void UniversalDeleter::operator()(const char* str) const
{
    free_string(str);
}

void UniversalDeleter::operator()(char* str) const
{
    free_string(str);
}

void UniversalDeleter::operator()(Transaction* transaction) const
{
    free_transaction(transaction);
}

} // namespace internal
} // namespace multy_core
