/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/src/json_helpers.h"

#include "multy_core/src/exception.h"
#include "multy_core/src/exception_stream.h"

#include "json/json.h"

#include <sstream>
#include <memory>

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

Json::Value parse_json(const std::string& str)
{
    Json::Value result;

    std::unique_ptr<Json::CharReader> reader(Json::CharReaderBuilder().newCharReader());
    std::string errors;

    if (!reader->parse(str.data(), str.data() + str.size(), &result, &errors))
    {
        THROW_EXCEPTION2(ERROR_INVALID_ARGUMENT, "Failed to parse JSON document.")
                << " Errors: " << errors;
    }

    return result;
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
