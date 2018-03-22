/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_CORE_GOLOS_ACCOUNT_H
#define MULTY_CORE_GOLOS_ACCOUNT_H

#include "multy_core/api.h"

#include "multy_core/src/account_base.h"
#include "multy_core/src/u_ptr.h"

namespace multy_core
{
namespace internal
{

class GolosPublicKey;
class GolosPrivateKey;
typedef UPtr<GolosPrivateKey> GolosPrivateKeyPtr;
typedef UPtr<GolosPublicKey> GolosPublicKeyPtr;

class MULTY_CORE_API GolosHDAccount : public HDAccountBase
{
public:
    GolosHDAccount(BlockchainType blockchain_type, const ExtendedKey& bip44_master_key, uint32_t index);
    ~GolosHDAccount();

    AccountPtr make_account(
            const ExtendedKey& parent_key,
            AddressType type,
            uint32_t index) const override;
};

AccountPtr make_golos_account(const char* serialized_private_key);

class MULTY_CORE_API GolosAccount : public AccountBase
{
public:
    GolosAccount(BlockchainType blockchain_type, GolosPrivateKeyPtr key, HDPath path);
    std::string get_address() const override;
    bool is_testnet() const;

private:
    const GolosPrivateKeyPtr m_private_key;
};

} // namespace wallet_core
} // namespace internal

#endif // MULTY_CORE_GOLOS_ACCOUNT_H
