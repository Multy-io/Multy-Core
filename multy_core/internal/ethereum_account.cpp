/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/internal/ethereum_account.h"

#include "multy_core/common.h"
#include "multy_core/internal/exception.h"
#include "multy_core/internal/key.h"
#include "multy_core/internal/utility.h"

extern "C" {
// TODO: Fix issue with not exported symbols from libwally-core for iOS builds,
//   and then re-enable using hex-converting functions from CCAN.
//#include "ccan/str/hex/hex.h"
#include "keccak-tiny/keccak-tiny.h"
} // extern "C"

#include "wally_core.h"
#include "wally_crypto.h"

#include <string.h>
#include <string>

namespace
{
using namespace wallet_core::internal;
typedef std::array<unsigned char, 20> EthereumAddressValue;

void throw_exception(const std::string& message)
{
    throw Exception("Ethereum:" + message);
}

struct EthereumPublicKey : public PublicKey
{
public:
    static const size_t DATA_SIZE = 64;
    typedef std::vector<uint8_t> KeyData;

    EthereumPublicKey(KeyData key_data)
        : m_data(std::move(key_data))
    {
        if (m_data.size() != DATA_SIZE)
        {
            throw_exception("invalid public key length");
        }
    }

    std::string to_string() const override
    {
        UPtr<char> out_str;
        throw_if_wally_error(
                wally_hex_from_bytes(
                        m_data.data(), m_data.size(), reset_sp(out_str)),
                "Failed to serialize Enthereum public key");
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

typedef UPtr<EthereumPublicKey> EthereumPublicKeyPtr;

struct EthereumPrivateKey : public PrivateKey
{
    typedef std::array<uint8_t, 32> KeyData;

    EthereumPrivateKey(const KeyData& data) : m_data(data)
    {
    }

    std::string to_string() const override
    {
        UPtr<char> out_str;
        throw_if_wally_error(
                wally_hex_from_bytes(
                        m_data.data(), m_data.size(), reset_sp(out_str)),
                "Failed to serialize Ethereum private key");
        return std::string(out_str.get());
    }

    PublicKeyPtr make_public_key() const override
    {
        unsigned char public_key_data[EC_PUBLIC_KEY_LEN];
        throw_if_wally_error(
                wally_ec_public_key_from_private_key(
                        m_data.data(), m_data.size(),
                        public_key_data, sizeof(public_key_data)),
                "Failed to derive public key from private key");

        EthereumPublicKey::KeyData uncompressed(EC_PUBLIC_KEY_UNCOMPRESSED_LEN, 0);
        throw_if_wally_error(
                wally_ec_public_key_decompress(
                        public_key_data, sizeof(public_key_data),
                        uncompressed.data(), uncompressed.size()),
                "Failed to uncompress public key");

        if (uncompressed[0] != 0x04)
        {
            throw_exception("Invalid uncompressed public key prefix");
        }
        uncompressed.erase(uncompressed.begin());

        return EthereumPublicKeyPtr(
                new EthereumPublicKey(std::move(uncompressed)));
    }

    PrivateKeyPtr clone() const override
    {
        return make_clone(*this);
    }

    BinaryDataPtr sign(const BinaryData& data) const override
    {
        throw_exception("Not implemented yet");
    }

    const KeyData& get_data() const
    {
        return m_data;
    }

private:
    const KeyData m_data;
};

EthereumAddressValue make_address(const EthereumPublicKey& key)
{
    const BinaryData key_data = key.get_content();
    std::array<unsigned char, SHA256_LEN> address_hash;
    throw_if_wally_error(
            sha3_256(
                    address_hash.data(), address_hash.size(),
                    key_data.data, key_data.len),
            "Failed to compute sha3 of public key");

    EthereumAddressValue result;
    static_assert(
            address_hash.size() - result.size() == 12,
            "Invalid EthereumAddressValue size");

    // Copy right 20 bytes
    memcpy(result.data(), address_hash.data() + 12, result.size());
    return result;
}

typedef UPtr<EthereumPrivateKey> EthereumPrivateKeyPtr;
typedef UPtr<EthereumPublicKey> EthereumPublicKeyPtr;

class EthereumAccount : public AccountBase
{
public:
    EthereumAccount(
            EthereumPrivateKeyPtr private_key, const HDPath& path = HDPath())
        : AccountBase(CURRENCY_ETHEREUM, *private_key, path),
          m_private_key(std::move(private_key))
    {
    }

    std::string get_address() const override
    {
        EthereumPublicKeyPtr public_key(
                reinterpret_cast<EthereumPublicKey*>(
                        m_private_key->make_public_key().release()));
        EthereumAddressValue m_address(make_address(*public_key));
        CharPtr out;
        wally_hex_from_bytes(m_address.data(), m_address.size(), reset_sp(out));
        return std::string(out.get());
    }

private:
    EthereumPrivateKeyPtr m_private_key;
};

} // namespace

namespace wallet_core
{
namespace internal
{

EthereumHDAccount::EthereumHDAccount(
        const ExtendedKey& bip44_master_key, uint32_t index)
    : HDAccountBase(bip44_master_key, CURRENCY_ETHEREUM, index)
{
}

AccountPtr EthereumHDAccount::make_account(
        const ExtendedKey& parent_key, AddressType type, uint32_t index) const
{
    ExtendedKeyPtr address_key;
    throw_if_error(make_child_key(&parent_key, index, reset_sp(address_key)));

    EthereumPrivateKey::KeyData data;
    static_assert(sizeof(address_key->key.priv_key) == data.max_size() + 1, "");
    memcpy(data.data(), address_key->key.priv_key + 1, data.size() - 1);

    EthereumPrivateKeyPtr private_key(new EthereumPrivateKey(data));

    AccountPtr result(
            new EthereumAccount(
                    std::move(private_key),
                    make_child_path(make_child_path(get_path(), type), index)));

    return std::move(result);
}

AccountPtr make_ethereum_account(const char* serialized_private_key)
{
    const size_t private_key_len = strlen(serialized_private_key);
    EthereumPrivateKey::KeyData key_data;
    if (private_key_len != key_data.max_size() * 2)
    {
        throw_exception("Serialized private key has invalid length");
    }
    size_t resulting_size = 0;
    throw_if_wally_error(
            wally_hex_to_bytes(serialized_private_key,
                    key_data.data(), key_data.size(), &resulting_size),
            "Failed to convert private key from hex string.");
    if (resulting_size != key_data.size())
    {
        throw_exception("Failed to deserialize private key");
    }
    throw_if_wally_error(
            wally_ec_private_key_verify(key_data.data(), key_data.size()),
            "Failed to verify private key");

    EthereumPrivateKeyPtr private_key(new EthereumPrivateKey(key_data));
    return AccountPtr(new EthereumAccount(std::move(private_key)));
}

} // namespace internal
} // namespace wallet_core
