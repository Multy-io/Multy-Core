/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/src/eos/eos_transaction.h"

#include "multy_core/eos.h"
#include "multy_core/src/eos/eos_account.h"
#include "multy_core/src/eos/eos_name.h"
#include "multy_core/src/eos/eos_binary_stream.h"
#include "multy_core/src/eos/eos_transaction_action.h"
#include "multy_core/src/eos/eos_transaction_transfer_action.h"

#include "multy_core/binary_data.h"
#include "multy_core/blockchain.h"

#include "multy_core/src/api/key_impl.h"
#include "multy_core/src/api/properties_impl.h"
#include "multy_core/src/property_predicates.h"
#include "multy_core/src/blockchain_facade_base.h"
#include "multy_core/src/binary_data_utility.h"
#include "multy_core/src/codec.h"
#include "multy_core/src/exception.h"
#include "multy_core/src/exception_stream.h"
#include "multy_core/src/utility.h"

#include "third-party/portable_endian.h"

#include <chrono>
#include <cstddef>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace {
using namespace multy_core::internal;

const uint32_t EOS_TIME_CONFIRM_TRANSACTION = 30; // seconds
const EosChainId EOS_TESTNET_CHAIN_ID = {0x03, 0x8f, 0x4b, 0x0f, 0xc8, 0xff, 0x18, 0xa4, 0xf0, 0x84, 0x2a, 0x8f, 0x05, 0x64, 0x61, 0x1f,
                                                      0x6e, 0x96, 0xe8, 0x53, 0x59, 0x01, 0xdd, 0x45, 0xe4, 0x3a, 0xc8, 0x69, 0x1a, 0x1c, 0x4d, 0xca};
const EosChainId EOS_MAINNET_CHAIN_ID = {0xac, 0xa3, 0x76, 0xf2, 0x06, 0xb8, 0xfc, 0x25, 0xa6, 0xed, 0x44, 0xdb, 0xdc, 0x66, 0x54, 0x7c,
                                                      0x36, 0xc6, 0xc3, 0x3e, 0x3a, 0x11, 0x9f, 0xfb, 0xea, 0xef, 0x94, 0x36, 0x42, 0xf0, 0xe9, 0x06};
const std::array<uint8_t, 32> EOS_ZERO_SHA256 = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

} // namespace


namespace multy_core
{
namespace internal
{

class EosTransactionSource : public TransactionSourceBase
{
public:
    explicit EosTransactionSource(BlockchainType blockchain_type)
        : address(m_properties, "address", Property::REQUIRED,
                [blockchain_type](const std::string& new_address) {
                    get_blockchain(BLOCKCHAIN_EOS)
                            .validate_address(blockchain_type,
                                    new_address.c_str());
                }),
          amount(m_properties, "amount", Property::OPTIONAL,
                &verify_bigger_than<BigInt, 0>)
    {}

public:
    PropertyT<std::string> address;
    PropertyT<BigInt> amount;
};

class EosTransactionDestination : public TransactionDestinationBase
{
public:
    explicit EosTransactionDestination(BlockchainType blockchain_type)
        : address(m_properties, "address", Property::REQUIRED,
                [blockchain_type](const std::string& new_address) {
                    get_blockchain(BLOCKCHAIN_EOS)
                            .validate_address(blockchain_type,
                                    new_address.c_str());
                }),
          amount(m_properties, "amount",
                 Property::OPTIONAL, &verify_bigger_than<BigInt, 1>)
    {}

public:
    PropertyT<std::string> address;
    PropertyT<BigInt> amount;
};

EosTransaction::EosTransaction(const Account& account)
    : TransactionBase(account.get_blockchain_type()),
      m_account(account),
      m_message(new BinaryData{nullptr, 0}),
      m_source(),
      m_destination(),
      m_explicit_expiration(
            get_transaction_properties(),
            "expiration",
            Property::OPTIONAL,
            [this](const std::string& new_expiration)
            {
                this->set_expiration(new_expiration);
            }),
      m_ref_block_num(
            get_transaction_properties(),
            "block_num",
            Property::REQUIRED),
      m_ref_block_prefix(
            get_transaction_properties(),
            "ref_block_prefix",
            Property::REQUIRED,
            verify_smaller_than<BigInt, std::numeric_limits<uint32_t>::max()>)
{
}

EosTransaction::~EosTransaction()
{
}

void EosTransaction::set_expiration(const std::string& new_expiration)
{
     m_expiration = parse_iso8601_string(new_expiration) + EOS_TIME_CONFIRM_TRANSACTION;
}

void EosTransaction::verify()
{
}

void EosTransaction::update()
{
    if (m_external_actions.empty())
    {
        if (!m_source)
        {
            THROW_EXCEPTION2(ERROR_TRANSACTION_NO_SOURCES,
                    "EOS transaction should have one source.");
        }

        if (!m_destination)
        {
            THROW_EXCEPTION2(ERROR_TRANSACTION_NO_DESTINATIONS,
                    "EOS transaction should have one destination.");
        }

        m_actions.clear();
        m_actions.push_back(EosTransactionActionPtr(new EosTransactionTransferAction(
                *m_source->address,
                *m_destination->address,
                *m_destination->amount,
                *m_message)));
    }

    sign();
}

void EosTransaction::sign()
{
    EosBinaryStream stream;

    serialize_to_stream(stream, SERIALIZE_FOR_SIGN);

    BinaryDataPtr signature = m_account.get_private_key()->sign(stream.get_content());
    m_signature.swap(signature);

}

BinaryDataPtr EosTransaction::serialize()
{
    update();

    EosBinaryStream data_stream;
    serialize_to_stream(data_stream, SERIALIZE);


    return make_clone(data_stream.get_content());
}

std::string EosTransaction::encode_serialized()
{
    update();

    CharPtr out_str;
    THROW_IF_WALLY_ERROR(
            wally_base58_from_bytes(
                    m_signature->data, m_signature->len,
                    0, reset_sp(out_str)),
                "Failed to base58-encode signature.");
    std::string sig_base58 = "SIG_K1_";
    sig_base58 += out_str.get();

    EosBinaryStream data_stream;
    serialize_to_stream(data_stream, SERIALIZE);

    std::string result = "{\"signatures\":[\"";
    result += sig_base58;
    result += "\"],\"packed_trx\":\"";
    result += to_hex_string(data_stream.get_content());
    result += "\",\"compression\":\"none\",\"packed_context_free_data\":\"\"}";

    return result;
}

void EosTransaction::serialize_to_stream(EosBinaryStream& stream, SerializationMode mode) const
{
    EosBinaryStream list;
    if (mode == SERIALIZE_FOR_SIGN)
    {
        if (m_account.get_blockchain_type().net_type == EOS_NET_TYPE_MAINNET)
        {
            list << EOS_MAINNET_CHAIN_ID;
        }
        else
        {
             list << EOS_TESTNET_CHAIN_ID;
        }
    }

    list << static_cast<uint32_t>(m_expiration); // time as uint32_t
    list << static_cast<uint16_t>(static_cast<uint32_t>(*m_ref_block_num));
    uint32_t ref_block_prefix;
    ref_block_prefix = static_cast<uint32_t>(m_ref_block_prefix.get_value().get_value_as_uint64());
    list << ref_block_prefix;
    list << static_cast<uint8_t>(0x00); // max_net_usage_words, we set zero byte
    list << static_cast<uint8_t>(0x00); // max_cpu_usage_ms, we set zero byte
    list << static_cast<uint8_t>(0x00); // delay_seconds, we set zero byte
    list << static_cast<uint8_t>(0x00); // size array context_free_actions and context_free_actions

    const auto& actions = m_external_actions.empty() ? m_actions : m_external_actions;
    list << static_cast<uint8_t>(actions.size());
    for (const auto& action : actions)
    {
        list << *action;
    }

    list << static_cast<uint8_t>(0x00);

    if (mode == SERIALIZE_FOR_SIGN)
    {
        list << EOS_ZERO_SHA256; // SHA256 context_free_data
    }

    stream << list.get_content();
}

BigInt EosTransaction::get_total_fee() const
{
    return BigInt(0);
}

BigInt EosTransaction::get_total_spent() const
{
    if (!m_destination)
    {
        THROW_EXCEPTION("Failed to calculate total spent.")
                << " Transaction has no destinations.";
    }

    return *m_destination->amount;
}

BigInt EosTransaction::estimate_total_fee(
        size_t /*sources_count*/,
        size_t /*destinations_count*/) const
{
    return BigInt(0);
}

Properties& EosTransaction::add_source()
{
    if (m_source)
    {
        THROW_EXCEPTION2(ERROR_TRANSACTION_TOO_MANY_SOURCES,
                "EOS transaction can have only one source.");
    }

    m_source = EosTransactionSourcePtr(new EosTransactionSource(
            get_blockchain_type()));

    return m_source->get_properties();
}

Properties& EosTransaction::add_destination()
{
    if (m_destination)
    {
        THROW_EXCEPTION2(ERROR_TRANSACTION_TOO_MANY_DESTINATIONS,
                "EOS transaction can have only one destination.");
    }

    m_destination = EosTransactionDestinationPtr(
            new EosTransactionDestination(get_blockchain_type()));

    return m_destination->get_properties();
}

Properties& EosTransaction::get_fee()
{
    THROW_EXCEPTION2(ERROR_FEATURE_NOT_SUPPORTED,
            "EOS transaction fee is not customizable.");
}

void EosTransaction::set_message(const BinaryData& payload)
{
    INVARIANT(payload.data != nullptr);
    if (payload.len > 255)
    {
        THROW_EXCEPTION2(ERROR_TRANSACTION_PAYLOAD_TO_BIG, "Message is to big.")
                << " Max message size is 255 bytes,"
                << " got: " << payload.len << " bytes.";
    }

    m_message = make_clone(payload);
}

void EosTransaction::set_action(EosTransactionActionPtr action)
{
    INVARIANT(action != nullptr);

    m_external_actions.emplace_back(std::move(action));
}

} // namespace internal
} // namespace multy_core
