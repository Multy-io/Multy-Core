/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/mnemonic.h"

#include "multy_core/common.h"
#include "multy_core/error.h"
#include "multy_core/internal/utility.h"

#include "wally_bip39.h"
#include "wally_core.h"
#include "libwally-core/src/wordlist.h"

#include <string>
#include <memory>
#include <stdlib.h>

namespace
{
using namespace wallet_core::internal;

size_t round_to_supported_entropy_size(size_t entropy_size)
{
    static const size_t supported_entropy_sizes[] = {
            BIP39_ENTROPY_LEN_128, BIP39_ENTROPY_LEN_160, BIP39_ENTROPY_LEN_192,
            BIP39_ENTROPY_LEN_224, BIP39_ENTROPY_LEN_256, BIP39_ENTROPY_LEN_288,
            BIP39_ENTROPY_LEN_320,
    };

    size_t result = BIP39_ENTROPY_LEN_320;
    for (size_t i = 0; i < array_size(supported_entropy_sizes); ++i)
    {
        if (entropy_size >= supported_entropy_sizes[i])
        {
            result = supported_entropy_sizes[i];
        }
        else
        {
            break;
        }
    }
    return result;
}
} // namespace

Error* make_mnemonic(EntropySource entropy_source, const char** mnemonic)
{
    static const size_t max_entropy_size = BIP39_ENTROPY_LEN_320;
    unsigned char entropy[max_entropy_size] = {'\0'};

    ARG_CHECK(entropy_source.fill_entropy);
    ARG_CHECK(mnemonic);

    try
    {
        const size_t entropy_size = round_to_supported_entropy_size(
                entropy_source.fill_entropy(
                        entropy_source.data, max_entropy_size, &entropy[0]));
        if (entropy_size == 0)
        {
            return MAKE_ERROR(
                    ERROR_BAD_ENTROPY,
                    "Unable to get required amount of entropy");
        }

        char* out = nullptr;
        THROW_IF_WALLY_ERROR(
                bip39_mnemonic_from_bytes(nullptr, entropy, entropy_size, &out),
                "Failed to generated mnemonic");
        *mnemonic = out;
    }
    CATCH_EXCEPTION_RETURN_ERROR();

    OUT_CHECK(*mnemonic);

    return nullptr;
}

Error* make_seed(const char* mnemonic, const char* password, BinaryData** seed)
{
    static const size_t max_seed_size = BIP39_SEED_LEN_512;

    ARG_CHECK(mnemonic);
    ARG_CHECK(seed);

    try
    {
        size_t written = 0;
        THROW_IF_WALLY_ERROR(
                bip39_mnemonic_validate(nullptr, mnemonic),
                "Invalid mnemonic value");

        std::unique_ptr<unsigned char[]> data(new unsigned char[max_seed_size]);
        THROW_IF_WALLY_ERROR(
                bip39_mnemonic_to_seed(
                        mnemonic, password, data.get(), max_seed_size,
                        &written),
                "Faield to generate seed from mnemonic");

        BinaryData* out = new BinaryData;
        out->data = data.release();
        out->len = written;
        *seed = out;
    }
    CATCH_EXCEPTION_RETURN_ERROR();

    OUT_CHECK(*seed);

    return nullptr;
}

Error* seed_to_string(const BinaryData* seed, const char** str)
{
    ARG_CHECK(seed);
    ARG_CHECK(seed->data);
    ARG_CHECK(str);

    char* out = nullptr;

    int result = wally_base58_from_bytes(seed->data, seed->len, 0, &out);
    if (result != WALLY_OK)
    {
        return internal_make_error(result, "Failed to convert seed to string", MULTY_CODE_LOCATION);
    }
    *str = out;
    OUT_CHECK(*str);

    return nullptr;
}

Error* mnemonic_get_dictionary(const char** new_dictionary)
{
    ARG_CHECK(new_dictionary);

    try
    {
        const words* dictionary = nullptr;
        THROW_IF_WALLY_ERROR(
                bip39_get_wordlist(nullptr, &dictionary),
                "Failed to obtain wordlist");

        // estimated dictionary size.
        std::string result;
        result.reserve(dictionary->len * 15);

        for (int i = 0; i < dictionary->len; ++i)
        {
            result += dictionary->indices[i];
            result += ' ';
        }

        *new_dictionary = copy_string(result);
    }
    CATCH_EXCEPTION_RETURN_ERROR();

    OUT_CHECK(*new_dictionary);

    return nullptr;
}
