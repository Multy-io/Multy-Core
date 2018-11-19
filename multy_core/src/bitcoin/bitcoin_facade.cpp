/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/src/bitcoin/bitcoin_facade.h"

#include "multy_core/bitcoin.h"

#include "multy_core/src/error_utility.h"
#include "multy_core/src/exception.h"
#include "multy_core/src/exception_stream.h"
#include "multy_core/src/bitcoin/bitcoin_account.h"
#include "multy_core/src/bitcoin/bitcoin_transaction.h"
#include "multy_core/src/bitcoin/bitcoin_transaction_segwit.h"
#include "multy_core/src/utility.h"

#include "multy_core/src/codec.h"

namespace
{
using namespace multy_core::internal;

BitcoinAccountType to_bitcoin_account_type(uint32_t account_type)
{
    switch (account_type)
    {
        case BITCOIN_ACCOUNT_DEFAULT:
            return BITCOIN_ACCOUNT_DEFAULT;
        case BITCOIN_ACCOUNT_SEGWIT:
            return BITCOIN_ACCOUNT_SEGWIT;
        default:
            THROW_EXCEPTION2(ERROR_INVALID_ARGUMENT,
                    "Unknown Bitcoin Account type.")
                    << " Got: " << account_type << ".";
    }

    return BITCOIN_ACCOUNT_DEFAULT;
}

} // namespace

namespace multy_core
{
namespace internal
{

BitcoinFacade::BitcoinFacade()
{
}

BitcoinFacade::~BitcoinFacade()
{
}

HDAccountPtr BitcoinFacade::make_hd_account(
        BlockchainType blockchain_type,
        uint32_t account_type,
        const ExtendedKey& master_key,
        uint32_t index) const
{
    return HDAccountPtr(new BitcoinHDAccount(
            blockchain_type,
            to_bitcoin_account_type(account_type),
            master_key,
        index));
}

AccountPtr BitcoinFacade::make_account(
        BlockchainType blockchain_type,
        uint32_t account_type,
        const char* serialized_private_key) const
{
    AccountPtr account = make_bitcoin_account(serialized_private_key,
            to_bitcoin_account_type(account_type));
    if (account->get_blockchain_type() != blockchain_type)
    {
        THROW_EXCEPTION("Private key doesn't match requested blockchain type.")
                << " Private key type:" << to_string(account->get_blockchain_type())
                << " Requested blockchain type:" << to_string(blockchain_type) << ".";
    }

    return account;
}

TransactionPtr BitcoinFacade::make_transaction(const Account& account) const
{
    const BitcoinAccount& bitcoin_account = static_cast<const BitcoinAccount&>(account);
    if (bitcoin_account.get_account_type() == BITCOIN_ACCOUNT_SEGWIT)
    {
        return TransactionPtr(new BitcoinTransactionSegWit(account.get_blockchain_type()));
    }
    else if (bitcoin_account.get_account_type() == BITCOIN_ACCOUNT_DEFAULT)
    {
        return TransactionPtr(new BitcoinTransaction(account.get_blockchain_type()));
    }
    else
    {
        THROW_EXCEPTION("Unknown account type");
    }
}

void BitcoinFacade::validate_address(BlockchainType blockchain_type, const char* address) const
{
    INVARIANT(address);

    BitcoinAddressType address_type;
    BitcoinNetType net_type;
    bitcoin_parse_address(address, &net_type, &address_type);
    if (address_type != BITCOIN_ADDRESS_P2PKH && address_type != BITCOIN_ADDRESS_P2SH)
    {
        THROW_EXCEPTION2(ERROR_INVALID_ADDRESS, "Not supported address type.")
                << " Only P2PKH and P2SH are supported, "
                << " given address type: " << address_type;
    }
    if (net_type != blockchain_type.net_type)
    {
        THROW_EXCEPTION2(ERROR_INVALID_ADDRESS, "Incompatitable net_type.")
                << " Requested: " << blockchain_type.net_type
                << ", address net type:" << net_type;
    }
}

std::string BitcoinFacade::encode_serialized_transaction(
        Transaction* transaction) const
{
    INVARIANT(transaction != nullptr);
    const BinaryDataPtr serialized_transaction = transaction->serialize();
    return encode(*serialized_transaction, CODEC_HEX);
}

} // internal
} // multy_core
