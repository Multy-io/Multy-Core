/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_CORE_BITCOIN_ACCOUNT_H
#define MULTY_CORE_BITCOIN_ACCOUNT_H

#include "multy_core/api.h"
#include "multy_core/bitcoin.h"

#include "multy_core/src/account_base.h"
#include "multy_core/src/u_ptr.h"

namespace multy_core
{
namespace internal
{

class BitcoinAccount;
struct BitcoinPublicKey;
struct BitcoinPrivateKey;
typedef UPtr<BitcoinAccount> BitcoinAccountPtr;
typedef UPtr<BitcoinPrivateKey> BitcoinPrivateKeyPtr;
typedef UPtr<BitcoinPublicKey> BitcoinPublicKeyPtr;

void bitcoin_hash_160(const BinaryData& input, BinaryData* output);

AccountPtr make_bitcoin_account(
        const char* private_key,
        BitcoinAccountType account_type);

/** Parse given base58 encoded address and verify it's checksumm.
 *
 * @param address - input address in base58 format
 * @param net_type - (out) resulting net type adress
 * @param address_type - (out) resulting Bitcoin Address Type
 * @return address in binary form, with no checksum and address version prefix.
 *
 * @throw Exception if something went wrong.
 */
BinaryDataPtr bitcoin_parse_address(
        const char*,
        BitcoinNetType*,
        BitcoinAddressType*);

class MULTY_CORE_API BitcoinHDAccount : public HDAccountBase
{
public:
    BitcoinHDAccount(
            BlockchainType blockchain_type,
            BitcoinAccountType account_type,
            const ExtendedKey& bip44_master_key,
            uint32_t index);

    ~BitcoinHDAccount();

    AccountPtr make_account(
            const ExtendedKey& parent_key,
            AddressType type,
            uint32_t index) const override;

private:
    const BitcoinAccountType m_account_type;
};

class MULTY_CORE_API BitcoinAccount : public AccountBase
{
public:
    BitcoinAccount(
            BlockchainType blockchain_type,
            BitcoinPrivateKeyPtr key,
            HDPath path);

    ~BitcoinAccount();

    virtual std::string get_address() const = 0;

    static std::string get_address_from_private_key(const PrivateKey& key);
    BitcoinAccountType get_account_type() const;

protected:
    const BitcoinPrivateKeyPtr m_private_key;
};

} // namespace internal
} // namespace multy_core

#endif // MULTY_CORE_BITCOIN_ACCOUNT_H
