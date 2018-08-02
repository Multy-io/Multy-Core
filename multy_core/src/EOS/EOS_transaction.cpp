/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/src/EOS/EOS_transaction.h"

#include "multy_core/EOS.h"
#include "multy_core/src/EOS/EOS_account.h"
#include "multy_core/src/EOS/eos_name.h"

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

typedef std::array<uint8_t, 32> EOSChainId;

const uint8_t EOS_PRECISION = 4;
const std::array<uint8_t, 7> EOS_TOKEN_NAME = {0x45, 0x4f, 0x53, 0x00, 0x00, 0x00, 0x00};
const uint32_t EOS_TIME_CONFIRM_TRANSACTION = 30; // seconds
const EOSChainId EOS_TESTNET_CHAIN_ID = {0x03, 0x8f, 0x4b, 0x0f, 0xc8, 0xff, 0x18, 0xa4, 0xf0, 0x84, 0x2a, 0x8f, 0x05, 0x64, 0x61, 0x1f,
                                                      0x6e, 0x96, 0xe8, 0x53, 0x59, 0x01, 0xdd, 0x45, 0xe4, 0x3a, 0xc8, 0x69, 0x1a, 0x1c, 0x4d, 0xca};
const EOSChainId EOS_MAINNET_CHAIN_ID = {0xac, 0xa3, 0x76, 0xf2, 0x06, 0xb8, 0xfc, 0x25, 0xa6, 0xed, 0x44, 0xdb, 0xdc, 0x66, 0x54, 0x7c,
                                                      0x36, 0xc6, 0xc3, 0x3e, 0x3a, 0x11, 0x9f, 0xfb, 0xea, 0xef, 0x94, 0x36, 0x42, 0xf0, 0xe9, 0x06};
const std::array<uint8_t, 32> EOS_ZERO_SHA256 = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

} // namespace


namespace multy_core
{
namespace internal
{

class EOSAuthorization
{
public:
    EOSAuthorization(const std::string& actor, const std::string& permission)
        : m_actor(actor), m_permission(permission)
    {
    }

    EosName get_actor() const
    {
        return m_actor;
    }

    EosName get_permission() const
    {
        return m_permission;
    }

private:
    const EosName m_actor;
    const EosName m_permission;
};

class EOSBinaryStream
{
public:
    EOSBinaryStream()
        : m_data()
    {}

    void write_data(const uint8_t* data, const size_t len)
    {
        INVARIANT(data != nullptr);

        m_data.insert(m_data.end(), data, data + len);
    }

    BinaryData get_content() const
    {
        return as_binary_data(m_data);
    }

private:
    std::vector<unsigned char> m_data;
};

class EOSTransactionAction
{
public:
    virtual ~EOSTransactionAction() = default;

    enum ActionType
    {
        TRANSFER
    };

    virtual ActionType get_type() const = 0;
    virtual void write_to_stream(EOSBinaryStream* /*stream*/) const = 0;
    virtual BinaryDataPtr make_data() const = 0;

    std::string get_type_name() const
    {
        static const std::unordered_map<size_t, std::string> NAMES =
        {
            {TRANSFER, "transfer"},
        };

        const ActionType type = get_type();
        auto p = NAMES.find(type);
        INVARIANT(p != NAMES.end());

        return p->second;
    }
};

template <typename T>
EOSBinaryStream& write_as_data(const T& data, EOSBinaryStream& stream)
{
    stream.write_data(
                reinterpret_cast<const uint8_t*>(&data), sizeof(data));
    return stream;
}

EOSBinaryStream& operator<<(EOSBinaryStream& stream, const EOSTransactionAction& op)
{
    op.write_to_stream(&stream);

    return stream;
}

EOSBinaryStream& operator<<(EOSBinaryStream& stream, const BinaryData& value)
{
    INVARIANT(value.data !=  nullptr);
    stream.write_data(value.data, value.len);

    return stream;
}

EOSBinaryStream& operator<<(EOSBinaryStream& stream, const uint8_t& value)
{
    stream.write_data(&value, 1);

    return stream;
}

EOSBinaryStream& operator<<(EOSBinaryStream& stream, const uint16_t& value)
{
    return write_as_data(htole16(value), stream);
}

EOSBinaryStream& operator<<(EOSBinaryStream& stream, const uint32_t& value)
{
    return write_as_data(htole32(value), stream);
}


EOSBinaryStream& operator<<(EOSBinaryStream& stream, const uint64_t& value)
{
    return write_as_data(htole64(value), stream);
}

EOSBinaryStream& operator<<(EOSBinaryStream& stream, const EosName& value)
{
    stream << value.get_data();

    return stream;
}

EOSBinaryStream& operator<<(EOSBinaryStream& stream, const EOSAuthorization& value)
{
    stream << value.get_actor();
    stream << value.get_permission();

    return stream;
}

EOSBinaryStream& operator<<(EOSBinaryStream& stream, const EOSChainId& value)
{
    stream.write_data(value.data(), value.size());

    return stream;
}

class EOSTransactionTransferAction : public EOSTransactionAction
{
public:
    EOSTransactionTransferAction(
            const std::string& from,
            const std::string& to,
            const BigInt amount,
            const BinaryData& memo)
        : m_from(from),
          m_to(to),
          m_amount(std::move(amount)),
          m_memo()
    {
        m_memo = make_clone(memo);
        m_authorizations.push_back(EOSAuthorization(from, "active"));
    }

    ActionType get_type() const override
    {
        return TRANSFER;
    }

    void write_to_stream(EOSBinaryStream* stream) const override
    {
        *stream << EosName("eosio.token");
        *stream << EosName("transfer");
        *stream << static_cast<uint8_t>(m_authorizations.size());
        for (const auto& authorization: m_authorizations)
        {
            *stream << authorization;
        }
        const auto& data = make_data();
        *stream << static_cast<uint8_t>(data->len);
        *stream << *data;
    }


    BinaryDataPtr make_data() const override
    {
        EOSBinaryStream data;
        data << m_from;
        data << m_to;
        data << m_amount.get_value_as_uint64();
        data << EOS_PRECISION;
        data << as_binary_data(EOS_TOKEN_NAME);
        data << static_cast<uint8_t>(m_memo->len);
        if (m_memo->len > 0)
        {
            data << *m_memo;
        }

        return make_clone(data.get_content());
    }


public:
    EosName m_from;
    EosName m_to;
    BigInt m_amount;
    BinaryDataPtr m_memo;
    std::vector<EOSAuthorization> m_authorizations;
};

class EOSTransactionSource : public TransactionSourceBase
{
public:
    explicit EOSTransactionSource(BlockchainType blockchain_type)
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

class EOSTransactionDestination : public TransactionDestinationBase
{
public:
    explicit EOSTransactionDestination(BlockchainType blockchain_type)
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

EOSTransaction::EOSTransaction(const Account& account)
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

EOSTransaction::~EOSTransaction()
{
}

void EOSTransaction::set_expiration(const std::string& new_expiration)
{
     m_expiration = parse_iso8601_string(new_expiration) + EOS_TIME_CONFIRM_TRANSACTION;
}

void EOSTransaction::verify()
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

}

void EOSTransaction::update()
{
    verify();
    m_actions.clear();
    m_actions.push_back(EOSTransactionActionPtr(new EOSTransactionTransferAction(
            *m_source->address,
            *m_destination->address,
            *m_destination->amount,
            *m_message)));
    sign();
}

void EOSTransaction::sign()
{
    EOSBinaryStream stream;

    serialize_to_stream(stream, SERIALIZE_FOR_SIGN);

    BinaryDataPtr signature = m_account.get_private_key()->sign(stream.get_content());
    m_signature.swap(signature);

}

BinaryDataPtr EOSTransaction::serialize()
{
    update();

    EOSBinaryStream data_stream;
    serialize_to_stream(data_stream, SERIALIZE);


    return make_clone(data_stream.get_content());
}

std::string EOSTransaction::encode_serialized()
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

    EOSBinaryStream data_stream;
    serialize_to_stream(data_stream, SERIALIZE);

    std::string result = "{\"signatures\":[\"";
    result += sig_base58;
    result += "\"],\"packed_trx\":\"";
    result += to_hex_string(data_stream.get_content());
    result += "\",\"compression\":\"none\",\"packed_context_free_data\":\"\"}";

    return result;
}


void EOSTransaction::serialize_to_stream(EOSBinaryStream& stream, SerializationMode mode) const
{
    EOSBinaryStream list;
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

    list << static_cast<uint8_t>(m_actions.size());
    list << *m_actions.at(0);

    list << static_cast<uint8_t>(0x00);

    if (mode == SERIALIZE_FOR_SIGN)
    {
        list << EOS_ZERO_SHA256; // SHA256 context_free_data
    }

    stream << list.get_content();
}

BigInt EOSTransaction::get_total_fee() const
{
    return BigInt(0);
}

BigInt EOSTransaction::get_total_spent() const
{
    if (!m_destination)
    {
        THROW_EXCEPTION("Failed to calculate total spent.")
                << " Transaction has no destinations.";
    }

    return *m_destination->amount;
}

BigInt EOSTransaction::estimate_total_fee(
        size_t /*sources_count*/,
        size_t /*destinations_count*/) const
{
    return BigInt(0);
}

Properties& EOSTransaction::add_source()
{
    if (m_source)
    {
        THROW_EXCEPTION2(ERROR_TRANSACTION_TOO_MANY_SOURCES,
                "EOS transaction can have only one source.");
    }

    m_source = EOSTransactionSourcePtr(new EOSTransactionSource(
            get_blockchain_type()));

    return m_source->get_properties();
}

Properties& EOSTransaction::add_destination()
{
    if (m_destination)
    {
        THROW_EXCEPTION2(ERROR_TRANSACTION_TOO_MANY_DESTINATIONS,
                "EOS transaction can have only one destination.");
    }

    m_destination = EOSTransactionDestinationPtr(
            new EOSTransactionDestination(get_blockchain_type()));

    return m_destination->get_properties();
}

Properties& EOSTransaction::get_fee()
{
    THROW_EXCEPTION2(ERROR_FEATURE_NOT_SUPPORTED,
            "EOS transaction fee is not customizable.");
}

void EOSTransaction::set_message(const BinaryData& value)
{
    if (value.len > 255)
    {
        THROW_EXCEPTION2(ERROR_TRANSACTION_PAYLOAD_TO_BIG, "Message is to big.")
                << " Max message size is 255 bytes,"
                << " got: " << value.len << " bytes.";
    }

    m_message = make_clone(value);
}

} // namespace internal
} // namespace multy_core
