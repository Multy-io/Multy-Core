/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/src/enum_name_map.h"

#include "multy_core/src/error_utility.h"
#include "multy_core/src/exception.h"
#include "multy_core/src/exception_stream.h"

#include <algorithm>
#include <sstream>
#include <cstring>

namespace
{
typedef multy_core::internal::EnumNameMapBase::value_type value_type;
typedef multy_core::internal::EnumNameMapBase::storage_type storage_type;
} // namespace

namespace multy_core
{
namespace internal
{

EnumNameMapBase::EnumNameMapBase(const char* type_name, std::vector<storage_type> values)
    : m_type_name(type_name),
      m_values(std::move(values))
{
    INVARIANT2(type_name != nullptr && type_name[0] != '\0', "Type name is nullptr or empty.");

    // Enum value name is nullptr or empty.
    INVARIANT2(std::all_of(m_values.begin(), m_values.end(),
            [](const storage_type& pair)
    {
        return pair.second && pair.second[0] != '\0';
    }), m_type_name);

    INVARIANT2(!m_values.empty(), m_type_name);
}

EnumNameMapBase::~EnumNameMapBase()
{
}

const char* EnumNameMapBase::get_type_name() const
{
    return m_type_name;
}

const char* EnumNameMapBase::get_name(const value_type value) const
{
    const char* name = find_name(value);
    if (name)
    {
        return name;
    }

    THROW_EXCEPTION2(ERROR_INVALID_ARGUMENT, "Unknown enum value.")
            << " Invalid value for " << m_type_name
            << " : \"" << static_cast<uint64_t>(value) << ".";
}

std::string EnumNameMapBase::get_name_or_dummy(const value_type value) const
{
    const char* name = find_name(value);
    if (name)
    {
        return name;
    }

    std::stringstream sstr;
    sstr << m_type_name << "(" << value << ")";

    return sstr.str();
}

value_type EnumNameMapBase::get_value(const char* name) const
{
    INVARIANT(name != nullptr);

    for (const auto& i : m_values)
    {
        if (strcmp(name, i.second) == 0)
        {
            return i.first;
        }
    }

    THROW_EXCEPTION2(ERROR_INVALID_ARGUMENT, "Unknown enum name.")
            << " Invalid name for " << m_type_name
            << " : \"" << (name ? name : "") << "\".";
}

std::vector<const char*> EnumNameMapBase::get_all_names() const
{
    std::vector<const char*> result;
    result.reserve(m_values.size());

    for (size_t i = 0; i < m_values.size(); ++i)
    {
        result.push_back(m_values[i].second);
    }

    return result;
}

const char* EnumNameMapBase::find_name(const value_type value) const
{
    for (const auto& i : m_values)
    {
        if (value == i.first)
        {
            return i.second;
        }
    }

    return nullptr;
}

} // namespace internal
} // namespace multy_core
