/* Copyright 2017 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_CORE_BITCOIN_ACCOUNT_H
#define MULTY_CORE_BITCOIN_ACCOUNT_H

#include "multy_core/api.h"

#include "multy_core/internal/account_base.h"
#include "multy_core/internal/u_ptr.h"

namespace wallet_core
{
namespace internal
{
enum BitcoinNetType
{
    BITCOIN_NET_DEFAULT = 0, // selected at run time, depending on internal settings.
    BITCOIN_MAINNET,
    BITCOIN_TESTNET,
};

class BitcoinPublicKey;
class BitcoinPrivateKey;
typedef UPtr<BitcoinPrivateKey> BitcoinPrivateKeyPtr;
typedef UPtr<BitcoinPublicKey> BitcoinPublicKeyPtr;

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

AccountPtr make_bitcoin_account(const char* private_key);

class MULTY_CORE_API BitcoinAccount : public AccountBase
{
public:
    BitcoinAccount(BitcoinPrivateKeyPtr key, HDPath path,
            BitcoinNetType net_type = BITCOIN_NET_DEFAULT);
    std::string get_address() const override;
    bool is_testnet() const;

private:
    const BitcoinPrivateKeyPtr m_private_key;
    const bool m_is_testnet;
};

} // namespace wallet_core
} // namespace internal

#endif // MULTY_CORE_BITCOIN_ACCOUNT_H
