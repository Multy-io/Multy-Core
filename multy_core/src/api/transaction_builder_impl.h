/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_CORE_SRC_API_TRANSACTION_BUILDER_IMPL_H
#define MULTY_CORE_SRC_API_TRANSACTION_BUILDER_IMPL_H

#include "multy_core/api.h"

#include "multy_core/src/object.h"
#include "multy_core/src/u_ptr.h"

struct Properties;

// Declared a struct (and out of multy_core::internal namespace)
// for consitency with a C-like interface.
// Exported only to make testing easier.
struct MULTY_CORE_API TransactionBuilder
        : public ::multy_core::internal::ObjectBase<TransactionBuilder>
{
public:
    typedef multy_core::internal::TransactionPtr TransactionPtr;

    TransactionBuilder();
    virtual ~TransactionBuilder();

    virtual TransactionPtr make_transaction() const = 0;
    virtual Properties& get_properties() = 0;
    virtual void validate() const = 0;

    static const void* get_object_magic();
};

#endif // MULTY_CORE_SRC_API_TRANSACTION_BUILDER_IMPL_H
