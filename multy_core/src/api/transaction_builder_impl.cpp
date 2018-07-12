/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/src/api/transaction_builder_impl.h"

TransactionBuilder::TransactionBuilder()
{}

TransactionBuilder::~TransactionBuilder()
{}

const void* TransactionBuilder::get_object_magic()
{
    RETURN_MAGIC();
}
