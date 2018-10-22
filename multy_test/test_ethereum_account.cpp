/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/account.h"
#include "multy_core/src/ethereum/ethereum_account.h"

#include "multy_core/ethereum.h"
#include "multy_core/mnemonic.h"
#include "multy_core/key.h"
#include "multy_core/src/api/account_impl.h"
#include "multy_core/src/api/key_impl.h"
#include "multy_core/src/api/sha3_impl.h"
#include "multy_core/src/utility.h"

#include "multy_test/serialized_keys_test_base.h"
#include "multy_test/supported_blockchains.h"
#include "multy_test/utility.h"
#include "multy_test/value_printers.h"
#include "multy_test/utility.h"

#include "gtest/gtest.h"

namespace
{
using namespace multy_core::internal;

const char* TEST_CASES_ADDRESS[] = {
    "0x54f46318d8f83c28b719ccf01ab4628e1e8f65fa",
    "0x256c6f6e7ccb5a6c2d7328a8ab9b79333beba1e1",
    "0x841d6a3c7854d3250433b4fddd6ce8b4093e7172",
    "0x23d0033fb9717563cb6d8b1e257a9d6027b48953",
    "0x2f87c0482c8d8490b3afb68cfe48bea81510a195",
    "0x6ef90e7dfec25f5b74a2c40eb05fce94cefe52a1",
    "0x390c61b3bee479ba9db509fb2277c16458553299",
    "0x60898387644b70c3d74999b30766947c09b2a9c6",
    "0x7a965b0a45a786ce9249e92ac1746053aa84a690",
    "0x8405b8c046b94deb5ca5adf81660164b853bbda4",
    "0x7b098532d8bffb846357e209094d122da98c5ad1",
    "0xb01f82aabacaa0ed45d041c227b55f0e2f78e897",
    "0x3f135cb1f425aed875b29e94153c0ccf9cec9f3f",
    "0x1d90a440b8faff514cfb8b0143df13ccd5b8a7e1",
    "0x01c7189e7580a75f07fdc4dbf3fee09a85cd9373"
};

SerializedKeyTestCase TEST_CASES[] = {
    {
        "6bd05118bf92e4236232db724a64abbf709f01b4f37041c88ae3ebf3eeed5596",
        "a7f74edbea542dc33840173d5b31486ba60e7e4e52e9fde4595c347ec55a3b6546601019888199d2f4c64132725c8194594ac63a0f29096701d1c9707febda09",
        "0x54f46318d8f83c28b719ccf01ab4628e1e8f65fa"
    },
    {
        "adff71bae77a106b790ce91e123f15ae003b8fa6295676d9948cb0c8a5b2828a",
        "7928abc8c77f618b7e9d1ee415bb3219976be7643757552b1aad7c790d9af262166cc0315c8e19cb28711e1a7349b4181f30554ce1c7de9adaad82bfe552af9c",
        "0x256c6f6e7ccb5a6c2d7328a8ab9b79333beba1e1"
    },
    {
        "4b337635ae3e3a206b975e09e3049c5e72655f4eed2821d3ac7ff1043f3f89c5",
        "2c062a116a3f93b75902270d14c407b0d2aede7dc8ce2e205f78703f80dc7def0abfd18fdf616d54f4ecb9b8dfb18666dcff2f0792d096f434b16ffc161ee019",
        "0x841d6a3c7854d3250433b4fddd6ce8b4093e7172"
    },
    {
        "dfdecaa72b39c7ac70303a8e7ecea6300e83c58b195ecd52d5c2d21e3460e343",
        "3256d4728477fed4bf2b61d0e97bd79afb15f3863edd2cff04b39e44fdd07e17e5674ea1192e832d666ae951ae5901a77a32af421c37731da6e1b418d3fb74ae",
        "0x23d0033fb9717563cb6d8b1e257a9d6027b48953"
    },
    {
        "6f7c00b940ef1fba31dafb1fcf4c2f0c5bf5e71948e8ea92094e0701cf425ab8",
        "ad9cee319424a5b0af8f6beeeee58e9775257f583c3bb3d88c320bdc920d46eb538399021e8004d68e9b27c4bfa1b5c1f95e394e7939b86886c84765fb6acf83",
        "0x2f87c0482c8d8490b3afb68cfe48bea81510a195"
    },
    {
        "772655d6fa88f78362fc7489ea4c643d5edcbd052af13e2c37c9de2cf20fbe77",
        "3118f1d006b6dfbf71ca4e6d2951cc7fd74909edd148df9d29625782e0fbf37c47c522ad053b0862c7a78b9058fe968d84fd012c2037ba7938b1cef1ca05ef71",
        "0x6ef90e7dfec25f5b74a2c40eb05fce94cefe52a1"
    },
    {
        "a104292f5fb5e49af675ea90748ec6e114c9b4035f77782f4503a6e3059b6dd0",
        "98f86eb3c1768ec59d67cbe0c06335fbb82e4a783d860e5a30a628c671f10797390142b5e4d6a8c2730160edf633003a827b597da2d2d7a61e034575c995e5b2",
        "0x390c61b3bee479ba9db509fb2277c16458553299"
    },
    {
        "3bddb9373d912591548fedabba36209bf9f7800f0290ac71eeb5f83866f1d691",
        "d6ea90f9e28c6228da2bcef04816189244a5593fde9665d56059d90bd131502c7d7f8f54c05ba74996a16d4aac263826644b2987648306cdb51919bda4718b3c",
        "0x60898387644b70c3d74999b30766947c09b2a9c6"
    },
    {
        "3a21f6f305f67625706c18f872bfaf58f655e8b3b570332d6af38af4535d0081",
        "8be23baec7804621fd3a2b58b155198f5e79a9f2eeaa3817e944be60c4e1bce8119391bb3b5afa9d674c03b18b90d5acfe8305d65a33181cd15aca438e121ce5",
        "0x7a965b0a45a786ce9249e92ac1746053aa84a690"
    },
    {
        "71dc93df5433faa33f97e65d2cc03ef7c0e04ce59576f0b77192d47f5fdeab3e",
        "419aa97f8da6b71ee75acc8b21dc9a851ec120334f4b7fd62b6d833c58afa49f487bd6424b6629844666af06d0a597a90b0fb6365dbb1e7d96fb9da9637c8ff5",
        "0x8405b8c046b94deb5ca5adf81660164b853bbda4"
    },
    {
        "ccd6653148ff43a8a96ac417a6392777c3d6a9fecd78fa02438692e948dcc9cd",
        "dcc583096c0127a5193e9979fb3e0a5a928c960b8056b64c626ccaf3ae960cd5cfa1599b137d9457e2f21e04bc479a74f99fa66a46aa9dbec5804f31da0b5da0",
        "0x7b098532d8bffb846357e209094d122da98c5ad1"
    },
    {
        "f32293168da86e1ac2a334e3bf5dd24b0e30a0a7fecf5459db8fa63c870469f8",
        "7e18b8fb656297dec98762a20a34e585cf43b4f367ef5c2eb919357a5213052baf61e0ea2b433f5d3b84021a62fc01457229bc6f92c719976bd7870a5dd9908b",
        "0xb01f82aabacaa0ed45d041c227b55f0e2f78e897"
    },
    {
        "6bc155d11a983461fcc6d3d7f33ea2e504203b95b2f70c133ca81caf22e74b01",
        "5abab429b25536984a89977d5632ec66287f748f3d14b74871423124bb43ba330e62b1d7bedf814f37d7ecf2311651c3727e298715ce3ff49179fe326dd3d707",
        "0x3f135cb1f425aed875b29e94153c0ccf9cec9f3f"
    },
    {
        "9e8dfca420f547598d0dcef7ee05d8923381e33634577cb06767e57215a66a75",
        "5d05b6b7617be5680f9bb4e368a370fa84396c608941a654892bf682eec50ca9809488701c1ad189a056fabe48e719188bfcab31e5fb4550bc38ce60766af425",
        "0x1d90a440b8faff514cfb8b0143df13ccd5b8a7e1"
    },
    {
        "40a0efc4a15e884874c919546b8031393733b9f19578264ded581a7ee1b2d91f",
        "54bacad7b4205095b64ad84450a2c8add9e439a64cfcf107f387fc47b494986d3bde54b751eee7ead26411ff5b90c4f10b2a8f51e11994ad7a0b058b6965eb4b",
        "0x01c7189e7580a75f07fdc4dbf3fee09a85cd9373"
    }
};

INSTANTIATE_TEST_CASE_P(
        Ethereum,
        SerializedKeyTestP,
        ::testing::Combine(
                ::testing::Values(
                        BlockchainType{BLOCKCHAIN_ETHEREUM, ETHEREUM_CHAIN_ID_MAINNET},
                        BlockchainType{BLOCKCHAIN_ETHEREUM, ETHEREUM_CHAIN_ID_RINKEBY}
                ),
                ::testing::Values(ACCOUNT_TYPE_DEFAULT),
                ::testing::ValuesIn(TEST_CASES)));

INSTANTIATE_TEST_CASE_P(
        Ethereum,
        CheckAddressTestP,
        ::testing::Combine(
                ::testing::Values(
                        BlockchainType{BLOCKCHAIN_ETHEREUM, ETHEREUM_CHAIN_ID_MAINNET},
                        BlockchainType{BLOCKCHAIN_ETHEREUM, ETHEREUM_CHAIN_ID_RINKEBY}
                ),
                ::testing::ValuesIn(TEST_CASES_ADDRESS)));

GTEST_TEST(EtheremAccountTest, PrivateKeySig)
{
    // Based on cpp-ethereum test case `Crypto.devcrypto.SignAndRecover`
    // Original test case, for the reference:
    //    auto sec = Secret{sha3("sec")};
    //    auto msg = sha3("msg");
    //    auto sig = sign(sec, msg);
    //    auto expectedSig = "b826808a8c41e00b7c5d71f211f005a84a7b97949d5e765831e1da4e34c9b8295d2a622eee50f25af78241c1cb7cfff11bcf2a13fe65dee1e3b86fd79a4e3ed000";
    //    BOOST_CHECK_EQUAL(sig.hex(), expectedSig);

    // Ethereum-sha3 (keccak_256 as we call it) of "sec":
    const char PRIVATE_KEY[] = "cc798d20ea341f838981c3df7f58f1bf453e6dcc2894c0d17d7da1b422a623c7";
    const char MESSAGE[] = "msg";

    AccountPtr account;
    HANDLE_ERROR(make_account(
            ETHEREUM_MAIN_NET,
            ACCOUNT_TYPE_DEFAULT,
            PRIVATE_KEY,
            reset_sp(account)));

    BinaryDataPtr message;
    HANDLE_ERROR(make_binary_data_from_bytes(
            reinterpret_cast<const unsigned char*>(MESSAGE),
            array_size(MESSAGE) - 1, reset_sp(message))); // -1 to ditch null-terminator

    const BinaryDataPtr expected_hash = keccak_256(*message);

    const BinaryDataPtr signature = account->get_private_key()->sign(*message);
    ASSERT_NE(nullptr, signature);

    BinaryDataPtr expected_signature;
    HANDLE_ERROR(make_binary_data_from_hex("b826808a8c41e00b7c5d71f211f005a84a7b97949d5e765831e1da4e34c9b8295d2a622eee50f25af78241c1cb7cfff11bcf2a13fe65dee1e3b86fd79a4e3ed000", reset_sp(expected_signature)));
    EXPECT_EQ(*expected_signature, *signature);
}

GTEST_TEST(EthereumBlockchainTest, validate_address)
{
    // There is no difference between main net and test net in terms of addresses,
    // so using only main net to check ETH validate_address() implementation.
    const BlockchainType ETH_MAINNET{BLOCKCHAIN_ETHEREUM, ETHEREUM_CHAIN_ID_MAINNET};
    EXPECT_ERROR(validate_address(ETH_MAINNET, "0x"));
    EXPECT_ERROR(validate_address(ETH_MAINNET, "0xa"));
    EXPECT_ERROR(validate_address(ETH_MAINNET, "0xZ"));
    EXPECT_ERROR(validate_address(ETH_MAINNET, "0xAA"));

    // Too big to be a valid address
    EXPECT_ERROR(validate_address(ETH_MAINNET, "b826808a8c41e00b7c5d71f211f005a84a7b97949d5e765831e1da4e34c9b8295d2a622eee50f25af78241c1cb7cfff11bcf2a13fe65dee1e3b86fd79a4e3ed000"));
    EXPECT_ERROR(validate_address(ETH_MAINNET, "0xb826808a8c41e00b7c5d71f211f005a84a7b97949d5e765831e1da4e34c9b8295d2a622eee50f25af78241c1cb7cfff11bcf2a13fe65dee1e3b86fd79a4e3ed000"));
}


struct PersonalSignTestCase
{
    const char* private_key;
    const char* message;
    const char* expected_signature;
};

class PersonalSignTestP : public ::testing::TestWithParam<PersonalSignTestCase>
{};

TEST_P(PersonalSignTestP, sign_with_key)
{
    const PersonalSignTestCase& param = GetParam();
    CharPtr signature;
    HANDLE_ERROR(ethereum_personal_sign(
            param.private_key,
            param.message, reset_sp(signature)));

    ASSERT_STREQ(param.expected_signature, signature.get());
}

// personal_sign test cases based on test cases from Metamask:
// https://github.com/MetaMask/eth-sig-util/blob/a8659ff2da34ab31372c812c6d1569b072650269/test/index.js#L252
static const PersonalSignTestCase META_MASK_PERSONAL_SIGN_TEST_CASES[] = {
    {
        "6969696969696969696969696969696969696969696969696969696969696969",
        // "hello world"
        "68656c6c6f20776f726c64",
        "ce909e8ea6851bc36c007a0072d0524b07a3ff8d4e623aca4c71ca8e57250c4d0a3fc38fa8fbaaa81ead4b9f6bd03356b6f8bf18bccad167d78891636e1d69561b"
    },
    {
        "6969696969696969696969696969696969696969696969696969696969696969",
      // some random binary message from parity's test
        "0cc175b9c0f1b6a831c399e26977266192eb5ffee6ae2fec3ad71c777531578f",
        "9ff8350cc7354b80740a3580d0e0fd4f1f02062040bc06b893d70906f8728bb5163837fd376bf77ce03b55e9bd092b32af60e86abce48f7b8d3539988ee5a9be1c",
    },
    {
        "4545454545454545454545454545454545454545454545454545454545454545",
        // random binary message data and pk from parity's test
        "0cc175b9c0f1b6a831c399e26977266192eb5ffee6ae2fec3ad71c777531578f",
        "a2870db1d0c26ef93c7b72d2a0830fa6b841e0593f7186bc6c7cc317af8cf3a42fda03bd589a49949aa05db83300cdb553116274518dbe9d90c65d0213f4af491b"
    }
};

INSTANTIATE_TEST_CASE_P(
        Metamask,
        PersonalSignTestP,
        ::testing::ValuesIn(META_MASK_PERSONAL_SIGN_TEST_CASES)
);

GTEST_TEST(EtheremAccountTest, account_change_private_key)
{
    const char* serialized_private_key = "5a37680b86fabdec299fa02bdfba8c9dfad08d796dc58c1d07527a751905bf71";
    const auto expected_private_key = test_utility::from_hex(serialized_private_key);

    AccountPtr account;
    HANDLE_ERROR(make_account(
            ETHEREUM_TEST_NET,
            ACCOUNT_TYPE_DEFAULT,
            serialized_private_key,
            reset_sp(account)));

    KeyPtr private_key;
    HANDLE_ERROR(account_get_key(account.get(), KEY_TYPE_PRIVATE, reset_sp(private_key)));

    ConstCharPtr private_key_str;
    HANDLE_ERROR(key_to_string(private_key.get(), reset_sp(private_key_str)));
    ASSERT_STREQ(serialized_private_key, private_key_str.get());

    for (int i = -32; i < 32; ++i)
    {
        SCOPED_TRACE(i);

        // change the account private key
        HANDLE_ERROR(account_change_private_key(account.get(), i, 0));
        HANDLE_ERROR(account_get_key(account.get(), KEY_TYPE_PRIVATE, reset_sp(private_key)));
        HANDLE_ERROR(key_to_string(private_key.get(), reset_sp(private_key_str)));
        ASSERT_STRNE(serialized_private_key, private_key_str.get());

        // reset private key back to original state
        const size_t vector_index = static_cast<size_t>(i < 0 ? expected_private_key.size() + i : i);
        HANDLE_ERROR(account_change_private_key(account.get(), i, expected_private_key[vector_index]));
        HANDLE_ERROR(account_get_key(account.get(), KEY_TYPE_PRIVATE, reset_sp(private_key)));
        HANDLE_ERROR(key_to_string(private_key.get(), reset_sp(private_key_str)));
        ASSERT_STREQ(serialized_private_key, private_key_str.get());
    }
}

GTEST_TEST(EtheremAccountInvalidArgsTest, account_change_private_key)
{
    AccountPtr account;
    HANDLE_ERROR(make_account(
            ETHEREUM_TEST_NET,
            ACCOUNT_TYPE_DEFAULT,
            "5a37680b86fabdec299fa02bdfba8c9dfad08d796dc58c1d07527a751905bf71",
            reset_sp(account)));

    EXPECT_ERROR(account_change_private_key(nullptr, 0, 0));
    EXPECT_ERROR(account_change_private_key(account.get(), -33, 0));
    EXPECT_ERROR(account_change_private_key(account.get(), 32, 0));
    EXPECT_ERROR(account_change_private_key(account.get(), -100, 0));
    EXPECT_ERROR(account_change_private_key(account.get(), 100, 0));
}

} // namespace
