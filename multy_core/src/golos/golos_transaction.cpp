/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/src/golos/golos_transaction.h"

#include "multy_core/golos.h"
#include "multy_core/src/golos/golos_account.h"

#include "multy_core/binary_data.h"
#include "multy_core/blockchain.h"

#include "multy_core/src/api/properties_impl.h"
#include "multy_core/src/blockchain_facade_base.h"
#include "multy_core/src/codec.h"
#include "multy_core/src/exception.h"
#include "multy_core/src/exception_stream.h"
#include "multy_core/src/utility.h"

#include <chrono>
#include <cstddef>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace multy_core
{
namespace internal
{

const char* ISO8601_TIME_FORMAT="%FT%T%z";

std::string format_iso8601_string(const std::time_t& time)
{
    std::ostringstream ss;
    ss << std::put_time(std::gmtime(&time), ISO8601_TIME_FORMAT);

    return ss.str();
}

std::time_t parse_iso8601_string(const std::string& str)
{
    std::tm tm;
    std::istringstream ss(str);
    ss >> std::get_time(&tm, ISO8601_TIME_FORMAT);
    const auto result = std::mktime(&tm);
    if (result < 0)
    {
        THROW_EXCEPTION("Invalid ISO8601 date/time value.");
    }

    return result;
}

std::time_t to_system_seconds(size_t seconds)
{
    return std::chrono::system_clock::to_time_t(
            std::chrono::system_clock::from_time_t(0)
            + std::chrono::seconds(seconds));
}

std::time_t get_system_time_now()
{
    return std::chrono::system_clock::to_time_t(
            std::chrono::system_clock::now());
}

class GolosBinaryStream
{
public:
    GolosBinaryStream()
        : m_data()
    {}

    void write_data(const unsigned char* data, size_t len)
    {
        if (!data)
        {
            THROW_EXCEPTION("Attempt to write null data.");
        }

        m_data.insert(m_data.end(), data, data + len);
    }

    BinaryData get_content() const
    {
        return as_binary_data(m_data);
    }

private:
    std::vector<unsigned char> m_data;
};

class GolosJsonStream
{
public:
    GolosJsonStream()
        : m_sstr()
    {}

    void write_string(const char* str, size_t len)
    {
        m_sstr.write(str, len);
    }

    std::string get_string() const
    {
        return m_sstr.str();
    }

private:
    std::ostringstream m_sstr;
};

GolosJsonStream& operator<<(GolosJsonStream& stream, const std::string& str)
{
    stream.write_string(str.c_str(), str.length());

    return stream;
}

GolosJsonStream& operator<<(GolosJsonStream& stream, const char* str)
{
    assert(str);
    const size_t length = strlen(str);
    stream.write_string(str, length);

    return stream;
}

class GolosTransactionOperation
{
public:
    virtual ~GolosTransactionOperation() = default;

    enum OperationType
    {
        TRANSFER
    };

    virtual OperationType get_type() const = 0;
    virtual void write_to_stream(GolosBinaryStream* /*stream*/) const = 0;
    virtual void write_to_stream(GolosJsonStream* stream) const = 0;

    std::string get_type_name() const
    {
        static const std::unordered_map<size_t, std::string> NAMES =
        {
            {TRANSFER, "transfer"},
        };

        const OperationType type = get_type();
        auto p = NAMES.find(type);
        assert(p != NAMES.end());

        return p->second;
    }
};

GolosJsonStream& operator<<(GolosJsonStream& stream, const GolosTransactionOperation& op)
{
    op.write_to_stream(&stream);

    return stream;
}

GolosBinaryStream& operator<<(GolosBinaryStream& stream, const GolosTransactionOperation& op)
{
    op.write_to_stream(&stream);

    return stream;
}

class GolosTransactionTransferOperation : public GolosTransactionOperation
{
public:
    GolosTransactionTransferOperation(
            std::string from,
            std::string to,
            BigInt amount,
            std::string token_name,
            std::string memo)
        : from(std::move(from)),
          to(std::move(to)),
          amount(std::move(amount)),
          token_name(std::move(token_name)),
          memo(std::move(memo))
    {}

    OperationType get_type() const override
    {
        return TRANSFER;
    }

    void write_to_stream(GolosBinaryStream* /*stream*/) const override
    {
    }

    void write_to_stream(GolosJsonStream* stream) const override
    {
        std::string amount_value = amount.get_value();
        if (amount_value.length() <= GOLOS_VALUE_DECIMAL_PLACES)
        {
            // Prepending small values with leading zeroes up to
            // GOLOS_VALUE_DECIMAL_PLACES + 1.

            // Golos implementation expects 64bit fractional part,
            // so that would take at most 19.2 decimal digits.
            // Rounding that up to 20 and here you go, 20 zeroes:
            const char* zeroes = "00000000000000000000";
            amount_value.insert(amount_value.begin(),
                    zeroes,
                    zeroes + (GOLOS_VALUE_DECIMAL_PLACES - amount_value.length() + 1));
        }
        amount_value.insert(amount_value.end() - GOLOS_VALUE_DECIMAL_PLACES, '.');
        //TODO: should we remove all trailing zeroes?

        char buf[1024];
        const int len = snprintf(buf, sizeof(buf),
                R"transfer(
    "from": "%s",
    "to": "%s",
    "amount": "%s %s",
    "memo": "%s"
)transfer",
                 from.c_str(),
                 to.c_str(),
                 amount_value.c_str(),
                 token_name.c_str(),
                 memo.c_str());
        if (len < 0 || static_cast<size_t>(len) == sizeof(buf))
        {
            THROW_EXCEPTION("Failed to format transfer operation as JSON.");
        }

        *stream << buf;
    }

public:
    std::string from;
    std::string to;
    BigInt amount;
    std::string token_name;
    std::string memo;
};

class GolosTransactionSource : public TransactionSourceBase
{
public:
    explicit GolosTransactionSource(BlockchainType blockchain_type)
        : address(m_properties, "address", Property::REQUIRED,
                [blockchain_type](const std::string& new_address) {
                    get_blockchain(BLOCKCHAIN_GOLOS)
                            .validate_address(blockchain_type,
                                    new_address.c_str());
                }),
          amount(m_properties, "amount", Property::OPTIONAL,
                [](const BigInt& new_amount)
                {
                    if (new_amount < BigInt(0))
                    {
                        THROW_EXCEPTION("Can't set negative amount as"
                                " transaction source value.");
                    }
                })
    {}

public:
    PropertyT<std::string> address;
    PropertyT<BigInt> amount;
};

class GolosTransactionDestination : public TransactionDestinationBase
{
public:
    explicit GolosTransactionDestination(BlockchainType blockchain_type)
        : address(m_properties, "address", Property::REQUIRED,
                [blockchain_type](const std::string& new_address) {
                    get_blockchain(BLOCKCHAIN_GOLOS)
                            .validate_address(blockchain_type,
                                    new_address.c_str());
                }),
          amount(m_properties, "amount", Property::OPTIONAL,
                [](const BigInt& new_amount)
                {
                    if (new_amount < BigInt(0))
                    {
                        THROW_EXCEPTION("Can't set negative amount as"
                                " transaction destination value.");
                    }
                })
    {}

public:
    PropertyT<std::string> address;
    PropertyT<BigInt> amount;
};

GolosTransaction::GolosTransaction(BlockchainType blockchain_type)
    : TransactionBase(blockchain_type),
      m_message(),
      m_source(),
      m_destination(),
      m_expires_in_seconds(
            GOLOS_EXPIRE_MIN_SECONDS,
            get_transaction_properties(),
            "expire_duration",
            Property::OPTIONAL,
            [](int32_t new_expire_duration)
            {
                if (new_expire_duration <= 10)
                {
                    THROW_EXCEPTION("Expire duration is too small.")
                            << " Expected at least : " << GOLOS_EXPIRE_MIN_SECONDS
                            << " Received: " << new_expire_duration;
                }
            }),
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
            "ref_block_num",
            Property::REQUIRED),
      m_ref_block_hash(
            get_transaction_properties(),
            "ref_block_hash",
            Property::REQUIRED,
            [](const BinaryData& data)
            {
                if (data.data == nullptr)
                {
                    THROW_EXCEPTION("Block hash data can't be nullptr.");
                }
                if (data.len != 160 / 8)
                {
                    THROW_EXCEPTION("Invalid block hash length.")
                            << " Expected: " << 160 /8
                            << ", received: " << data.len << ".";
                }
            })
{
}

GolosTransaction::~GolosTransaction()
{
}

void GolosTransaction::set_expiration(const std::string& new_expiration)
{
    m_expiration = parse_iso8601_string(new_expiration);
}

void GolosTransaction::verify()
{
    if (!m_source)
    {
        THROW_EXCEPTION2(ERROR_TRANSACTION_NO_SOURCES,
                "Golos transaction should have one source.");
    }

    if (!m_destination)
    {
        THROW_EXCEPTION2(ERROR_TRANSACTION_NO_DESTINATIONS,
                "Golos transaction should have one destination.");
    }

    if (!m_expires_in_seconds.is_set() && !m_explicit_expiration.is_set())
    {
        THROW_EXCEPTION("Expiration is not set.")
                << " Set it either with \"" <<  m_expires_in_seconds.get_name()
                << "\" or \"" << m_explicit_expiration.get_name() << "\".";
    }
}

void GolosTransaction::update()
{
    verify();

    if (!m_explicit_expiration.is_set())
    {
        m_expiration = std::chrono::system_clock::to_time_t(
                std::chrono::system_clock::now()
                + std::chrono::seconds(m_expires_in_seconds.get_value()));
    }

    const BinaryData& message = m_message ? *m_message : as_binary_data("");
    m_operation.reset(new GolosTransactionTransferOperation
    {
        *m_source->address,
        *m_destination->address,
        *m_destination->amount,
        GOLOS_TOKEN_NAME,
        std::string(reinterpret_cast<const char*>(message.data), message.len)
    });

    //TODO: binary-serialize this TX, sign it and update m_signature.
    m_signature = make_clone(as_binary_data("FAKE GOLOS TX SIGNATURE"));
}

BinaryDataPtr GolosTransaction::serialize()
{
    update();

    const uint16_t ref_block_num = static_cast<uint16_t>(
            static_cast<uint32_t>(*m_ref_block_num));

    const uint32_t ref_block_prefix =
            reinterpret_cast<const uint32_t*>(
                    m_ref_block_hash.get_value()->data)[1];

    GolosJsonStream operations_stream;
    operations_stream << "[\""
            << m_operation->get_type_name() << "\","
            << *m_operation << "]";

    char buffer[1024] = {'\0'};
    snprintf(buffer, sizeof(buffer), R"json(
    {
        "ref_block_num": %ud,
        "ref_block_prefix": %ud,
        "expiration": "%s",
        "operations": [%s],
        "extensions": [],
        "signatures": ["%s"]
    }
    )json",
            ref_block_num,
            ref_block_prefix,
            format_iso8601_string(m_expiration).c_str(),
            operations_stream.get_string().c_str(),
            encode(*m_signature, CODEC_HEX).c_str()
            );

    return make_clone(as_binary_data(buffer));
}

BigInt GolosTransaction::get_total_fee() const
{
    return BigInt(0);
}

BigInt GolosTransaction::get_total_spent() const
{
    if (!m_destination)
    {
        THROW_EXCEPTION("Failed to calculate total spent.")
                << " Transaction has no destinations.";
    }

    return *m_destination->amount;
}

BigInt GolosTransaction::estimate_total_fee(
        size_t /*sources_count*/,
        size_t /*destinations_count*/) const
{
    return BigInt(0);
}

Properties& GolosTransaction::add_source()
{
    if (m_source)
    {
        THROW_EXCEPTION2(ERROR_TRANSACTION_TOO_MANY_SOURCES,
                "Golos transaction can have only one source.");
    }

    m_source = GolosTransactionSourcePtr(new GolosTransactionSource(
            get_blockchain_type()));

    return m_source->get_properties();
}

Properties& GolosTransaction::add_destination()
{
    if (m_destination)
    {
        THROW_EXCEPTION2(ERROR_TRANSACTION_TOO_MANY_DESTINATIONS,
                "Golos transaction can have only one destination.");
    }

    m_destination = GolosTransactionDestinationPtr(
            new GolosTransactionDestination(get_blockchain_type()));

    return m_destination->get_properties();
}

Properties& GolosTransaction::get_fee()
{
    THROW_EXCEPTION2(ERROR_FEATURE_NOT_SUPPORTED,
            "Golos transaction fee is not customizable.");
}

void GolosTransaction::set_message(const BinaryData& value)
{
    m_message = make_clone(value);
}

} // namespace internal
} // namespace multy_core
