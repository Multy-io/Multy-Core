/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/src/json_helpers.h"

#include "json/json.h"

#include <sstream>

namespace multy_core
{
namespace internal
{

std::string to_string(const Json::Value& value)
{
    std::stringstream sstr;
    Json::StreamWriterBuilder().newStreamWriter()->write(value, &sstr);

    return sstr.str();
}

typedef std::initializer_list<std::pair<const char*, Json::Value>> ObjectValues;
Json::Value make_json_object(ObjectValues object_values)
{
    Json::Value result(Json::objectValue);
    for (const auto& i : object_values)
    {
        result[i.first] = std::move(i.second);
    }

    return result;
}

Json::Value make_json_array(std::initializer_list<Json::Value> array_values)
{
    Json::Value result(Json::arrayValue);

    for (const auto& i : array_values)
    {
        result.append(std::move(i));
    }

    return result;
}

} // namespace internal
} // namespace multy_core
