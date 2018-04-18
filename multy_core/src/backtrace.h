/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_CORE_BACKTRACE_H
#define MULTY_CORE_BACKTRACE_H

#include "multy_core/api.h"

#include <cstddef>
#include <string>

MULTY_CORE_API std::string get_backtrace(size_t skip_frames = 0);

#endif // MULTY_CORE_BACKTRACE_H
