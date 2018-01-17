/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/src/api/transaction_impl.h"

Transaction::Transaction()
{
}

Transaction::~Transaction()
{
}

const void* Transaction::get_object_magic()
{
    RETURN_MAGIC();
}
