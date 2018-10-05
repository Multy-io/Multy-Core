/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/src/bitcoin/bitcoin_key.h"

#include "multy_core/src/exception.h"
#include "multy_core/src/exception_stream.h"
#include "multy_core/src/utility.h"
#include "multy_core/src/hash.h"

#include "wally_crypto.h"

#include <cstring>

namespace
{
using namespace multy_core::internal;

const uint8_t PRIVATE_KEY_EXPORT_PREFIXES[2] = {
    // Main net:
    0x80,
    // Test net:
    0xEF
};

const uint8_t WIF_COMPRESSED_PUBLIC_KEY_SUFFIX = 0x01;

uint8_t get_private_key_prefix(BitcoinNetType net_type)
{
    assert(net_type < array_size(PRIVATE_KEY_EXPORT_PREFIXES));
    return PRIVATE_KEY_EXPORT_PREFIXES[net_type];
}

} // namespace

namespace multy_core
{
namespace internal
{

BitcoinPublicKey::BitcoinPublicKey(KeyData key_data)
    : m_data(std::move(key_data))
{
}

BitcoinPublicKey::~BitcoinPublicKey()
{}

std::string BitcoinPublicKey::to_string() const
{
    CharPtr out_str;
    THROW_IF_WALLY_ERROR(
            wally_base58_from_bytes(
                    m_data.data(), m_data.size(), 0, reset_sp(out_str)),
            "Failed to serialize Bitcoin public key.");

    return std::string(out_str.get());
}

const BinaryData BitcoinPublicKey::get_content() const
{
    return as_binary_data(m_data);
}

PublicKeyPtr BitcoinPublicKey::clone() const
{
    return make_clone(*this);
}


BitcoinPrivateKey::BitcoinPrivateKey(KeyData data,
        BitcoinNetType net_type,
        BitcoinAccountType /*account_type*/,
        PublicKeyFormat public_key_format)
    : m_data(std::move(data)),
      m_net_type(net_type),
      m_public_key_format(public_key_format)
{
    ec_validate_private_key(as_binary_data(m_data));
}

BitcoinPrivateKey::BitcoinPrivateKey(const BinaryData& data,
        BitcoinNetType net_type,
        BitcoinAccountType /*account_type*/,
        PublicKeyFormat public_key_format)
    : m_data(data.data, data.data + data.len),
      m_net_type(net_type),
      m_public_key_format(public_key_format)
{
    ec_validate_private_key(as_binary_data(m_data));
}

BitcoinPrivateKey::~BitcoinPrivateKey()
{}

std::string BitcoinPrivateKey::to_string() const
{
    KeyData data(m_data);
    data.insert(data.begin(), get_private_key_prefix(m_net_type));
    if (m_public_key_format == EC_PUBLIC_KEY_COMPRESSED)
    {
        data.push_back(WIF_COMPRESSED_PUBLIC_KEY_SUFFIX);
    }

    CharPtr out_str;
    THROW_IF_WALLY_ERROR(
            wally_base58_from_bytes(
                    data.data(), data.size(),
                    BASE58_FLAG_CHECKSUM,
                    reset_sp(out_str)),
            "Failed to serialize Bitcoin private key.");
    wally_bzero(data.data(), data.size());

    return std::string(out_str.get());
}

PublicKeyPtr BitcoinPrivateKey::make_public_key() const
{
    const size_t public_key_size =
            (m_public_key_format == EC_PUBLIC_KEY_COMPRESSED)
                    ? EC_PUBLIC_KEY_LEN : EC_PUBLIC_KEY_UNCOMPRESSED_LEN;
    BitcoinPublicKey::KeyData key_data(public_key_size, 0);
    BinaryData public_key_data = as_binary_data(key_data);

    ec_private_to_public_key(as_binary_data(m_data),
            m_public_key_format,
            &public_key_data);

    return PublicKeyPtr(new BitcoinPublicKey(key_data));
}

PrivateKeyPtr BitcoinPrivateKey::clone() const
{
    return make_clone(*this);
}

BinaryDataPtr BitcoinPrivateKey::sign(const BinaryData& data) const
{
    auto data_hash = do_hash<SHA2_DOUBLE, 256>(data);

    std::array<uint8_t, EC_SIGNATURE_LEN> signature;
    THROW_IF_WALLY_ERROR(
            wally_ec_sig_from_bytes(
                    m_data.data(), m_data.size(),
                    data_hash.data(), data_hash.size(),
                    EC_FLAG_ECDSA, signature.data(), signature.size()),
            "Failed to sign binary data with private key.");
    wally_bzero(data_hash.data(), data_hash.size());

    std::array<uint8_t, EC_SIGNATURE_DER_MAX_LEN> der_signature;
    size_t written;
    THROW_IF_WALLY_ERROR(
            wally_ec_sig_to_der(
                    signature.data(), signature.size(),
                    der_signature.data(), der_signature.size(), &written),
            "Failed to convert signature to DER format.");
    wally_bzero(signature.data(), signature.size());

    return make_clone(BinaryData{der_signature.data(), written});
}

BitcoinNetType BitcoinPrivateKey::get_net_type() const
{
    return m_net_type;
}

BitcoinPrivateKeyPtr make_bitcoin_private_key_from_wif(
        const char* wif_string,
        BitcoinAccountType account_type)
{
    INVARIANT(wif_string != nullptr);

    size_t resulting_size = 0;
    THROW_IF_WALLY_ERROR(
            wally_base58_get_length(wif_string, &resulting_size),
            "Faield to process base-58 encoded private key.");

    std::vector<unsigned char> key_data(resulting_size, 0);

    THROW_IF_WALLY_ERROR(
            wally_base58_to_bytes(
                    wif_string, BASE58_FLAG_CHECKSUM, key_data.data(),
                    key_data.size(), &resulting_size),
            "Faield to deserialize base-58 encoded private key.");

    if (resulting_size > key_data.size() || resulting_size < EC_PRIVATE_KEY_LEN)
    {
        THROW_EXCEPTION("Failed to deserialize private key.");
    }
    key_data.resize(resulting_size);

    BitcoinNetType net_type = BITCOIN_NET_TYPE_MAINNET;
    // WIF, drop first 0x80 byte
    if (key_data[0] == 0x80 || key_data[0] == 0xef)
    {
        net_type = (key_data[0] == 0xef) ? BITCOIN_NET_TYPE_TESTNET : BITCOIN_NET_TYPE_MAINNET;
        key_data.erase(key_data.begin());
    }
    bool use_compressed_public_key = false;
    // WIF, drop last 0x01 byte
    const char* compressed_pefixes = net_type == BITCOIN_NET_TYPE_MAINNET ? "LK" : "c";
    if (strchr(compressed_pefixes, wif_string[0])
            && key_data.back() == WIF_COMPRESSED_PUBLIC_KEY_SUFFIX)
    {
        key_data.erase(key_data.end() - 1);
        use_compressed_public_key = true;
    }

    THROW_IF_WALLY_ERROR(
            wally_ec_private_key_verify(key_data.data(), key_data.size()),
            "Failed to verify private key.");

    return BitcoinPrivateKeyPtr(new BitcoinPrivateKey(
            std::move(key_data),
            net_type,
            account_type,
            (use_compressed_public_key ? EC_PUBLIC_KEY_COMPRESSED : EC_PUBLIC_KEY_UNCOMPRESSED)));
}

} // namespace internal
} // namespace multy_core
