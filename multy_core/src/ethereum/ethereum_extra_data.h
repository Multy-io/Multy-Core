/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_TRANSACTION_ETHEREUM_EXTRA_DATA_H
#define MULTY_TRANSACTION_ETHEREUM_EXTRA_DATA_H

#include "multy_core/src/transaction_base.h"
#include "multy_core/src/ethereum/ethereum_account.h"
#include "multy_core/src/api/properties_impl.h"
#include "multy_core/src/api/big_int_impl.h"
#include "multy_core/src/utility.h"

#include <string>

namespace multy_core
{
namespace internal
{

struct EthereumTransactionDestination : public TransactionDestinationBase
{
    EthereumTransactionDestination()
        : string_address(get_properties(), "address", Property::REQUIRED,
            [this](const std::string& new_address)
            {
                this->address = ethereum_parse_address(new_address.c_str());
            }),
          amount(get_properties(), "amount"),
          address()
    {}

    EthereumTransactionDestination& operator=(const EthereumTransactionDestination& other)
    {
        string_address.set_value(other.string_address.get_value());
        amount.set_value(other.amount.get_value());
        address = make_clone(*other.address);

        return *this;
    }

public:
    PropertyT<std::string> string_address;
    PropertyT<BigInt> amount;
    BinaryDataPtr address;
};

struct EthereumTransactionSource : public TransactionSourceBase
{
    EthereumTransactionSource()
        : amount(get_properties(), "amount")
    {}

    PropertyT<BigInt> amount;
};

struct EthereumTransactionFee : public TransactionFeeBase
{
    EthereumTransactionFee()
        : gas_price(get_properties(), "gas_price"),
          gas_limit(get_properties(), "gas_limit", Property::OPTIONAL),
          total_fee()
    {}

    PropertyT<BigInt> gas_price;
    PropertyT<BigInt> gas_limit;

    BigInt total_fee;
};



} // namespace internal
} // namespace multy_core

#endif // MULTY_TRANSACTION_ETHEREUM_EXTRA_DATA_H
