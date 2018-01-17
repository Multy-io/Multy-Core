/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_CORE_TRANSACTION_BASE_H
#define MULTY_CORE_TRANSACTION_BASE_H

#include "multy_core/api.h"

#include "multy_core/src/api/transaction_impl.h"
#include "multy_core/src/api/properties_impl.h"

namespace multy_core
{
namespace internal
{

class MULTY_CORE_API TransactionFeeBase
{
public:
    TransactionFeeBase();

    Properties& get_properties();

protected:
    ~TransactionFeeBase();

protected:
    Properties m_properties;
};

class MULTY_CORE_API TransactionDestinationBase
{
public:
    TransactionDestinationBase();

    Properties& get_properties();

protected:
    ~TransactionDestinationBase();

protected:
    Properties m_properties;
};

class MULTY_CORE_API TransactionSourceBase
{
public:
    TransactionSourceBase();

    Properties& get_properties();

protected:
    ~TransactionSourceBase();

protected:
    Properties m_properties;
};

class MULTY_CORE_API TransactionBase : public Transaction
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
} // namespaec multy_core

#endif // MULTY_CORE_TRANSACTION_BASE_H
