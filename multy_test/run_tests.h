/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_TEST_RUN_TESTS_H
#define MULTY_TEST_RUN_TESTS_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef BUILDING_MULTY_TESTS
#    define BUILDING_MULTY_TESTS 0
#endif

#if defined(_WIN32)
#    if (BUILDING_MULTY_TESTS)
#        define MULTY_TESTS_API __declspec(dllexport)
#    else
#        define MULTY_TESTS_API
#    endif
#elif defined(__GNUC__) && (BUILDING_MULTY_TESTS)
#    define MULTY_TESTS_API __attribute__ ((visibility ("default")))
#else
#    define MULTY_TESTS_API
#endif

MULTY_TESTS_API int run_tests(int argc, char **argv);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // MULTY_TEST_RUN_TESTS_H
