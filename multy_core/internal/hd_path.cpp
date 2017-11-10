/* Copyright 2017 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */
 
#include "multy_core/internal/hd_path.h"

#include <sstream>

namespace wallet_core
{
namespace internal
{
std::string to_string(const HDPath& path)
{
    if (path.size() == 0)
    {
        return "";
    }

    std::stringstream stream;
    stream << "m";
    for (const auto& p : path)
    {
        stream << "/" << p;
    }
    return stream.str();
}

HDPath make_child_path(HDPath parent_path, uint32_t child_chain_code)
{
    HDPath result(std::move(parent_path));
    result.push_back(child_chain_code);
    return result;
}

} // namespace internal
} // namespace wallet_core

