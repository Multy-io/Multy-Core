/* Copyright 2017 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_transaction/internal/properties.h"

#include "multy_transaction/internal/amount.h"

#include "multy_core/common.h"
#include "multy_core/internal/key.h"
#include "multy_core/internal/utility.h"

#include <exception>
#include <string>
#include <vector>

namespace
{
using namespace wallet_core::internal;
typedef Properties::Binder Binder;

enum ValueType
{
    VALUE_TYPE_INT32,
    VALUE_TYPE_AMOUNT,
    VALUE_TYPE_STRING,
    VALUE_TYPE_BINARY_DATA,
    VALUE_TYPE_PUBLIC_KEY,
    //        VALUE_TYPE_PRIVATE_KEY
};

ValueType deduce_value_type(const int32_t&)
{
    return VALUE_TYPE_INT32;
}

ValueType deduce_value_type(const std::string&)
{
    return VALUE_TYPE_STRING;
}

ValueType deduce_value_type(const Amount&)
{
    return VALUE_TYPE_AMOUNT;
}

ValueType deduce_value_type(const BinaryData&)
{
    return VALUE_TYPE_BINARY_DATA;
}

ValueType deduce_value_type(const PublicKey&)
{
    return VALUE_TYPE_PUBLIC_KEY;
}

// Property::ValueType deduce_value_type(const PrivateKey&)
//{
//    return Property::VALUE_TYPE_PRIVATE_KEY;
//}

template <typename T, typename D>
ValueType deduce_value_type(const std::unique_ptr<T, D>& value)
{
    return deduce_value_type(*value);
}

std::string get_value_type_string(ValueType type)
{
    switch (type)
    {
        case VALUE_TYPE_INT32:
            return "int32";
        case VALUE_TYPE_STRING:
            return "string";
        case VALUE_TYPE_AMOUNT:
            return "Amount";
        case VALUE_TYPE_BINARY_DATA:
            return "BinaryData";
        case VALUE_TYPE_PUBLIC_KEY:
            return "PublicKey";
            //        case Property::VALUE_TYPE_PRIVATE_KEY:
            //            return "PrivateKey";
    }
    return "UNKNOWN TYPE";
}

struct BinderBase : public Binder
{
    BinderBase(
            const Properties& properties,
            std::string name,
            const void* value,
            ValueType value_type,
            Property::Trait trait)
        : m_properties(properties),
          m_name(std::move(name)),
          m_value(value),
          m_type(value_type),
          m_trait(trait),
          m_is_set(false)
    {
    }

    virtual ~BinderBase()
    {
    }

    template <typename T>
    void throw_unexpected_type(
            const std::string& name, ValueType expected_type, const T& value)
    {
        m_properties.throw_exception(
                "Invalid value type \""
                + ::get_value_type_string(deduce_value_type((value)))
                + "\" for property \"" + name + "\" expected "
                + ::get_value_type_string(expected_type));
    }

    void set_value(const int32_t& value)
    {
        throw_unexpected_type(m_name, m_type, value);
    }

    void set_value(const Amount& value)
    {
        throw_unexpected_type(m_name, m_type, value);
    }

    void set_value(const std::string& value)
    {
        throw_unexpected_type(m_name, m_type, value);
    }

    void set_value(const BinaryData& value)
    {
        throw_unexpected_type(m_name, m_type, value);
    }

    void set_value(const PublicKey& value)
    {
        throw_unexpected_type(m_name, m_type, value);
    }

    std::string get_name() const
    {
        return m_name;
    }

    ValueType get_value_type() const
    {
        return m_type;
    }
    Property::Trait get_trait() const
    {
        return m_trait;
    }

    std::string get_value_type_string() const
    {
        return ::get_value_type_string(m_type);
    }

    std::string get_property_spec() const
    {
        return m_name + " : " + get_value_type_string() + " "
                + (m_trait == Property::REQUIRED ? "mandatory" : "optional");
    }

    bool is_set() const
    {
        return m_is_set;
    }

    const void* get_value() const
    {
        return m_value;
    }

    void handle_exception(const char* action)
    {
        try
        {
            throw;
        }
        catch (const std::exception& e)
        {
            m_properties.throw_exception(std::string(" Failed to ") + action
                    + " of property \"" + m_name + "\" : " + e.what());
        }
        catch (...)
        {
            m_properties.throw_exception(std::string("Failed to ") + action
                    + " of property \"" + m_name
                    + "\" due to unknown exception");
        }
    }

    virtual void reset_value() = 0;

protected:
    const Properties& m_properties;
    const std::string m_name;
    const void* m_value;
    const ValueType m_type;
    const Property::Trait m_trait;
    bool m_is_set;
};

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

void copy_value(const PrivateKey& from, PrivateKeyPtr* to)
{
    *to = std::move(from.clone());
}

template <typename T, typename D>
void copy_value(const T& from, std::unique_ptr<T, D>* to)
{
    *to = std::move(make_clone(from));
}

template <typename T, typename ValuePredicate>
struct BinderT : public BinderBase
{
    typedef typename Property::PredicateArgTraits<T>::ArgumentType ArgumentType;
    BinderT(const Properties& properties,
            std::string name,
            T* value,
            Property::Trait trait,
            ValuePredicate predicate = ValuePredicate())
        : BinderBase(
                  properties,
                  std::move(name),
                  value,
                  deduce_value_type(*value),
                  trait),
          m_value(value),
          m_predicate(std::move(predicate))
    {
    }

    void set_value(const ArgumentType& new_value) override
    {
        try
        {
            if (m_predicate)
            {
                m_predicate(new_value);
            }

            copy_value(new_value, m_value);
            m_is_set = true;
        }
        catch (...)
        {
            handle_exception("set value");
        }
    }

    void reset_value() override
    {
        // TODO: Since BinaryData has no default constructor, it is plain
        // dangerous to reset value like that, need to have some template
        // "reset" function to do that properly.
        // copy_value(T(), value);
        m_is_set = false;
    }

    T* m_value;
    ValuePredicate m_predicate;
};

} // namespace

Property::Property(Properties& properties) : m_properties(properties)
{
}

Property::~Property()
{
}

// throws exception if value is unset
void Property::throw_if_unset(const void* property_var) const
{
    if (!is_set(property_var))
    {
        const Properties::Binder& b
                = m_properties.get_property_by_value(property_var);
        throw std::runtime_error(
                m_properties.get_name() + " property \"" + b.get_name()
                + "\" is not set.");
    }
}
bool Property::is_set(const void* property_var) const
{
    return m_properties.is_set(property_var);
}

// void Property::set_value(const PrivateKey& value)
//{
//    throw_unexpected_type(m_name, m_type, value);
//}

Properties::Binder::~Binder()
{
}

Properties::Properties(std::string name) : m_name(std::move(name))
{
}

void Properties::reset_property(const std::string& name)
{
    get_property(name).reset_value();
}

std::vector<const Binder*> Properties::get_all_properties() const
{
    std::vector<const Binder*> result;
    result.reserve(m_properties.size());
    for (const auto& i : m_properties)
    {
        result.push_back(i.second.get());
    }
    return result;
}

std::string Properties::get_name() const
{
    return m_name;
}

bool Properties::validate(std::vector<std::string>* validation_details) const
{
    bool all_required_properties_set = true;
    for (const auto& i : m_properties)
    {
        const Binder& property = *i.second;
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
        Property::Predicate<int32_t> predicate)
{
    do_bind_property(name, value, trait, std::move(predicate));
}

void Properties::bind_property(
        const std::string& name,
        std::string* value,
        Property::Trait trait,
        Property::Predicate<std::string> predicate)
{
    do_bind_property(name, value, trait, std::move(predicate));
}

void Properties::bind_property(
        const std::string& name,
        Amount* value,
        Property::Trait trait,
        Property::Predicate<Amount> predicate)
{
    do_bind_property(name, value, trait, std::move(predicate));
}

void Properties::bind_property(
        const std::string& name,
        BinaryDataPtr* value,
        Property::Trait trait,
        Property::Predicate<BinaryData> predicate)
{
    do_bind_property(name, value, trait, std::move(predicate));
}

void Properties::bind_property(
        const std::string& name,
        PublicKeyPtr* value,
        Property::Trait trait,
        Property::Predicate<PublicKey> predicate)
{
    do_bind_property(name, value, trait, std::move(predicate));
}

// void Properties::bind_property(
//        const std::string& name,
//        PrivateKeyPtr* value,
//        Property::Trait trait,
//        Predicate<PrivateKey> predicate)
//{
//    do_bind_property(name, value, trait, std::move(predicate));
//}

template <typename T, typename P>
void Properties::do_bind_property(
        const std::string& name, T* value, Property::Trait trait, P predicate)
{
    if (!value)
    {
        throw_exception("Attempt to bind property to nullptr");
    }

    make_property(name, value)
            .reset(new BinderT<T, P>(
                    *this, name, value, trait, std::move(predicate)));
}

bool Properties::unbind_property(const std::string& name)
{
    const auto p = m_properties.find(name);
    if (p == m_properties.end())
    {
        return false;
    }
    const auto p2 = m_property_name_by_value.find(p->second->get_value());
    m_properties.erase(p);
    if (p2 == m_property_name_by_value.end())
    {
        // Internal inconsistncy, incident should be logged.
    }
    else
    {
        m_property_name_by_value.erase(p2);
    }
    return true;
}

bool Properties::unbind_property_by_value(const void* value)
{
    const auto p2 = m_property_name_by_value.find(value);
    if (p2 == m_property_name_by_value.end())
    {
        return false;
    }

    auto p = m_properties.find(p2->second);
    m_property_name_by_value.erase(p2);
    if (p == m_properties.end())
    {
        // Internal inconsistncy, incident should be logged.
    }
    else
    {
        m_properties.erase(p);
    }
    return true;
}

Binder& Properties::get_property(const std::string& name)
{
    return const_cast<Binder&>(
            const_cast<const Properties*>(this)->get_property(name));
}

const Binder& Properties::get_property(const std::string& name) const
{
    auto i = m_properties.find(name);
    if (i == m_properties.end())
    {
        throw_exception("Property with name \"" + name + "\" does not exists.");
    }
    return *i->second;
}

Binder& Properties::get_property_by_value(void* value)
{
    return const_cast<Binder&>(
            const_cast<const Properties*>(this)->get_property_by_value(value));
}

const Binder& Properties::get_property_by_value(const void* value) const
{
    auto p = m_property_name_by_value.find(value);
    if (p == m_property_name_by_value.end())
    {
        throw_exception("Can't find property bound to value.");
    }

    return get_property(p->second);
}

Properties::BinderPtr& Properties::make_property(
        const std::string& name, const void* value)
{
    auto p = m_properties.insert(std::make_pair(name, BinderPtr()));
    if (!p.second)
    {
        throw_exception("Property \"" + name + "\" already exists.");
    }

    auto p2 = m_property_name_by_value.insert(std::make_pair(value, name));
    if (!p2.second)
    {
        m_properties.erase(p.first);
        throw_exception(
                "Value is already bound to property \"" + p2.first->second
                + "\".");
    }
    return p.first->second;
}

bool Properties::is_set(const void* value) const
{
    auto p = m_property_name_by_value.find(value);
    if (p != m_property_name_by_value.end())
    {
        return get_property(p->second).is_set();
    }
    return false;
}

void Properties::throw_exception(const std::string& message) const
{
    throw std::runtime_error(get_name() + " : " + message);
}
