/* Copyright 2017 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_TRANSACTION_TRANSACTION_BASE_H
#define MULTY_TRANSACTION_TRANSACTION_BASE_H

#include "multy_transaction/api.h"

#include "multy_transaction/internal/transaction.h"
#include "multy_transaction/internal/properties.h"

namespace multy_transaction
{
namespace internal
{

struct MULTY_TRANSACTION_API TransactionFeeBase : public TransactionFee
{
    virtual ~TransactionFeeBase();

    Amount get_total() const override;
    Properties& get_properties() override;

    void set_total(const Amount& new_total);

protected:
    Amount m_total;
    Properties m_properties;
};

struct MULTY_TRANSACTION_API TransactionChangeBase : public TransactionChange
{
    virtual ~TransactionChangeBase();

    Amount get_total() const override;
    Properties& get_properties() override;

    void set_total(const Amount& new_total);

protected:
    Amount m_total;
    Properties m_properties;
};

class MULTY_TRANSACTION_API TransactionBase : public Transaction
{
public:
    TransactionBase(Currency currency, uint32_t traits);

    Currency get_currency() const override;
    uint32_t get_traits() const override;
    Properties& get_transaction_properties() override;

protected:
    Properties m_properties;

private:
    Currency m_currency;
    uint32_t m_traits;
};

} // namespace internal
} // namespaec multy_transaction

#endif // MULTY_TRANSACTION_TRANSACTION_BASE_H
