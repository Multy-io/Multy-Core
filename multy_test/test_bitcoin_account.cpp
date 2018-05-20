/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/account.h"
#include "multy_core/key.h"
#include "multy_core/src/bitcoin/bitcoin_account.h"

#include "multy_core/src/api/account_impl.h"
#include "multy_core/src/api/key_impl.h"
#include "multy_core/src/u_ptr.h"
#include "multy_core/src/utility.h"

#include "multy_test/serialized_keys_test_base.h"
#include "multy_test/supported_blockchains.h"
#include "multy_test/utility.h"
#include "multy_test/value_printers.h"

#include "gtest/gtest.h"

namespace
{
using namespace multy_core::internal;
using namespace test_utility;

const char* BITCOIN_ADDRESSES_MAINNET_P2PKH[] =
{
    "12pWhnTAfMro4rJVk32YjvFq1NqtwmBNwU",
    "1fjRrB4XXJWeiw1686zCKYGSNjFqLchYQ",
    "14kHzG9194ojtiXFdbcdTkUCUsFbEfu5MW",
    "15XSfkydZEYkWyQGSUp9gGrC1dTi6wM2Ei",
    "15prjQSoMX5cCKhxSAgcdz47bZRN2ge2th",
    "18NpPnJfTaDyAPaz36m3TUHcT77dGHRnoK",
    "19cmiDmj4FwJVvcfDZXhC8kYeJLZytisZ8",
    "1F5y5E5FMc5YzdJtB9hLaUe43GDxEKXENJ",
    "1M9TbQnXzESQZjR1J3pMooQUpHEqUnrKZh",
    "1Mh7iAQstKFJDePMJ5MG1C1ztsnmuLV5Y6",
    "1Mu8765kCAuP5NaoKZUMgBieTT7KqcUBbZ",
    "1QFqqMUD55ZV3PJEJZtaKCsQmjLT6JkjvJ",
    "12T6zBkXZT5Tyg1W7ssXL27MLoE3c8NmwX",
};

const char* BITCOIN_ADDRESSES_TESTNET_P2PKH[] =
{
    "mzqiDnETWkunRDZxjUQ34JzN1LDevh5DpU",
    "mpJDSHJcytfxp9asgo2pqihabHmmJkqJuM",
    "mfgq7S1Va1GREFgN66MVoxX35X6juKov6A",
    "mk6a6qeXNXuQDpA4DPxuouTJJTeFYJAkep",
    "n2Mm3o7uQ9QibAnMPi3rjoXAjkVXikyRpj",
    "n4jX2S4iL9eNknDL3ELCTZtupqxswDxK9x",
    "n1ZE4fDzH5usehyo52XUGsz3DjK6YBZPue"
};

const char* BITCOIN_ADDRESSES_MAINNET_P2SH[] =
{
    "3EktnHQD7RiAE6uzMj2ZifT9YgRrkSgzQX",
    "3P14159f73E4gFr7JterCCQh9QjiTjiZrG",
    "342ftSRCvFHfCeFFBuz4xwbeqnDw6BGUey",
    "347N1Thc213QqfYCz3PZkjoJpNv5b14kBd",
    "3C5DeqHzyRaHX4pSitwFykaEZwc5Ves42F",
    "3G636p5qPjxzJj58Tm9uaRpHWR8aUYaGt2",
};

const char* BITCOIN_ADDRESSES_TESTNET_P2SH[] =
{
    "2N7EKxnxZEaHNyz4hCHhzymLnEE9Nf3vSJ3",
    "2Mz6DxmESjuXMCUR3nMFotaFETX9JYDaMbW",
    "2MygBGCpaBVSsXCdvUzgkTNdtL3jGqeF6wE",
    "2NFu6AJf6foqyVwC78UFRTXafHQ38d3oyWt",
    "2MzuV16NLy1zFr3KDmMd1sN8EgAqyCbBBzG",
    "2NB9MQSvXUq2pA7AHSyvqxetv811mRZMNUd",
};

SerializedKeyTestCase BITCOIN_MAINNET_P2PKH_KEY_DATA[] = {
        {
            "L5GRrPvFZswYD74UdHWsg1yVbZqvMDe9jj6frutVx8Y6Y2mgWtEk",
            "",
            "12pWhnTAfMro4rJVk32YjvFq1NqtwmBNwU"
        },
        {
            "L33peSRvmY3MyRMdVqMKx72h4v8wugNhMwLCyzfD8xS1eZTRvkW1",
            "mUyLSFTU6XUuNYJjHEoMJiUEsZdva77TYxV8tY4PD2VN",
            "1fjRrB4XXJWeiw1686zCKYGSNjFqLchYQ"
        },
        {
            "L2ZG56cGSPDyvbAJoxUKmhUVRmKpG5bmRoY775ZYNPvvCnx58b9n",
            "oUfskkKeD4ttBMtdMkvZNHdCHjzquRqnFVuXTjkMKfYx",
            "14kHzG9194ojtiXFdbcdTkUCUsFbEfu5MW"
        },
        {
            "KwHeGE1qDjmBe4ecJoZww6WMj7BqnxsjXx4HWPneSRQDY7NBph4z",
            "fA7fqi3yibH7cUwfMH7BBaghhDYmZSLt5BmRUqe4koGH",
            "15XSfkydZEYkWyQGSUp9gGrC1dTi6wM2Ei"
        },
        {
            "L3fQB13UBadjNAGAZpL2us4xw6YWRV1ykgK8akvtQks3wQcgVAjG",
            "2Ahw45DdDSLcsSRbwiajV1QDzDaMfw2XH6Kt91ZRebDFv",
            "15prjQSoMX5cCKhxSAgcdz47bZRN2ge2th"
        },
        {
            "L3cN7tZ3gmnRQtFcqD6R44s6kzUfebUJ8GrmGCtWj65M6C9LX1J4",
            "2BQab9VLwL8HnDwJYvZqP4wntAx2MEVLHfFieTVs384Wu",
            "18NpPnJfTaDyAPaz36m3TUHcT77dGHRnoK"
        },
        {
            "KxgcSQsS3LZBHm1T6K8ijsNyobWEbN3v5QZKDPY4XSYoLCqdeHpr",
            "bjgoXKUmQnTYKxGU7VDC5RZ2WjGebudF7UoFJoY2Hqc8",
            "19cmiDmj4FwJVvcfDZXhC8kYeJLZytisZ8"
        },
        {
            "5KC4ejrDjv152FGwP386VD1i2NYc5KkfSMyv1nGy1VGDxGHqVY3",
            "MxXTacR6Fijwv6uAukGRUbb16J3UBjuxshFcCL8RzY6Cy6hM6b1Xv8kgmK3rnBTyP2"
            "m8yd7NxErcpy6vRKUZ4tRJ",
            "1F5y5E5FMc5YzdJtB9hLaUe43GDxEKXENJ"
        },
        {
            "5HtqcFguVHA22E3bcjJR2p4HHMEGnEXxVL5hnxmPQvRedSQSuT4",
            "PPT6wvt6t9EwpQpfba6LmL3ZoYqVfk5JoEEFB935rBs5t5uX4BjuhqUNB6wNwRzYv6"
            "hUFcWH4rbuwjMUMmRg2YBB",
            "1M9TbQnXzESQZjR1J3pMooQUpHEqUnrKZh"
        },
        {
            "L1gz2qcYqxn3rzMW5hdZZDAR8hB4mke6NnpYvjxGHSfYGJ9bFLJV",
            "ymybJgusbqjGD2oGw224ESjRTK3mJJKR6au6kPhagrkG",
            "1Mh7iAQstKFJDePMJ5MG1C1ztsnmuLV5Y6"
        },
        {
            "L5ABUi5Dup2KUXduFpRTa2jZyaK8Jv8gxKS71XXvCe2KWxqSWP6s",
            "jxJGdL3YiAQz3kzXDU1YniQUcSKoBtLd1qZefTRmhUYc",
            "1Mu8765kCAuP5NaoKZUMgBieTT7KqcUBbZ"
        },
        {
            "5HxWvvfubhXpYYpS3tJkw6fq9jE9j18THftkZjHHfmFiWtmAbrj",
            "MhYEtBZVFPa7stZRurPVT8JFGztqZLMj5f9K53BvLhqgZ9CfHsNDNFeiC5hkWb2GZs"
            "QALysHYrVPaURsNfaPER9r",
            "1QFqqMUD55ZV3PJEJZtaKCsQmjLT6JkjvJ"
        },
        {
            "KzpFvx31hSeesrRzEUYdZkcebNV5HfehYCdjtHB5kNEFBudXEBao",
            "",
            "12T6zBkXZT5Tyg1W7ssXL27MLoE3c8NmwX"
        }
};

SerializedKeyTestCase BITCOIN_MAINNET_P2SH_P2WPKH_KEY_DATA[] = {
        {
            "L1Ffwzw2HAKoiLWzYawsaVL5XByeYYPdHyGwYyEMWjwGMwsaxdsv",
            "",
            "3MXpZGSNv8zWjUhNQF4q9EcJgJAz6Ap4gu"
        },
        {
            "KwHeGE1qDjmBe4ecJoZww6WMj7BqnxsjXx4HWPneSRQDY7NBph4z",
            "fA7fqi3yibH7cUwfMH7BBaghhDYmZSLt5BmRUqe4koGH",
            "36CZDHmSMqtEyKky2xBuCmae2Jan6FynRe"
        },
        {
            "KwTECtLxekB95Pam68KNUa4XK6GhmJ2vah4xpDvk7qUmZMQR6d9A",
            "mUyLSFTU6XUuNYJjHEoMJiUEsZdva77TYxV8tY4PD2VN",
            "3PfWZkKQ2QkfZmSd8AdsA9tSFtQTc2qRuT"
        },
        {
            "L2ZG56cGSPDyvbAJoxUKmhUVRmKpG5bmRoY775ZYNPvvCnx58b9n",
            "oUfskkKeD4ttBMtdMkvZNHdCHjzquRqnFVuXTjkMKfYx",
            "38AdBTBZZ3TFYZNtR5UyWLukfphCaKEbVf"
        },
        {
            "L3fQB13UBadjNAGAZpL2us4xw6YWRV1ykgK8akvtQks3wQcgVAjG",
            "2Ahw45DdDSLcsSRbwiajV1QDzDaMfw2XH6Kt91ZRebDFv",
            "3NNhrGrNQsQZrsCARLnHBbtCSbF9sWW5PN"
        },
        {
            "L3cN7tZ3gmnRQtFcqD6R44s6kzUfebUJ8GrmGCtWj65M6C9LX1J4",
            "2BQab9VLwL8HnDwJYvZqP4wntAx2MEVLHfFieTVs384Wu",
            "34kRTALka9DYayFAWDxQnbMNAMJBZwzUE7"
        },
        {
            "KxgcSQsS3LZBHm1T6K8ijsNyobWEbN3v5QZKDPY4XSYoLCqdeHpr",
            "bjgoXKUmQnTYKxGU7VDC5RZ2WjGebudF7UoFJoY2Hqc8",
            "3CpjAaWyVRvNbPyWzw2WWfvJiYaPZAr96p"
        },
        {
            "L1gz2qcYqxn3rzMW5hdZZDAR8hB4mke6NnpYvjxGHSfYGJ9bFLJV",
            "ymybJgusbqjGD2oGw224ESjRTK3mJJKR6au6kPhagrkG",
            "3Eg9JiExocGGn84wYefzLzzoiAmHN2QAFi"
        },
        {
            "L5ABUi5Dup2KUXduFpRTa2jZyaK8Jv8gxKS71XXvCe2KWxqSWP6s",
            "jxJGdL3YiAQz3kzXDU1YniQUcSKoBtLd1qZefTRmhUYc",
            "33wqBwHG6rhcTPrsJvyQ1bNsqX8SZ47MRu"
        },
        {
            "KzpFvx31hSeesrRzEUYdZkcebNV5HfehYCdjtHB5kNEFBudXEBao",
            "",
            "3QDpHFJZFkP65W1rGepMiiYWMkcJgtaGM9"
        }
};

// public key in base 32 format.
SerializedKeyTestCase BITCOIN_TESTNET_P2SH_P2WPKH_KEY_DATA[] = {
        {
            "cQv3v5zwrES3Pfswwen5tRmhFFm9iXJuNHQ2pkHS5TRXnS9aVTq7",
            "",
            "2MxCc5BwTL9zJMQSQe1MnHLwscXtYzEssTG"
        },
        {
            "cUr5VKeDDCm5KFpBgza2Xktrf8VxPmmh9hLJ26RpD3FCray5iqgf",
            "02e4eb34ae5eac1447da4e5fa2a9de734014f988d3af336e1f4728b46dfe7e29c5",
            "2N1JZBatXZVYC9vwB6truPaBExZYPdapDhB"
        },
        {
            "cQx7dy9bnAXhDTS5z1zNxaiedgUmnvZuqFzpFvxHz1utjjToRPCQ",
            "02326800b109413ac0558839c65a2f3e1f7f12a8bf782ae51e6e94f79b3da33f8e",
            "2N5AEiyZEwzfvbZ5hVms7MvnmP6dNgHSH6p"
        },
        {
            "cTmh5ruAnuU9ziwZmHWbjyreKxDRFPU2k2pdjzyDA93U8pPHh4kN",
            "03cbd33b97193abe97fd872c3eda4be7b3c0d7ba3dce9fd5fd5d7f630e0283fd6b",
            "2N5XGmweCMaTStJxNfe56hJzfKVBpLuxDu8"
        },
        {
            "cSYBWN6JbLwm17tJJWZspaQemsFwb6zf5rFkzG4HXjaQpVXqxN5n",
            "033f39deb7a4caa47707e710219c344d25fea5feae29d80f16bce4380507d5de8f",
            "2NF5D225PV9ZUnwv1aU7madnaPePYeKW2C4"
        }
};

INSTANTIATE_TEST_CASE_P(
        DISABLED_Bitcoin,
        SerializedKeyTestP,
        ::testing::Combine(
                ::testing::Values(BITCOIN_TEST_NET),
                ::testing::Values(BITCOIN_ACCOUNT_SEGWIT),
                ::testing::ValuesIn(BITCOIN_TESTNET_P2SH_P2WPKH_KEY_DATA)));

INSTANTIATE_TEST_CASE_P(
        DISABLED_Bitcoin_P2WPKH_,
        SerializedKeyTestP,
        ::testing::Combine(
                ::testing::Values(BITCOIN_MAIN_NET),
                ::testing::Values(BITCOIN_ACCOUNT_SEGWIT),
                ::testing::ValuesIn(BITCOIN_MAINNET_P2SH_P2WPKH_KEY_DATA)));


INSTANTIATE_TEST_CASE_P(
        Bitcoin_P2PKH,
        SerializedKeyTestP,
        ::testing::Combine(
                ::testing::Values(BITCOIN_MAIN_NET),
                ::testing::Values(BITCOIN_ACCOUNT_P2PKH),
                ::testing::ValuesIn(BITCOIN_MAINNET_P2PKH_KEY_DATA)));

INSTANTIATE_TEST_CASE_P(
        Bitcoin_MAINNET_P2PKH,
        CheckAddressTestP,
        ::testing::Combine(
            ::testing::Values(BITCOIN_MAIN_NET),
            ::testing::ValuesIn(BITCOIN_ADDRESSES_MAINNET_P2PKH)));

INSTANTIATE_TEST_CASE_P(
        Bitcoin_TESTNET_P2PKH,
        CheckAddressTestP,
        ::testing::Combine(
            ::testing::Values(BITCOIN_TEST_NET),
            ::testing::ValuesIn(BITCOIN_ADDRESSES_TESTNET_P2PKH)));

INSTANTIATE_TEST_CASE_P(
        Bitcoin_MAINNET_P2SH,
        CheckAddressTestP,
        ::testing::Combine(
            ::testing::Values(BITCOIN_MAIN_NET),
            ::testing::ValuesIn(BITCOIN_ADDRESSES_MAINNET_P2SH)));

INSTANTIATE_TEST_CASE_P(
        Bitcoin_TESTNET_P2SH,
        CheckAddressTestP,
        ::testing::Combine(
            ::testing::Values(BITCOIN_TEST_NET),
            ::testing::ValuesIn(BITCOIN_ADDRESSES_TESTNET_P2SH)));

struct SignTestCase
{
    const char* private_key;
    const char* message;
    const char* expected_signature;
};

const SignTestCase SIGN_CASES[]  = {
    {
        "5HxWvvfubhXpYYpS3tJkw6fq9jE9j18THftkZjHHfmFiWtmAbrj",
        "Very deterministic message",
        "304402205dbbddda71772d95ce91cd2d14b592cfbc1dd0aabd6a394b6c2d377bbe59d3"
        "1d022014ddda21494a4e221f0824f0b8b924c43fa43c0ad57dccdaa11f81a6bd4582f6"
    },
    { // Same key as above, but in "compact" format
        "Kwr371tjA9u2rFSMZjTNun2PXXP3WPZu2afRHTcta6KxEUdm1vEw",
        "Very deterministic message",
        "304402205dbbddda71772d95ce91cd2d14b592cfbc1dd0aabd6a394b6c2d377bbe59d3"
        "1d022014ddda21494a4e221f0824f0b8b924c43fa43c0ad57dccdaa11f81a6bd4582f6"
    },
    {
        "5KC4ejrDjv152FGwP386VD1i2NYc5KkfSMyv1nGy1VGDxGHqVY3",
        "Very deterministic message",
        "3044022052d8a32079c11e79db95af63bb9600c5b04f21a9ca33dc129c2bfa8ac9dc1c"
        "d5022061d8ae5e0f6c1a16bde3719c64c2fd70e404b6428ab9a69566962e8771b5944d"
    },
    { // Same key as above, but in "compact" format
        "L3Hq7a8FEQwJkW1M2GNKDW28546Vp5miewcCzSqUD9kCAXrJdS3g",
        "Very deterministic message",
        "3044022052d8a32079c11e79db95af63bb9600c5b04f21a9ca33dc129c2bfa8ac9dc1c"
        "d5022061d8ae5e0f6c1a16bde3719c64c2fd70e404b6428ab9a69566962e8771b5944d"
    }
};

class BitcoinTestSign : public ::testing::TestWithParam<SignTestCase>
{
};

TEST_P(BitcoinTestSign, SignWithPrivateKey)
{
    const char* private_key_data = GetParam().private_key;
    const char* signature_data = GetParam().expected_signature;
    const char* message_data = GetParam().message;

    AccountPtr account;
    HANDLE_ERROR(make_account(
            BITCOIN_MAIN_NET,
            BITCOIN_ACCOUNT_P2PKH,
            private_key_data,
            reset_sp(account)));
    ASSERT_NE(nullptr, account);

    PrivateKeyPtr private_key = account->get_private_key();
    BinaryDataPtr signature = private_key->sign(as_binary_data(message_data));

    EXPECT_EQ(as_binary_data(from_hex(signature_data)), *signature);
}

INSTANTIATE_TEST_CASE_P(
        Bitcoin, BitcoinTestSign, ::testing::ValuesIn(SIGN_CASES));

} // namespace
