/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_CORE_API_H_INCLUDED
#define MULTY_CORE_API_H_INCLUDED

#ifndef BUILDING_MULTY_CORE
#    define BUILDING_MULTY_CORE 0
#endif

#if defined(_WIN32)
#    if (BUILDING_MULTY_CORE)
#        define MULTY_CORE_API __declspec(dllexport)
#    else
#        define MULTY_CORE_API
#    endif
#elif defined(__GNUC__) && (BUILDING_MULTY_CORE)
#    define MULTY_CORE_API __attribute__ ((visibility ("default")))
#else
#    define MULTY_CORE_API
#endif

#endif // MULTY_CORE_API_H_INCLUDED
