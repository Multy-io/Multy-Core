/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_CORE_SRC_ENUM_NAME_MAP_H
#define MULTY_CORE_SRC_ENUM_NAME_MAP_H

#include <initializer_list>
#include <string>
#include <vector>

namespace multy_core
{
namespace internal
{

class EnumNameMapBase
{
public:
    typedef size_t value_type;
    typedef std::pair<value_type, const char*> storage_type;

    EnumNameMapBase(const char* type_name, std::vector<storage_type> values);
    ~EnumNameMapBase();

    const char* get_type_name() const;

    const char* get_name(value_type value) const;
    std::string get_name_or_dummy(const value_type value) const;

    value_type get_value(const char* name) const;
    value_type get_value(const std::string& name) const;

private:
    const char* find_name(value_type value) const;

private:
    const char* m_type_name;
    const std::vector<storage_type> m_values;
};

template <typename EnumType>
class EnumNameMap
{
public:
    struct EnumPair
    {
        EnumType value;
        const char* name;
    };

    EnumNameMap(const char* type_name, std::initializer_list<EnumPair> values)
        : m_impl(type_name, std::move(to_storage(values)))
    {
    }

    static std::vector<EnumNameMapBase::storage_type> to_storage(std::initializer_list<EnumPair> values)
    {
        std::vector<EnumNameMapBase::storage_type> result;
        result.reserve(values.size());
        for (const auto& v : values)
        {
            result.emplace_back(v.value, v.name);
        }

        return result;
    }

    const char* get_type_name() const
    {
        return m_impl.get_type_name();
    }

    const char* get_name(const EnumType value) const
    {
        return m_impl.get_name(value);
    }

    std::string get_name_or_dummy(const EnumType value) const
    {
        return m_impl.get_name_or_dummy(value);
    }

    EnumType get_value(const char* name) const
    {
        return static_cast<EnumType>(m_impl.get_value(name));
    }

    EnumType get_value(const std::string& name) const
    {
        return get_value(name.c_str());
    }

private:
    EnumNameMapBase m_impl;
};

} // namespace internal
} // namespace multy_core

#endif // MULTY_CORE_SRC_ENUM_NAME_MAP_H
