/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license
 *
 * See LICENSE for details
 */

#include "multy_core/src/codec.h"
#include "multy_core/src/exception.h"

#include "multy_test/value_printers.h"
#include "multy_test/utility.h"

#include "gtest/gtest.h"

#include <memory>

namespace
{
using namespace test_utility;
using namespace multy_core::internal;

const char* LICENSE_TEXT = R"LICENSE(Note: the modules in the third-party or tools directories have their own licenses, but except where noted in an individual source file, the rest of the code is covered by the following license:
Copyright 2018 Idealnaya rabota LLC (Republic of Belarus)
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute (with limitations described below), sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
You shall not distribute in binary form any substantial portion of the Software if it has any modifications (in source code or assets) compared to original distribution from Idealnaya rabota LLC (Republic of Belarus) obtained from the official repository at https://github.com/Multy-io/ or https://github.com/Appscrunch.
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.)LICENSE";
} // namespace

struct CodecTestCase
{
    const char* string;
    const char* encoded;
};

void PrintTo(const CodecTestCase& test_case, std::ostream* ostr)
{
    *ostr << "CodecTestCase{\n\t\"" << test_case.string << "\","
        "\n\t\"" << test_case.encoded << "\"\n}";
}

class CodecTestP : public ::testing::TestWithParam<::testing::tuple<CodecType, CodecTestCase>>
{};

TEST_P(CodecTestP, encoding)
{
    const CodecType codec_type = ::testing::get<0>(GetParam());
    const CodecTestCase& test_case = ::testing::get<1>(GetParam());

    ASSERT_EQ(test_case.encoded, encode(as_binary_data(test_case.string), codec_type));
}

TEST_P(CodecTestP, decoding)
{
    const CodecType codec_type = ::testing::get<0>(GetParam());
    const CodecTestCase& test_case = ::testing::get<1>(GetParam());

    ASSERT_EQ(as_binary_data(test_case.string), *decode(test_case.encoded, codec_type));
}

const CodecTestCase HEX_TEST_CASES[] =
{
    {
        "",
        ""
    },
    {
        "hello",
        "68656c6c6f"
    },
    {
        "MULTY",
        "4d554c5459"
    },
    {
        LICENSE_TEXT,
        "4e6f74653a20746865206d6f64756c657320696e"
        "207468652074686972642d7061727479206f7220"
        "746f6f6c73206469726563746f72696573206861"
        "7665207468656972206f776e206c6963656e7365"
        "732c206275742065786365707420776865726520"
        "6e6f74656420696e20616e20696e646976696475"
        "616c20736f757263652066696c652c2074686520"
        "72657374206f662074686520636f646520697320"
        "636f76657265642062792074686520666f6c6c6f"
        "77696e67206c6963656e73653a0a436f70797269"
        "676874203230313820496465616c6e6179612072"
        "61626f7461204c4c43202852657075626c696320"
        "6f662042656c61727573290a5065726d69737369"
        "6f6e20697320686572656279206772616e746564"
        "2c2066726565206f66206368617267652c20746f"
        "20616e7920706572736f6e206f627461696e696e"
        "67206120636f7079206f66207468697320736f66"
        "747761726520616e64206173736f636961746564"
        "20646f63756d656e746174696f6e2066696c6573"
        "20287468652022536f66747761726522292c2074"
        "6f206465616c20696e2074686520536f66747761"
        "726520776974686f757420726573747269637469"
        "6f6e2c20696e636c7564696e6720776974686f75"
        "74206c696d69746174696f6e2074686520726967"
        "68747320746f207573652c20636f70792c206d6f"
        "646966792c206d657267652c207075626c697368"
        "2c2064697374726962757465202877697468206c"
        "696d69746174696f6e7320646573637269626564"
        "2062656c6f77292c207375626c6963656e73652c"
        "20616e642f6f722073656c6c20636f7069657320"
        "6f662074686520536f6674776172652c20616e64"
        "20746f207065726d697420706572736f6e732074"
        "6f2077686f6d2074686520536f66747761726520"
        "6973206675726e697368656420746f20646f2073"
        "6f2c207375626a65637420746f2074686520666f"
        "6c6c6f77696e6720636f6e646974696f6e733a0a"
        "596f75207368616c6c206e6f7420646973747269"
        "6275746520696e2062696e61727920666f726d20"
        "616e79207375627374616e7469616c20706f7274"
        "696f6e206f662074686520536f66747761726520"
        "69662069742068617320616e79206d6f64696669"
        "636174696f6e732028696e20736f757263652063"
        "6f6465206f72206173736574732920636f6d7061"
        "72656420746f206f726967696e616c2064697374"
        "7269627574696f6e2066726f6d20496465616c6e"
        "617961207261626f7461204c4c43202852657075"
        "626c6963206f662042656c6172757329206f6274"
        "61696e65642066726f6d20746865206f66666963"
        "69616c207265706f7369746f7279206174206874"
        "7470733a2f2f6769746875622e636f6d2f4d756c"
        "74792d696f2f206f722068747470733a2f2f6769"
        "746875622e636f6d2f417070736372756e63682e"
        "0a5468652061626f766520636f70797269676874"
        "206e6f7469636520616e64207468697320706572"
        "6d697373696f6e206e6f74696365207368616c6c"
        "20626520696e636c7564656420696e20616c6c20"
        "636f70696573206f72207375627374616e746961"
        "6c20706f7274696f6e73206f662074686520536f"
        "6674776172652e0a54484520534f465457415245"
        "2049532050524f56494445442022415320495322"
        "2c20574954484f55542057415252414e5459204f"
        "4620414e59204b494e442c204558505245535320"
        "4f5220494d504c4945442c20494e434c5544494e"
        "4720425554204e4f54204c494d4954454420544f"
        "205448452057415252414e54494553204f46204d"
        "45524348414e544142494c4954592c204649544e"
        "45535320464f52204120504152544943554c4152"
        "20505552504f534520414e44204e4f4e494e4652"
        "494e47454d454e542e20494e204e4f204556454e"
        "54205348414c4c2054484520415554484f525320"
        "4f5220434f5059524947485420484f4c44455253"
        "204245204c4941424c4520464f5220414e592043"
        "4c41494d2c2044414d41474553204f52204f5448"
        "4552204c494142494c4954592c20574845544845"
        "5220494e20414e20414354494f4e204f4620434f"
        "4e54524143542c20544f5254204f52204f544845"
        "52574953452c2041524953494e472046524f4d2c"
        "204f5554204f46204f5220494e20434f4e4e4543"
        "54494f4e20574954482054484520534f46545741"
        "5245204f522054484520555345204f52204f5448"
        "4552204445414c494e475320494e205448452053"
        "4f4654574152452e"
    }
};

const CodecTestCase BASE58_TEST_CASES[] =
{
    {
        "",
        ""
    },
    {
        "hello",
        "Cn8eVZg"
    },
    {
        "MULTY",
        "9j3PVxc"
    },
    {
        LICENSE_TEXT,
        "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"
    }
};

const CodecTestCase BASE32_TEST_CASES[] =
{
    {
        "",
        ""
    },
    {
        "hello",
        "nbswy3dp"
    },
    {
        "MULTY",
        "jvkuyvcz"
    },
    // Test cases from RFC, with lovercase letters
    {
        "f",
        "my======"
    },
    {
        "fo", "mzxq===="
    },
    {
        "foo",
        "mzxw6==="
    },
    {
        "foob", "mzxw6yq="
    },
    {
        "fooba", "mzxw6ytb"
    },
    {
        "r", "oi======"
    },
    {
        "foobar", "mzxw6ytboi======"
    },
    // testing huge input
    {
        LICENSE_TEXT,
        "jzxxizj2eb2gqzjanvxwi5lmmvzsa2loeb2gqzjaorugs4tefvygc4tupeqg64raorxw63"
        "dtebsgs4tfmn2g64tjmvzsa2dbozssa5dimvuxeidpo5xca3djmnsw443fomwcaytvoqqg"
        "k6ddmvyhiidxnbsxezjanzxxizleebuw4idbnyqgs3tenf3gszdvmfwca43povzggzjamz"
        "uwyzjmeb2gqzjaojsxg5ban5tca5dimuqgg33emuqgs4zamnxxmzlsmvscaytzeb2gqzja"
        "mzxwy3dpo5uw4zzanruwgzloonstucsdn5yhs4tjm5uhiibsgaytqicjmrswc3domf4wci"
        "dsmfrg65dbebgeyqzafbjgk4dvmjwgsyzan5tcaqtfnrqxe5ltfeffazlsnvuxg43jn5xc"
        "a2ltebugk4tfmj4saz3smfxhizlefqqgm4tfmuqg6zramnugc4thmuwca5dpebqw46jaob"
        "sxe43pnyqg6ytumfuw42lom4qgciddn5yhsidpmyqhi2djomqhg33gor3wc4tfebqw4zba"
        "mfzxg33dnfqxizleebsg6y3vnvsw45dboruw63ramzuwyzlteauhi2dfearfg33gor3wc4"
        "tfeiusyidun4qgizlbnqqgs3raorugkictn5thi53bojssa53jorug65luebzgk43uojuw"
        "g5djn5xcyidjnzrwy5lenfxgoidxnf2gq33voqqgy2lnnf2gc5djn5xca5dimuqhe2lhnb"
        "2hgidun4qhk43ffqqgg33qpewca3lpmruwm6jmebwwk4thmuwca4dvmjwgs43ifqqgi2lt"
        "orzgsytvorssakdxnf2gqidmnfwws5dboruw63ttebsgk43dojuwezleebrgk3dpo4usyi"
        "dtovrgy2ldmvxhgzjmebqw4zbpn5zca43fnrwcay3pobuwk4zan5tca5dimuqfg33gor3w"
        "c4tffqqgc3teeb2g6idqmvzg22luebygk4ttn5xhgidun4qho2dpnuqhi2dfebjw6ztuo5"
        "qxezjanfzsaztvojxgs43imvsca5dpebsg6idtn4wca43vmjvgky3ueb2g6idunbssaztp"
        "nrwg653jnztsay3pnzsgs5djn5xhgoqklfxxkidtnbqwy3banzxxiidenfzxi4tjmj2xiz"
        "janfxcaytjnzqxe6jamzxxe3jamfxhsidtovrhg5dbnz2gsylmebyg64tunfxw4idpmyqh"
        "i2dfebjw6ztuo5qxezjanftca2luebugc4zamfxhsidnn5sgsztjmnqxi2lpnzzsakdjny"
        "qhg33vojrwkiddn5sgkidpoiqgc43tmv2hgkjamnxw24dbojswiidun4qg64tjm5uw4ylm"
        "ebsgs43uojuwe5lunfxw4idgojxw2icjmrswc3domf4wcidsmfrg65dbebgeyqzafbjgk4"
        "dvmjwgsyzan5tcaqtfnrqxe5ltfeqg6ytumfuw4zleebthe33neb2gqzjan5tgm2ldnfqw"
        "yidsmvyg643jorxxe6jamf2ca2duoryhgorpf5tws5diovrc4y3pnuxu25lmor4s22lpf4"
        "qg64ranb2hi4dthixs6z3joruhkyromnxw2l2bobyhgy3sovxgg2bobjkgqzjamfrg65tf"
        "ebrw64dzojuwo2duebxg65djmnssaylomqqhi2djomqhazlsnvuxg43jn5xca3tporuwgz"
        "jaonugc3dmebrgkidjnzrwy5lemvsca2loebqwy3bamnxxa2lfomqg64raon2we43umfxh"
        "i2lbnqqha33soruw63ttebxwmidunbssau3pmz2hoylsmuxauvciiuqfgt2gkrlucusfeb"
        "evgicqkjhvmskeivccaisbkmqesuzcfqqfoskujbhvkvbak5aveusbjzkfsicpiyqectsz"
        "ebfustsefqqekwcqkjcvguzaj5jcasknkbgesrkefqqestsdjrkuiskoi4qeevkuebhe6v"
        "bajreu2skuivccavcpebkeqrjak5aveusbjzkesrktebhumicnivjegscbjzkecqsjjrev"
        "iwjmebdesvcoivjvgicgj5jcaqjakbavevcjinkuyqksebifkusqj5jukicbjzccatspjz"
        "eu4rssjfheorknivhfilrajfhcatspebcvmrkokqqfgscbjrgcavciiuqecvkujbhveuza"
        "j5jcaq2pkbmveskhjbkcascpjrcekustebbekicmjfauetcfebde6uraifhfsicdjraust"
        "jmebcectkbi5cvgicpkiqe6vciivjcatcjifbestcjkrmsyicxjbcviscfkiqestraifhc"
        "aqkdkreu6traj5dcaq2pjzkfeqkdkqwcavcpkjkcat2sebhviscfkjlusu2ffqqecusjkn"
        "eu4rzaizje6tjmebhvkvbaj5dcat2sebeu4icdj5he4rkdkreu6trak5evisbakreekict"
        "j5dfiv2bkjcsat2sebkeqrjakvjukicpkiqe6vciivjcarcfifgestshkmqestrakreeki"
        "ctj5dfiv2bkjcs4==="
    }
};


INSTANTIATE_TEST_CASE_P(
        Hex, CodecTestP,
        ::testing::Combine(
            ::testing::Values(CODEC_HEX),
            ::testing::ValuesIn(HEX_TEST_CASES)
));

INSTANTIATE_TEST_CASE_P(
        Base58, CodecTestP,
        ::testing::Combine(
            ::testing::Values(CODEC_BASE58),
            ::testing::ValuesIn(BASE58_TEST_CASES)
));

INSTANTIATE_TEST_CASE_P(
        Base32, CodecTestP,
        ::testing::Combine(
            ::testing::Values(CODEC_BASE32),
            ::testing::ValuesIn(BASE32_TEST_CASES)
));
