/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/src/ethereum/ethereum_address.h"

#include "multy_core/ethereum.h"
#include "multy_core/binary_data.h"

#include "multy_core/src/exception.h"
#include "multy_core/src/exception_stream.h"
#include "multy_core/src/utility.h"
#include "multy_core/src/u_ptr.h"
#include "multy_core/src/codec.h"

#include <string.h>

namespace
{
using namespace multy_core::internal;

const char* ETHEREUM_ADDRESS_PREFIX = "0x";
const size_t ETHEREUM_ADDRESS_PREFIX_LENGTH = strlen(ETHEREUM_ADDRESS_PREFIX);
} // namespace

namespace multy_core
{
namespace internal
{

EthereumAddress::EthereumAddress()
    : m_owned_data(),
      m_data(as_binary_data(""))
{}

EthereumAddress::~EthereumAddress()
{}

EthereumAddress::EthereumAddress(const EthereumAddress& other)
    : m_owned_data(make_clone(other.m_data)),
      m_data(*m_owned_data)
{}

EthereumAddress& EthereumAddress::operator=(const EthereumAddress& other)
{
    m_owned_data = make_clone(other.m_data);
    m_data = *m_owned_data;

    return *this;
}

EthereumAddress::EthereumAddress(const std::string& address)
    : EthereumAddress(from_string(address.c_str()))
{}

EthereumAddress::EthereumAddress(const BinaryData& data)
    : EthereumAddress(make_clone(data))
{
}

EthereumAddress::EthereumAddress(BinaryDataPtr data)
    : m_owned_data(std::move(data)),
      m_data(*m_owned_data)
{
    INVARIANT(m_owned_data.get() && m_owned_data->data != nullptr);

    if (m_owned_data->len != ETHEREUM_BINARY_ADDRESS_SIZE)
    {
        THROW_EXCEPTION2(ERROR_INVALID_ADDRESS,
                "Ethereum address invalid size.")
                << " Expected: " << ETHEREUM_BINARY_ADDRESS_SIZE
                << ", got: " << m_owned_data->len << ".";
    }
}

EthereumAddress EthereumAddress::from_string(const std::string& address)
{
    if (address.length() < ETHEREUM_BINARY_ADDRESS_SIZE * 2)
    {
        THROW_EXCEPTION2(ERROR_INVALID_ADDRESS,
                "Invalid serialized address size.")
                << " Expected at least: " << ERROR_INVALID_ADDRESS * 2
                << " got: " << address.length() << ".";
    }

    size_t size = address.size();
    const char* p_address = address.c_str();

    if (strncmp(p_address, ETHEREUM_ADDRESS_PREFIX, ETHEREUM_ADDRESS_PREFIX_LENGTH) == 0)
    {
        p_address += strlen(ETHEREUM_ADDRESS_PREFIX);
        size -= ETHEREUM_ADDRESS_PREFIX_LENGTH;
    }

    return EthereumAddress(decode(p_address, size, CODEC_HEX));
}

std::string EthereumAddress::to_string(const EthereumAddress& address)
{
    return ETHEREUM_ADDRESS_PREFIX + encode(address.address_data(), CODEC_HEX);
}

const BinaryData& EthereumAddress::address_data() const
{
    return m_data;
}


} // namespace internal
} // namespace multy_core
