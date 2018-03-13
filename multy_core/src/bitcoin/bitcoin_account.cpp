/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */
#include "multy_core/src/bitcoin/bitcoin_account.h"

#include "multy_core/common.h"

#include "multy_core/src/api/key_impl.h"
#include "multy_core/src/exception.h"
#include "multy_core/src/hd_path.h"
#include "multy_core/src/utility.h"
#include "multy_core/src/exception_stream.h"

#include "secp256k1.h"
#include "wally_core.h"
#include "wally_crypto.h"

#include <sstream>
#include <string.h>
#include <string>

namespace
{
using namespace multy_core::internal;

enum BitcoinAddressType {
    P2PKH,
    P2SH
};

const uint8_t ADDRESS_PREFIXES[2][2] = {
    // Main net:
    {
        // P2PKH:
        0x0,
        // P2SH:
        0x05
    },
    // Test net:
    {
        // P2PKH:
        0x6f,
        // P2SH:
        0xC4
    }
};

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

uint8_t get_address_prefix(BitcoinNetType net_type, BitcoinAddressType address_type)
{
    assert(net_type < array_size(ADDRESS_PREFIXES));
    assert(address_type < array_size(ADDRESS_PREFIXES[net_type]));

    return ADDRESS_PREFIXES[net_type][address_type];
}

std::string to_hex_string(const uint8_t value)
{
    std::stringstream stream;
    stream << std::hex;
    stream <<value;
    return stream.str();
}

} // namespace

namespace multy_core
{
namespace internal
{

struct BitcoinPublicKey : public PublicKey
{
public:
    typedef std::vector<uint8_t> KeyData;

    BitcoinPublicKey(KeyData key_data)
        : m_data(std::move(key_data))
    {
    }

    std::string to_string() const override
    {
        UPtr<char> out_str;
        THROW_IF_WALLY_ERROR(
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

    BitcoinPrivateKey(KeyData data, BitcoinNetType net_type)
        : m_data(std::move(data)),
          m_net_type(net_type),
          m_use_compressed_public_key(true)
    {
    }

    BitcoinPrivateKey(const BinaryData& data, BitcoinNetType net_type)
        : m_data(data.data, data.data + data.len),
          m_net_type(net_type),
          m_use_compressed_public_key(true)
    {
    }

    std::string to_string() const override
    {
        KeyData data(m_data);
        data.insert(data.begin(), get_private_key_prefix(m_net_type));
        if (m_use_compressed_public_key)
        {
            data.push_back(WIF_COMPRESSED_PUBLIC_KEY_SUFFIX);
        }

        CharPtr out_str;
        THROW_IF_WALLY_ERROR(
                wally_base58_from_bytes(
                        data.data(), data.size(),
                        BASE58_FLAG_CHECKSUM,
                        reset_sp(out_str)),
                "Failed to serialize Bitcoin private key");
        wally_bzero(data.data(), data.size());

        return std::string(out_str.get());
    }

    PublicKeyPtr make_public_key() const override
    {
        BitcoinPublicKey::KeyData key_data(EC_PUBLIC_KEY_LEN, 0);
        THROW_IF_WALLY_ERROR(
                wally_ec_public_key_from_private_key(
                        m_data.data(), m_data.size(),
                        key_data.data(), key_data.size()),
                "Failed to derive public key from private key");

        if (!m_use_compressed_public_key)
        {
            BitcoinPublicKey::KeyData uncompressed_data(
                    EC_PUBLIC_KEY_UNCOMPRESSED_LEN, 0);
            THROW_IF_WALLY_ERROR(
                    wally_ec_public_key_decompress(
                            key_data.data(), key_data.size(),
                            uncompressed_data.data(), uncompressed_data.size()),
                    "Failed to uncompress public key");
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
        THROW_IF_WALLY_ERROR(
                wally_sha256d(data.data, data.len, hash.data(), hash.size()),
                "Failed to hash input binary data");

        std::array<uint8_t, EC_SIGNATURE_LEN> signature;
        THROW_IF_WALLY_ERROR(
                wally_ec_sig_from_bytes(
                        m_data.data(), m_data.size(), hash.data(), hash.size(),
                        EC_FLAG_ECDSA, signature.data(), signature.size()),
                "Failed to sign binary data with private key");
        wally_bzero(hash.data(), hash.size());

        std::array<uint8_t, EC_SIGNATURE_DER_MAX_LEN> der_signature;
        size_t written;
        THROW_IF_WALLY_ERROR(
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
    const KeyData m_data;    // private key data with no prefix
    const BitcoinNetType m_net_type;
    bool m_use_compressed_public_key;
};

BitcoinAccount::BitcoinAccount(BlockchainType blockchain_type,
        BitcoinPrivateKeyPtr key, HDPath path)
    : AccountBase(blockchain_type, *key, path),
      m_private_key(std::move(key))
{
}

BitcoinAccount::~BitcoinAccount()
{
}

void bitcoin_hash_160(const BinaryData& input, BinaryData* output)
{
    if (!output)
    {
        THROW_EXCEPTION("Invalid argument: output is null.");
    }

    THROW_IF_WALLY_ERROR(
            wally_hash160(
                    input.data, input.len,
                    const_cast<unsigned char*>(output->data), output->len),
            "hash160 failed.");
}

std::string BitcoinAccount::get_address() const
{
    // P2PKH address generated from public key.
    // https://en.bitcoin.it/wiki/Technical_background_of_version_1_Bitcoin_addresses

    unsigned char pub_hash[HASH160_LEN + 1] = {'\0'};

    // 1 - Take the corresponding public key generated with it (33 or 65
    // bytes)
    PublicKeyPtr public_key(m_private_key->make_public_key());
    BinaryData key_data = public_key->get_content();

    // 2 - Perform SHA-256 hashing on the public key
    // 3 - Perform RIPEMD-160 hashing on the result of SHA-256
    {
        // Leave the first byte intact for prefix.
        BinaryData hash_data = power_slice(as_binary_data(pub_hash), 1, -1);
        bitcoin_hash_160(key_data, &hash_data);
    }

    // 4 - Add version byte in front of RIPEMD-160 hash
    //      (0x00 for Main Network)
    pub_hash[0] = get_address_prefix(
            static_cast<BitcoinNetType>(m_blockchain_type.net_type), P2PKH);

    // 5 - Perform SHA-256 hash on the extended RIPEMD-160 result
    // 6 - Perform SHA-256 hash on the result of the previous SHA-256 hash
    // 8 - Add the 4 checksum bytes from stage 7 at the end of extended
    //      RIPEMD-160 hash from stage 4.
    // 9 - Convert the result from a byte string into a base58 string
    //      using Base58Check encoding.
    CharPtr base58_string_ptr;
    THROW_IF_WALLY_ERROR(
            wally_base58_from_bytes(
                    pub_hash, sizeof(pub_hash), BASE58_FLAG_CHECKSUM,
                    reset_sp(base58_string_ptr)),
            "Converting to base58 failed");
    std::string result(base58_string_ptr.get());

    return result;
}

BitcoinHDAccount::BitcoinHDAccount(
        BlockchainType blockchain_type,
        const ExtendedKey& bip44_master_key,
        uint32_t index)
    : HDAccountBase(blockchain_type, bip44_master_key, index)
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

    const BinaryData priv_key_data = as_binary_data(address_key->key.priv_key);
    UPtr<BitcoinPrivateKey> private_key(
            new BitcoinPrivateKey(
                    slice(priv_key_data, 1, priv_key_data.len - 1),
                    static_cast<BitcoinNetType>(get_blockchain_type().net_type)));

    return AccountPtr(
            new BitcoinAccount(
                    get_blockchain_type(),
                    std::move(private_key),
                    make_child_path(make_child_path(get_path(), type), index)));
}

AccountPtr make_bitcoin_account(const char* private_key)
{
    size_t resulting_size = 0;
    THROW_IF_WALLY_ERROR(
            wally_base58_get_length(private_key, &resulting_size),
            "Faield to process base-58 encoded private key");

    std::vector<unsigned char> key_data(resulting_size, 0);

    THROW_IF_WALLY_ERROR(
            wally_base58_to_bytes(
                    private_key, BASE58_FLAG_CHECKSUM, key_data.data(),
                    key_data.size(), &resulting_size),
            "Faield to deserialize base-58 encoded private key");

    if (resulting_size > key_data.size() || resulting_size < EC_PRIVATE_KEY_LEN)
    {
        THROW_EXCEPTION("Failed to deserialize private key");
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
    if (strchr(compressed_pefixes, private_key[0])
            && key_data.back() == WIF_COMPRESSED_PUBLIC_KEY_SUFFIX)
    {
        key_data.erase(key_data.end() - 1);
        use_compressed_public_key = true;
    }

    THROW_IF_WALLY_ERROR(
            wally_ec_private_key_verify(key_data.data(), key_data.size()),
            "Failed to verify private key");

    BitcoinPrivateKeyPtr key(new BitcoinPrivateKey(std::move(key_data), net_type));
    key->set_use_compressed_public_key(use_compressed_public_key);

    const BlockchainType blockchain_type{BLOCKCHAIN_BITCOIN, net_type};
    return AccountPtr(
            new BitcoinAccount(blockchain_type, std::move(key), HDPath()));
}

BinaryDataPtr bitcoin_parse_address(const char* address,
                                    BitcoinNetType* net_type,
                                    BitcoinAddressType* address_type)
{
    BinaryDataPtr out_binary_data;
    size_t binary_size = strlen(address);
    std::vector<uint8_t> decoded(binary_size, 0);

    THROW_IF_WALLY_ERROR(
                wally_base58_to_bytes(
                    address, BASE58_FLAG_CHECKSUM, decoded.data(),
                    decoded.size(), &binary_size),
                "Invalid address");

    decoded.resize(binary_size);
    if (decoded.empty())
    {
        THROW_EXCEPTION("Failed to decode address.");
    }

    // Save address type and remove it from decoded.
    const uint8_t address_version = decoded[0];
    decoded.erase(decoded.begin());

    throw_if_error(make_binary_data_from_bytes(
                       decoded.data(), decoded.size(),
                       reset_sp(out_binary_data)));

    bool found = false;
    for (size_t net = 0; net < array_size(ADDRESS_PREFIXES); ++net)
    {
        for (size_t address = 0; address < array_size(ADDRESS_PREFIXES[net]); ++address)
        {
            if (address_version == ADDRESS_PREFIXES[net][address])
            {
                *net_type = static_cast<BitcoinNetType>(net);
                *address_type = static_cast<BitcoinAddressType>(address);
                found = true;
            }
        }
    }

    if (!found)
    {
        THROW_EXCEPTION("Unknown address type.")
                << " Address type prefix: "
                << to_hex_string(address_version);
    }

    return out_binary_data;
}

} // namespace internal
} // namespace multy_core
