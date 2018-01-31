/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license
 *
 * See LICENSE for details
 */

#include "multy_core/sha3.h"

#include "multy_core/src/u_ptr.h"
#include "multy_core/src/utility.h"

#include "multy_test/utility.h"
#include "multy_test/value_printers.h"

#include "gtest/gtest.h"

#include <array>
#include <iostream>

#include <string.h>

namespace
{
using namespace multy_core::internal;
using namespace test_utility;

const size_t SHA3_SIZES[] = {
    224,
    256,
    384,
    512
};

enum InputDataFormat
{
    FORMAT_HEX,
    FORMAT_STR,
};

struct Sha3TestCase
{
    InputDataFormat input_format;
    const char* input;
    size_t input_len;
    std::array<const char*, 4> expected_hash;
};

void PrintTo(const Sha3TestCase& test_case, std::ostream* out)
{
    *out << "Sha3TestCase {\""
         << (test_case.input ? test_case.input : "<nullptr>")
         << "\", "
         << test_case.input_len
         << "}";
}

// Based on NIST sample vectors for SHA3:
// https://csrc.nist.gov/projects/cryptographic-standards-and-guidelines/example-values#aHashing
const Sha3TestCase NIST_CASES[] = {
    {
        FORMAT_HEX,
        "",
        0,
        {
            "6B4E03423667DBB73B6E15454F0EB1ABD4597F9A1B078E3F5B5A6BC7",
            "A7FFC6F8BF1ED76651C14756A061D662F580FF4DE43B49FA82D80A4B80F8434A",
            "0C63A75B845E4F7D01107D852E4C2485C51A50AAAA94FC61995E71BBEE983A2AC3713831264ADB47FB6BD1E058D5F004",
            "A69F73CCA23A9AC5C8B567DC185A756E97C982164FE25859E0D1DCC1475C80A615B2123AF1F5F94C11E3E9402C3AC558F500199D95B6D3E301758586281DCD26"
        }
    },
//    { // TODO: is seems like either we've messed up the test case,
//      // the best guess for now is the input data being invalid in this one.
//        FORMAT_HEX,
//        "c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5",
//        200,
//        {
//            "9376816ABA503F72F96CE7EB65AC095DEEE3BE4BF9BBC2A1CB7E11E0",
//            "79F38ADEC5C20307A98EF76E8324AFBFD46CFD81B22E3973C65FA1BD9DE31787",
//            "1881DE2CA7E41EF95DC4732B8F5F002B189CC1E42B74168ED1732649CE1DBCDD76197A31FD55EE989F2D7050DD473E8F",
//            "E76DFAD22084A8B1467FCF2FFA58361BEC7628EDF5F3FDC0E4805DC48CAEECA81B7C13C30ADF52A3659584739A2DF46BE589C51CA1A4A8416DF6545A1CE8BA00"
//        }
//    },
};

// https://www.di-mgt.com.au/sha_testvectors.html#testvectors
const Sha3TestCase DIMGT_CASES[] = {
    {
        FORMAT_STR,
        "abc",
        3,
        {
            "e642824c3f8cf24ad09234ee7d3c766fc9a3a5168d0c94ad73b46fdf",
            "3a985da74fe225b2045c172d6bd390bd855f086e3e9d525b46bfe24511431532",
            "ec01498288516fc926459f58e2c6ad8df9b473cb0fc08c2596da7cf0e49be4b298d88cea927ac7f539f1edf228376d25",
            "b751850b1a57168a5693cd924b6b096e08f621827444f70d884f5d0240d2712e10e116e9192af3c91a7ec57647e3934057340b4cf408d5a56592f8274eec53f0"
        }
    },
    {
        FORMAT_STR,
        "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq",
        448 / 8,
        {
            "8a24108b154ada21c9fd5574494479ba5c7e7ab76ef264ead0fcce33",
            "41c0dba2a9d6240849100376a8235e2c82e1b9998a999e21db32dd97496d3376",
            "991c665755eb3a4b6bbdfb75c78a492e8c56a22c5c4d7e429bfdbc32b9d4ad5aa04a1f076e62fea19eef51acd0657c22",
            "04a371e84ecfb5b8b77cb48610fca8182dd457ce6f326a0fd3d7ec2f1e91636dee691fbe0c985302ba1b0d8dc78c086346b533b49c030d99a27daf1139d6e75e"
        }
    },
    {
        FORMAT_STR,
        "abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu",
        896 / 8,
        {
            "543e6868e1666c1a643630df77367ae5a62a85070a51c14cbf665cbc",
            "916f6061fe879741ca6469b43971dfdb28b1a32dc36cb3254e812be27aad1d18",
            "79407d3b5916b59c3e30b09822974791c313fb9ecc849e406f23592d04f625dc8c709b98b43b3852b337216179aa7fc7",
            "afebb2ef542e6579c50cad06d2e578f9f8dd6881d7dc824d26360feebf18a4fa73e3261122948efcfd492e74e82e2189ed0fb440d187f382270cb455f21dd185"
        }
    }
};

class Sha3TestP : public ::testing::TestWithParam<::testing::tuple<Sha3TestCase, size_t>>
{
public:
    void SetUp() override
    {
        const Sha3TestCase& test_case = ::testing::get<0>(GetParam());
        *const_cast<size_t*>(&hash_size) = ::testing::get<1>(GetParam());
        ASSERT_NE(nullptr, test_case.input);

        // A trick to initialize a const object;
        BinaryDataPtr& input_ref = *const_cast<BinaryDataPtr*>(&input);
        if (test_case.input_format == FORMAT_HEX)
        {
            HANDLE_ERROR(make_binary_data_from_hex(
                    test_case.input,
                    reset_sp(input_ref)));
            ASSERT_STRCASEEQ(test_case.input, to_hex(*input).c_str());
        }
        else if (test_case.input_format == FORMAT_STR)
        {
            HANDLE_ERROR(make_binary_data_from_bytes(
                    reinterpret_cast<const unsigned char*>(test_case.input),
                    test_case.input_len, reset_sp(input_ref)));
            ASSERT_EQ(0, memcmp(test_case.input, input->data, test_case.input_len));
        }
        EXPECT_EQ(test_case.input_len, input->len);

        const size_t* p = std::find(
                std::begin(SHA3_SIZES), std::end(SHA3_SIZES), hash_size);
        if (p == std::end(SHA3_SIZES))
        {
            FAIL() << "Unsupported hash length: " << hash_size;
        }
        const int hash_index = p - std::begin(SHA3_SIZES);
        ASSERT_LT(-1, hash_index);
        ASSERT_GT(array_size(SHA3_SIZES), hash_index);
        ASSERT_EQ(hash_size, *p);
        const char* hash_data = test_case.expected_hash[hash_index];

        // A trick to initialize a const object;
        BinaryDataPtr& hash_ref = *const_cast<BinaryDataPtr*>(&expected_hash);
        HANDLE_ERROR(make_binary_data_from_hex(hash_data, reset_sp(hash_ref)));
        ASSERT_EQ(hash_size / 8, expected_hash->len);

        ASSERT_STRCASEEQ(hash_data, to_hex(*expected_hash).c_str());
    }

public:
    // Const to prevent accidental modification in tests.
    const BinaryDataPtr input;
    const BinaryDataPtr expected_hash;
    const size_t hash_size = 0;
};

TEST_P(Sha3TestP, Correctness)
{
    SCOPED_TRACE(hash_size);
    BinaryDataPtr hash;
    HANDLE_ERROR(make_binary_data(hash_size / 8, reset_sp(hash)));

    HANDLE_ERROR(sha3(input.get(), hash.get()));
    EXPECT_EQ(*expected_hash, *hash);
}

INSTANTIATE_TEST_CASE_P(
        NIST, Sha3TestP,
        ::testing::Combine(
                ::testing::ValuesIn(NIST_CASES),
                ::testing::ValuesIn(SHA3_SIZES)
        )
);

INSTANTIATE_TEST_CASE_P(
        DIMGT, Sha3TestP,
        ::testing::Combine(
                ::testing::ValuesIn(DIMGT_CASES),
                ::testing::ValuesIn(SHA3_SIZES)
        )
);

GTEST_TEST(Sha3TestInvalidArgs, sha3)
{
    const BinaryData input{nullptr, 0};
    BinaryData empty_output{nullptr, 0};

    std::array<uint8_t, SHA3_256> hash_buffer;
    BinaryData output{hash_buffer.data(), hash_buffer.size()};

    EXPECT_ERROR(sha3(nullptr, &output));
    EXPECT_ERROR(sha3(&input, nullptr));
    EXPECT_ERROR(sha3(&input, &empty_output));
}

GTEST_TEST(Sha3SmokeTest, sha3)
{
    const BinaryData input{(const unsigned char*)"foo_bar", 7};

    std::array<uint8_t, SHA3_256> hash_buffer;
    BinaryData output{hash_buffer.data(), hash_buffer.size()};

    HANDLE_ERROR(sha3(&input, &output));
}

} // namespace
