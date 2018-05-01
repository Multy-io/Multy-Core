/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/common.h"

#include "multy_core/src/u_ptr.h"
#include "multy_core/src/utility.h"

#include "generated/version.h"

#include "wally_core.h"

#include <string.h>
#include <sstream>

#ifndef MULTY_CORE_VERSION_MAJOR
#define MULTY_CORE_VERSION_MAJOR 0
#endif // MULTY_CORE_VERSION_MAJOR

#ifndef MULTY_CORE_VERSION_MINOR
#define MULTY_CORE_VERSION_MINOR 0
#endif // MULTY_CORE_VERSION_MINOR

#ifndef MULTY_CORE_VERSION_BUILD
#define MULTY_CORE_VERSION_BUILD 0
#endif // MULTY_CORE_VERSION_BUILD

#ifndef MULTY_CORE_VERSION_NOTE
#define MULTY_CORE_VERSION_NOTE ""
#endif // MULTY_CORE_VERSION_NOTE

#ifndef MULTY_CORE_VERSION_COMMIT
#define MULTY_CORE_VERSION_COMMIT ""
#endif // MULTY_CORE_VERSION_COMMIT

namespace
{
using namespace multy_core::internal;

const Version CURRENT_VERSION = {
    MULTY_CORE_VERSION_MAJOR,
    MULTY_CORE_VERSION_MINOR,
    MULTY_CORE_VERSION_BUILD,
    MULTY_CORE_VERSION_NOTE,
    MULTY_CORE_VERSION_COMMIT
};

std::string format_version(const Version& version)
{
    std::stringstream sstr;
    sstr << version.major << "." << version.minor << "." << version.build;
    if (version.note && strlen(version.note))
    {
        sstr << "-" << version.note;
    }
    if (version.commit && strlen(version.commit))
    {
        sstr << "(" << version.commit << ")";
    }
    return sstr.str();
}

} // namespace

Error* get_version(Version* version)
{
    ARG_CHECK(version);
    *version = CURRENT_VERSION;
    return nullptr;
}

Error* make_version_string(const char** out_version_string)
{
    ARG_CHECK(out_version_string);
    try
    {
        *out_version_string = copy_string(format_version(CURRENT_VERSION));
    }
    CATCH_EXCEPTION_RETURN_ERROR(ERROR_SCOPE_GENERIC);

    OUT_CHECK(*out_version_string);
    return nullptr;
}

void free_string(const char* str)
{
    if (!str)
    {
        return;
    }
    wally_free_string(const_cast<char*>(str));
}
