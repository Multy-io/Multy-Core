/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_CORE_BITCOIN_ACCOUNT_H
#define MULTY_CORE_BITCOIN_ACCOUNT_H

#include "multy_core/api.h"

#include "multy_core/src/account_base.h"
#include "multy_core/src/u_ptr.h"

namespace multy_core
{
namespace internal
{
enum BitcoinNetType
{
    BITCOIN_NET_DEFAULT = 0, // selected at run time, depending on internal settings.
    BITCOIN_MAINNET,
    BITCOIN_TESTNET,
};
enum BitcoinAddressType
{
    BITCOIN_ADDRESS_P2PKH,
    BITCOIN_ADDRESS_P2SH,
};
enum BITCON_ADDRESS_VERSION {
    BITCOIN_ADDRESS_VERSION_MAIN_NET_P2PKH = 0x0,
    BITCOIN_ADDRESS_VERSION_MAIN_NET_P2SH = 0x05,
    BITCOIN_ADDRESS_VERSION_TEST_NET_P2PKH = 0x6F,
    BITCOIN_ADDRESS_VERSION_TEST_NET_P2SH = 0xC4
};

struct BitcoinPublicKey;
struct BitcoinPrivateKey;
typedef UPtr<BitcoinPrivateKey> BitcoinPrivateKeyPtr;
typedef UPtr<BitcoinPublicKey> BitcoinPublicKeyPtr;

AccountPtr make_bitcoin_account(const char* private_key);

/** Parse given base58 encoded address and verify it's checksumm.
 *
 * @param address - input address in base58 format
 * @param net_type - (out) resulting net type adress
 * @param address_type - (out) resulting Bitcoin Address Type
 * @return address in binary form, with no checksum and address version prefix.
 *
 * @throw Exception if something went wrong.
 */
BinaryDataPtr parse_bitcoin_address(const char*,
                                    enum BitcoinNetType*,
                                    enum BitcoinAddressType*);


class MULTY_CORE_API BitcoinHDAccount : public HDAccountBase
{
public:
    BitcoinHDAccount(const ExtendedKey& bip44_master_key, uint32_t index);
    ~BitcoinHDAccount();

    AccountPtr make_account(
            const ExtendedKey& parent_key,
            AddressType type,
            uint32_t index) const override;
};

class MULTY_CORE_API BitcoinAccount : public AccountBase
{
public:
    BitcoinAccount(BitcoinPrivateKeyPtr key, HDPath path,
            BitcoinNetType net_type = BITCOIN_NET_DEFAULT);
    ~BitcoinAccount();

    std::string get_address() const override;
    bool is_testnet() const;

private:
    const BitcoinPrivateKeyPtr m_private_key;
    const bool m_is_testnet;
};

} // namespace internal
} // namespace multy_core

#endif // MULTY_CORE_BITCOIN_ACCOUNT_H
