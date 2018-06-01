/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_CORE_BITCOIN_KEY_H
#define MULTY_CORE_BITCOIN_KEY_H

#include "multy_core/bitcoin.h"

#include "multy_core/src/api/key_impl.h"
#include "multy_core/src/ec_key_utils.h"

struct BinaryData;

#include <vector>
#include <cstdint>
#include <string>

namespace multy_core
{
namespace internal
{

struct BitcoinPrivateKey;
typedef UPtr<BitcoinPrivateKey> BitcoinPrivateKeyPtr;

struct BitcoinPublicKey : public PublicKey
{
public:
    typedef std::vector<uint8_t> KeyData;

    explicit BitcoinPublicKey(KeyData key_data);
    ~BitcoinPublicKey();

    std::string to_string() const override;
    const BinaryData get_content() const override;
    PublicKeyPtr clone() const override;

private:
    const KeyData m_data;
};

struct BitcoinPrivateKey : public PrivateKey
{
    typedef std::vector<uint8_t> KeyData;

    BitcoinPrivateKey(KeyData data,
            BitcoinNetType net_type,
            BitcoinAccountType account_type,
            PublicKeyFormat public_key_format);

    BitcoinPrivateKey(const BinaryData& data,
            BitcoinNetType net_type,
            BitcoinAccountType account_type,
            PublicKeyFormat public_key_format);
    ~BitcoinPrivateKey();

    std::string to_string() const override;
    PublicKeyPtr make_public_key() const override;
    PrivateKeyPtr clone() const override;
    BinaryDataPtr sign(const BinaryData& data) const override;

    BitcoinNetType get_net_type() const;

private:
    // TODO: use a shared_pointer to KeyData here to keep a single copy of
    // the private key data in memory.
    const KeyData m_data;    // private key data with no prefix
    const BitcoinNetType m_net_type;
    const PublicKeyFormat m_public_key_format;
    const BitcoinAccountType m_account_type;
};

BitcoinPrivateKeyPtr make_bitcoin_private_key_from_wif(const char* wif_string, BitcoinAccountType account_type);

} // namespace internal
} // namespace multy_core

#endif // MULTY_CORE_BITCOIN_KEY_H
