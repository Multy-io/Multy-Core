/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/src/account_base.h"

#include "multy_core/src/exception.h"
#include "multy_core/src/api/key_impl.h"
#include "multy_core/src/utility.h"

namespace
{
using namespace multy_core::internal;

// According to bip 44, complete address path looks like:
// m / purpose' / coin_type' / account' / change / address_index
const size_t BIP44_PURPOSE = 0;
const size_t BIP44_COIN_TYPE = 1;
const size_t BIP44_ACCOUNT = 2;
const size_t BIP44_ACCOUNT_PATH_DEPTH = BIP44_ACCOUNT + 1;

const uint32_t BIP44_PURPOSE_CHAIN_CODE = hardened_index(0x2C);
const uint32_t BIP44_TESTNET_CHAIN_CODE = hardened_index(CHAIN_INDEX_TEST);

static_assert(
        BIP44_PURPOSE_CHAIN_CODE == 0x8000002C,
        "invalid hardened index derivation function implementation");

static_assert(
        BIP44_TESTNET_CHAIN_CODE == 0x80000001,
        "invalid hardened index derivation function implementation");

} // namepace

namespace multy_core
{
namespace internal
{
AccountBase::AccountBase(
        BlockchainType blockchain_type,
        const HDPath& path)
    : m_blockchain_type(blockchain_type),
      m_path(path)
{
}

AccountBase::~AccountBase()
{
}

HDPath AccountBase::get_path() const
{
    return m_path;
}

PrivateKeyPtr AccountBase::get_private_key() const
{
    return get_private_key_ref().clone();
}

PublicKeyPtr AccountBase::get_public_key() const
{
    return get_private_key_ref().make_public_key();
}

BlockchainType AccountBase::get_blockchain_type() const
{
    return m_blockchain_type;
}

HDAccountBase::HDAccountBase(
        BlockchainType blockchain_type,
        uint32_t chain_index,
        const ExtendedKey& bip44_master_key,
        uint32_t index)
    : m_blockchain_type(blockchain_type),
      m_account_key(),
      m_bip44_path(BIP44_ACCOUNT_PATH_DEPTH)
{
    // BIP44 derive account key:
    // master key -> blockchain key -> account key.
    ExtendedKeyPtr purpose_key, chain_key;
    const uint32_t chain_code = hardened_index(chain_index);
    const uint32_t account_index = hardened_index(index);

    throw_if_error(
            make_child_key(
                    &bip44_master_key, BIP44_PURPOSE_CHAIN_CODE,
                    reset_sp(purpose_key)));
    throw_if_error(
            make_child_key(
                    purpose_key.get(), chain_code,
                    reset_sp(chain_key)));
    throw_if_error(
            make_child_key(
                    chain_key.get(), account_index,
                    reset_sp(m_account_key)));

    m_bip44_path[BIP44_PURPOSE] = BIP44_PURPOSE_CHAIN_CODE;
    m_bip44_path[BIP44_COIN_TYPE] = chain_code;
    m_bip44_path[BIP44_ACCOUNT] = account_index;
}

HDAccountBase::~HDAccountBase()
{
}

HDPath HDAccountBase::get_path() const
{
    return m_bip44_path;
}

BlockchainType HDAccountBase::get_blockchain_type() const
{
    return m_blockchain_type;
}

ExtendedKeyPtr HDAccountBase::get_account_key() const
{
    return make_clone(*m_account_key);
}

AccountPtr HDAccountBase::make_leaf_account(
        AddressType type, uint32_t index) const
{
    ExtendedKeyPtr key_ptr;
    throw_if_error(
            make_child_key(
                    m_account_key.get(), static_cast<uint32_t>(type),
                    reset_sp(key_ptr)));

    return  make_account(*key_ptr, type, index);
}

} // namespace multy_core
} // namespace internal
