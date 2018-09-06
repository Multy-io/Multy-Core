/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/src/api/transaction_impl.h"

#include "multy_core/src/exception.h"

#include <string>

Transaction::Transaction()
{
}

Transaction::~Transaction()
{
}

std::string  Transaction::encode_serialized()
{
    THROW_EXCEPTION("Not implemented");
}

const void* Transaction::get_object_magic()
{
    RETURN_MAGIC();
}
