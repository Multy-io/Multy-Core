/* Copyright 2017 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_transaction/internal/properties.h"

#include "multy_transaction/internal/amount.h"

#include "multy_core/internal/key.h"
#include "multy_core/common.h"
#include "multy_core/internal/utility.h"

#include <exception>
#include <string>
#include <vector>

namespace
{
using namespace wallet_core::internal;

Property::ValueType deduce_value_type(const int32_t&)
{
    return Property::VALUE_TYPE_INT32;
}

Property::ValueType deduce_value_type(const std::string&)
{
    return Property::VALUE_TYPE_STRING;
}

Property::ValueType deduce_value_type(const Amount&)
{
    return Property::VALUE_TYPE_AMOUNT;
}

Property::ValueType deduce_value_type(const BinaryData&)
{
    return Property::VALUE_TYPE_BINARY_DATA;
}

Property::ValueType deduce_value_type(const PublicKey&)
{
    return Property::VALUE_TYPE_PUBLIC_KEY;
}

template <typename T, typename D>
Property::ValueType deduce_value_type(const std::unique_ptr<T, D>& value)
{
    return deduce_value_type(*value);
}

std::string get_value_type_string(Property::ValueType type)
{
    switch (type)
    {
        case Property::VALUE_TYPE_INT32:
            return "int32";
        case Property::VALUE_TYPE_STRING:
            return "string";
        case Property::VALUE_TYPE_AMOUNT:
            return "Amount";
        case Property::VALUE_TYPE_BINARY_DATA:
            return "BinaryData";
        case Property::VALUE_TYPE_PUBLIC_KEY:
            return "PublicKey";
    }
    return "UNKNOWN TYPE";
}

template <typename T>
void copy_value(const T& from, T* to)
{
    *to = from;
}

void copy_value(const BinaryData& from, BinaryDataPtr* to)
{
    *to = std::move(make_clone(from));
}

void copy_value(const PublicKey& from, PublicKeyPtr* to)
{
    *to = std::move(from.clone());
}

template <typename T>
struct PropertyTraits
{
    typedef T ArgumentType;
};

template <>
struct PropertyTraits<PublicKeyPtr>
{
    typedef PublicKey ArgumentType ;
};

template <>
struct PropertyTraits<BinaryDataPtr>
{
    typedef BinaryData ArgumentType ;
};

template <typename T, typename ValuePredicate>
struct PropertyT : public Property
{
    typedef typename PropertyTraits<T>::ArgumentType ArgumentType;
    PropertyT(
            std::string name,
            T* value,
            Trait trait,
            ValuePredicate predicate = ValuePredicate())
        : Property(name, deduce_value_type(*value), trait),
          value(value),
          predicate(predicate)
    {
    }

    void set_value(const ArgumentType& new_value) override
    {
        try
        {
            if (predicate)
            {
                predicate(new_value);
            }

            copy_value(new_value, value);
            m_is_set = true;
        }
        catch (const std::exception& e)
        {
            throw std::runtime_error(
                    "Failed to set value of property \"" + m_name + "\" : "
                    + e.what());
        }
        catch (...)
        {
            throw std::runtime_error(
                    "Failed to set value of property \"" + m_name
                    + "\" due to unknown exception");
        }
    }

    void reset_value() override
    {
        *value = T();
        m_is_set = false;
    }

    T* value;
    ValuePredicate predicate;
};

template <typename T>
void throw_unexpected_type(
        const std::string& name,
        Property::ValueType expected_type,
        const T& value)
{
    throw std::runtime_error(
            "Invalid value type \""
            + get_value_type_string(deduce_value_type((value)))
            + "\" for property \"" + name + "\" expected "
            + get_value_type_string(expected_type));
}

} // namespace

Property::Property(std::string name, ValueType value_type, Trait trait)
    : m_name(std::move(name)),
      m_type(value_type),
      m_trait(trait),
      m_is_set(false)
{
}

Property::~Property()
{
}

void Property::set_value(const int32_t& value)
{
    throw_unexpected_type(m_name, m_type, value);
}

void Property::set_value(const Amount& value)
{
    throw_unexpected_type(m_name, m_type, value);
}

void Property::set_value(const std::string& value)
{
    throw_unexpected_type(m_name, m_type, value);
}

void Property::set_value(const BinaryData& value)
{
    throw_unexpected_type(m_name, m_type, value);
}

void Property::set_value(const PublicKey& value)
{
    throw_unexpected_type(m_name, m_type, value);
}

const std::string Property::get_name() const
{
    return m_name;
}

Property::ValueType Property::get_value_type() const
{
    return m_type;
}
Property::Trait Property::get_trait() const
{
    return m_trait;
}

std::string Property::get_value_type_string() const
{
    return ::get_value_type_string(m_type);
}

std::string Property::get_property_spec() const
{
    return m_name + " : " + get_value_type_string() + " "
            + (m_trait == REQUIRED ? "mandatory" : "optional");
}

bool Property::is_set() const
{
    return m_is_set;
}

void Properties::reset_property(const std::string& name)
{
    get_property(name).reset_value();
}

std::vector<const Property*> Properties::get_properties() const
{
    std::vector<const Property*> result;
    result.reserve(m_properties.size());
    for (const auto& i : m_properties)
    {
        result.push_back(i.second.get());
    }
    return result;
}

bool Properties::validate(std::vector<std::string>* validation_details) const
{
    bool all_required_properties_set = true;
    for (const auto& i : m_properties)
    {
        const Property& property = *i.second;
        if ((property.get_trait() == Property::REQUIRED) && !property.is_set())
        {
            all_required_properties_set = false;
            if (validation_details)
            {
                validation_details->push_back(property.get_name());
            }
        }
    }
    return all_required_properties_set;
}

void Properties::bind_property(
        const std::string& name,
        int32_t* value,
        Property::Trait trait,
        Properties::Predicate<int32_t> predicate)
{
    do_bind_property(name, value, trait, std::move(predicate));
}

void Properties::bind_property(
        const std::string& name,
        std::string* value,
        Property::Trait trait,
        Properties::Predicate<std::string> predicate)
{
    do_bind_property(name, value, trait, std::move(predicate));
}

void Properties::bind_property(
        const std::string& name,
        Amount* value,
        Property::Trait trait,
        Properties::Predicate<Amount> predicate)
{
    do_bind_property(name, value, trait, std::move(predicate));
}

void Properties::bind_property(
        const std::string& name,
        BinaryDataPtr* value,
        Property::Trait trait,
        Predicate<BinaryData> predicate)
{
    do_bind_property(name, value, trait, std::move(predicate));
}

void Properties::bind_property(
        const std::string& name,
        PublicKeyPtr* value,
        Property::Trait trait,
        Properties::Predicate<PublicKey> predicate)
{
    do_bind_property(name, value, trait, std::move(predicate));
}

template <typename T, typename P>
void Properties::do_bind_property(
        const std::string& name, T* value, Property::Trait trait, P predicate)
{
    if (!value)
    {
        throw std::runtime_error("Attempt to bind property to nullptr");
    }

    make_property(name).reset(
            new PropertyT<T, P>(name, value, trait, std::move(predicate)));
}

Property& Properties::get_property(const std::string& name)
{
    auto i = m_properties.find(name);
    if (i == m_properties.end())
    {
        throw std::runtime_error(
                "Property with name \"" + name + "\" does not exists.");
    }
    return *i->second;
}

Properties::PropertyPtr& Properties::make_property(const std::string& name)
{
    auto p = m_properties.insert(
            std::make_pair(name, std::unique_ptr<Property>()));
    if (!p.second)
    {
        throw std::runtime_error("Property \"" + name + "\" already exists.");
    }

    return p.first->second;
}
