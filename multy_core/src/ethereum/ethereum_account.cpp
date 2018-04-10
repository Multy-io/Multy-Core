/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/src/ethereum/ethereum_account.h"

#include "multy_core/ethereum.h"
#include "multy_core/common.h"

#include "multy_core/src/ec_key_utils.h"
#include "multy_core/src/exception.h"
#include "multy_core/src/exception_stream.h"
#include "multy_core/src/api/key_impl.h"
#include "multy_core/src/utility.h"
#include "multy_core/src/hash.h"

extern "C" {
#include "ccan/str/hex/hex.h"
#include "libwally-core/src/internal.h"
} // extern "C"
#include "secp256k1_recovery.h"

#include "secp256k1/include/secp256k1_recovery.h"

#include "wally_core.h"
#include "wally_crypto.h"

#include <string.h>
#include <string>

namespace
{
using namespace multy_core::internal;
const size_t ETHEREUM_BINARY_ADDRESS_SIZE = 20;
const char* ETHEREUM_ADDRESS_PREFIX = "0x";

typedef std::array<unsigned char, ETHEREUM_BINARY_ADDRESS_SIZE> EthereumAddressValue;

struct EthereumPublicKey : public PublicKey
{
public:
    static const size_t DATA_SIZE = 64;
    typedef std::vector<uint8_t> KeyData;

    explicit EthereumPublicKey(KeyData key_data)
        : m_data(std::move(key_data))
    {
        if (m_data.size() != DATA_SIZE)
        {
            THROW_EXCEPTION("Invalid public key length.");
        }
    }

    std::string to_string() const override
    {
        UPtr<char> out_str;
        THROW_IF_WALLY_ERROR(
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

    explicit EthereumPrivateKey(const KeyData& data) : m_data(data)
    {
    }

    std::string to_string() const override
    {
        UPtr<char> out_str;
        THROW_IF_WALLY_ERROR(
                wally_hex_from_bytes(
                        m_data.data(), m_data.size(), reset_sp(out_str)),
                "Failed to serialize Ethereum private key");
        return std::string(out_str.get());
    }

    PublicKeyPtr make_public_key() const override
    {
        EthereumPublicKey::KeyData key_data(EC_PUBLIC_KEY_UNCOMPRESSED_LEN, 0);
        BinaryData public_key_data = as_binary_data(key_data);

        ec_private_to_public_key(as_binary_data(m_data),
                EC_PUBLIC_KEY_UNCOMPRESSED,
                &public_key_data);

        if (key_data[0] != 0x04)
        {
            THROW_EXCEPTION("Invalid uncompressed public key prefix.");
        }
        key_data.erase(key_data.begin());
        return PublicKeyPtr(new EthereumPublicKey(std::move(key_data)));
    }

    PrivateKeyPtr clone() const override
    {
        return make_clone(*this);
    }

    BinaryDataPtr sign(const BinaryData& data) const override
    {
        const auto& data_hash = do_hash<KECCAK, 256>(data);

        secp256k1_ecdsa_recoverable_signature signature;
        if (!secp256k1_ecdsa_sign_recoverable(secp_ctx(), &signature,
                data_hash.data(), m_data.data(), nullptr, nullptr))
        {
            THROW_EXCEPTION("Failed to sign with private key.");
        }

        std::array<unsigned char, 65> signature_data;
        int recovery_id = 0;
        secp256k1_ecdsa_recoverable_signature_serialize_compact(
                secp_ctx(), signature_data.data(), &recovery_id, &signature);

        signature_data.back() = static_cast<unsigned char>(recovery_id);
        BinaryDataPtr result;
        throw_if_error(
                make_binary_data_from_bytes(
                        signature_data.data(), signature_data.size(),
                        reset_sp(result)));
        return result;
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
    const auto& address_hash = do_hash<KECCAK, 256>(key_data);

    EthereumAddressValue result;
    static_assert(
            sizeof(address_hash) - result.size() == 12,
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
            BlockchainType blockchain_type,
            EthereumPrivateKeyPtr private_key,
            const HDPath& path = HDPath())
        : AccountBase(blockchain_type, *private_key, path),
          m_private_key(std::move(private_key))
    {
    }

    std::string get_address() const override
    {
        EthereumPublicKeyPtr public_key(
                reinterpret_cast<EthereumPublicKey*>(
                        m_private_key->make_public_key().release()));

        EthereumAddressValue address(make_address(*public_key));

        std::string result(hex_str_size(strlen(ETHEREUM_ADDRESS_PREFIX))
                + hex_str_size(address.size()),
                '\0');
        if (!hex_encode(address.data(), address.size(),
                const_cast<char*>(result.data()), result.size()))
        {
            THROW_EXCEPTION("Faield to hex-encode Ethereum address.");
        }
        result.insert(0, ETHEREUM_ADDRESS_PREFIX);
        trim_excess_trailing_null(&result);

        return result;
    }

private:
    EthereumPrivateKeyPtr m_private_key;
};

uint32_t get_chain_index(BlockchainType blockchain_type)
{
    if (blockchain_type.blockchain == BLOCKCHAIN_ETHEREUM)
    {
        static const EthereumChainId TESTNETS[] = {
            ETHEREUM_CHAIN_ID_ROPSTEN,
            ETHEREUM_CHAIN_ID_RINKEBY,
            ETHEREUM_CHAIN_ID_ROOTSTOCK_TESTNET,
            ETHEREUM_CHAIN_ID_KOVAN,
            ETHEREUM_CHAIN_ID_ETC_TESTNET
        };

        if (contains(TESTNETS, blockchain_type.net_type))
        {
            return CHAIN_INDEX_TEST;
        }
    }
    return blockchain_type.blockchain;
}

} // namespace

namespace multy_core
{
namespace internal
{

EthereumHDAccount::EthereumHDAccount(
        BlockchainType blockchain_type,
        const ExtendedKey& bip44_master_key,
        uint32_t index)
    : HDAccountBase(blockchain_type, get_chain_index(blockchain_type), bip44_master_key, index)
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
                    get_blockchain_type(),
                    std::move(private_key),
                    make_child_path(make_child_path(get_path(), type), index)));

    return result;
}

AccountPtr make_ethereum_account(const char* serialized_private_key)
{
    const size_t private_key_len = strlen(serialized_private_key);
    EthereumPrivateKey::KeyData key_data;
    if (private_key_len != key_data.max_size() * 2)
    {
        THROW_EXCEPTION("Serialized private key has invalid length");
    }
    size_t resulting_size = 0;
    THROW_IF_WALLY_ERROR(
            wally_hex_to_bytes(serialized_private_key,
                    key_data.data(), key_data.size(), &resulting_size),
            "Failed to convert private key from hex string.");
    if (resulting_size != key_data.size())
    {
        THROW_EXCEPTION("Failed to deserialize private key");
    }
    THROW_IF_WALLY_ERROR(
            wally_ec_private_key_verify(key_data.data(), key_data.size()),
            "Failed to verify private key");

    const BlockchainType blockchain_type{BLOCKCHAIN_ETHEREUM, ETHEREUM_CHAIN_ID_MAINNET};
    EthereumPrivateKeyPtr private_key(new EthereumPrivateKey(key_data));
    return AccountPtr(new EthereumAccount(blockchain_type, std::move(private_key)));
}

BinaryDataPtr ethereum_parse_address(const char* address)
{
    if (!address)
    {
        THROW_EXCEPTION("Invalid address: nullptr.");
    }

    if (strncmp(address, ETHEREUM_ADDRESS_PREFIX, strlen(ETHEREUM_ADDRESS_PREFIX)) == 0)
    {
        address += strlen(ETHEREUM_ADDRESS_PREFIX);
    }

    BinaryDataPtr binary_address;
    throw_if_error(make_binary_data_from_hex(address,
            reset_sp(binary_address)));

    if (binary_address->len != ETHEREUM_BINARY_ADDRESS_SIZE)
    {
        THROW_EXCEPTION("Invalid decoded address size.")
                << " Expected size: " << ETHEREUM_BINARY_ADDRESS_SIZE
                << " Actual size: " << binary_address->len;
    }

    return binary_address;
}

} // namespace internal
} // namespace multy_core
