/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/src/ethereum/ethereum_stream.h"

#include "multy_core/src/ethereum/ethereum_extra_data.h"

#include "multy_core/src/exception_stream.h"
#include "multy_core/src/hash.h"
#include "multy_core/src/utility.h"
#include "multy_core/binary_data.h"

#include <array>
#include <unordered_map>
#include <vector>

namespace
{
using namespace multy_core::internal;

const size_t ETHEREUM_SIZE_VARIABLE_FUNCTION_CONTRACT = 32;
static const std::array<const uint8_t, ETHEREUM_SIZE_VARIABLE_FUNCTION_CONTRACT> ZEROES = {0};

} // namespace


namespace multy_core
{
namespace internal
{

EthereumContractMethodHash make_method_hash(const char* methodSignature)
{
    const auto hash = do_hash<KECCAK, 256>(methodSignature);
    return EthereumContractMethodHash{hash[0], hash[1], hash[2], hash[3]};
}

EthereumPayloadDataStream::EthereumPayloadDataStream()
        :m_data()
{
    m_data.reserve(256);
}

EthereumPayloadDataStream::~EthereumPayloadDataStream()
{
}

BinaryData EthereumPayloadDataStream::get_content() const
{
    return BinaryData{m_data.data(), m_data.size()};
}

void EthereumPayloadDataStream::write_data(const void* data, size_t size)
{
    INVARIANT(data != nullptr);
    const uint8_t* d = reinterpret_cast<const uint8_t*>(data);
    m_data.insert(m_data.end(), d, d + size);
}

EthereumPayloadAsUint256Stream::EthereumPayloadAsUint256Stream()
        :m_data()
{
    m_data.reserve(256);
}

EthereumPayloadAsUint256Stream::~EthereumPayloadAsUint256Stream()
{
}

BinaryData EthereumPayloadAsUint256Stream::get_content() const
{
    return BinaryData{m_data.data(), m_data.size()};
}

void EthereumPayloadAsUint256Stream::write_data(const void* data, size_t size)
{
    INVARIANT(data != nullptr);
    INVARIANT(size <= ETHEREUM_SIZE_VARIABLE_FUNCTION_CONTRACT);

    const uint8_t* d = reinterpret_cast<const uint8_t*>(data);
    // Padding value from left side to be 32-bytes aligned
    m_data.insert(m_data.end(), ZEROES.data(), ZEROES.data() + (ETHEREUM_SIZE_VARIABLE_FUNCTION_CONTRACT - size));
    m_data.insert(m_data.end(), d, d + size);
}

EthereumPayloadAsUint256Stream& operator<<(EthereumPayloadAsUint256Stream& stream, const BinaryData& value)
{
    stream.write_data(value.data, value.len);

    return stream;
}

EthereumPayloadAsUint256Stream& operator<<(EthereumPayloadAsUint256Stream& stream, const BigInt& value)
{
    const BinaryDataPtr data = value.export_as_binary_data(BigInt::EXPORT_BIG_ENDIAN);
    if (data->len > ETHEREUM_SIZE_VARIABLE_FUNCTION_CONTRACT)
    {
        THROW_EXCEPTION2(ERROR_BIG_INT_TOO_BIG_FOR_UINT256,
                "BigInt value is too big for Ethereum smart contract method call.");
    }
    stream << *data;

    return stream;
}

EthereumPayloadDataStream& operator<<(EthereumPayloadDataStream& stream, const EthereumContractMethodHash& value)
{
    stream.write_data(value.data(), value.size());

    return stream;
}

EthereumPayloadDataStream& operator<<(EthereumPayloadDataStream& stream, const EthereumPayloadAsUint256Stream& value)
{
    stream.write_data(value.get_content().data, value.get_content().len);

    return stream;
}

EthereumPayloadAsUint256Stream& operator<<(EthereumPayloadAsUint256Stream& stream, const EthereumAddress& value)
{
    stream.write_data(value.address_data().data, value.address_data().len);

    return stream;
}

} // namespace internal
} // namespace multy_core
