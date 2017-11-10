/* Copyright 2017 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/internal/account_base.h"

#include "multy_core/internal/key.h"

#include "multy_core/internal/utility.h"

namespace
{
using namespace wallet_core::internal;
constexpr uint32_t hardened_index(uint32_t index)
{
    return index | HARDENED_INDEX_BASE;
}

uint64_t to_map_key(AddressType type, uint32_t index)
{
    uint64_t result = type;
    result <<= 32;
    result |= index;
    return result;
}

uint32_t to_chain_code(Currency currency)
{
    // See: https://github.com/satoshilabs/slips/blob/master/slip-0044.md
    static const uint32_t currency_chain_codes[] = {
            0x80000000, // BITCOIN
            0x8000003c, // EHTER
    };

    if (array_size(currency_chain_codes) < static_cast<size_t>(currency))
    {
        throw std::runtime_error("Can't convert currency to the chain code");
    }
    return currency_chain_codes[currency];
}
// According to bip 44, complete address path looks like:
// m / purpose' / coin_type' / account' / change / address_index
const size_t BIP44_PURPOSE = 0;
const size_t BIP44_COIN_TYPE = 1;
const size_t BIP44_ACCOUNT = 2;
const size_t BIP44_ACCOUNT_PATH_DEPTH = BIP44_ACCOUNT + 1;

const uint32_t BIP44_PURPOSE_CHAIN_CODE = hardened_index(44);
static_assert(
        BIP44_PURPOSE_CHAIN_CODE == 0x8000002C,
        "invalid hardened index derivation function implementation");
} // namepace

namespace wallet_core
{
namespace internal
{
AccountBase::AccountBase(
        Currency currency,
        const PrivateKey& private_key_ref,
        const HDPath& path)
    : m_currency(currency),
      m_private_key_ref(private_key_ref),
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
    return m_private_key_ref.clone();
}

PublicKeyPtr AccountBase::get_public_key() const
{
    return m_private_key_ref.make_public_key();
}

Currency AccountBase::get_currency() const
{
    return m_currency;
}

HDAccountBase::HDAccountBase(
        const ExtendedKey& master_key, Currency currency, uint32_t index)
    : m_currency(currency), m_bip44_path(BIP44_ACCOUNT_PATH_DEPTH)
{
    // BIP44 derive account key:
    // master key -> currency key -> account key.
    ExtendedKeyPtr purpose_key, currency_key;
    const uint32_t currency_index = to_chain_code(currency);
    const uint32_t account_index = hardened_index(index);

    throw_if_error(
            make_child_key(
                    &master_key, BIP44_PURPOSE_CHAIN_CODE,
                    reset_sp(purpose_key)));
    throw_if_error(
            make_child_key(
                    purpose_key.get(), currency_index,
                    reset_sp(currency_key)));
    throw_if_error(
            make_child_key(
                    currency_key.get(), account_index,
                    reset_sp(m_account_key)));

    m_bip44_path[BIP44_PURPOSE] = BIP44_PURPOSE_CHAIN_CODE;
    m_bip44_path[BIP44_COIN_TYPE] = currency_index;
    m_bip44_path[BIP44_ACCOUNT] = account_index;
}

HDAccountBase::~HDAccountBase()
{
}

HDPath HDAccountBase::get_path() const
{
    return m_bip44_path;
}

Currency HDAccountBase::get_currency() const
{
    return m_currency;
}

AccountPtr HDAccountBase::make_leaf_account(
        AddressType type, uint32_t index) const
{
    ExtendedKeyPtr key_ptr;
    throw_if_error(
            make_child_key(
                    m_account_key.get(), static_cast<uint32_t>(type),
                    reset_sp(key_ptr)));

    AccountPtr new_account = make_account(*key_ptr, type, index);
    // TODO: use glsl::not_null
    if (!new_account)
    {
        throw std::runtime_error(
                "Internal error: make_account() returned a null");
    }

    return std::move(new_account);
}

} // namespace wallet_core
} // namespace internal
