/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/mnemonic.h"

#include "multy_core/common.h"
#include "multy_core/error.h"
#include "multy_core/internal/utility.h"

#include "multy_test/run_tests.h"

#include <iostream>
#include <memory>
#include <string.h>

namespace
{

size_t feed_silly_entropy(void*, size_t size, void* dest)
{
    /** Poor man's entropy, using uninitialized data from stack, which is:
     * - Fast;
     * - Unsecure;
     * - Somewhat predictable;
     * And hence SHOULD not be used in production, but Ok in POC driver program.
     */
    static const size_t entropy_max_size = 1024;
    unsigned char silly_entropy[entropy_max_size];

    if (size > entropy_max_size)
    {
        return 0;
    }

    memcpy(dest, silly_entropy, size);
    return size;
}

using namespace wallet_core::internal;

} // namespace

int main(int argc, char** argv)
{
#ifdef WITH_TESTS
    return run_tests(argc, argv);
#else
    try
    {
        const EntropySource entropy{nullptr, &feed_silly_entropy};

        ConstCharPtr mnemonic;
        throw_if_error(make_mnemonic(entropy, reset_sp(mnemonic)));
        std::cout << "Generated mnemonic: " << mnemonic.get() << std::endl;

        std::cout << "Enter password: ";
        std::string password;
        std::getline(std::cin, password);

        BinaryDataPtr seed;
        throw_if_error(
                make_seed(mnemonic.get(), password.c_str(), reset_sp(seed)));

        ConstCharPtr seed_string;
        throw_if_error(seed_to_string(seed.get(), reset_sp(seed_string)));
        std::cout << "Seed: " << seed_string.get() << std::endl;
    }
    catch (Error* e)
    {
        std::cerr << "Got error: " << e->message << std::endl;
        free_error(e);
        return -1;
    }
    return 0;
#endif
}
