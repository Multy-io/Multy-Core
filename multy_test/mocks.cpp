/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license
 *
 * See LICENSE for details
 */

#include "multy_test/mocks.h"
#include "multy_test/utility.h"

namespace
{
using namespace multy_core::internal;
} // namespace

std::string TestPublicKey::to_string() const
{
    return "test_public_key_string";
}

const BinaryData TestPublicKey::get_content() const
{
    static const char* PUBLIC_KEY_DATA = "private_key";
    return as_binary_data(PUBLIC_KEY_DATA);
}

PublicKeyPtr TestPublicKey::clone() const
{
    return make_clone(*this);
}

std::string TestPrivateKey::to_string() const
{
    return "test_private_key_string";
}

PublicKeyPtr TestPrivateKey::make_public_key() const
{
    return PublicKeyPtr(new TestPublicKey);
}

PrivateKeyPtr TestPrivateKey::clone() const
{
    return make_clone(*this);
}

BinaryDataPtr TestPrivateKey::sign(const BinaryData& data) const
{
    throw std::runtime_error("Not implemented");
}

TestAccount::TestAccount(BlockchainType blockchain_type,
        std::string address,
        Account::HDPath path,
        Account::PrivateKeyPtr private_key,
        Account::PublicKeyPtr public_key)
    : blockchain_type(blockchain_type),
      address(address),
      path(std::move(path)),
      private_key(std::move(private_key)),
      public_key(std::move(public_key))
{
}

HDPath TestAccount::get_path() const
{
    return path;
}

BlockchainType TestAccount::get_blockchain_type() const
{
    return blockchain_type;
}

std::string TestAccount::get_address() const
{
    return address;
}

PrivateKeyPtr TestAccount::get_private_key() const
{
    return private_key->clone();
}

PublicKeyPtr TestAccount::get_public_key() const
{
    return public_key->clone();
}

TestHDAccount::TestHDAccount(BlockchainType blockchain_type,
        std::string address,
        HDAccount::HDPath path,
        PrivateKeyPtr private_key,
        PublicKeyPtr public_key)
    : m_blockchain_type(blockchain_type),
      m_address(address),
      m_path(path),
      m_private_key(std::move(private_key)),
      m_public_key(std::move(public_key))
{
}

HDPath TestHDAccount::get_path() const
{
    return m_path;
}

BlockchainType TestHDAccount::get_blockchain_type() const
{
    return m_blockchain_type;
}

AccountPtr TestHDAccount::make_leaf_account(AddressType type, uint32_t index) const
{
    return AccountPtr(
                new TestAccount(
                    m_blockchain_type,
                    m_address,
                    m_path,
                    m_private_key->clone(),
                    m_public_key->clone()));
}

ExtendedKeyPtr TestHDAccount::get_account_key() const
{
    return make_clone(test_utility::make_dummy_extended_key());
}

TestTransaction::TestTransaction(const BigInt& total_value)
    : m_total(total_value),
      m_properties(ERROR_SCOPE_TRANSACTION, "TestTransaction")
{}

BlockchainType TestTransaction::get_blockchain_type() const
{
    return m_blockchain;
}

void TestTransaction::update()
{
}

Transaction::BinaryDataPtr TestTransaction::serialize()
{
    static const char TEST_TX[] = "TEST_TX";
    return make_clone(as_binary_data(TEST_TX));
}

BigInt TestTransaction::estimate_total_fee(size_t /*sources_count*/, size_t /*destinations_count*/) const
{
    return m_total;
}

BigInt TestTransaction::get_total_fee() const
{
    return m_total;
}

BigInt TestTransaction::get_total_spent() const
{
    return m_total;
}

Properties& TestTransaction::add_source()
{
    return m_properties;
}

Properties& TestTransaction::add_destination()
{
    return m_properties;
}

Properties& TestTransaction::get_fee()
{
    return m_properties;
}

Properties& TestTransaction::get_transaction_properties()
{
    return m_properties;
}

void TestTransaction::set_message(const BinaryData& value)
{
}
