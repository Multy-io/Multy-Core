/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_CORE_SRC_HD_PATH_H
#define MULTY_CORE_SRC_HD_PATH_H

#include <cstdint>
#include <string>
#include <vector>
#include <initializer_list>

namespace multy_core
{
namespace internal
{
// Hierarchical Deterministic path (bip32, bip44).
// TODO: create a class for HDPath with proper methods.
typedef std::vector<uint32_t> HDPath;

const uint32_t HARDENED_INDEX_BASE = 0x80000000;

void append_child(uint32_t child_chain_code, HDPath* parent_path);

HDPath make_child_path(HDPath parent_path, uint32_t child_chain_code);
std::string to_string(const HDPath& path);

inline HDPath make_child_path(HDPath parent_path, const std::initializer_list<uint32_t>& children_chain_codes)
{
    HDPath result(std::move(parent_path));
    for (const auto& child_chain_code : children_chain_codes)
    {
        append_child(child_chain_code, &result);
    }
    return result;
}

} // namespace multy_core

} // namespace internal

#endif // HD_PATH_H
