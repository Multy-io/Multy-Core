/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/src/ethereum/ethereum_account.h"

#include "multy_core/ethereum.h"
#include "multy_core/common.h"

#include "multy_core/src/ethereum/ethereum_address.h"

#include "multy_core/src/ec_key_utils.h"
#include "multy_core/src/exception.h"
#include "multy_core/src/exception_stream.h"
#include "multy_core/src/api/key_impl.h"
#include "multy_core/src/utility.h"
#include "multy_core/src/hash.h"
#include "multy_core/src/binary_data_utility.h"

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

struct EthereumPublicKey : public PublicKey
{
public:
    static const size_t DATA_SIZE = 64;
    typedef std::vector<uint8_t> KeyData;

    explicit EthereumPublicKey(KeyData key_data)
        : m_data(std::move(key_data))
    {
        INVARIANT(m_data.size() == DATA_SIZE);
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
    enum {KEY_SIZE = 32};
    typedef std::vector<uint8_t> KeyData;

    explicit EthereumPrivateKey(KeyData data)
        : m_data(std::move(data))
    {
        if (m_data.size() != KEY_SIZE)
        {
            THROW_EXCEPTION2(ERROR_INVALID_ARGUMENT, "Invalid Ethereum private key size.")
                    << " Expected: " << KEY_SIZE
                    << " got: " << m_data.size();
        }
        ec_validate_private_key(as_binary_data(m_data));
    }

    explicit EthereumPrivateKey(BinaryData data)
        : EthereumPrivateKey(KeyData(data.data, data.data + data.len))
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
            THROW_EXCEPTION2(ERROR_KEY_CORRUPT,
                    "Invalid uncompressed public key prefix.");
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
            THROW_EXCEPTION2(ERROR_KEY_CANT_SIGN_WITH_PRIVATE_KEY,
                    "Failed to sign with private key.");
        }

        std::array<unsigned char, 65> signature_data;
        int recovery_id = 0;
        secp256k1_ecdsa_recoverable_signature_serialize_compact(
                secp_ctx(), signature_data.data(), &recovery_id, &signature);

        // NOTE: If we are to support pre-EIP155 signatures, we should add 27 to the recovery_id;
        // Please see Ethereum yellow paper and EIP155
        // https://github.com/ethereum/EIPs/blob/master/EIPS/eip-155.md
        signature_data.back() = static_cast<unsigned char>(recovery_id);

        return make_clone(as_binary_data(signature_data));
    }

    const KeyData& get_data() const
    {
        return m_data;
    }

private:
    const KeyData m_data;
};

EthereumAddress make_address(const EthereumPublicKey& key)
{
    const BinaryData key_data = key.get_content();
    const auto& address_hash = do_hash<KECCAK, 256>(key_data);

    // Copy right 20 bytes (256/8 - 12 = 20)
    return EthereumAddress(slice(address_hash, 12, address_hash.size() - 12));
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
        : AccountBase(blockchain_type, path),
          m_private_key(std::move(private_key))
    {
    }

    std::string get_address() const override
    {
        EthereumPublicKeyPtr public_key(
                reinterpret_cast<EthereumPublicKey*>(
                        m_private_key->make_public_key().release()));

        return EthereumAddress::to_string(make_address(*public_key));
    }

    const PrivateKey& get_private_key_ref() const override
    {
        return *m_private_key;
    }

    void change_private_key(int position, unsigned char byte) override
    {
        auto private_key_data = m_private_key->get_data();
        const int size = private_key_data.size();
        const int pos = position < 0 ? size + position : position;

        if (pos < 0 || pos >= size)
        {
            THROW_EXCEPTION2(ERROR_INVALID_ARGUMENT,
                    "Requested private key position is out of range.")
                    << "position requested: " << position
                    << "private key length: " << size << ".";
        }

        private_key_data[pos] = byte;
        m_private_key.reset(new EthereumPrivateKey(std::move(private_key_data)));
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

    EthereumPrivateKeyPtr private_key(
                new EthereumPrivateKey(
                    power_slice(address_key->key.priv_key, 1, -1)));

    AccountPtr result(
            new EthereumAccount(
                    get_blockchain_type(),
                    std::move(private_key),
                    make_child_path(make_child_path(get_path(), type), index)));

    return result;
}

AccountPtr make_ethereum_account(BlockchainType blockchain_type,
        const char* serialized_private_key)
{
    INVARIANT(serialized_private_key);

    const size_t private_key_len = strlen(serialized_private_key);
    EthereumPrivateKey::KeyData key_data(EthereumPrivateKey::KEY_SIZE);
    if (private_key_len != EthereumPrivateKey::KEY_SIZE * 2)
    {
        THROW_EXCEPTION2(ERROR_KEY_INVALID_SERIALIZED_STRING,
                "Serialized private key has invalid length.");
    }

    size_t resulting_size = 0;
    THROW_IF_WALLY_ERROR2(
            wally_hex_to_bytes(serialized_private_key,
                    key_data.data(), key_data.size(), &resulting_size),
            ERROR_KEY_INVALID_SERIALIZED_STRING,
            "Failed to convert private key from hex string.");
    if (resulting_size != key_data.size())
    {
        THROW_EXCEPTION2(ERROR_KEY_INVALID_SERIALIZED_STRING,
                "Failed to deserialize private key");
    }
    THROW_IF_WALLY_ERROR2(
            wally_ec_private_key_verify(key_data.data(), key_data.size()),
            ERROR_KEY_CORRUPT,
            "Failed to verify private key.");

    EthereumPrivateKeyPtr private_key(new EthereumPrivateKey(std::move(key_data)));
    return AccountPtr(new EthereumAccount(blockchain_type, std::move(private_key)));
}

} // namespace internal
} // namespace multy_core
