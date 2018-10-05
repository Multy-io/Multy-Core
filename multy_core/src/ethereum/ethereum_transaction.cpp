/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/src/ethereum/ethereum_transaction.h"

#include "multy_core/src/ethereum/ethereum_token.h"
#include "multy_core/src/ethereum/ethereum_extra_data.h"
#include "multy_core/src/exception_stream.h"
#include "multy_core/src/api/key_impl.h"
#include "multy_core/src/binary_data_utility.h"
#include "multy_core/src/utility.h"

namespace
{
using namespace multy_core::internal;

// based on cpp-ethereum/libdevcore/RLP.h
const uint8_t RLP_MAX_LENGTH_BYTES = 8;
const uint8_t RLP_DATA_IMM_LEN_START = 0x80;
const uint8_t RLP_LIST_IMM_LEN_START = 0xc0;
const uint8_t RLP_DATA_IMM_LEN_COUNT = RLP_LIST_IMM_LEN_START - RLP_DATA_IMM_LEN_START - RLP_MAX_LENGTH_BYTES;
const uint8_t RLP_DATA_IND_LEN_ZERO = RLP_DATA_IMM_LEN_START + RLP_DATA_IMM_LEN_COUNT - 1;
const uint8_t RLP_LIST_IMM_LEN_COUNT = 256 - RLP_LIST_IMM_LEN_START - RLP_MAX_LENGTH_BYTES;
const uint8_t RLP_LIST_IND_LEN_ZERO = RLP_LIST_IMM_LEN_START + RLP_LIST_IMM_LEN_COUNT - 1;

template <typename T>
struct IntWrapper
{
    const T value;
    const size_t size;

    IntWrapper(const T& value, const size_t size)
        : value(value),
          size(size)
    {}
};

template <typename T>
IntWrapper<uint8_t> as_uint8(const T& value)
{
    return IntWrapper<uint8_t>(value, 1);
}

IntWrapper<uint8_t> as_uint8(const BigInt& value)
{
    return IntWrapper<uint8_t>(value.get_value_as_uint64(), 1);
}

template <typename T>
IntWrapper<T> as_int(const T& value, size_t size)
{
    return IntWrapper<T>(value, size);
}

} // namespace

namespace multy_core
{
namespace internal
{

size_t get_bytes_len(const BigInt& value)
{
    return value.get_exported_size_in_bytes();
}

struct EthereumDataStream
{
    EthereumDataStream()
        : m_data()
    {
        m_data.reserve(256);
    }

    void write_data(const void* data, size_t size)
    {
        const uint8_t* d = reinterpret_cast<const uint8_t*>(data);
        m_data.insert(m_data.end(), d, d + size);
    }

    BinaryData get_content()
    {
        return BinaryData{m_data.data(), m_data.size()};
    }

protected:
    std::vector<std::uint8_t> m_data;
};

struct EthereumDataStreamList : public EthereumDataStream
{
    size_t length() const
    {
        return m_data.size();
    }
    const std::uint8_t* data() const
    {
        return m_data.data();
    }
};

template <typename T>
void write_int(const T& value, EthereumDataStream* stream);
EthereumDataStream& operator<<(EthereumDataStream& stream, const BinaryData& data);

EthereumDataStream& operator<<(EthereumDataStream& stream, const uint8_t& data)
{
    write_int(data, &stream);
    return stream;
}

EthereumDataStream& operator<<(EthereumDataStream& stream, const uint16_t& data)
{
    write_int(data, &stream);
    return stream;
}

EthereumDataStream& operator<<(EthereumDataStream& stream, const uint32_t& data)
{
    write_int(data, &stream);
    return stream;
}

EthereumDataStream& operator<<(EthereumDataStream& stream, const IntWrapper<uint8_t>& wrapper)
{
    stream.write_data(&wrapper.value, sizeof(wrapper.value));
    return stream;
}

// TODO: this implementation has a bug for values with `wrapper.size > 2`
//template <typename T>
//EthereumDataStream& operator<<(EthereumDataStream& stream, const IntWrapper<T>& wrapper)
//{
//    assert(wrapper.size <= sizeof(wrapper.value));
//    T value = wrapper.value;
//    for (size_t i = 0; i < wrapper.size; ++i)
//    {
//        value >>= 8;
//        stream << as_uint8(value);
//    }
//    return stream;
//}

EthereumDataStream& operator<<(EthereumDataStream& stream, const IntWrapper<BigInt>& wrapper)
{
    const BinaryDataPtr data = wrapper.value.export_as_binary_data(BigInt::EXPORT_BIG_ENDIAN);
    if (wrapper.size > data->len)
    {
        THROW_EXCEPTION("Can't serialize BigInt as fixed-size blob.")
                << " Required size: " << wrapper.size
                << " available size: " << data->len;
    }

    const size_t offset = data->len - wrapper.size;
    const size_t size = data->len - offset;
    const BinaryData to_write = slice(*data, offset, size);

    stream.write_data(to_write.data, to_write.len);
    return stream;
}

template <typename T>
std::string to_string(const T& t)
{
    std::stringstream sstr;
    sstr << t;
    return sstr.str();
}

std::string to_string(const BigInt& amount)
{
    return amount.get_value();
}

template <typename T>
void write_int(const T& value, EthereumDataStream* stream)
{
    if (value == 0)
    {
        *stream << as_uint8(RLP_DATA_IMM_LEN_START);
    }
    else if (value < RLP_DATA_IMM_LEN_START)
    {
        *stream << as_uint8(value);
    }
    else
    {
        const size_t len = get_bytes_len(value);
        if (len < RLP_DATA_IMM_LEN_START)
        {
            *stream << as_uint8(len + RLP_DATA_IMM_LEN_START);
        }
        else
        {
            const size_t len_of_len = get_bytes_len(len);
            if (RLP_DATA_IND_LEN_ZERO + len_of_len > 0xff)
            {
                THROW_EXCEPTION("Number is too big for RLP") << to_string(value);
            }
            *stream << as_uint8(RLP_DATA_IND_LEN_ZERO + len_of_len);
            *stream << as_int(BigInt(static_cast<uint64_t>(len)), len_of_len);
        }
        *stream << as_int(BigInt(value), len);
    }
}

EthereumDataStream& operator<<(EthereumDataStream& stream, const BigInt& data)
{
    write_int(data, &stream);
    return stream;
}

template <typename T, typename D>
EthereumDataStream& operator<<(EthereumDataStream& stream, const std::unique_ptr<T, D>& ptr)
{
    INVARIANT(ptr != nullptr);

    return stream << *ptr;
}

template <typename T>
EthereumDataStream& operator<<(EthereumDataStream& stream, const PropertyT<T>& property)
{
    return stream << *property;
}

EthereumDataStream& operator<<(EthereumDataStream& stream, const BinaryData& binary_data)
{
    const size_t len = binary_data.len;
    const unsigned char* data = binary_data.data;
    if (len == 1 && data[0] < RLP_DATA_IMM_LEN_START)
    {
        stream << as_uint8(data[0]);
    }
    else
    {
        if (len < RLP_DATA_IMM_LEN_COUNT)
        {
            stream << as_uint8(len + RLP_DATA_IMM_LEN_START);
        }
        else
        {
            size_t length_size = get_bytes_len(len);
            if (RLP_DATA_IND_LEN_ZERO + length_size > 0xFF)
            {
                THROW_EXCEPTION("BinaryData is too big for RLP")
                        << " length : " << len;
            }

            stream << as_uint8(RLP_DATA_IND_LEN_ZERO + length_size);
            stream << as_int(BigInt(static_cast<uint64_t>(len)), length_size);
        }
        stream.write_data(data, len);
    }
    return stream;
}

EthereumDataStream& operator<<(EthereumDataStream& stream, const EthereumDataStreamList& list)
{
    const size_t len = list.length();
    const unsigned char* data = list.data();

    if (len < RLP_LIST_IMM_LEN_COUNT)
    {
        stream << as_uint8(len + RLP_LIST_IMM_LEN_START);
    }
    else
    {
        const size_t length_size = get_bytes_len(len);
        if (RLP_LIST_IND_LEN_ZERO + length_size > 0xFF)
        {
            THROW_EXCEPTION("List is too big for RLP. ")
                    << " Length: " << len;
        }

        stream << as_uint8(RLP_LIST_IND_LEN_ZERO + length_size);
        stream << as_int(BigInt(static_cast<uint64_t>(len)), length_size);
    }

    stream.write_data(data, len);
    return stream;
}

struct EthereumTransactionSignature
{
    EthereumTransactionSignature()
        : recovery_id(0),
          m_signature_data()
    {}

    void set_signature(BinaryDataPtr signature_data)
    {
        if (signature_data->len != 65)
        {
            THROW_EXCEPTION2(ERROR_TRANSACTION_INVALID_SIGNATURE,
                    "Invalid signature size.")
                    << " Expected: 65 bytes, got: " << signature_data->len;
        }
        m_signature_data.swap(signature_data);
        recovery_id = m_signature_data->data[64];
    }
    void write_to_stream(size_t offset, EthereumDataStream* stream) const
    {
        //(m_signature->v + offset) << (u256)m_vrs->r << (u256)m_vrs->s;
        *stream << uint32_t(offset + recovery_id)
                << skip_leading_zeroes(slice(*m_signature_data, 0, 32))
                << skip_leading_zeroes(slice(*m_signature_data, 0 + 32, 32));
    }

private:
    uint8_t recovery_id;
    BinaryDataPtr m_signature_data;
};

EthereumTransaction::EthereumTransaction(const Account& account)
    : TransactionBase(account.get_blockchain_type()),
      m_account(account),
      m_nonce(get_transaction_properties(), "nonce", Property::REQUIRED,
              [](const BigInt& new_nonce)
              {
                    if (new_nonce < -1)
                    {
                        THROW_EXCEPTION2(ERROR_INVALID_ARGUMENT,
                                "Nonce should be non-negative.");
                    }
              }),
      m_chain_id(static_cast<EthereumChainId>(account.get_blockchain_type().net_type)),
      m_token_transfer(
              get_transaction_properties(),
              "token_transfer",
              Property::OPTIONAL,
              [this](const std::string& new_token_transfer)
              {
                    this->on_token_transfer_set(new_token_transfer);
              }),
      m_fee(new EthereumTransactionFee),
      m_source(),
      m_destination(),
      m_internal_destination(),
      m_signature()
{
}

EthereumTransaction::~EthereumTransaction()
{
}

BinaryDataPtr EthereumTransaction::serialize()
{
    update();
    sign();

    EthereumDataStream data_stream;
    serialize_to_stream(data_stream, SERIALIZE_WITH_SIGNATURE);

    return make_clone(data_stream.get_content());
}

void EthereumTransaction::serialize_to_stream(EthereumDataStream& stream, SerializationMode mode) const
{
    EthereumDataStreamList list;
    list << m_nonce;
    list << m_fee->gas_price;
    list << m_fee->gas_limit;
    list << m_internal_destination->address;
    list << m_internal_destination->amount;
    if (m_token_transfer_data)
    {
        const BinaryDataPtr call_method = m_token_transfer_data->serialize(*m_destination);
        list <<  *call_method;
    }
    else if(m_payload && (m_payload->data != nullptr))
    {
        list << m_payload;
    }
    else
    {
        list << BinaryData{nullptr, 0};
    }

    if (mode == SERIALIZE_WITH_SIGNATURE)
    {
        INVARIANT(m_signature != nullptr);

        const uint32_t offset = m_chain_id*2 + 35;
        m_signature->write_to_stream(offset, &list);
    }
    else if (mode == SERIALIZE_WITH_CHAIN_ID)
    {
        list << static_cast<uint32_t>(m_chain_id) << 0u << 0u;
    }
    stream << list;
}

void EthereumTransaction::on_token_transfer_set(const std::string& value)
{
    EthereumSmartContractPayloadPtr new_data = parse_token_transfer_data(value);
    m_token_transfer_data.swap(new_data);
}

BigInt EthereumTransaction::get_total_spent() const
{
    return (m_internal_destination ? *m_internal_destination->amount : BigInt(0))
            + get_total_fee();
}

BigInt EthereumTransaction::get_total_fee() const
{
    return m_fee->total_fee;
}

void EthereumTransaction::verify()
{
    if (!m_source)
    {
        THROW_EXCEPTION2(ERROR_TRANSACTION_NO_SOURCES,
                "Transaction doesn't have a source.");
    }

    if (!m_destination)
    {
        THROW_EXCEPTION2(ERROR_TRANSACTION_NO_DESTINATIONS,
                "Transaction doesn't have a destination.");
    }

    std::string missing_properties;
    if (!validate_all_properties(&missing_properties))
    {
        THROW_EXCEPTION2(ERROR_NOT_ALL_REQUIRED_PROPERTIES_SET,
                "Not all required properties set.")
                << "\n" << missing_properties;
    }
}

void EthereumTransaction::update()
{
    verify();

    if (!m_internal_destination)
    {
        m_internal_destination.reset(new EthereumTransactionDestination);
    }
    if (!m_token_transfer_data)
    {
        *m_internal_destination = *m_destination;
    }
    else
    {
        m_internal_destination = m_token_transfer_data->get_destination();
    }

    BigInt diff = m_source->amount.get_value() - m_internal_destination->amount.get_value();
    m_fee->total_fee = *m_fee->gas_limit * *m_fee->gas_price;

    diff -= m_fee->total_fee;
    if (diff < 0)
    {
        THROW_EXCEPTION2(ERROR_TRANSACTION_INSUFFICIENT_FUNDS,
                "Transaction is trying to spend more than available.");
    }
}

void EthereumTransaction::sign()
{
    EthereumDataStream data_stream;
    serialize_to_stream(data_stream,
            m_chain_id > 0 ? SERIALIZE_WITH_CHAIN_ID : SERIALIZE);

    m_signature.reset(new EthereumTransactionSignature);
    m_signature->set_signature(m_account.get_private_key()->sign(
            data_stream.get_content()));
}

BigInt EthereumTransaction::estimate_total_fee(size_t, size_t) const
{
    const uint64_t PAYLOAD_ZERO_BYTE_GAS = 4;
    const uint64_t PAYLOAD_NONZERO_BYTE_GAS = 68;
    const uint64_t TX_BASE_GAS = 21000;

    const BinaryData& data = m_payload ? *m_payload : BinaryData{nullptr, 0};
    const uint64_t zero_bytes_count = std::count(data.data, data.data + data.len, 0);

    const uint64_t estimated_gas = TX_BASE_GAS
            + zero_bytes_count * PAYLOAD_ZERO_BYTE_GAS
            + (data.len - zero_bytes_count) * PAYLOAD_NONZERO_BYTE_GAS;
    return estimated_gas * *m_fee->gas_price;
}

Properties& EthereumTransaction::add_source()
{
    if (m_source)
    {
        THROW_EXCEPTION2(ERROR_TRANSACTION_TOO_MANY_SOURCES,
                "Multiple sources are not supported.");
    }
    m_source.reset(new EthereumTransactionSource);
    return m_source->get_properties();
}

Properties& EthereumTransaction::add_destination()
{
    if (m_destination)
    {
        THROW_EXCEPTION2(ERROR_TRANSACTION_TOO_MANY_DESTINATIONS,
                "Multiple destinations are not supported.");
    }
    m_destination.reset(new EthereumTransactionDestination);
    return m_destination->get_properties();
}

Properties& EthereumTransaction::get_fee()
{
    return m_fee->get_properties();
}

void EthereumTransaction::set_message(const BinaryData& value)
{
    m_payload = make_clone(value);
}
} // namespace internal
} // namespace multy_core
