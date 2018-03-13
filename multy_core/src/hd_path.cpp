/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/src/hd_path.h"

#include <sstream>

namespace multy_core
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

void append_child(uint32_t child_chain_code, HDPath* parent_path)
{
    parent_path->push_back(child_chain_code);
}

HDPath make_child_path(HDPath parent_path, uint32_t child_chain_code)
{
    HDPath result(std::move(parent_path));
    append_child(child_chain_code, &result);
    return result;
}

} // namespace internal
} // namespace multy_core
