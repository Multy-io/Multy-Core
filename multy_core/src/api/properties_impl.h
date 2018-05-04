/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_CORE_SRC_API_PROPERTIES_IMPL_H
#define MULTY_CORE_SRC_API_PROPERTIES_IMPL_H

#include "multy_core/api.h"
#include "multy_core/error.h"

#include "multy_core/src/object.h"
#include "multy_core/src/u_ptr.h"

#include <cstdint>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

struct BigInt;
struct BinaryData;
struct PrivateKey;
struct PrivateKey;
struct CodeLocation;

struct Properties;

class MULTY_CORE_API Property
{
public:
    enum Trait
    {
        OPTIONAL,
        REQUIRED,
        READONLY
    };

    template <typename T>
    struct PredicateArgTraits
    {
        typedef T ArgumentType;
    };

    template <typename T, typename D>
    struct PredicateArgTraits<std::unique_ptr<T, D>>
    {
        typedef T ArgumentType;
    };

    template <typename U>
    using Predicate = std::function<void(typename PredicateArgTraits<U>::ArgumentType const&)>;

    bool is_set() const;
    std::string get_name() const;

    void set_trait(Trait new_trait);
    Trait get_trait() const;

protected:
    // Disallowing copying and moving.
    // Moving is dangerous since in Properties we bind to a specific variable address.
    Property(const Property&) = delete;
    Property& operator=(const Property&) = delete;
    Property(Property&&) = delete;
    Property& operator=(const Property&&) = delete;

protected:
    explicit Property(Properties& properties, const void* value_ptr);
    virtual ~Property();

    void throw_exception(ErrorCode error_code, std::string message, const CodeLocation& location) const;

    // throws exception if value is unset.
    void throw_if_unset() const;

    Properties& get_properties();
    const Properties& get_properties() const;

protected:
    Properties& m_properties;
    const void* m_value_ptr;
};

/** Properties system, allows to bind existing variable to the string name and
 * set it's value in type-safe manner via API.
 */
// TODO: split into PropertiesClient (no binding\unbinding) and Properties (as
// it is now).
struct MULTY_CORE_API Properties : public ::multy_core::internal::ObjectBase<Properties>
{
public:
    explicit Properties(ErrorScope error_scope, const std::string& name);

    struct MULTY_CORE_API Binder
    {
        virtual ~Binder();
        virtual void set_value(const int32_t& /*value*/) = 0;
        virtual void set_value(const BigInt& /*value*/) = 0;
        virtual void set_value(const std::string& /*value*/) = 0;
        virtual void set_value(const BinaryData& /*value*/) = 0;
        virtual void set_value(const PrivateKey& /*value*/) = 0;

        virtual void get_value(int32_t* /*value*/) const = 0;
        virtual void get_value(BigInt* /*value*/) const = 0;
        virtual void get_value(std::string* /*value*/) const = 0;
        virtual void get_value(multy_core::internal::BinaryDataPtr* /*value*/) const = 0;
        virtual void get_value(multy_core::internal::PrivateKeyPtr* /*value*/) const = 0;

        virtual void reset_value() = 0;

        virtual std::string get_property_spec() const = 0;
        virtual Property::Trait get_trait() const = 0;
        virtual void set_trait(Property::Trait) = 0;

        virtual bool is_set() const = 0;
        virtual const void* get_value() const = 0;
        virtual std::string get_name() const = 0;
    };

    typedef std::unique_ptr<Binder> BinderPtr;
    template <typename U>
    using Predicate = Property::Predicate<U>;

    /** Set specific value to a bound property.
     *
     * @param name - name of the property to set.
     * @param value - new value of the property.
     * @exception is thrown if:
     *         - property is not found by name;
     *         - property predicate throws an exception;
     *         - value copying routine throws an exception.
     */
    template <typename T>
    void set_property_value(const std::string& name, const T& value)
    {
        get_property(name).set_value(value);
        set_dirty();
    }

    template <typename T>
    void get_property_value(const std::string& name, T* out_value) const
    {
        get_property(name).get_value(out_value);
    }

    void reset_property(const std::string& name);

    /** Validate all bound properties.
     *
     * Check if all required bound properties are set.
     * @param unset_properties_names - optional, names of required properties that
     * was not set.
     * @return true if all required properties was set, false otherwise.
     * side-effect: on success causes is_dirty() to return false until any property is modified or reset.
     */
    bool validate(std::vector<std::string>* unset_properties_names) const;

    /** Returns true if validation is required.
     *
     * That means either:
     *  - no validation was performed yet;
     *  - some property was changed\reset after last validation;
     *  - previous validation failed.
     */
    bool is_dirty() const;

    /** Explicitly marks this as dirty, not public API.*/
    void set_dirty() const;

    /** Bind value to named property.
     *
     * Note that value must be alive during Properties lifetime and setting
     *     a new value actually writes to the given address.
     * @param name - property name.
     * @param value - pointer to the property value destination.
     * @param predicate - predicate that is going to verify vailidity of the
     *         new value. Predicate may throw an exception describing why given
     *         value is rejected.
     * @exception is thrown if:
     *     - name is already used;
     *     - value is nullptr;
     *     - value is already bound as property.
     */
    template <typename T>
    void bind_property(
            const std::string& name, T* value, Predicate<T> predicate)
    {
        bind_property(name, value, Property::REQUIRED, std::move(predicate));
    }

    void bind_property(
            const std::string& name,
            int32_t* value,
            Property::Trait trait = Property::REQUIRED,
            Predicate<int32_t> predicate = Predicate<int32_t>());

    void bind_property(
            const std::string& name,
            std::string* value,
            Property::Trait trait = Property::REQUIRED,
            Predicate<std::string> predicate = Predicate<std::string>());

    void bind_property(
            const std::string& name,
            BigInt* value,
            Property::Trait trait = Property::REQUIRED,
            Predicate<BigInt> predicate = Predicate<BigInt>());

    void bind_property(
            const std::string& name,
            multy_core::internal::BinaryDataPtr* value,
            Property::Trait trait = Property::REQUIRED,
            Predicate<BinaryData> predicate = Predicate<BinaryData>());

    void bind_property(
            const std::string& name,
            multy_core::internal::PrivateKeyPtr* value,
            Property::Trait trait = Property::REQUIRED,
            Predicate<PrivateKey> predicate = Predicate<PrivateKey>());

    // Returns true if property was unbound, false if there was not such
    // property.
    bool unbind_property(const std::string& name);
    bool unbind_property_by_value(const void* value);

    /** Check if property bound to this value was explictly set.
     *
     * @param value - address of the value used in bind_property().
     * @return true if value is bound to property and property was set,
     *         false otherwise;
     */
    bool is_set(const void* value) const;

    Binder& get_property(const std::string& name);
    const Binder& get_property(const std::string& name) const;

    Binder& get_property_by_value(const void*);
    const Binder& get_property_by_value(const void*) const;

    /** Get all properties.
     *
     * @return all properties, ownership is not transferred.
     */
    std::vector<const Binder*> get_all_properties() const;

    std::string get_name() const;

    // Not a part of public interface.
    void throw_exception(ErrorCode error, const std::string& message, const CodeLocation& location) const;

    static const void* get_object_magic();

private:
    template <typename T, typename P>
    void do_bind_property(
            const std::string& name,
            T* value,
            Property::Trait trait,
            P predicate);

protected:
    BinderPtr& make_property(const std::string& name, const void* value);

private:
    const ErrorScope m_error_scope;
    const std::string m_name;
    std::map<std::string, BinderPtr> m_properties;
    std::map<const void*, std::string> m_property_name_by_value;
    mutable bool m_is_dirty;
};

template <typename T>
class PropertyT : public Property
{
public:
    PropertyT(
            Properties& props,
            const std::string& name,
            Trait trait = Property::REQUIRED,
            Predicate<T> predicate = Predicate<T>())
        : PropertyT(T(), props, name, trait, std::move(predicate))
    {}

    PropertyT(
            T initial_value,
            Properties& props,
            const std::string& name,
            Trait trait = Property::REQUIRED,
            Predicate<T> predicate = Predicate<T>())
        : Property(props, &m_value),
          m_value(std::move(initial_value))
    {
        props.bind_property(name, &m_value, trait, std::move(predicate));
        // TODO: set value through props.set_value();
    }

    const T& operator*() const
    {
        return get_value();
    }

    T& operator*()
    {
        return get_value();
    }

    const T& operator->() const
    {
        return get_value();
    }

    T& operator->()
    {
        return get_value();
    }

    const T& get_value() const
    {
        throw_if_unset();
        return m_value;
    }

    T& get_value()
    {
        throw_if_unset();
        return m_value;
    }

    void set_value(const typename PredicateArgTraits<T>::ArgumentType& new_value)
    {
        m_properties.get_property_by_value(&m_value).set_value(new_value);
    }

    const T& get_default_value(const T& default_value) const
    {
        if (!is_set())
        {
            return default_value;
        }
        return m_value;
    }

private:
    T m_value;
};

#endif // MULTY_CORE_SRC_API_PROPERTIES_IMPL_H
