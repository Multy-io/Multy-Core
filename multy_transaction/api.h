/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_TRANSACTION_API_H_INCLUDED
#define MULTY_TRANSACTION_API_H_INCLUDED

#ifndef BUILDING_MULTY_TRANSACTION
#    define BUILDING_MULTY_TRANSACTION 0
#endif

#if defined(_WIN32)
#    if (BUILDING_MULTY_CORE)
#        define MULTY_TRANSACTION_API __declspec(dllexport)
#    else
#        define MULTY_TRANSACTION_API
#    endif
#elif defined(__GNUC__) && (BUILDING_MULTY_TRANSACTION)
#    define MULTY_TRANSACTION_API __attribute__ ((visibility ("default")))
#else
#    define MULTY_TRANSACTION_API
#endif

#endif // MULTY_TRANSACTION_API_H_INCLUDED
