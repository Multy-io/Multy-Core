/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_TRANSACTION_INTERNAL_UNIVERSAL_DELETER_H
#define MULTY_TRANSACTION_INTERNAL_UNIVERSAL_DELETER_H

#include "multy_transaction/api.h"

#include "multy_core/internal/u_ptr.h"

#include <memory>

struct Amount;
struct Fee;
struct Transaction;

extern "C" {
void free_amount(Amount*);
void free_transaction(Transaction*);
} // extern "C"

namespace multy_transaction
{
namespace internal
{

template <typename T, void (*f)(T*)>
struct Deleter
{
    void operator()(T* value)
    {
        f(value);
    }
};

template <typename T, void (*f)(T*)>
using UPtr = std::unique_ptr<T, Deleter<T, f>>;

typedef UPtr<Amount, free_amount> AmountPtr;
typedef UPtr<Transaction, free_transaction> TransactionPtr;

} // namespace multy_transaction
} // namespace internal

#endif // MULTY_TRANSACTION_INTERNAL_UNIVERSAL_DELETER_H
