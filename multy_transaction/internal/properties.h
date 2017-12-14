/* Copyright 2017 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_TRANSACTION_INTERNAL_PROPERTIES_H
#define MULTY_TRANSACTION_INTERNAL_PROPERTIES_H

#include "multy_transaction/api.h"

#include "multy_core/internal/u_ptr.h"

#include <cstdint>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

struct Amount;
struct BinaryData;
struct PrivateKey;
struct PublicKey;

struct Properties;

class MULTY_TRANSACTION_API Property
{
public:
    enum Trait
    {
        OPTIONAL,
        REQUIRED,
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

protected:
    Property(Properties& properties);
    virtual ~Property();

    // throws exception if value is unset
    void throw_if_unset(const void*) const;
    bool is_set(const void*) const;

protected:
    Properties& m_properties;
};

/* Properties system, allows to bind existing variable to the string name and
 * set it's value in type-safe manner via API.
 */
// TODO: split into PropertiesClient (no binding\unbinding) and Properties (as
// it is now).
struct MULTY_TRANSACTION_API Properties
{
    explicit Properties(const std::string& name);

    struct MULTY_TRANSACTION_API Binder
    {
        virtual ~Binder();
        virtual void set_value(const int32_t& /*value*/) = 0;
        virtual void set_value(const Amount& /*value*/) = 0;
        virtual void set_value(const std::string& /*value*/) = 0;
        virtual void set_value(const BinaryData& /*value*/) = 0;
        virtual void set_value(const PublicKey& /*value*/) = 0;
        virtual void reset_value() = 0;

        virtual std::string get_property_spec() const = 0;
        virtual Property::Trait get_trait() const = 0;
        virtual bool is_set() const = 0;
        virtual const void* get_value() const = 0;
        virtual std::string get_name() const = 0;
    };

    typedef std::unique_ptr<Binder> BinderPtr;

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
    void set_property(const std::string& name, const T& value)
    {
        get_property(name).set_value(value);
    }

    void reset_property(const std::string& name);

    /** Validate all bound properties.
     *
     * Check if all required bound properties are set.
     * @param validation_details - optional, names of required properties that
     * was not set.
     * @return true if all required properties was set, false otherwise.
     */
    bool validate(std::vector<std::string>* validation_details) const;

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
            const std::string& name, T* value, Property::Predicate<T> predicate)
    {
        bind_property(name, value, Property::REQUIRED, std::move(predicate));
    }

    void bind_property(
            const std::string& name,
            int32_t* value,
            Property::Trait trait = Property::REQUIRED,
            Property::Predicate<int32_t> predicate = Property::Predicate<int32_t>());

    void bind_property(
            const std::string& name,
            std::string* value,
            Property::Trait trait = Property::REQUIRED,
            Property::Predicate<std::string> predicate = Property::Predicate<std::string>());

    void bind_property(
            const std::string& name,
            Amount* value,
            Property::Trait trait = Property::REQUIRED,
            Property::Predicate<Amount> predicate = Property::Predicate<Amount>());

    void bind_property(
            const std::string& name,
            wallet_core::internal::BinaryDataPtr* value,
            Property::Trait trait = Property::REQUIRED,
            Property::Predicate<BinaryData> predicate = Property::Predicate<BinaryData>());

    void bind_property(
            const std::string& name,
            wallet_core::internal::PublicKeyPtr* value,
            Property::Trait trait = Property::REQUIRED,
            Property::Predicate<PublicKey> predicate = Property::Predicate<PublicKey>());

    //    void bind_property(
    //            const std::string& name,
    //            wallet_core::internal::PrivateKeyPtr* value,
    //            Property::Trait trait = Property::REQUIRED,
    //            Predicate<PrivateKey> predicate = Predicate<PrivateKey>());

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

    Binder& get_property_by_value(void*);
    const Binder& get_property_by_value(const void*) const;

    /** Get all properties.
     *
     * @return all properties, ownership is not transferred.
     */
    std::vector<const Binder*> get_all_properties() const;

    std::string get_name() const;

    // Not a part of public interface.
    void throw_exception(const std::string& message) const;

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
    const std::string m_name;
    std::map<std::string, BinderPtr> m_properties;
    std::map<const void*, std::string> m_property_name_by_value;
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
        : Property(props),
          m_value(std::move(initial_value))
    {
        props.bind_property(name, &m_value, trait, std::move(predicate));
    }

    const T& get_value() const
    {
        throw_if_unset(&m_value);
        return m_value;
    }

    T& get_value()
    {
        throw_if_unset(&m_value);
        return m_value;
    }

    const T& get_default_value(const T& default_value) const
    {
        if (!is_set())
        {
            return default_value;
        }
        return m_value;
    }

    bool is_set() const
    {
        return Property::is_set(&m_value);
    }

private:
    T m_value;
};

#endif // MULTY_TRANSACTION_INTERNAL_PROPERTIES_H
