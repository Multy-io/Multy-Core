/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/internal/bitcoin_account.h"

#include "multy_core/common.h"

#include "multy_core/internal/exception.h"
#include "multy_core/internal/hd_path.h"
#include "multy_core/internal/key.h"
#include "multy_core/internal/utility.h"

#include "secp256k1.h"
#include "wally_core.h"
#include "wally_crypto.h"

#include <string.h>
#include <string>

namespace
{
using namespace wallet_core::internal;

// This is a setting controlling default accout network mode for new accounts.
const BitcoinNetType DEFAULT_NET_TYPE = BITCOIN_TESTNET;

bool is_testnet(BitcoinNetType net_type)
{
    if (net_type == BITCOIN_NET_DEFAULT)
    {
        net_type = DEFAULT_NET_TYPE;
    }
    return net_type == BITCOIN_TESTNET;
}

} // namespace

namespace wallet_core
{
namespace internal
{

struct BitcoinPublicKey : public PublicKey
{
public:
    typedef std::vector<uint8_t> KeyData;

    BitcoinPublicKey(KeyData key_data) : m_data(std::move(key_data))
    {
    }

    std::string to_string() const override
    {
        UPtr<char> out_str;
        throw_if_wally_error(
                wally_base58_from_bytes(
                        m_data.data(), m_data.size(), 0, reset_sp(out_str)),
                "Failed to serialize Bitcoin public key");
        return std::string(out_str.get());
    }

    const BinaryData get_content() const override
    {
        return BinaryData{m_data.data(), m_data.size()};
    }

    PublicKeyPtr clone() const override
    {
        return make_clone(*this);
    }

private:
    const KeyData m_data;
};

struct BitcoinPrivateKey : public PrivateKey
{
    typedef std::vector<uint8_t> KeyData;

    BitcoinPrivateKey(KeyData data)
        : m_data(std::move(data)), m_use_compressed_public_key(false)
    {
    }

    BitcoinPrivateKey(const BinaryData& data)
        : m_data(data.data, data.data + data.len),
          m_use_compressed_public_key(false)
    {
    }

    std::string to_string() const override
    {
        UPtr<char> out_str;
        throw_if_wally_error(
                wally_base58_from_bytes(
                        m_data.data(), m_data.size(), BASE58_FLAG_CHECKSUM,
                        reset_sp(out_str)),
                "Failed to serialize Bitcoin private key");
        return std::string(out_str.get());
    }

    PublicKeyPtr make_public_key() const override
    {
        BitcoinPublicKey::KeyData key_data(EC_PUBLIC_KEY_LEN, 0);
        throw_if_wally_error(
                wally_ec_public_key_from_private_key(
                        m_data.data(), m_data.size(), key_data.data(),
                        key_data.size()),
                "Failed to derive public key from private key");

        if (!m_use_compressed_public_key)
        {
            BitcoinPublicKey::KeyData uncompressed_data(
                    EC_PUBLIC_KEY_UNCOMPRESSED_LEN, 0);
            throw_if_wally_error(
                    wally_ec_public_key_decompress(
                            key_data.data(), key_data.size(),
                            uncompressed_data.data(), uncompressed_data.size()),
                    "(1) Failed to uncompress public key");
            std::swap(key_data, uncompressed_data);
        }

        return PublicKeyPtr(new BitcoinPublicKey(key_data));
    }

    PrivateKeyPtr clone() const override
    {
        return make_clone(*this);
    }

    BinaryDataPtr sign(const BinaryData& data) const override
    {
        std::array<uint8_t, SHA256_LEN> hash;
        throw_if_wally_error(
                wally_sha256d(data.data, data.len, hash.data(), hash.size()),
                "Failed to hash input binary data");

        std::array<uint8_t, EC_SIGNATURE_LEN> signature;
        throw_if_wally_error(
                wally_ec_sig_from_bytes(
                        m_data.data(), m_data.size(), hash.data(), hash.size(),
                        EC_FLAG_ECDSA, signature.data(), signature.size()),
                "Failed to sign binary data with private key");
        wally_bzero(hash.data(), hash.size());

        std::array<uint8_t, EC_SIGNATURE_DER_MAX_LEN> der_signature;
        size_t written;
        throw_if_wally_error(
                wally_ec_sig_to_der(
                        signature.data(), signature.size(),
                        der_signature.data(), der_signature.size(), &written),
                "Failed to convert signature to DER format");
        wally_bzero(signature.data(), signature.size());

        return make_clone(BinaryData{der_signature.data(), written});
    }

    void set_use_compressed_public_key(bool compressed_public_key)
    {
        m_use_compressed_public_key = compressed_public_key;
    }

private:
    // TODO: use a shared_pointer to KeyData here to keep a single copy of
    // the private key data in memory.
    const KeyData m_data;
    bool m_use_compressed_public_key;
};

BitcoinAccount::BitcoinAccount(
        BitcoinPrivateKeyPtr key, HDPath path, BitcoinNetType net_type)
    : AccountBase(CURRENCY_BITCOIN, *key, path),
      m_private_key(std::move(key)),
      m_is_testnet(::is_testnet(net_type))
{
}

std::string BitcoinAccount::get_address() const
{
    // P2PKH address generated from public key.
    // https://en.bitcoin.it/wiki/Technical_background_of_version_1_Bitcoin_addresses

    unsigned char hash_hash[SHA256_LEN] = {'\0'};
    unsigned char pub_hash[HASH160_LEN + 1] = {'\0'};
    unsigned char address[HASH160_LEN + 1 + 4] = {'\0'};

    // 1 - Take the corresponding public key generated with it (33 or 65
    // bytes)
    PublicKeyPtr public_key(m_private_key->make_public_key());
    BinaryData key_data = public_key->get_content();
    unsigned char uncompressed_key[EC_PUBLIC_KEY_UNCOMPRESSED_LEN] = {'\0'};

    // 2 - Perform SHA-256 hashing on the public key
    // 3 - Perform RIPEMD-160 hashing on the result of SHA-256
    throw_if_wally_error(
            wally_hash160(
                    key_data.data, key_data.len,
                    // Leave space for prefix at step 4.
                    pub_hash + 1, sizeof(pub_hash) - 1),
            "(2&3) Hashing of public key failed");

    // 4 - Add version byte in front of RIPEMD-160 hash
    //      (0x00 for Main Network)
    pub_hash[0] = m_is_testnet ? 0x6F : 0x00;

    // 5 - Perform SHA-256 hash on the extended RIPEMD-160 result
    // 6 - Perform SHA-256 hash on the result of the previous SHA-256 hash
    // 8 - Add the 4 checksum bytes from stage 7 at the end of extended
    //      RIPEMD-160 hash from stage 4.
    // 9 - Convert the result from a byte string into a base58 string
    //      using Base58Check encoding.
    CharPtr base58_string_ptr;
    throw_if_wally_error(
            wally_base58_from_bytes(
                    pub_hash, sizeof(pub_hash), BASE58_FLAG_CHECKSUM,
                    reset_sp(base58_string_ptr)),
            "Converting to base58 failed");
    std::string result(base58_string_ptr.get());

    return result;
}

bool BitcoinAccount::is_testnet() const
{
    return m_is_testnet;
}

BitcoinHDAccount::BitcoinHDAccount(
        const ExtendedKey& bip44_master_key, uint32_t index)
    : HDAccountBase(bip44_master_key, CURRENCY_BITCOIN, index)
{
}

BitcoinHDAccount::~BitcoinHDAccount()
{
}

AccountPtr BitcoinHDAccount::make_account(
        const ExtendedKey& parent_key, AddressType type, uint32_t index) const
{
    ExtendedKeyPtr address_key;
    throw_if_error(make_child_key(&parent_key, index, reset_sp(address_key)));

    BinaryData key_data{&address_key->key.priv_key[1],
                        sizeof(address_key->key.priv_key) - 1};
    UPtr<BitcoinPrivateKey> private_key(new BitcoinPrivateKey(key_data));

    return AccountPtr(
            new BitcoinAccount(
                    std::move(private_key),
                    make_child_path(make_child_path(get_path(), type), index)));
}

AccountPtr make_bitcoin_account(const char* private_key)
{
    size_t resulting_size;
    throw_if_wally_error(
            wally_base58_get_length(private_key, &resulting_size),
            "Faield to process base-58 encoded private key");

    std::vector<unsigned char> key_data(resulting_size, 0);

    throw_if_wally_error(
            wally_base58_to_bytes(
                    private_key, BASE58_FLAG_CHECKSUM, key_data.data(),
                    key_data.size(), &resulting_size),
            "Faield to deserialize base-58 encoded private key");

    if (resulting_size > key_data.size() || resulting_size < EC_PRIVATE_KEY_LEN)
    {
        throw Exception("Failed to deserialize private key");
    }
    key_data.resize(resulting_size);

    BitcoinNetType net_type = BITCOIN_MAINNET;
    // WIF, drop first 0x80 byte
    if (key_data[0] == 0x80 || key_data[0] == 0xef)
    {
        net_type = (key_data[0] == 0xef) ? BITCOIN_TESTNET : BITCOIN_MAINNET;
        key_data.erase(key_data.begin());
    }
    bool use_compressed_public_key = false;
    // WIF, drop last 0x01 byte
    const char* compressed_pefixes = net_type == BITCOIN_MAINNET ? "LK" : "c";
    if (strchr(compressed_pefixes, private_key[0]) && key_data.back() == 0x01)
    {
        key_data.erase(key_data.end() - 1);
        use_compressed_public_key = true;
    }

    throw_if_wally_error(
            wally_ec_private_key_verify(key_data.data(), key_data.size()),
            "Failed to verify private key");

    BitcoinPrivateKeyPtr key(new BitcoinPrivateKey(std::move(key_data)));
    key->set_use_compressed_public_key(use_compressed_public_key);

    return AccountPtr(new BitcoinAccount(std::move(key), HDPath(), net_type));
}

} // namespace wallet_core
} // namespace internal
