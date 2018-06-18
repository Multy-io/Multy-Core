/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/src/bitcoin/bitcoin_transaction.h"

#include "multy_core/common.h"

#include "multy_core/src/api/account_impl.h"
#include "multy_core/src/api/key_impl.h"
#include "multy_core/src/codec.h"
#include "multy_core/src/exception_stream.h"
#include "multy_core/src/api/properties_impl.h"
#include "multy_core/src/bitcoin/bitcoin_account.h"
#include "multy_core/src/exception.h"
#include "multy_core/src/hash.h"
#include "multy_core/src/u_ptr.h"
#include "multy_core/src/utility.h"
#include "multy_core/src/bitcoin/bitcoin_opcode.h"
#include "multy_core/src/bitcoin/bitcoin_stream.h"
#include "multy_core/src/bitcoin/bitcoin_transaction_base.h"
#include "multy_core/src/property_predicates.h"

#include "wally_crypto.h"

#include "third-party/portable_endian.h"

#include <algorithm>
#include <limits>
#include <sstream>
#include <string.h>
#include <unordered_map>

namespace
{
using namespace multy_core::internal;
const size_t BITCOIN_MAX_MESSAGE_LENGTH = 75;
const size_t BITCOIN_DUST_RELAY_FEE_PER_BYTE = 3;
const uint64_t BITCOIN_AVERAGE_OUTPUT_AND_INPUT_SIZE_NON_SEGWIT = 182;
const uint64_t BITCOIN_AVERAGE_OUTPUT_AND_INPUT_SIZE_SEGWIT = 98;
const uint32_t BITCOIN_INPUT_SEQ_FINAL = 0xFFFFFFFF;

// BIP-125: Any value less than (BITCOIN_INPUT_SEQ_FINAL - 1) would do, see
// https://github.com/bitcoin/bips/blob/master/bip-0125.mediawiki
const uint32_t BITCOIN_INPUT_SEQ_REPLACEABLE = BITCOIN_INPUT_SEQ_FINAL - 2;

bool is_dust_amount(const BigInt& amount, bool is_segwit)
{
    // NOTE: "Dust" is defined in terms of BITCOIN_DUST_RELAY_FEE_PER_BYTE,
    // which has units satoshis-per-byte.
    // If you'd pay more in fees than the value of the output
    // to spend something, then we consider it dust.
    // A typical spendable non-segwit txout is 34 bytes big, and will
    // need a CTxIn of at least 148 bytes to spend:
    // so dust is a spendable txout less than
    // 182*BITCOIN_DUST_RELAY_FEE_PER_BYTE (in satoshis).
    // 546 satoshis at the default rate of 3 sat/B.
    // A typical spendable segwit txout is 31 bytes big, and will
    // need a CTxIn of at least 67 bytes to spend:
    // so dust is a spendable txout less than
    // 98*BITCOIN_DUST_RELAY_FEE_PER_BYTE (in satoshis).
    // 294 satoshis at the default rate of 3 sat/B.
    // Look for more info: https://github.com/bitcoin/bitcoin/blob/e057589dc67f25da6779b60d0e247a3730adbc6d/src/policy/policy.cpp#L18

    const uint64_t nsize = is_segwit ? BITCOIN_AVERAGE_OUTPUT_AND_INPUT_SIZE_SEGWIT: BITCOIN_AVERAGE_OUTPUT_AND_INPUT_SIZE_NON_SEGWIT;
    if (amount < BigInt(nsize * BITCOIN_DUST_RELAY_FEE_PER_BYTE))
    {
        return true;
    }
    return false;
}

} // namespace

namespace multy_core
{
namespace internal
{

BitcoinTransactionDestination::BitcoinTransactionDestination(BitcoinNetType net_type)
    : BitcoinTransactionDestinationBase(net_type)
{
}

BitcoinTransactionSource::BitcoinTransactionSource()
    : BitcoinTransactionSourceBase()
{
}

BitcoinTransactionSource::~BitcoinTransactionSource()
{
}

BitcoinTransaction::BitcoinTransaction(BlockchainType blockchain_type)
    : BitcoinTransactionBase(blockchain_type),
      m_version(1),
      m_is_segwit_transaction(0),
      m_lock_time(0),
      m_is_replaceable(1, get_transaction_properties(),
                "is_replaceable", Property::OPTIONAL),
      m_message()
{
    register_properties("", m_fee->get_properties());
}

BinaryDataPtr BitcoinTransaction::serialize()
{
    update();
    sign();

    BitcoinDataStream data_stream;

    serialize_to_stream(&data_stream, WITH_POSITIVE_CHANGE_AMOUNT);
    return make_clone(data_stream.get_content());
}

template <typename T>
void BitcoinTransaction::serialize_to_stream(T* stream, DestinationsToUse destinations_to_use) const
{
    // nVersion
    *stream << m_version;
    // txins
    *stream << as_compact_size(m_sources.size());
    for (const auto& source : m_sources)
    {
        *stream << *(static_cast<const BitcoinTransactionSource*>(source.get()));
    }
    // txouts
    const auto destinations = get_non_zero_destinations(destinations_to_use);
    *stream << as_compact_size(destinations.size());
    for (const auto& destination : destinations)
    {
        *stream << *destination;
    }
    // nLockTime
    *stream << m_lock_time;
}

bool BitcoinTransaction::is_segwit() const
{
    return m_is_segwit_transaction != 0;
}

BitcoinTransaction::Destinations BitcoinTransaction::get_non_zero_destinations(DestinationsToUse destinations_to_use) const
{
    // TODO: include also change destination and payload-only destination.
    Destinations result;
    if (m_message)
    {
        result.push_back(m_message.get());
    }

    for (const auto& dest : m_destinations)
    {
        //const BitcoinTransactionDestinationPtr destin = static_cast<const BitcoinTransactionDestinationPtr>(dest);
        if ( *dest->amount > BigInt(0) ||
             (destinations_to_use == WITH_NONPOSITIVE_CHANGE_AMOUNT ? *dest->is_change : 0) )
        {
            result.push_back(dest.get());
        }
    }
    return result;

}

size_t BitcoinTransaction::estimate_transaction_size() const
{
    // Estimate size of the transaction based on number of sources and
    // destinations.
    // Note that this estimation is valid only for non-segwit transactions.
    const size_t sources_count = m_sources.size();
    const size_t destinations_count = get_non_zero_destinations(WITH_NONPOSITIVE_CHANGE_AMOUNT).size();
    // look function estimate_total_fee
    return static_cast<int64_t>(
            sources_count * (150 + 32) + destinations_count * 34 + 10);
}

BigInt BitcoinTransaction::get_total_spent() const
{
    BigInt total_spent(0);
    for (const auto& d : m_destinations)
    {
        if (!*d->is_change)
        {
            total_spent += *d->amount;
        }
    }

    return total_spent + get_total_fee();
}

BigInt BitcoinTransaction::estimate_total_fee(
        size_t sources_count, size_t destinations_count) const
{
    //change size input because public key may be in uncompressed format
    //compressed and uncompressed format differs in 32 byte
    const int64_t transaction_size
            = sources_count * (150 + 32) + destinations_count * 34 + 10;
    return transaction_size * m_fee->get_amount_per_byte();
}

void BitcoinTransaction::verify() const
{
    BitcoinTransactionBase::verify();

    size_t source_index = 0;
    for (const auto& s : m_sources)
    {
        const auto& public_key = s->private_key->make_public_key();
        std::array<uint8_t, HASH160_LEN> public_key_hash;
        BinaryData public_key_hash_data = as_binary_data(public_key_hash);
        bitcoin_hash_160(public_key->get_content(), &public_key_hash_data);
        BinaryDataPtr sig_script = make_script_pub_key(public_key_hash_data, BITCOIN_ADDRESS_P2PKH);

        if (*sig_script != **s->prev_transaction_out_script_pubkey)
        {
            AccountPtr error_account = make_bitcoin_account(
                    s->private_key->to_string().c_str(),
                    // TODO: get account type from initial account, and put it here.
                    BITCOIN_ACCOUNT_DEFAULT);

            THROW_EXCEPTION2(ERROR_TRANSACTION_INVALID_PRIVATE_KEY,
                    "Source can't be spent using given private key.")
                    << " Source index: " << source_index
                    << ", corresponding address: "<< error_account->get_address()
                    << ", given prev_tx_out_script_pubkey: "
                    << encode(**s->prev_transaction_out_script_pubkey, CODEC_HEX)
                    << ".";
        }
        ++source_index;
    }
    if (m_blockchain_type.net_type == BITCOIN_NET_TYPE_MAINNET)
    {
        for (const auto& d : m_destinations)
        {
            if (!*d->is_change && is_dust_amount(d->amount.get_value(), false))
            {
                THROW_EXCEPTION2(ERROR_TRANSACTION_TRANSFER_AMOUNT_TOO_SMALL,
                        // Destination # amount is considered dust and will be rejected by the node.
                        "Bitcoin dust output.");
            }
        }
    }
}


void BitcoinTransaction::update()
{
    verify();

    size_t change_destinations_count = 0;
    BitcoinTransactionDestination* change_destination = nullptr;
    for (const auto& source : m_sources)
    {
        if (m_is_replaceable.get_value())
        {
            *source->sequence = BITCOIN_INPUT_SEQ_REPLACEABLE;
        }
        else
        {
            *source->sequence = BITCOIN_INPUT_SEQ_FINAL;
        }
    }

    for (const auto& d : m_destinations)
    {
        if (*d->is_change)
        {
            change_destination = static_cast<BitcoinTransactionDestination*>(d.get());
            ++change_destinations_count;
        }
    }
    if (change_destinations_count > 1)
    {
        THROW_EXCEPTION2(ERROR_TRANSACTION_TOO_MANY_CHANGE_DESTINATIONS,
                "Transaction should have only one change address.")
                << " Currenctly there are: " << change_destinations_count;
    }

    // In order to compute transaction total cost we'll have to serialize tx.
    if (change_destination)
    {
        change_destination->amount.get_value() = BigInt{0};
        uint64_t tx_size = get_transaction_serialized_size(WITH_NONPOSITIVE_CHANGE_AMOUNT);
        BigInt expected_fee = tx_size * *m_fee->amount_per_byte;
        BigInt current_fee = calculate_diff();
        const BigInt remainder = current_fee - expected_fee;

        // NOTE: Not adding a change if output is an "dust". cost of spending money from that output exceeds its value.
        //
        // Most of the nodes reject TX with fee less than 1 Satoshi per byte.
        // So we assume that cost of spending an output is roughly equivalent
        // to byte length of corresponding input in serialized TX.
        // Average input and output size is:
        //  * 182 bytes for non-SegWit TX
        //  * 98 bytes for SegWit TX
        //
        // Not adding change for leftovers less than said limits increases fee for this TX.

        if (!is_dust_amount(remainder, false))
        {
            // not setting value with set_value(), since that would fail for read-only property.
            change_destination->amount.get_value() += remainder;

            tx_size = get_transaction_serialized_size(WITH_POSITIVE_CHANGE_AMOUNT);
            expected_fee = tx_size * *m_fee->amount_per_byte;
            current_fee = calculate_diff();
            while (expected_fee > current_fee)
            {
                // NOTE: get shortage fee from change_destination amount
                //      diff_fee is always negative here.
                const BigInt diff_fee = current_fee - expected_fee;
                change_destination->amount.get_value() += diff_fee;
                // Recalculate transaction fee after changes in transaction
                tx_size = get_transaction_serialized_size(WITH_POSITIVE_CHANGE_AMOUNT);
                expected_fee = tx_size * *m_fee->amount_per_byte;
                current_fee = calculate_diff();
            }

            if (is_dust_amount(change_destination->amount.get_value(), false))
            {
                change_destination->amount.get_value() = BigInt{0};
            }
        }
    }

    m_fee->validate_fee(calculate_diff(), get_transaction_serialized_size(WITH_POSITIVE_CHANGE_AMOUNT));

    if (get_non_zero_destinations(WITH_POSITIVE_CHANGE_AMOUNT).size() == 0)
    {
        THROW_EXCEPTION2(ERROR_TRANSACTION_CHANGE_IS_TOO_SMALL_AND_NO_OTHER_DESTINATIONS,
                "Transaction change is to small because of high fee.");
    }
}

uint64_t BitcoinTransaction::get_transaction_serialized_size(DestinationsToUse destinations_to_use)
{
    sign();
    BitcoinBytesCountStream counter_stream;
    serialize_to_stream(&counter_stream, destinations_to_use);
    return counter_stream.get_bytes_count();
}

void BitcoinTransaction::sign()
{
    // Sign inputs:
    // for every input:
    //      reset sig script with nullptr
    // for every input:
    //      set sig_script with prev_tx_pubscript
    //      serialize transaction
    //      save sig script to the variable
    // once signing is done
    // for every input:
    //      reset sig script from variable

    // for all inputs but the one being signed, sig script should be empty.
    for (auto& source : m_sources)
    {
        source->script_signature.reset();
    }

    // Sign all inputs one by one and save sig_scripts for later.
    std::unordered_map<BitcoinTransactionSourceBase*, BinaryDataPtr> sig_scripts;
    for (auto& source : m_sources)
    {
        source->script_signature.swap(
                source->prev_transaction_out_script_pubkey.get_value());
        BinaryDataPtr sig_script;

        {
            BitcoinDataStream hash_stream;
            serialize_to_stream(&hash_stream, WITH_POSITIVE_CHANGE_AMOUNT);
            hash_stream << uint32_t(1); // signature version

            const PrivateKeyPtr& private_key = source->private_key.get_value();
            BinaryDataPtr signature
                    = private_key->sign(hash_stream.get_content());

            BitcoinDataStream sig_script_stream;
            sig_script_stream << as_compact_size(signature->len + 1);
            sig_script_stream << *signature;
            sig_script_stream << uint8_t(1); // hash-code type

            const PublicKeyPtr& public_key = private_key->make_public_key();
            const BinaryData& public_key_data = public_key->get_content();
            sig_script_stream << as_compact_size(public_key_data.len);
            sig_script_stream << public_key_data;

            sig_script = make_clone(sig_script_stream.get_content());
        }
        source->script_signature.swap(
                source->prev_transaction_out_script_pubkey.get_value());

        const auto p = sig_scripts.emplace(source.get(), std::move(sig_script));
        INVARIANT(p.second == true);
    }

    // restore all sig scripts
    for (auto& source : m_sources)
    {
        const auto p = sig_scripts.find(source.get());
        INVARIANT(p != sig_scripts.end());

        source->script_signature.swap(p->second);
    }
}

Properties& BitcoinTransaction::add_source()
{
    BitcoinTransactionSourcePtr source(new BitcoinTransactionSource());
    m_sources.emplace_back(std::move(source));
    return register_properties(
            make_id("#", m_sources.size() - 1),
            m_sources.back()->get_properties());
}

Properties& BitcoinTransaction::add_destination()
{
    BitcoinTransactionDestinationPtr destination(
            new BitcoinTransactionDestination(get_net_type()));
    m_destinations.emplace_back(std::move(destination));
    return register_properties(
            make_id("#", m_destinations.size() - 1),
            m_destinations.back()->get_properties());
}

void BitcoinTransaction::set_message(const BinaryData& value)
{
    if (value.len > BITCOIN_MAX_MESSAGE_LENGTH)
    {
        THROW_EXCEPTION2(ERROR_TRANSACTION_PAYLOAD_TO_BIG,
                "Transaction message is too big.")
                << " Max length: " << BITCOIN_MAX_MESSAGE_LENGTH
                << " actual length: " << value.len << ".";
    }
    if (!m_message)
    {
        m_message.reset(new BitcoinTransactionDestination(get_net_type()));
        m_message->address.set_trait(Property::READONLY);
        m_message->amount.set_trait(Property::READONLY);
    }
    BitcoinDataStream sig_stream;
    sig_stream << OP_RETURN;
    sig_stream << as_compact_size(value.len);
    sig_stream.write_data(value.data, value.len);
    m_message->sig_script = make_clone(sig_stream.get_content());
}

} // namespace internal
} // namespaec multy_core
