/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/src/bitcoin/bitcoin_transaction.h"

#include "multy_core/common.h"

#include "multy_core/src/api/account_impl.h"
#include "multy_core/src/api/key_impl.h"
#include "multy_core/src/exception_stream.h"
#include "multy_core/src/api/properties_impl.h"
#include "multy_core/src/bitcoin/bitcoin_account.h"
#include "multy_core/src/exception.h"
#include "multy_core/src/u_ptr.h"
#include "multy_core/src/utility.h"

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

uint32_t bitcoin_traits()
{
    return 1 << TRANSACTION_REQUIRES_EXPLICIT_SOURCE
            | 1 << TRANSACTION_SUPPORTS_MULTIPLE_SOURCES
            | 1 << TRANSACTION_SUPPORTS_MULTIPLE_DESTINATIONS
            | 1 << TRANSACTION_SUPPORTS_FEE;
}

typedef uint8_t OpCode;

template <typename T>
std::string make_id(const std::string& base, const T& suffix)
{
    std::ostringstream ostr;
    ostr << base << suffix;
    return ostr.str();
}

void verify_non_negative_amount(const BigInt& amount)
{
    if (amount < BigInt(0))
    {
        THROW_EXCEPTION("BigInt value is negative.");
    }
}

} // namespace

namespace multy_core
{
namespace internal
{
class BitcoinStream
{
public:
    virtual ~BitcoinStream()
    {
    }

    virtual BitcoinStream& write_data(const uint8_t* data, uint32_t len) = 0;
};

class BitcoinDataStream : public BitcoinStream
{
public:
    BitcoinDataStream()
        : m_data()
    {}

    BitcoinDataStream& write_data(const uint8_t* data, uint32_t len) override
    {
        if (!data)
        {
            THROW_EXCEPTION("Attempt to write null data.");
        }

        m_data.insert(m_data.end(), data, data + len);
        return *this;
    }

    BinaryData get_content() const
    {
        return BinaryData{m_data.data(), m_data.size()};
    }

private:
    std::vector<uint8_t> m_data;
};

class BitcoinHashStream : public BitcoinStream
{
public:
    BitcoinHashStream& write_data(const uint8_t* data, uint32_t len) override
    {
        m_stream.write_data(data, len);
        return *this;
    }

    BinaryData get_hash() const
    {
        const BinaryData data = m_stream.get_content();

        THROW_IF_WALLY_ERROR(
                wally_sha256d(
                        data.data, data.len, m_hash.data(), m_hash.size()),
                "Failed to compute SHA256");

        return BinaryData{m_hash.data(), m_hash.size()};
    }

private:
    BitcoinDataStream m_stream;
    mutable std::array<uint8_t, SHA256_LEN> m_hash;
};

// Does no writing, only counting how many bytes would have been written.
class BitcoinBytesCountStream : public BitcoinStream
{
public:
    BitcoinStream& write_data(const uint8_t* /*data*/, uint32_t len)
    {
        bytes_count += len;
        return *this;
    }

    size_t get_bytes_count() const
    {
        return bytes_count;
    }

private:
    size_t bytes_count = 0;
};

void write_compact_size(uint64_t size, BitcoinStream* stream);

template <typename T>
BitcoinStream& write_as_data(const T& data, BitcoinStream& stream)
{
    return stream.write_data(
            reinterpret_cast<const uint8_t*>(&data), sizeof(data));
}

BitcoinStream& operator<<(BitcoinStream& stream, uint8_t data)
{
    return write_as_data(data, stream);
}

BitcoinStream& operator<<(BitcoinStream& stream, uint16_t data)
{
    return write_as_data(htole16(data), stream);
}

BitcoinStream& operator<<(BitcoinStream& stream, uint32_t data)
{
    return write_as_data(htole32(data), stream);
}

BitcoinStream& operator<<(BitcoinStream& stream, uint64_t data)
{
    return write_as_data(htole64(data), stream);
}

BitcoinStream& operator<<(BitcoinStream& stream, int8_t data)
{
    return write_as_data(data, stream);
}

BitcoinStream& operator<<(BitcoinStream& stream, int16_t data)
{
    return write_as_data(htole16(data), stream);
}

BitcoinStream& operator<<(BitcoinStream& stream, int32_t data)
{
    return write_as_data(htole32(data), stream);
}

BitcoinStream& operator<<(BitcoinStream& stream, int64_t data)
{
    return write_as_data(htole64(data), stream);
}

BitcoinStream& operator<<(BitcoinStream& stream, const BigInt& data)
{
    // Any BigInt we are going to serialize to transaction is non-negative.
    return stream << data.get_value_as_uint64();
}

BitcoinStream& operator<<(BitcoinStream& stream, const BinaryData& data)
{
    return stream.write_data(
            reinterpret_cast<const uint8_t*>(data.data), data.len);
}

template <typename T>
struct CompactSizeWrapper
{
    const T& value;
};

template <typename T>
BitcoinStream& operator<<(
        BitcoinStream& stream, const CompactSizeWrapper<T>& value)
{
    write_compact_size(value.value, &stream);
    return stream;
}

template <typename T>
CompactSizeWrapper<T> as_compact_size(const T& value)
{
    return CompactSizeWrapper<T>{value};
}

BitcoinStream& operator<<(BitcoinStream& stream, const std::string& str)
{
    stream << as_compact_size(str.size());
    return stream.write_data(
            reinterpret_cast<const uint8_t*>(str.data()), str.size());
}

BitcoinStream& operator<<(BitcoinStream& stream, const PublicKey& key)
{
    const BinaryData& key_data = key.get_content();
    return stream << as_compact_size(key_data.len) << key_data;
}

template <typename T>
BitcoinStream& operator<<(BitcoinStream& stream, const PropertyT<T>& value)
{
    return stream << value.get_value();
}

struct ReversedBinaryData
{
    const BinaryData& data;
};

ReversedBinaryData reverse(const BinaryData& data)
{
    return ReversedBinaryData{data};
}

BitcoinStream& operator<<(BitcoinStream& stream, const ReversedBinaryData& data)
{
    for (int i = data.data.len - 1; i >= 0; --i)
    {
        stream << data.data.data[i];
    }
    return stream;
}

void write_compact_size(uint64_t size, BitcoinStream* stream)
{
    if (size < 253)
    {
        *stream << static_cast<uint8_t>(size);
    }
    else if (size <= std::numeric_limits<uint16_t>::max())
    {
        *stream << static_cast<uint8_t>(253);
        *stream << static_cast<uint16_t>(size);
    }
    else if (size <= std::numeric_limits<uint32_t>::max())
    {
        *stream << static_cast<uint8_t>(254);
        *stream << static_cast<uint32_t>(size);
    }
    else
    {
        *stream << static_cast<uint8_t>(255);
        *stream << size;
    }
}

class BitcoinTransactionDestination
{
public:
    BitcoinTransactionDestination()
        : properties("TransactionDestination"),
          amount(properties,
                 "amount",
                 Property::REQUIRED,
                 verify_non_negative_amount),
          address(properties, "address", Property::REQUIRED),
          is_change(false, properties, "is_change", Property::OPTIONAL,
                  [this](int32_t new_value) {
                      if (new_value < 0)
                      {
                          THROW_EXCEPTION("is_change can't be negative");
                      }
                      this->on_change_set(new_value);
                  }),
          sig_script()
    {
    }

    void update()
    {
        BitcoinDataStream sig_stream;
        sig_stream << OpCode(0x76); // OP_DUP
        sig_stream << OpCode(0xA9); // OP_HASH160


        BitcoinNetType net_type;
        BitcoinAddressType address_type;
        BinaryDataPtr decoded_ptr = parse_bitcoin_address(
                                            address->c_str(),
                                            &net_type, &address_type);

        sig_stream << as_compact_size(decoded_ptr->len);
        sig_stream.write_data(decoded_ptr->data, decoded_ptr->len);

        sig_stream << OpCode(0x88); // OP_EQUALVERIFY
        sig_stream << OpCode(0xAC); // OP_CHECKSIG

        sig_script = make_clone(sig_stream.get_content());
    }

    void on_change_set(bool new_value)
    {
        // amount is optional for change-address destination.
        amount.set_trait(new_value ? Property::READONLY : Property::REQUIRED);
    }

    friend BitcoinStream& operator<<(
            BitcoinStream& stream,
            const BitcoinTransactionDestination& destination)
    {
        if (!destination.sig_script)
        {
            THROW_EXCEPTION(
                    "BitcoinTransactionDestination: sig_script is nullptr.");
        }

        stream << destination.amount;
        stream << as_compact_size(destination.sig_script->len);
        stream << *destination.sig_script;

        return stream;
    }

public:
    Properties properties;

    PropertyT<BigInt> amount;
    PropertyT<std::string> address;
    PropertyT<int32_t> is_change;

    BinaryDataPtr sig_script;
};

class BitcoinTransactionFee
{
public:
    BitcoinTransactionFee()
        : properties("TransactionFee"),
          amount_per_byte(
                  properties,
                  "amount_per_byte",
                  Property::OPTIONAL,
                  verify_non_negative_amount),
          min_amount_per_byte(
                  properties, "min_amount_per_byte", Property::OPTIONAL)
    {
    }

    const BigInt& get_amount_per_byte() const
    {
        return amount_per_byte.get_value();
    }

    void validate_fee(const BigInt& leftover, size_t transaction_size) const
    {
        const BigInt min_fee(
                min_amount_per_byte.get_default_value(BigInt(1))
                        .get_value_as_uint64()
                * transaction_size);
        if (leftover < min_fee)
        {
            THROW_EXCEPTION("Transaction total fee is too low. ")
                    << leftover.get_value() << " < " << min_fee.get_value();
        }
    }

public:
    Properties properties;

    PropertyT<BigInt> amount_per_byte;
    PropertyT<BigInt> min_amount_per_byte;
};

class BitcoinTransactionSource
{
public:
    BitcoinTransactionSource()
        : properties("TransactionSource"),
          prev_transaction_hash(
                  properties,
                  "prev_tx_hash",
                  Property::REQUIRED,
                  [](const BinaryData& new_tx_out_hash) {
                      if (new_tx_out_hash.len != 32)
                      {
                          THROW_EXCEPTION(
                                  "Previous transaction hash should be"
                                  "exactly 32 bytes long");
                      }
                  }),
          prev_transaction_out_index(properties, "prev_tx_out_index"),
          prev_transaction_out_script_pubkey(
                  properties, "prev_tx_out_script_pubkey"),
          private_key(properties, "private_key", Property::REQUIRED),
          script_signature(),
          script_witness(),
          amount(properties, "amount", Property::REQUIRED)
    {
    }

    ~BitcoinTransactionSource()
    {
    }

    void update()
    {
        //        BitcoinDataStream sig_stream;
        //        sig_stream <<
        //        static_cast<uint8_t>(prev_transaction_out_index);
        //        sig_stream << *prev_transaction_hash;
        //        BinaryDataPtr signature =
        //        private_key.sign(sig_stream.get_content());

        //        BitcoinDataStream script_signature_stream;
        //        script_signature_stream << *signature;
        //        script_signature_stream <<
        //        private_key.make_public_key()->get_content();

        //        script_signature =
        //        make_clone(script_signature_stream.get_content());
    }

    friend BitcoinStream& operator<<(
            BitcoinStream& stream, const BitcoinTransactionSource& source)
    {
        stream << reverse(**source.prev_transaction_hash);
        stream << source.prev_transaction_out_index;

        if (source.script_signature)
        {
            stream << as_compact_size(source.script_signature->len);
            stream << *source.script_signature;
        }
        else
        {
            stream << as_compact_size(0);
        }

        stream << source.seq;

        return stream;
    }

public:
    Properties properties;

    PropertyT<BinaryDataPtr> prev_transaction_hash;
    PropertyT<int32_t> prev_transaction_out_index;
    PropertyT<BinaryDataPtr> prev_transaction_out_script_pubkey;
    PropertyT<PrivateKeyPtr> private_key;

    // not a property since set by Transaction or Source itself.
    uint32_t seq = 0xffffffff;
    BinaryDataPtr script_signature;
    BinaryDataPtr script_witness; // serialized separately.

    PropertyT<BigInt> amount; // Not serialized:
};

BitcoinTransaction::BitcoinTransaction()
    : TransactionBase(CURRENCY_BITCOIN, bitcoin_traits()),
      m_version(1),
      m_is_segwit_transaction(0),
      m_lock_time(0),
      m_fee(new BitcoinTransactionFee),
      m_sources(),
      m_destinations()
{
    //    m_properties.bind_property("segwit_mode", &m_is_segwit_transaction);
    //    m_properties.bind_property("lock_time", &m_lock_time);
    register_properties("", m_fee->properties);
}

BinaryDataPtr BitcoinTransaction::serialize()
{
    verify();
    update_state();
    sign();

    BitcoinDataStream data_stream;

    serialize_to_stream(&data_stream);

    const BinaryData stream_content = data_stream.get_content();
    BinaryDataPtr result;
    throw_if_error(binary_data_clone(&stream_content, reset_sp(result)));
    return result;
}

template <typename T>
void BitcoinTransaction::serialize_to_stream(T* stream) const
{
    T& dest_stream = *stream;

    dest_stream << m_version;
    dest_stream << as_compact_size(m_sources.size());
    for (const auto& source : m_sources)
    {
        dest_stream << *source;
    }

    const auto destinations = get_non_zero_destinations();
    dest_stream << as_compact_size(destinations.size());
    for (const auto& destination : destinations)
    {
        dest_stream << *destination;
    }

    dest_stream << m_lock_time;
}

bool BitcoinTransaction::is_segwit() const
{
    return m_is_segwit_transaction != 0
            && std::any_of(
                       m_sources.begin(), m_sources.end(),
                       [](const BitcoinTransactionSourcePtr& source) -> bool {
                           return source->script_witness
                                   && source->script_witness->len > 0;
                       });
}

BitcoinTransaction::Destinations BitcoinTransaction::get_non_zero_destinations() const
{
    // TODO: include also change destination and payload-only destination.
    Destinations result;
    for (const auto& dest : m_destinations)
    {
        if (*dest->amount > BigInt(0) || *dest->is_change)
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
    const size_t destinations_count = get_non_zero_destinations().size();
    // look function estimate_total_fee
    return static_cast<int64_t>(
            sources_count * (147 + 32) + destinations_count * 34 + 10);
}

BigInt BitcoinTransaction::get_total_fee() const
{
    return calculate_diff();
}

BigInt BitcoinTransaction::estimate_total_fee(
        size_t sources_count, size_t destinations_count) const
{
    //change size input because public key may be in uncompressed format
    //compressed and uncompressed format differs in 32 byte
    const int64_t transaction_size
            = sources_count * (147 + 32) + destinations_count * 34 + 10;
    return transaction_size * m_fee->get_amount_per_byte();
}

BigInt BitcoinTransaction::calculate_diff() const
{
    BigInt available(0);
    for (auto& s : m_sources)
    {
        available += *s->amount;
    }

    BigInt total_spent(0);
    for (const auto& d : m_destinations)
    {
        total_spent += *d->amount;
    }
    return available -= total_spent;
}

void BitcoinTransaction::verify() const
{
    if (m_sources.empty())
    {
        THROW_EXCEPTION("Transaction should have at least one source.");
    }

    if (m_destinations.empty())
    {
        THROW_EXCEPTION("Transaction should have at least one destination.");
    }

    std::string missing_properties;
    if (!validate_all_properties(&missing_properties))
    {
        THROW_EXCEPTION("Not all required properties set:\n")
                << missing_properties;
    }

    const BigInt diff = calculate_diff();
    if (diff < 0)
    {
        THROW_EXCEPTION("Transaction is trying to spend more than available in "
                "inputs")
                << " : available - spent = "<< diff.get_value();
    }
}


void BitcoinTransaction::update_state()
{
    for (auto& s : m_sources)
    {
        s->update();
    }

    size_t change_destinations_count = 0;
    BitcoinTransactionDestination* change_destination = nullptr;
    for (const auto& d : m_destinations)
    {
        d->update();
        if (*d->is_change)
        {
            change_destination = d.get();
            ++change_destinations_count;
        }
    }
    if (change_destinations_count > 1)
    {
        THROW_EXCEPTION("Transaction should have only one change address.")
                << " Currenctly there are: " << change_destinations_count;
    }

    // In order to compute transaction total cost we'll have to serialize tx.
    if (change_destination)
    {
        sign();
        BitcoinBytesCountStream counter_stream;
        serialize_to_stream(&counter_stream);
        const uint64_t tx_size = counter_stream.get_bytes_count();

        const BigInt tx_cost = tx_size * *m_fee->amount_per_byte;
        const BigInt current_fee = calculate_diff();
        const BigInt remainder = current_fee - tx_cost;
        if (remainder > 0)
        {
            // not setting value with set_value(), since that would fail for read-only property.
            change_destination->amount.get_value() += remainder;
        }
    }

    m_fee->validate_fee(calculate_diff(), estimate_transaction_size());
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
    std::unordered_map<BitcoinTransactionSource*, BinaryDataPtr> sig_scripts;
    for (auto& source : m_sources)
    {
        source->script_signature.swap(
                source->prev_transaction_out_script_pubkey.get_value());
        BinaryDataPtr sig_script;

        {
            BitcoinDataStream hash_stream;
            serialize_to_stream(&hash_stream);
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
        if (!p.second)
        {
            THROW_EXCEPTION("Failed to save transaction source signature");
        }
    }

    // restore all sig scripts
    for (auto& source : m_sources)
    {
        const auto p = sig_scripts.find(source.get());
        if (p == sig_scripts.end())
        {
            THROW_EXCEPTION("Failed to set transaction source signature");
        }
        source->script_signature.swap(p->second);
    }
}

Properties& BitcoinTransaction::add_source()
{
    BitcoinTransactionSourcePtr source(new BitcoinTransactionSource());
    m_sources.emplace_back(std::move(source));
    return register_properties(
            make_id("#", m_sources.size() - 1), m_sources.back()->properties);
}

Properties& BitcoinTransaction::add_destination()
{
    BitcoinTransactionDestinationPtr destination(
            new BitcoinTransactionDestination());
    m_destinations.emplace_back(std::move(destination));
    return register_properties(
            make_id("#", m_destinations.size() - 1),
            m_destinations.back()->properties);
}

Properties& BitcoinTransaction::get_fee()
{
    return m_fee->properties;
}

} // namespace internal
} // namespaec multy_core
