/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/src/EOS/EOS_account.h"

#include "multy_core/EOS.h"

#include "multy_core/common.h"
#include "multy_core/src/api/key_impl.h"
#include "multy_core/src/hash.h"
#include "multy_core/src/utility.h"
#include "multy_core/src/exception.h"
#include "multy_core/src/exception_stream.h"
#include "multy_core/src/ec_key_utils.h"

#include "wally_crypto.h"

extern "C" {
#include "libwally-core/src/internal.h"
}
#include "libwally-core/src/secp256k1/include/secp256k1.h"

#include <cassert>

#include <string.h>

#define EOS_RECOVERY_PARAM_COMPRESSED 4
#define EOS_RECOVERY_PARAM_COMPACT 27
#define EOS_SIGNATURE_SIZE  65


namespace
{
using namespace multy_core::internal;

const size_t EOS_KEY_HASH_SIZE = 4;
const char EOS_PUBLIC_KEY_STRING_PREFIX[] = "EOS";
const uint8_t EOS_KEY_PREFIX[] = {0x80};
typedef std::array<unsigned char, EOS_SIGNATURE_SIZE + 4> EOS_SIGNATURE_ARRAY;  // 4 bytes using for hashsum

uint32_t get_chain_index(BlockchainType blockchain_type)
{
    if (blockchain_type.blockchain == BLOCKCHAIN_EOS
            && blockchain_type.net_type == EOS_NET_TYPE_TESTNET)
    {
        return CHAIN_INDEX_TEST;
    }

    return blockchain_type.blockchain;
}

// Check that signature is in canonical format
// For details please see: bitshares/bitshares1-core#1129
bool is_canonical_signature(const EOS_SIGNATURE_ARRAY& c )
{
    return !(c[1] & 0x80)
           && !(c[1] == 0 && !(c[2] & 0x80))
           && !(c[33] & 0x80)
           && !(c[33] == 0 && !(c[34] & 0x80));
}

// This extended nonce function is based on Golos implementation
static int extended_nonce_function( unsigned char *nonce32, const unsigned char *msg32,
        const unsigned char *key32, const unsigned char* /*algo16*/,
        void *data, unsigned int /*attempt*/)
{
    unsigned int* extra = static_cast<unsigned int*>(data);
    (*extra)++;
    return secp256k1_nonce_function_default(nonce32, msg32, key32, nullptr, nullptr, *extra);
}
} // namespace

namespace multy_core
{
namespace internal
{

class EOSPublicKey : public PublicKey
{
public:
    typedef std::vector<uint8_t> KeyData;
    explicit EOSPublicKey(KeyData data)
        : m_data(std::move(data))
    {}

    ~EOSPublicKey()
    {
    }

    PublicKeyPtr clone() const override
    {
        return make_clone(*this);
    }

    const BinaryData get_content() const override
    {
        return as_binary_data(m_data);
    }

    std::string to_string() const override
    {
        KeyData data(m_data);
        const auto check = do_hash<RIPEMD, 160>(m_data);
        data.insert(data.end(), check.begin(), check.begin() + EOS_KEY_HASH_SIZE);

        CharPtr out_str;
        THROW_IF_WALLY_ERROR(
                wally_base58_from_bytes(
                        data.data(), data.size(),
                        0, reset_sp(out_str)),
                    "Failed to base58-encode public key.");
        wally_bzero(data.data(), data.size());

        return EOS_PUBLIC_KEY_STRING_PREFIX + std::string(out_str.get());
    }

private:
    KeyData m_data;
};

class EOSPrivateKey : public PrivateKey
{
public:
    typedef std::vector<uint8_t> KeyData;
    explicit EOSPrivateKey(const KeyData& data)
        : m_data(data)
    {}

    explicit EOSPrivateKey(const BinaryData& data)
        : m_data(data.data, data.data + data.len)
    {}

    PublicKeyPtr make_public_key() const override
    {
        EOSPublicKey::KeyData key_data(EC_PUBLIC_KEY_LEN, 0);
        BinaryData public_key_data = as_binary_data(key_data);

        ec_private_to_public_key(as_binary_data(m_data),
                EC_PUBLIC_KEY_COMPRESSED,
                &public_key_data);

        return PublicKeyPtr(new EOSPublicKey(std::move(key_data)));
    }

    PrivateKeyPtr clone() const override
    {
        return make_clone(*this);
    }

    BinaryDataPtr sign(const BinaryData& data) const override
    {
        auto data_hash = do_hash<SHA2, 256>(data);

        EOS_SIGNATURE_ARRAY result;
        int recovery_id = 0;
        unsigned int counter = 0;
        do
        {
            secp256k1_ecdsa_signature signature;
            if (!secp256k1_ecdsa_sign(secp_ctx(), &signature,
                    data_hash.data(), m_data.data(), extended_nonce_function, &counter, &recovery_id))
            {
                THROW_EXCEPTION2(ERROR_KEY_CANT_SIGN_WITH_PRIVATE_KEY,
                        "Failed to sign with private key.");
            }

            if (!secp256k1_ecdsa_signature_serialize_compact(secp_ctx(), result.data() + 1, &signature))
            {
                THROW_EXCEPTION2(ERROR_KEY_CANT_SIGN_WITH_PRIVATE_KEY,
                        "Failed serialize signature in compact format.");
            }
        } while(!is_canonical_signature(result));

        result.begin()[0] = EOS_RECOVERY_PARAM_COMPRESSED + EOS_RECOVERY_PARAM_COMPACT + recovery_id;

        std::vector<unsigned char> temp(result.begin(), result.end()-4);
        temp.push_back(0x4b);  // K
        temp.push_back(0x31);  // 1
        auto hash = do_hash<RIPEMD, 160>(as_binary_data(temp));

        // write hashsum
        for (int i = 0; i < 4; ++i) {
            result[EOS_SIGNATURE_SIZE + i] = hash[i];
        }

        return make_clone(as_binary_data(result));
    }

    std::string to_string() const override
    {
        KeyData data(m_data);
        data.insert(data.begin(),
                std::begin(EOS_KEY_PREFIX), std::end(EOS_KEY_PREFIX));

        CharPtr out_str;
        THROW_IF_WALLY_ERROR(
                wally_base58_from_bytes(
                        data.data(), data.size(),
                        BASE58_FLAG_CHECKSUM,
                        reset_sp(out_str)),
                "Failed to base58-encode private key.");
        wally_bzero(data.data(), data.size());

        return std::string(out_str.get());
    }

private:
    KeyData m_data;
};

EOSHDAccount::EOSHDAccount(
        BlockchainType blockchain_type,
        const ExtendedKey& bip44_master_key,
        uint32_t index)
    : HDAccountBase(blockchain_type, get_chain_index(blockchain_type), bip44_master_key, index)
{}

EOSHDAccount::~EOSHDAccount()
{}

AccountPtr EOSHDAccount::make_account(
        const ExtendedKey& parent_key,
        AddressType type,
        uint32_t index) const
{
    ExtendedKeyPtr address_key;
    throw_if_error(make_child_key(&parent_key, index, reset_sp(address_key)));

    const BinaryData priv_key_data = as_binary_data(address_key->key.priv_key);
    EOSPrivateKeyPtr private_key(
            new EOSPrivateKey(
                    slice(priv_key_data, 1, priv_key_data.len - 1)));

    return AccountPtr(
            new EOSAccount(
                    get_blockchain_type(),
                    std::move(private_key),
                    make_child_path(get_path(),{type, index})));
}

AccountPtr make_EOS_account(BlockchainType blockchain_type,
        const char* serialized_private_key)
{
    // serialized key structure like: BASE58("EOS" + DATA + HASH(DATA))
    INVARIANT(serialized_private_key);

    EOSPrivateKey::KeyData private_key_bytes(strlen(serialized_private_key), 0);
    BinaryData private_key_bytes_data = as_binary_data(private_key_bytes);
    size_t private_key_size = 0;
    THROW_IF_WALLY_ERROR(
            wally_base58_to_bytes(serialized_private_key,
                    0, // Can't use BASE58_FLAG_CHECKSUM here since we save
                       // and use first iteration of hash: `hash1`.
                       // See below for details.
                    const_cast<unsigned char*>(private_key_bytes_data.data), private_key_bytes_data.len,
                    &private_key_size),
            "Failed to decode base58 private key");
    if (private_key_size < EOS_KEY_HASH_SIZE + sizeof(EOS_KEY_PREFIX))
    {
        THROW_EXCEPTION2(ERROR_KEY_INVALID_SERIALIZED_STRING,
                "Invalid decoded private key size.")
                << " Actual: " << private_key_size
                << ", expected at least: " << EOS_KEY_HASH_SIZE + sizeof(EOS_KEY_PREFIX);
    }
    if (!std::equal(std::begin(EOS_KEY_PREFIX), std::end(EOS_KEY_PREFIX),
            private_key_bytes_data.data))
    {
        THROW_EXCEPTION2(ERROR_KEY_INVALID_SERIALIZED_STRING,
                "Invalid private key prefix.");
    }

    private_key_bytes_data.len = private_key_size;

    BinaryData key_data = slice(private_key_bytes_data,
            0, private_key_size - EOS_KEY_HASH_SIZE);

    const BinaryData key_hash = slice(private_key_bytes_data,
            private_key_size - EOS_KEY_HASH_SIZE, EOS_KEY_HASH_SIZE);

    const auto hash1 = do_hash<SHA2, 256>(key_data);
    const auto hash2 = do_hash<SHA2, 256>(hash1);

    if (key_hash != slice(as_binary_data(hash1), 0, EOS_KEY_HASH_SIZE)
        && key_hash != slice(as_binary_data(hash2), 0, EOS_KEY_HASH_SIZE))
    {
        THROW_EXCEPTION2(ERROR_KEY_CORRUPT, "Invalid private key checksum.");
    }
    // skipping prefix
    key_data = slice(key_data, sizeof(EOS_KEY_PREFIX), key_data.len - sizeof(EOS_KEY_PREFIX));

    EOSPrivateKeyPtr private_key(new EOSPrivateKey(key_data));

    return AccountPtr(new EOSAccount(
            blockchain_type,
            std::move(private_key),
            HDPath{}));
}

EOSAccount::EOSAccount(BlockchainType blockchain_type, EOSPrivateKeyPtr key, HDPath path)
    : AccountBase(blockchain_type, *key, std::move(path)),
      m_private_key(std::move(key))
{}

std::string EOSAccount::get_address() const
{
    THROW_EXCEPTION2(ERROR_FEATURE_NOT_SUPPORTED,
            "Not supported: can't derive address from private key "
            "for EOS blockchain.");
}

} // namespace wallet_core
} // namespace internal
