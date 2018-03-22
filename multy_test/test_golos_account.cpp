/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_test/serialized_keys_test_base.h"
#include "multy_core/src/golos/golos_account.h"

#include "multy_core/golos.h"

#include "gtest/gtest.h"

namespace
{
using namespace multy_core::internal;
const BlockchainType GOLOS_MAINNET{BLOCKCHAIN_GOLOS, GOLOS_NET_TYPE_MAINNET};

const SerializedKeyTestCase GOLOS_KEYS[] =
{
    {
        "5JVFFWRLwz6JoP9kguuRFfytToGU6cLgBVTL9t6NB3D3BQLbUBS",
        "GLS58g5rWYS3XFTuGDSxLVwiBiPLoAyCZgn6aB9Ueh8Hj5qwQA3r6",
        ""
    },
    {
        "5KTUAWTc4xCGWRoMmpYBGqRYSm5oXt7u3d8i8XiBXAgo8rDrhnD",
        "GLS5RBP7ZQPkstMRKfEzSCh4naLRwdHssAjiTi2NarT8kbQQChikB",
        ""
    },
    {
        "5JHZUbXzg54nc2LXauHkFDuqJwRNp5ip6JzH2Yk7dThFdYJ8vF4",
        "GLS6GuZcZoZbVjLUgoKdYNKuRUG3wUnfp6pSJErgrG7UMr4ts3fua",
        ""
    },
    {
        "5KCUxfvXwSY5cTT1D9q6akocfQRe4TfTeNHFyC2ht3gua8kK6dG",
        "GLS6tLGuw8q7pg9UoSzTA4nxgtTBrNvghmDYpLPKbV4BKYNbcgsS4",
        ""
    },
    {
        "5KUbqZaSbRRKfnrQLD1QSGwJ147j3t9KrXum5ru22pbV4896DpG",
        "GLS719NwHP3raPpbjn5hPVBCoTUK9XEXc1j5KfcQvXCEEoPEL6xr2",
        ""
    },
    {
        "5JYYBf1woHkvdv8kL7K7i36kVVQTyvNUUD3R5Nuemto2jTZZVu1",
        "GLS7UpGbmzcHhDLaJM7SwHqXSM6ZtTejFz879q6NiviHs8W17D7Rk",
        ""
    },
    {
        "5Ji5kxCztZnrQURdvtXNuZ9fEHfxDHBvYnuqYUHMgX6PBDHkSUN",
        "GLS7di4wQ3MrpgNzXtvaURHsZ8g48h7CJxr3HKHpepugZz88TPVFK",
        ""
    },
    {
        "5K9xmjs8DPUMGMJhrR7V4TuWRtGRjDCLDXD5pSYJ25BfMotcGjn",
        "GLS7kw5KEMgZ9arJjftsspca7Uyu1JH2ULwQNzdM67pRsF3MFFZqS",
        ""
    },
    {
        "5Kh6Upg9HBKQmPpyLjVKbY4JP48NFCbJdk6pE1Z1Wun6S45PEdf",
        "GLS7pXXH8cY56Zu6tutFHepaSa71oMwQQvcYXrobyxMy5u24tTRcv",
        ""
    },
    {
        "5HrWvB9PMC8wwtyzSJS1B15Eye8WsQpdeJGVdGKMV6nu4kARyzS",
        "GLS7umQTC3gqZwSNXCLi8mEY68KHsHRKpfrEDoYX92swm6pqkAYZY",
        ""
    },
    {
        "5KUip2RVuTm1MKdzFwFfkoxe11DJCMr4w4BeAmHiz67HJgrEGbK",
        "GLS7vFsynJfTVs1fVBKWgWmduKnge5uqWcqhynvEHKQ5E2EiWrPhM",
        ""
    },
    {
        "5JWZ166stP4nyKD1fVSjbBc9cNkfZYWv3dbabfHwjeM7mu9fznT",
        "GLS82jz6r5ExfmtxsfWeqbuFYBufkN1qnGY9hc8FQoA3o3U8WGAiA",
        ""
    },
    {
        "5Jxwckq5KEUpL1jamArjg9LcJGz1iStWM5dB49fo4sYzYMuBgrk",
        "GLS8Ari5qhGeXcFiBKHDYHftkLh2eUEQmtgS6ZBZL6aXC1RyGLnFj",
        ""
    },
    {
        "5Hrs3q4q98GXMWAMpRNLbKjNGvGQdXYFSMSMy5Z1rCpG9qTx3Bo",
        "GLS8GQoJDABEDzu1vasHErViLwLL5WPyaSmwpYiTwaT4no6S4EhN6",
        ""
    },
    {
        "5KN93hDdQSsN5Eu4yzXvmPZroM7fJEg9WShxRd7wufKskpgqeg4",
        "GLS8PMBjjSoRgVDXDHRBBV3H2ew661wXp17eyqJr64NQFzvQrvv7t",
        ""
    },
    {
        "5JmysxAzRwdZQxV6MtXehhgbVcHdG6SzTX4Yibt42nCgnHhani8",
        "GLS8YhCkkEZ1MoJDAKQNAFTKdjU66qF8jBTDoqjLyfCC5TvzPxxLK",
        ""
    },
    {
        "5J8JZhGaUifjwHPtyStQEDHDrgkp5nzJNgiWSyJrMVe58AreL28",
        "GLS8j8GKNmhr2ZiQuD5Axn3HFjGceiwDpKF84BNnFmzJJexXXAoud",
        ""
    }
};

INSTANTIATE_TEST_CASE_P(
        Golos,
        SerializedKeyTestP,
        ::testing::Combine(
                ::testing::Values(GOLOS_MAINNET),
                ::testing::ValuesIn(GOLOS_KEYS)));

} // namespace
