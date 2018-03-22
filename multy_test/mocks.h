/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license
 *
 * See LICENSE for details
 */

#ifndef MULTY_TEST_MOCKS_H
#define MULTY_TEST_MOCKS_H

#include "multy_core/src/api/key_impl.h"
#include "multy_core/src/api/account_impl.h"
#include "multy_core/src/api/transaction_impl.h"
#include "multy_core/src/api/properties_impl.h"
#include "multy_core/src/utility.h"
#include "multy_core/src/bitcoin/bitcoin_account.h"

#include <string>

struct TestPublicKey : public PublicKey
{
    std::string to_string() const override;
    const BinaryData get_content() const override;
    PublicKeyPtr clone() const override;
};

struct TestPrivateKey : public PrivateKey
{
    std::string to_string() const override;

    PublicKeyPtr make_public_key() const override;

    PrivateKeyPtr clone() const override;
    BinaryDataPtr sign(const BinaryData& data) const override;
};

inline ::multy_core::internal::PrivateKeyPtr make_test_private_key()
{
    return ::multy_core::internal::PrivateKeyPtr(new TestPrivateKey);
}

inline ::multy_core::internal::PublicKeyPtr make_test_public_key()
{
    return ::multy_core::internal::PublicKeyPtr(new TestPublicKey);
}

struct TestAccount : public Account
{
public:
    TestAccount(
            BlockchainType blockchain_type,
            std::string address,
            HDPath path,
            PrivateKeyPtr private_key,
            PublicKeyPtr public_key);

    HDPath get_path() const;
    BlockchainType get_blockchain_type() const;
    std::string get_address() const;
    PrivateKeyPtr get_private_key() const;
    PublicKeyPtr get_public_key() const;

private:
    const BlockchainType blockchain_type;
    const std::string address;
    const HDPath path;
    const PrivateKeyPtr private_key;
    const PublicKeyPtr public_key;
};

struct TestHDAccount : public HDAccount
{
    TestHDAccount(
            BlockchainType blockchain_type,
            std::string address,
            HDPath path,
            PrivateKeyPtr private_key,
            PublicKeyPtr public_key);

    HDPath get_path() const override;
    BlockchainType get_blockchain_type() const override;
    AccountPtr make_leaf_account(
            AddressType type, uint32_t index) const override;
    ExtendedKeyPtr get_account_key() const override;

private:
    const BlockchainType m_blockchain_type;
    const std::string m_address;
    const HDPath m_path;
    const PrivateKeyPtr m_private_key;
    const PublicKeyPtr m_public_key;
};

struct TestTransaction : public Transaction
{
    explicit TestTransaction(const BigInt& total_value = BigInt(0));

    BlockchainType get_blockchain_type() const override;
    void update() override;
    BinaryDataPtr serialize() override;
    BigInt estimate_total_fee(size_t sources_count, size_t destinations_count) const override;
    BigInt get_total_fee() const override;
    BigInt get_total_spent() const override;
    Properties& add_source() override;
    Properties& add_destination() override;
    Properties& get_fee() override;
    Properties& get_transaction_properties() override;
    void set_message(const BinaryData& value) override;

private:
    const BlockchainType m_blockchain = BlockchainType{BLOCKCHAIN_BITCOIN, BITCOIN_NET_TYPE_MAINNET};
    const BigInt m_total;
    Properties m_properties;
    const BinaryDataPtr m_binarydata;
};

#endif // MULTY_TEST_MOCKS_H
