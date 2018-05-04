/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/src/ethereum/ethereum_token.h"

#include "multy_core/src/ethereum/ethereum_extra_data.h"

#include "multy_core/src/exception_stream.h"
#include "multy_core/src/utility.h"

#include <array>
#include <regex>
#include <unordered_map>
#include <vector>


namespace
{
using namespace multy_core::internal;

enum EthereumTokenStandard
{
    ETHEREUM_TOKEN_STANDARD_ERC20
};

const size_t ETHEREUM_SIZE_VARIABLE_FUNCTION_CONTRACT = 32;
const size_t ETH_METHOD_HASH_SIZE = 4;
typedef std::array<uint8_t, ETH_METHOD_HASH_SIZE> EthereumContractMethodHash;

} // namespace


namespace multy_core
{
namespace internal
{
EthereumSmartContractPayload::~EthereumSmartContractPayload()
{
}

struct EthereumContractPayloadStream
{
public:
    EthereumContractPayloadStream()
        : m_data()
    {
        m_data.reserve(256);
    }

    void write_data(const void* data, size_t size)
    {
        const uint8_t* d = reinterpret_cast<const uint8_t*>(data);
        m_data.insert(m_data.end(), d, d + size);
    }

    BinaryDataPtr get_content()
    {
        return make_clone(as_binary_data(m_data));
    }

    ~EthereumContractPayloadStream()
    {
    }

protected:
    std::vector<std::uint8_t> m_data;
};


EthereumContractPayloadStream& operator<<(EthereumContractPayloadStream& stream, const BinaryData& data)
{
    INVARIANT(data.data != nullptr);
    INVARIANT(data.len <= ETHEREUM_SIZE_VARIABLE_FUNCTION_CONTRACT);

    static const std::array<uint8_t, ETHEREUM_SIZE_VARIABLE_FUNCTION_CONTRACT> ZEROES = {0};
    stream.write_data(ZEROES.data(), ETHEREUM_SIZE_VARIABLE_FUNCTION_CONTRACT - data.len);
    stream.write_data(data.data, data.len);
    return stream;
}

EthereumContractPayloadStream& operator<<(EthereumContractPayloadStream& stream, const EthereumContractMethodHash& method_hash)
{
    stream.write_data(method_hash.data(), method_hash.size());
    return stream;
}

EthereumContractPayloadStream& operator<<(EthereumContractPayloadStream& stream, const BigInt& value)
{
    const BinaryDataPtr data = value.export_as_binary_data(BigInt::EXPORT_BIG_ENDIAN);
    if (data->len > ETHEREUM_SIZE_VARIABLE_FUNCTION_CONTRACT)
    {
        THROW_EXCEPTION2(ERROR_TRANSACTION_SPECIFIC_ERROR_BASE,
                "Invalid amount to transfer.")
                << " Expected: less than " << ETHEREUM_SIZE_VARIABLE_FUNCTION_CONTRACT << " bytes, got: " << data->len;
    }
    stream << *data;
    return stream;
}

class EthereumSmartContractPayloadBase: public EthereumSmartContractPayload
{
public:
    EthereumSmartContractPayloadBase(
            const EthereumTokenStandard standard,
            const EthereumContractMethodHash method_hash,
            BinaryDataPtr contract_address)
        : m_standard(standard),
        m_method_hash(method_hash),
        m_contract_address(std::move(contract_address))
    {
    }

    virtual ~EthereumSmartContractPayloadBase()
    {
    }

    EthereumTransactionDestinationPtr get_destination() override
    {
        EthereumTransactionDestinationPtr contract_destination(new EthereumTransactionDestination());
        contract_destination->address = make_clone(*m_contract_address);
        contract_destination->amount.set_value(BigInt{0});

        return contract_destination;
    }

protected:
    EthereumTokenStandard m_standard;
    EthereumContractMethodHash m_method_hash;
    BinaryDataPtr m_contract_address;
};

class ERC20TransferToken : public EthereumSmartContractPayloadBase
{
public:
    explicit ERC20TransferToken(BinaryDataPtr contract_address)
        : EthereumSmartContractPayloadBase(ETHEREUM_TOKEN_STANDARD_ERC20,
                {0xa9, 0x05, 0x9c, 0xbb},
                std::move(contract_address))
    {
    }

    BinaryDataPtr serialize(const EthereumTransactionDestination& receiver) const override
    {
        EthereumContractPayloadStream list;
        list << m_method_hash;
        list << *receiver.address;
        list << *receiver.amount;

        return list.get_content();
    }

    virtual ~ERC20TransferToken()
    {
    }
};

class ERC20ApproveToken : public EthereumSmartContractPayloadBase
{
public:
    explicit ERC20ApproveToken(BinaryDataPtr contract_address)
        : EthereumSmartContractPayloadBase(ETHEREUM_TOKEN_STANDARD_ERC20,
                {0x09, 0x5e, 0xa7, 0xb3},
                std::move(contract_address))
    {
    }

    BinaryDataPtr serialize(const EthereumTransactionDestination& receiver) const override
    {
        EthereumContractPayloadStream list;
        list << m_method_hash;
        list << *receiver.address;
        list << *receiver.amount;

        return list.get_content();
    }

    virtual ~ERC20ApproveToken()
    {
    }
};

EthereumTokenStandard get_token_standard_by_name(const std::string& name)
{
    static const std::unordered_map<std::string, EthereumTokenStandard> STANDARDS =
    {
        {
            "ERC20", ETHEREUM_TOKEN_STANDARD_ERC20
        }
    };

    const auto s = STANDARDS.find(name);
    if (s == STANDARDS.end())
    {
        THROW_EXCEPTION2(ERROR_TRANSACTION_TOKEN_TRANSFER_INVALID_STANDARD,
                "Unknown Ethereum token standard.")
                << " Got: " << name << ".";
    }

    return s->second;
}

EthereumSmartContractPayloadPtr parse_token_transfer_data(const std::string& value)
{
    static const std::regex SPLIT_TOKEN_RE(":");

    std::sregex_token_iterator iterator(value.begin(), value.end(), SPLIT_TOKEN_RE, -1);

    if (iterator == std::sregex_token_iterator())
    {
        THROW_EXCEPTION2(ERROR_TRANSACTION_TOKEN_TRANSFER_MISSING_STANDARD,
                "Missing Token Smart Contract standard.");
    }
    const EthereumTokenStandard token_standard = get_token_standard_by_name(*iterator);
    ++iterator;

    if (iterator == std::sregex_token_iterator())
    {
        THROW_EXCEPTION2(ERROR_TRANSACTION_TOKEN_TRANSFER_MISSING_ADDRESS,
                "Missing Token Smart Contract address.");
    }
    BinaryDataPtr contract_address = ethereum_parse_address(iterator->str().c_str());
    ++iterator;

    if (iterator == std::sregex_token_iterator())
    {
        THROW_EXCEPTION2(ERROR_TRANSACTION_TOKEN_TRANSFER_MISSING_METHOD,
                "Missing Token Smart Contract method.");
    }

    EthereumSmartContractPayloadPtr result;

    switch (token_standard)
    {
        case ETHEREUM_TOKEN_STANDARD_ERC20:
            if (iterator->str() == "transfer")
            {
                result.reset(new ERC20TransferToken(std::move(contract_address)));
            }
            else if (iterator->str() == "approve")
            {
                result.reset(new ERC20ApproveToken(std::move(contract_address)));
            }
            else
            {
               THROW_EXCEPTION2(ERROR_TRANSACTION_TOKEN_TRANSFER_INVALID_METHOD,
                     "Unsupported Ethereum token transfer method.")
                     << " Got: \" " << token_standard << "\".";
            }
            break;
        default:
            THROW_EXCEPTION2(ERROR_TRANSACTION_TOKEN_TRANSFER_INVALID_STANDARD,
                    "Invalid Ethereum token standard.")
                    << " Got: \" " << token_standard << "\".";;
            break;
    }

    return result;
}

} // namespace internal
} // namespace multy_core
