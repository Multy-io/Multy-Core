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

class EosPublicKey;
class EosPrivateKey;
typedef UPtr<EosPrivateKey> EosPrivateKeyPtr;
typedef UPtr<EosPublicKey> EosPublicKeyPtr;

class MULTY_CORE_API EosHDAccount : public HDAccountBase
{
public:
    EosHDAccount(BlockchainType blockchain_type, const ExtendedKey& bip44_master_key, uint32_t index);
    ~EosHDAccount();

    AccountPtr make_account(
            const ExtendedKey& parent_key,
            AddressType type,
            uint32_t index) const override;
};

AccountPtr make_EOS_account(BlockchainType blockchain_type,
        const char* serialized_private_key);

class MULTY_CORE_API EosAccount : public AccountBase
{
public:
    EosAccount(BlockchainType blockchain_type, EosPrivateKeyPtr key, HDPath path);
    std::string get_address() const override;

    const PrivateKey& get_private_key_ref() const override;
private:
    const EosPrivateKeyPtr m_private_key;
};

} // namespace wallet_core
} // namespace internal

#endif // MULTY_CORE_EOS_ACCOUNT_H
