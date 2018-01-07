/* Copyright 2018 by Multy.io
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

class MULTY_TRANSACTION_API TransactionBase : public Transaction
{
public:
    TransactionBase(Currency currency, uint32_t traits);

    Currency get_currency() const override;
    uint32_t get_traits() const override;
    Properties& get_transaction_properties() override;

protected:
    bool validate_all_properties(std::string* not_set_properties) const;
    Properties& register_properties(const std::string& name, Properties&);
    Properties m_properties;

private:
    Currency m_currency;
    uint32_t m_traits;
    std::vector<std::pair<std::string, Properties*>> m_all_properties;
};

} // namespace internal
} // namespaec multy_transaction

#endif // MULTY_TRANSACTION_TRANSACTION_BASE_H
