/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/src/api/properties_impl.h"

#include "multy_core/src/api/big_int_impl.h"

#include "multy_core/common.h"
#include "multy_core/src/api/key_impl.h"
#include "multy_core/src/exception.h"
#include "multy_core/src/exception_stream.h"
#include "multy_core/src/utility.h"

#include <exception>
#include <string>
#include <vector>

#define THROW_PROPERTY_EXCEPTION(msg) throw_exception(ERROR_GENERAL_ERROR, msg , MULTY_CODE_LOCATION)
#define THROW_PROPERTY_EXCEPTION2(err, msg) throw_exception(err, msg , MULTY_CODE_LOCATION)

namespace
{
using namespace multy_core::internal;
typedef Properties::Binder Binder;
using ValueType = Binder::ValueType;

// If you find that compilation fails du to this function definition missing,
// you should add a new template specialization for your type.
template <typename T>
constexpr ValueType deduce_value_type();

template <>
constexpr ValueType deduce_value_type<int32_t>()
{
    return Properties::Binder::ValueType::VALUE_TYPE_INT32;
}

template <>
constexpr ValueType deduce_value_type<std::string>()
{
    return Properties::Binder::ValueType::VALUE_TYPE_STRING;
}

template <>
constexpr ValueType deduce_value_type<BigInt>()
{
    return Properties::Binder::ValueType::VALUE_TYPE_BIG_INT;
}

template <>
constexpr ValueType deduce_value_type<BinaryData>()
{
    return Properties::Binder::ValueType::VALUE_TYPE_BINARY_DATA;
}

template <>
constexpr ValueType deduce_value_type<PrivateKey>()
{
    return Properties::Binder::ValueType::VALUE_TYPE_PRIVATE_KEY;
}

template <typename T>
constexpr ValueType deduce_value_type()
{
    // effectively, strip std::unique_ptr<T> to T
    return deduce_value_type<typename Property::PredicateArgTraits<T>::ArgumentType>();
}

std::string get_value_type_string(ValueType type)
{
    switch (type)
    {
        case Properties::Binder::ValueType::VALUE_TYPE_INT32:
            return "int32";
        case Properties::Binder::ValueType::VALUE_TYPE_STRING:
            return "string";
        case Properties::Binder::ValueType::VALUE_TYPE_BIG_INT:
            return "BigInt";
        case Properties::Binder::ValueType::VALUE_TYPE_BINARY_DATA:
            return "BinaryData";
        case Properties::Binder::ValueType::VALUE_TYPE_PRIVATE_KEY:
            return "PrivateKey";
    }
    return "UNKNOWN TYPE";
}

struct BinderBase : public Binder
{
    BinderBase(
            const Properties& properties,
            const std::string& name,
            const void* value,
            ValueType value_type,
            Property::Trait trait)
        : m_properties(properties),
          m_name(name),
          m_raw_value(value),
          m_type(value_type),
          m_trait(trait),
          m_is_set(false)
    {
    }

    BinderBase(const BinderBase&) = delete;
    BinderBase& operator=(const BinderBase&) = delete;
    BinderBase(BinderBase&&) = delete;
    BinderBase& operator=(BinderBase&&) = delete;

    virtual ~BinderBase()
    {
    }

    void throw_exception(ErrorCode error_code, std::string message, const CodeLocation& location) const
    {
        m_properties.throw_exception(error_code, message, location);
    }

    template <typename T>
    void throw_unexpected_type(const T*) const
    {
        THROW_PROPERTY_EXCEPTION("Invalid value type \""
                + ::get_value_type_string(deduce_value_type<T>())
                + "\" for property \"" + m_name + "\" expected "
                + ::get_value_type_string(m_type));
    }

    void set_value(const int32_t& value) override
    {
        throw_unexpected_type(&value);
    }

    void set_value(const BigInt& value) override
    {
        throw_unexpected_type(&value);
    }

    void set_value(const std::string& value) override
    {
        throw_unexpected_type(&value);
    }

    void set_value(const BinaryData& value) override
    {
        throw_unexpected_type(&value);
    }

    void set_value(const PrivateKey& value) override
    {
        throw_unexpected_type(&value);
    }

    void get_value(int32_t* value) const override
    {
        throw_unexpected_type(value);
    }
    void get_value(BigInt* value) const override
    {
        throw_unexpected_type(value);
    }
    void get_value(std::string* value) const override
    {
        throw_unexpected_type(value);
    }
    void get_value(multy_core::internal::BinaryDataPtr* value) const override
    {
        throw_unexpected_type(value);
    }
    void get_value(multy_core::internal::PrivateKeyPtr* value) const  override
    {
        throw_unexpected_type(value);
    }

    std::string get_name() const override
    {
        return m_name;
    }

    ValueType get_value_type() const override
    {
        return m_type;
    }
    Property::Trait get_trait() const override
    {
        return m_trait;
    }
    void set_trait(Property::Trait new_trait) override
    {
        m_trait = new_trait;
    }

    std::string get_value_type_string() const
    {
        return ::get_value_type_string(m_type);
    }

    std::string get_property_spec() const override
    {
        return m_name + " : " + get_value_type_string() + " "
                + (m_trait == Property::REQUIRED ? "mandatory" : "optional");
    }

    bool is_set() const override
    {
        return m_is_set;
    }

    const void* get_value() const override
    {
        return m_raw_value;
    }

    void reset_value() override
    {
        reset();
    }

    void handle_exception(const char* action, const CodeLocation& location) const
    {
        try
        {
            throw;
        }
        catch (const Exception& e)
        {
            throw_exception(e.get_error_code(),
                    build_exception_message(action, e.get_message()),
                    e.get_location());
        }
        catch (const std::exception& e)
        {
            throw_exception(ERROR_GENERAL_ERROR,
                    build_exception_message(action, e.what()), location);
        }
        catch (...)
        {
            throw_exception(ERROR_GENERAL_ERROR,
                    build_exception_message(action, "unknown exception"),
                    location);
        }
    }

    void set()
    {
        m_is_set = true;
        m_properties.set_dirty();
    }

    void reset()
    {
        m_is_set = false;
        m_properties.set_dirty();
    }
private:
    std::string build_exception_message(const char* action, std::string extra_message) const
    {
        return std::string(" Failed to ") + action
                + " of property \"" + m_name + "\" : " + extra_message + ".";
    }

private:
    const Properties& m_properties;
    const std::string m_name;
    const void* m_raw_value;
    const ValueType m_type;
    Property::Trait m_trait;
    bool m_is_set;
};

template <typename T>
void copy_value(const T& from, T* to)
{
    *to = from;
}

void copy_value(const BinaryData& from, BinaryDataPtr* to)
{
    make_clone(from).swap(*to);
}

void copy_value(const PrivateKey& from, PrivateKeyPtr* to)
{
    from.clone().swap(*to);
}

template <typename T, typename D>
void copy_value(const T& from, std::unique_ptr<T, D>* to)
{
    make_clone(from).swap(*to);
}

template <typename T>
const T& to_argument_type(const T& value)
{
    return value;
}

template <typename T, typename D>
const T& to_argument_type(const std::unique_ptr<T, D>& value)
{
    if (!value)
    {
        THROW_EXCEPTION2(ERROR_GENERAL_ERROR, "Value is nullptr.");
    }

    return *value;
}

template <typename T, typename ValuePredicate>
struct ValueBinderT : public BinderBase
{
    typedef typename Property::PredicateArgTraits<T>::ArgumentType ArgumentType;
    static const ValueType VAL_TYPE = deduce_value_type<ArgumentType>();
    ValueBinderT(const Properties& properties,
            const std::string& name,
            T* value,
            Property::Trait trait,
            ValuePredicate predicate = ValuePredicate())
        : BinderBase(properties, name, value, VAL_TYPE, trait),
          m_value(value),
          m_predicate(std::move(predicate))
    {
    }

    void set_value(const ArgumentType& new_value) override
    {
        try
        {
            if (get_trait() == Property::READONLY)
            {
                THROW_EXCEPTION("property is read-only.");
            }

            if (m_predicate)
            {
                m_predicate(new_value);
            }

            copy_value(new_value, m_value);

            set();
        }
        catch (...)
        {
            handle_exception("set value", MULTY_CODE_LOCATION);
        }
    }

    void get_value(T* out_value) const override
    {
        try
        {
            if (!out_value)
            {
                THROW_EXCEPTION2(ERROR_INVALID_ARGUMENT,
                        "out_value must not be nullptr.");
            }
            copy_value(to_argument_type(*m_value), out_value);
        }
        catch(...)
        {
            handle_exception("get value", MULTY_CODE_LOCATION);
        }
    }

    T* m_value;
    ValuePredicate m_predicate;
};

template <typename ArgumentType>
struct FunctionBinderT : public BinderBase
{
    static const ValueType VAL_TYPE = deduce_value_type<ArgumentType>();
    FunctionBinderT(const Properties& properties,
            const std::string& name,
            void* value,
            Properties::Writer<ArgumentType> writer,
            Properties::Reader<ArgumentType> reader,
            Property::Trait trait)
        : BinderBase(properties, name, value, VAL_TYPE, trait),
        m_writer(std::move(writer)),
        m_reader(std::move(reader))
    {
    }

    void set_value(const ArgumentType& new_value) override
    {
        try
        {
            m_writer(new_value);
            set();
        }
        catch (...)
        {
            handle_exception("set value", MULTY_CODE_LOCATION);
        }
    }

    void get_value(typename property_traits::WriterReturnTraits<ArgumentType>::ReturnType* out_value) const override
    {
        try
        {
            if (!out_value)
            {
                THROW_EXCEPTION2(ERROR_INVALID_ARGUMENT,
                        "out_value must not be nullptr.");
            }
            copy_value(to_argument_type(m_reader()), out_value);
        }
        catch(...)
        {
            handle_exception("get value", MULTY_CODE_LOCATION);
        }
    }

private:
    Properties::Writer<ArgumentType> m_writer;
    Properties::Reader<ArgumentType> m_reader;
};

} // namespace

Property::Property(Properties& properties, const void* value_ptr)
    : m_properties(properties),
      m_value_ptr(value_ptr)
{
}

Property::~Property()
{
    if (m_properties.is_valid())
    {
        m_properties.unbind_property_by_value(m_value_ptr);
    }
}

void Property::throw_exception(ErrorCode err_code, std::string message, const CodeLocation& location) const
{
    return get_properties().throw_exception(err_code, message, location);
}

void Property::throw_if_unset() const
{
    if (!is_set())
    {
        const Properties::Binder& b
                = get_properties().get_property_by_value(m_value_ptr);
        if (b.get_trait() == Property::REQUIRED)
        {
        THROW_PROPERTY_EXCEPTION(m_properties.get_name() + " property \""
                + b.get_name() + "\" is not set.");
        }
    }
}
bool Property::is_set() const
{
    return get_properties().is_set(m_value_ptr);
}

std::string Property::get_name() const
{
    return get_properties().get_property_by_value(m_value_ptr).get_name();
}

void Property::set_trait(Trait new_trait)
{
    get_properties().get_property_by_value(m_value_ptr).set_trait(new_trait);
}

Property::Trait Property::get_trait() const
{
    return get_properties().get_property_by_value(m_value_ptr).get_trait();
}

Properties& Property::get_properties()
{
    INVARIANT(m_properties.is_valid());

    return m_properties;
}

const Properties& Property::get_properties() const
{
    return const_cast<Property*>(this)->get_properties();
}

Properties::Binder::~Binder()
{
}

Properties::Properties(ErrorScope error_scope, const std::string& name)
    : m_error_scope(error_scope),
      m_name(name),
      m_properties(),
      m_property_name_by_value(),
      m_is_dirty(true)
{
}

void Properties::reset_property(const std::string& name)
{
    get_property(name).reset_value();
    set_dirty();
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

bool Properties::validate(std::vector<std::string>* unset_properties_names) const
{
    bool all_required_properties_set = true;
    for (const auto& i : m_properties)
    {
        const Binder& property = *i.second;
        if ((property.get_trait() == Property::REQUIRED) && !property.is_set())
        {
            all_required_properties_set = false;
            if (unset_properties_names)
            {
                unset_properties_names->push_back(property.get_name());
            }
        }
    }
    m_is_dirty = !all_required_properties_set;
    return all_required_properties_set;
}

void Properties::validate(const CodeLocation& location) const
{
    std::vector<std::string> unset_properties_names;
    if (!validate(&unset_properties_names))
    {
        throw_exception(ERROR_NOT_ALL_REQUIRED_PROPERTIES_SET,
                "Missing properties:" + join("\n", unset_properties_names),
                location);
    }
}

std::string Properties::get_specification() const
{
    std::stringstream sstr;
    for (const auto p : get_all_properties())
    {
        sstr << p->get_property_spec() << "\n";
    }

    return sstr.str();
}

bool Properties::is_dirty() const
{
    return m_is_dirty;
}

void Properties::set_dirty() const
{
    m_is_dirty = true;
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
        BigInt* value,
        Property::Trait trait,
        Property::Predicate<BigInt> predicate)
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
        PrivateKeyPtr* value,
        Property::Trait trait,
        Property::Predicate<PrivateKey> predicate)
{
    do_bind_property(name, value, trait, std::move(predicate));
}

void Properties::bind_functional_property(
        const std::string& name,
        void* value,
        Properties::Writer<int32_t> writer,
        Properties::Reader<int32_t> reader,
        Property::Trait trait)
{
    do_bind_functional_property(name, value,
            std::move(writer), std::move(reader), trait);
}

void Properties::bind_functional_property(
        const std::string& name,
        void* value,
        Properties::Writer<std::string> writer,
        Properties::Reader<std::string> reader,
        Property::Trait trait)
{
    do_bind_functional_property(name, value,
            std::move(writer), std::move(reader), trait);
}

void Properties::bind_functional_property(
        const std::string& name,
        void* value,
        Properties::Writer<BigInt> writer,
        Properties::Reader<BigInt> reader,
        Property::Trait trait)
{
    do_bind_functional_property(name, value,
            std::move(writer), std::move(reader), trait);
}

void Properties::bind_functional_property(
        const std::string& name,
        void* value,
        Properties::Writer<BinaryData> writer,
        Properties::Reader<BinaryData> reader,
        Property::Trait trait)
{
    do_bind_functional_property(name, value,
            std::move(writer), std::move(reader), trait);
}

void Properties::bind_functional_property(
        const std::string& name,
        void* value,
        Properties::Writer<PrivateKey> writer,
        Properties::Reader<PrivateKey> reader,
        Property::Trait trait)
{
    do_bind_functional_property(name, value,
            std::move(writer), std::move(reader), trait);
}

template <typename T, typename P>
void Properties::do_bind_property(
        const std::string& name, T* value, Property::Trait trait, P predicate)
{
    INVARIANT(value != nullptr);

    make_property(name, value).reset(new ValueBinderT<T, P>(
            *this, name, value, trait, std::move(predicate)));
}

template <typename Arg>
void Properties::do_bind_functional_property(
        const std::string& name,
        void* value,
        Properties::Writer<Arg> writer,
        Properties::Reader<Arg> reader,
        Property::Trait trait)
{
    INVARIANT(value != nullptr);

    make_property(name, value).reset(new FunctionBinderT<Arg>(
            *this, name, value, std::move(writer), std::move(reader), trait));
}

bool Properties::unbind_property(const std::string& name)
{
    INVARIANT(!name.empty());

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
    INVARIANT(value != nullptr);

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
        THROW_PROPERTY_EXCEPTION("Property with name \"" + name + "\" does not exists.");
    }
    return *i->second;
}

Binder& Properties::get_property_by_value(const void* value)
{
    return const_cast<Binder&>(
            const_cast<const Properties*>(this)->get_property_by_value(value));
}

const Binder& Properties::get_property_by_value(const void* value) const
{
    auto p = m_property_name_by_value.find(value);
    if (p == m_property_name_by_value.end())
    {
        THROW_PROPERTY_EXCEPTION("Can't find property bound to value.");
    }

    return get_property(p->second);
}

// TODO: update function to take BinderPtr as parameter.
Properties::BinderPtr& Properties::make_property(
        const std::string& name, const void* value)
{
    auto p = m_properties.insert(std::make_pair(name, BinderPtr()));
    if (!p.second)
    {
        THROW_PROPERTY_EXCEPTION("Property with name \"" + name + "\" already exists.");
    }

    auto p2 = m_property_name_by_value.insert(std::make_pair(value, name));
    if (!p2.second)
    {
        m_properties.erase(p.first);
        THROW_PROPERTY_EXCEPTION("Value is already bound to property \""
                + p2.first->second + "\".");
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

void Properties::throw_exception(ErrorCode error_code, const std::string& message, const CodeLocation& location) const
{
    throw Exception(set_error_scope(m_error_scope, error_code),
            get_name().c_str(), location) << " : " << message;
}

const void* Properties::get_object_magic()
{
    RETURN_MAGIC();
}
