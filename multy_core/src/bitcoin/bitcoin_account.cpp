/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */
#include "multy_core/src/bitcoin/bitcoin_account.h"

#include "multy_core/bitcoin.h"
#include "multy_core/src/bitcoin/bitcoin_key.h"

#include "multy_core/common.h"
#include "multy_core/src/exception.h"
#include "multy_core/src/exception_stream.h"
#include "multy_core/src/hd_path.h"
#include "multy_core/src/utility.h"
#include "multy_core/src/hash.h"

#include "wally_core.h"

#include <sstream>
#include <string.h>
#include <string>

namespace
{
using namespace multy_core::internal;

const size_t HASH160_BYTE_LEN = 160/8;

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

uint8_t get_address_prefix(BitcoinNetType net_type, BitcoinAddressType address_type)
{
    assert(net_type < array_size(ADDRESS_PREFIXES));
    assert(address_type < array_size(ADDRESS_PREFIXES[net_type]));

    return ADDRESS_PREFIXES[net_type][address_type];
}

std::string uint8_to_hex_string(const uint8_t value)
{
    std::stringstream stream;
    stream << std::hex;
    stream <<value;
    return stream.str();
}

uint32_t get_chain_index(BlockchainType blockchain_type)
{
    if (blockchain_type.blockchain == BLOCKCHAIN_BITCOIN
            && blockchain_type.net_type == BITCOIN_NET_TYPE_TESTNET)
    {
        return CHAIN_INDEX_TEST;
    }
    return blockchain_type.blockchain;
}

class BitcoinP2PKHAccount : public BitcoinAccount
{
public:
    using BitcoinAccount::BitcoinAccount;

    std::string get_address() const override
    {
        // P2PKH address generated from public key.
        // https://en.bitcoin.it/wiki/Technical_background_of_version_1_Bitcoin_addresses

        unsigned char pub_hash[HASH160_BYTE_LEN + 1] = {'\0'};

        // 1 - Take the corresponding public key generated with it (33 or 65
        // bytes)
        PublicKeyPtr public_key(m_private_key->make_public_key());
        BinaryData key_data = public_key->get_content();

        // 2 - Perform SHA-256 hashing on the public key
        // 3 - Perform RIPEMD-160 hashing on the result of SHA-256
        {
            // Leave the first byte intact for prefix.
            auto data = power_slice(as_binary_data(pub_hash), 1, -1);
            do_hash_inplace<BITCOIN_HASH, 160>(key_data, &data);
        }

        // 4 - Add version byte in front of RIPEMD-160 hash
        //      (0x00 for Main Network)
        pub_hash[0] = get_address_prefix(
                static_cast<BitcoinNetType>(m_blockchain_type.net_type),
                BITCOIN_ADDRESS_P2PKH);

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
                "Converting to base58 failed.");
        std::string result(base58_string_ptr.get());

        return result;
    }
};

class BitcoinP2SHWPKHAccount : public BitcoinAccount
{
public:
    using BitcoinAccount::BitcoinAccount;

    std::string get_address() const override
    {
        unsigned char pub_hash[HASH160_BYTE_LEN + 1] = {'\0'};

        // 1 - Take the corresponding public key generated with it (33 or 65
        // bytes)
        {
            unsigned char segwit_script[HASH160_BYTE_LEN + 2] = {'\0'};
            // Leave the first two bytes intact for script opcode
            BinaryData hash_pub_key = power_slice(as_binary_data(segwit_script), 2, -2);

            PublicKeyPtr public_key(m_private_key->make_public_key());
            BinaryData key_data = public_key->get_content();
            // 2 - Perform SHA-256 hashing on the public key
            // 3 - Perform RIPEMD-160 hashing on the result of SHA-256
            do_hash_inplace<BITCOIN_HASH, 160>(key_data, &hash_pub_key);
            // 4 - Perform make script segWit for lock bitcoins
            segwit_script[0] = 0x00; // Version byte witness
            segwit_script[1] = HASH160_BYTE_LEN; // Witness program is 20 bytes

            // Leave the first byte intact for prefix.
            BinaryData hash_data = power_slice(as_binary_data(pub_hash), 1, -1);
            // 5 - Perform SHA-256 hashing on the segWit script
            // 6 - Perform RIPEMD-160 hashing on the result of SHA-256
            do_hash_inplace<BITCOIN_HASH, 160>(segwit_script, &hash_data);
        }

        // 7 - Add version byte in front of RIPEMD-160 hash
        //      (0x05 for Main Network)
        pub_hash[0] = get_address_prefix(
                static_cast<BitcoinNetType>(m_blockchain_type.net_type),
                BITCOIN_ADDRESS_P2SH);

        // 8 - Perform SHA-256 hash on the extended RIPEMD-160 result
        // 9 - Perform SHA-256 hash on the result of the previous SHA-256 hash
        // 10 - Add the 4 checksum bytes from stage 7 at the end of extended
        //      RIPEMD-160 hash from stage 4.
        // 11 - Convert the result from a byte string into a base58 string
        //      using Base58Check encoding.
        CharPtr base58_string_ptr;
        THROW_IF_WALLY_ERROR(
                wally_base58_from_bytes(
                        pub_hash, sizeof(pub_hash), BASE58_FLAG_CHECKSUM,
                        reset_sp(base58_string_ptr)),
                "Converting to base58 failed.");
        std::string result(base58_string_ptr.get());

        return result;
    }
};

AccountPtr make_bitcoin_account(
        BlockchainType blockchain_type,
        BitcoinAccountType account_type,
        BitcoinPrivateKeyPtr key,
        HDPath path)
{
    if (account_type == BITCOIN_ACCOUNT_P2PKH)
    {
        return AccountPtr(new BitcoinP2PKHAccount(
                blockchain_type,
                std::move(key),
                std::move(path)));
    }
    if (account_type == BITCOIN_ACCOUNT_SEGWIT)
    {
        return AccountPtr(new BitcoinP2SHWPKHAccount(
                blockchain_type,
                std::move(key),
                std::move(path)));
    }

    THROW_EXCEPTION2(ERROR_INVALID_ARGUMENT, "Unknown BitcoinAccountType.")
            << " Value: " << account_type << ".";

    return nullptr;
}

} // namespace

namespace multy_core
{
namespace internal
{

BitcoinAccount::BitcoinAccount(BlockchainType blockchain_type,
        BitcoinPrivateKeyPtr key,
        HDPath path)
    : AccountBase(blockchain_type, path),
      m_private_key(std::move(key))
{
}

BitcoinAccount::~BitcoinAccount()
{
}

const PrivateKey& BitcoinAccount::get_private_key_ref() const
{
    return *m_private_key;
}

BitcoinHDAccount::BitcoinHDAccount(
        BlockchainType blockchain_type,
        BitcoinAccountType account_type,
        const ExtendedKey& bip44_master_key,
        uint32_t index)
    : HDAccountBase(blockchain_type,
            get_chain_index(blockchain_type), bip44_master_key, index),
      m_account_type(account_type)
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
    BitcoinPrivateKeyPtr private_key(
            new BitcoinPrivateKey(
                    slice(priv_key_data, 1, priv_key_data.len - 1),
                    static_cast<BitcoinNetType>(get_blockchain_type().net_type),
                    m_account_type,
                    EC_PUBLIC_KEY_COMPRESSED));

    return ::make_bitcoin_account(
            get_blockchain_type(),
            m_account_type,
            std::move(private_key),
            make_child_path(make_child_path(get_path(), type), index));
}

AccountPtr make_bitcoin_account(
        const char* private_key,
        BitcoinAccountType account_type)
{
    BitcoinPrivateKeyPtr key = make_bitcoin_private_key_from_wif(private_key, account_type);

    const BlockchainType blockchain_type{BLOCKCHAIN_BITCOIN, key->get_net_type()};
    return ::make_bitcoin_account(
            blockchain_type, account_type, std::move(key), HDPath());
}

BinaryDataPtr bitcoin_parse_address(const char* address,
                                    BitcoinNetType* net_type,
                                    BitcoinAddressType* address_type)
{
    INVARIANT(address != nullptr);
    INVARIANT(net_type != nullptr);
    INVARIANT(address_type != nullptr);

    BinaryDataPtr out_binary_data;
    size_t binary_size = strlen(address);
    std::vector<uint8_t> decoded(binary_size, 0);

    THROW_IF_WALLY_ERROR(
                wally_base58_to_bytes(
                    address, BASE58_FLAG_CHECKSUM, decoded.data(),
                    decoded.size(), &binary_size),
                "Invalid address.");

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
                << uint8_to_hex_string(address_version);
    }

    return out_binary_data;
}

} // namespace internal
} // namespace multy_core
