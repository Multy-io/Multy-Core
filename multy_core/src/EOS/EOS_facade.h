/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_CORE_EOS_FACADE_H
#define MULTY_CORE_EOS_FACADE_H

#include "multy_core/src/blockchain_facade_base.h"

#include "multy_core/binary_data.h"
#include "multy_core/blockchain.h"

#include <string>
#include <stdint.h>

struct Account;
struct BinaryData;

namespace multy_core
{
namespace internal
{

class EosFacade : public BlockchainFacadeBase
{
public:
    EosFacade();
    ~EosFacade();

    HDAccountPtr make_hd_account(
            BlockchainType blockchain_type,
            uint32_t account_type,
            const ExtendedKey& master_key,
            uint32_t index) const override;

    AccountPtr make_account(
            BlockchainType blockchain_type,
            uint32_t account_type,
            const char* serialized_private_key) const override;

    TransactionPtr make_transaction(const Account&) const override;

    TransactionBuilderPtr make_transaction_builder(
            const Account& account,
            uint32_t type,
            const char* action) const override;

    void validate_address(BlockchainType blockchain_type, const char*) const override;

    std::string encode_serialized_transaction(
            Transaction* transaction) const override;
};

} // internal
} // multy_core

#endif // MULTY_CORE_EOS_FACADE_H
