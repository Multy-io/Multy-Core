/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/src/bitcoin/bitcoin_transaction_base.h"

#include "multy_core/bitcoin.h"

#include "multy_core/src/property_predicates.h"
#include "multy_core/src/u_ptr.h"
#include "multy_core/src/utility.h"
#include "multy_core/src/bitcoin/bitcoin_account.h"
#include "multy_core/src/bitcoin/bitcoin_stream.h"
#include "multy_core/src/bitcoin/bitcoin_opcode.h"

#include "multy_core/binary_data.h"

namespace
{
using namespace multy_core::internal;

BinaryDataPtr make_script_pub_key_from_address(const BitcoinNetType expected_net_type, const std::string& address)
{
    BitcoinNetType net_type;
    BitcoinAddressType address_type;
    BinaryDataPtr binary_address = bitcoin_parse_address(address.c_str(),
        &net_type, &address_type);
    INVARIANT(binary_address != nullptr);

    if (address_type != BITCOIN_ADDRESS_P2PKH && address_type != BITCOIN_ADDRESS_P2SH)
    {
        THROW_EXCEPTION2(ERROR_INVALID_ADDRESS, "Unsupported address type.")
                << " Address type: " << address_type;
    }

    if (expected_net_type != net_type)
    {
        THROW_EXCEPTION2(ERROR_INVALID_ADDRESS, "Wrong net type for address.")
                << " Expected: " << expected_net_type
                << " actual:" << net_type;
    }

    return make_script_pub_key(*binary_address, address_type);
}

} // namespace

namespace multy_core
{
namespace internal
{

BinaryDataPtr make_script_pub_key(const BinaryData& public_key_hash, BitcoinAddressType address_type)
{
    BitcoinDataStream sig_stream;
    if (address_type == BITCOIN_ADDRESS_P2PKH)
    {
        sig_stream << OP_DUP;
        sig_stream << OP_HASH160;

        sig_stream << as_compact_size(public_key_hash.len);
        sig_stream << public_key_hash;

        sig_stream << OP_EQUALVERIFY;
        sig_stream << OP_CHECKSIG;
    }
    else if (address_type == BITCOIN_ADDRESS_P2SH)
    {
        sig_stream << OP_HASH160;

        sig_stream << as_compact_size(public_key_hash.len);
        sig_stream << public_key_hash;

        sig_stream << OP_EQUAL;
    }
    return make_clone(sig_stream.get_content());
}

BitcoinTransactionDestinationBase::BitcoinTransactionDestinationBase(BitcoinNetType net_type)
    : amount(m_properties,
             "amount",
             Property::REQUIRED,
             &verify_bigger_than<BigInt, 0, ERROR_INVALID_ARGUMENT>),
      address(m_properties,
              "address",
              Property::REQUIRED,
              [this](const std::string &new_address) {
                    this->sig_script = make_script_pub_key_from_address(this->m_net_type, new_address);
              }),
      is_change(false, m_properties, "is_change", Property::OPTIONAL,
              [this](int32_t new_value) {
                  if (new_value < 0)
                  {
                      THROW_EXCEPTION2(ERROR_INVALID_ARGUMENT,
                            "is_change can't be negative.");
                  }
                  this->on_change_set(new_value);
              }),
      sig_script(),
      m_net_type(net_type)
{
}

void BitcoinTransactionDestinationBase::on_change_set(bool new_value)
{
    // amount is optional for change-address destination.
    amount.set_trait(new_value ? Property::READONLY : Property::REQUIRED);
}

BitcoinTransactionDestinationBase::~BitcoinTransactionDestinationBase()
{
}

BitcoinTransactionSourceBase::BitcoinTransactionSourceBase()
    : prev_transaction_hash(
          m_properties,
          "prev_tx_hash",
          Property::REQUIRED,
          [](const BinaryData& new_tx_out_hash) {
              if (new_tx_out_hash.len != 32)
              {
                  THROW_EXCEPTION2(
                          ERROR_TRANSACTION_SOURCE_INVALID_PREV_TX_HASH,
                          "Previous transaction hash should be"
                          "exactly 32 bytes long.");
              }
          }),
      prev_transaction_out_index(
          m_properties,
          "prev_tx_out_index",
          Property::REQUIRED),
      prev_transaction_out_script_pubkey(
          m_properties,
          "prev_tx_out_script_pubkey",
          Property::REQUIRED),
      private_key(
          m_properties,
          "private_key",
          Property::REQUIRED),
      sequence(static_cast<int32_t>(BITCOIN_INPUT_SEQ_FINAL),
                      m_properties, "sequence", Property::OPTIONAL),
      amount(m_properties, "amount", Property::REQUIRED),
      script_signature()
{
}

BitcoinTransactionSourceBase::~BitcoinTransactionSourceBase()
{
}

BitcoinTransactionFeeBase::BitcoinTransactionFeeBase()
    : amount_per_byte(m_properties,
                      "amount_per_byte",
                      Property::OPTIONAL,
                      &verify_bigger_than<BigInt, 1, ERROR_TRANSACTION_FEE_TOO_LOW>)
{
}

const BigInt& BitcoinTransactionFeeBase::get_amount_per_byte() const
{
    return amount_per_byte.get_value();
}

void BitcoinTransactionFeeBase::validate_fee(const BigInt &leftover, uint64_t transaction_size) const
{
    const BigInt min_transaction_fee(amount_per_byte.get_value() * transaction_size);

    if (leftover < min_transaction_fee)
    {
        THROW_EXCEPTION2(ERROR_TRANSACTION_FEE_TOO_LOW,
                "Transaction total fee is too low. ")
                << leftover.get_value() << " < " << min_transaction_fee.get_value();
    }
}

BitcoinTransactionBase::BitcoinTransactionBase(BlockchainType blockchain_type)
    : TransactionBase(blockchain_type),
      m_sources(),
      m_destinations(),
      m_fee(new BitcoinTransactionFeeBase)
{
}

BitcoinTransactionBase::~BitcoinTransactionBase()
{
}

BigInt BitcoinTransactionBase::get_total_fee() const
{
    return calculate_diff();
}

Properties& BitcoinTransactionBase::get_fee()
{
    return m_fee->get_properties();
}

BitcoinNetType BitcoinTransactionBase::get_net_type() const
{
    return static_cast<BitcoinNetType>(get_blockchain_type().net_type);
}

void BitcoinTransactionBase::verify() const
{
    if (m_sources.empty())
    {
        THROW_EXCEPTION2(ERROR_TRANSACTION_NO_SOURCES,
                "Transaction should have at least one source.");
    }

    if (m_destinations.empty())
    {
        THROW_EXCEPTION2(ERROR_TRANSACTION_NO_DESTINATIONS,
                "Transaction should have at least one destination.");
    }

    std::string missing_properties;
    if (!validate_all_properties(&missing_properties))
    {
        THROW_EXCEPTION2(ERROR_NOT_ALL_REQUIRED_PROPERTIES_SET,
                "Not all required properties set.")
                << "\n" << missing_properties << ".";
    }

    const BigInt diff = calculate_diff();
    if (diff < 0)
    {
        THROW_EXCEPTION2(ERROR_TRANSACTION_INSUFFICIENT_FUNDS,
                "Transaction is trying to spend more than available in inputs")
                << " : available - spent = "<< diff.get_value();
    }
}

BigInt BitcoinTransactionBase::calculate_diff() const
{
    BigInt available(0);
    for (const auto& s: m_sources)
    {
        available += *s->amount;
    }

    BigInt total_spent(0);
    for (const auto& d: m_destinations)
    {
        total_spent += *d->amount;
    }

    return available -= total_spent;
}
} // namespace internal
} // namespace multy_core
