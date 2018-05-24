/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_TRANSACTION_TOKEN_ETHEREUM_H
#define MULTY_TRANSACTION_TOKEN_ETHEREUM_H

#include "multy_core/src/u_ptr.h"

#include <memory>
#include <string>

namespace multy_core
{
namespace internal
{
struct EthereumTransactionDestination;
typedef std::unique_ptr<EthereumTransactionDestination> EthereumTransactionDestinationPtr;

class EthereumSmartContractPayload
{
public:
    virtual ~EthereumSmartContractPayload();

    /**
     * Serializes smart contract call as transaction payload.
     * Uses receiver destination for getting address and token amount to transfer.
     */
    virtual BinaryDataPtr serialize(const EthereumTransactionDestination& receiver) const = 0;
    virtual EthereumTransactionDestinationPtr get_destination() = 0;
};

typedef std::unique_ptr<EthereumSmartContractPayload> EthereumSmartContractPayloadPtr;

EthereumSmartContractPayloadPtr parse_token_transfer_data(const std::string& value);

} // namespace internal
} // namespace multy_core

#endif // MULTY_TRANSACTION_TOKEN_ETHEREUM_H
