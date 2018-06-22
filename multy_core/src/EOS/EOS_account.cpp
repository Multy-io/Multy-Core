/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/src/EOS/EOS_account.h"

#include "multy_core/EOS.h"

#include "multy_core/common.h"
#include "multy_core/src/api/key_impl.h"
#include "multy_core/src/utility.h"
#include "multy_core/src/exception.h"
#include "multy_core/src/exception_stream.h"

#include <cassert>

#include <string.h>


namespace
{
using namespace multy_core::internal;

uint32_t get_chain_index(BlockchainType blockchain_type)
{
    if (blockchain_type.blockchain == BLOCKCHAIN_EOS
            && blockchain_type.net_type == EOS_NET_TYPE_TESTNET)
    {
        return CHAIN_INDEX_TEST;
    }
    return blockchain_type.blockchain;
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
        THROW_EXCEPTION("Not implemented yet");
    }

    std::string to_string() const override
    {
        THROW_EXCEPTION("Not implemented yet");
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
        THROW_EXCEPTION("Not implemented yet");
    }

    PrivateKeyPtr clone() const override
    {
        THROW_EXCEPTION("Not implemented yet");
    }

    BinaryDataPtr sign(const BinaryData& /*data*/) const override
    {
        THROW_EXCEPTION("Not implemented yet");
    }

    std::string to_string() const override
    {
        THROW_EXCEPTION("Not implemented yet");
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
        const ExtendedKey& /*parent_key*/,
        AddressType /*type*/,
        uint32_t /*index*/) const
{
   THROW_EXCEPTION("Not implemented yet");
}

AccountPtr make_EOS_account(BlockchainType /*blockchain_type*/,
        const char* /*serialized_private_key*/)
{
    THROW_EXCEPTION("Not implemented yet");
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
