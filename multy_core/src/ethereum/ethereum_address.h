/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_CORE_SRC_ETHEREUM_ADDRESS_H
#define MULTY_CORE_SRC_ETHEREUM_ADDRESS_H

#include "multy_core/binary_data.h"
#include "multy_core/src/u_ptr.h"

#include <string>

namespace multy_core
{
namespace internal
{
class EthereumAddress
{
public:
    EthereumAddress();
    ~EthereumAddress();

    EthereumAddress(const EthereumAddress& other);
    EthereumAddress& operator=(const EthereumAddress& other);

    EthereumAddress(EthereumAddress&& other) = default;
    EthereumAddress& operator=(EthereumAddress&& other) = default;

    explicit EthereumAddress(const std::string& address);
    explicit EthereumAddress(const BinaryData& data);
    // steals value
    explicit EthereumAddress(BinaryDataPtr data);

    const BinaryData& address_data() const;

    static EthereumAddress from_string(const std::string& address);
    static std::string to_string(const EthereumAddress& address);

private:
    BinaryDataPtr m_owned_data;
    BinaryData m_data;
};

} // namespace internal
} // namespace multy_core

#endif // MULTY_CORE_SRC_ETHEREUM_ADDRESS_H
