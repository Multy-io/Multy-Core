/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_CORE_EOS_ACCOUNT_H
#define MULTY_CORE_EOS_ACCOUNT_H

#include "multy_core/api.h"

#include "multy_core/src/account_base.h"
#include "multy_core/src/u_ptr.h"

namespace multy_core
{
namespace internal
{

class EOSPublicKey;
class EOSPrivateKey;
typedef UPtr<EOSPrivateKey> EOSPrivateKeyPtr;
typedef UPtr<EOSPublicKey> EOSPublicKeyPtr;

class MULTY_CORE_API EOSHDAccount : public HDAccountBase
{
public:
    EOSHDAccount(BlockchainType blockchain_type, const ExtendedKey& bip44_master_key, uint32_t index);
    ~EOSHDAccount();

    AccountPtr make_account(
            const ExtendedKey& parent_key,
            AddressType type,
            uint32_t index) const override;
};

AccountPtr make_EOS_account(BlockchainType blockchain_type,
        const char* serialized_private_key);

class MULTY_CORE_API EOSAccount : public AccountBase
{
public:
    EOSAccount(BlockchainType blockchain_type, EOSPrivateKeyPtr key, HDPath path);
    std::string get_address() const override;
    bool is_testnet() const;

private:
    const EOSPrivateKeyPtr m_private_key;
};

} // namespace wallet_core
} // namespace internal

#endif // MULTY_CORE_EOS_ACCOUNT_H
