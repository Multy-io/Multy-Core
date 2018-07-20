/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_CORE_SRC_JSON_HELPERS_H
#define MULTY_CORE_SRC_JSON_HELPERS_H

#include <string>
#include <initializer_list>
#include <utility>

#include "json/json.h"

namespace multy_core
{
namespace internal
{

std::string to_string(const Json::Value& value);

Json::Value make_json_object(std::initializer_list<std::pair<const char*, Json::Value>> object_values);
Json::Value make_json_array(std::initializer_list<Json::Value> array_values);

// Syntax sugar to reduce number of braces in make_json_array() :
template <typename... Args>
Json::Value make_json_array(Args... args)
{
    return make_json_array({args...});
}

} // namespace internal
} // namespace multy_core

#endif // MULTY_CORE_SRC_JSON_HELPERS_H
