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

struct BinaryData;
struct PublicKey;
struct Amount;

struct Property
{
    enum ValueType
    {
        VALUE_TYPE_INT32,
        VALUE_TYPE_AMOUNT,
        VALUE_TYPE_STRING,
        VALUE_TYPE_BINARY_DATA,
        VALUE_TYPE_PUBLIC_KEY
    };

    enum Trait
    {
        OPTIONAL,
        REQUIRED,
    };

    Property(std::string name, ValueType value_type, Trait trait);
    virtual ~Property();

    virtual void set_value(const int32_t& /*value*/);
    virtual void set_value(const Amount& /*value*/);
    virtual void set_value(const std::string& /*value*/);
    virtual void set_value(const BinaryData& /*value*/);
    virtual void set_value(const PublicKey& /*value*/);

    virtual void reset_value() = 0;

    const std::string get_name() const;
    ValueType get_value_type() const;
    Trait get_trait() const;

    std::string get_value_type_string() const;
    std::string get_property_spec() const;
    bool is_set() const;

protected:
    const std::string m_name;
    const ValueType m_type;
    const Trait m_trait;
    bool m_is_set;
};

struct MULTY_TRANSACTION_API Properties
{
    typedef std::unique_ptr<Property> PropertyPtr;

    /** Set specific value to a bound property.
     *
     * May throw an exception if property is not found or property predicate
     * throws an exception.
     * @param name - name of the property to set.
     * @param value - new value of the property.
     */
    template <typename T>
    void set_property(const std::string& name, const T& value)
    {
        get_property(name).set_value(value);
    }

    void reset_property(const std::string& name);

    std::vector<const Property*> get_properties() const;

    /** Validate all bound properties.
     *
     * Checks all if required bound properties are set.
     * @param validation_details - optional, names of required properties that
     * was not set.
     * @return true if all required properties was set, false otherwise.
     */
    bool validate(std::vector<std::string>* validation_details) const;

    template <typename T>
    using Predicate = std::function<void(const T&)>;

    template <typename T>
    void bind_property(
            const std::string& name,
            T* value,
            Predicate<T> predicate)
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
            Amount* value,
            Property::Trait trait = Property::REQUIRED,
            Predicate<Amount> predicate = Predicate<Amount>());

    void bind_property(
            const std::string& name,
            wallet_core::internal::BinaryDataPtr* value,
            Property::Trait trait = Property::REQUIRED,
            Predicate<BinaryData> predicate = Predicate<BinaryData>());

    void bind_property(
            const std::string& name,
            wallet_core::internal::PublicKeyPtr* value,
            Property::Trait trait = Property::REQUIRED,
            Predicate<PublicKey> predicate = Predicate<PublicKey>());

private:
    template <typename T, typename P>
    void do_bind_property(
            const std::string& name,
            T* value,
            Property::Trait trait,
            P predicate);

protected:
    Property& get_property(const std::string& name);
    PropertyPtr& make_property(const std::string& name);

private:
    std::map<std::string, PropertyPtr> m_properties;
};

#endif // MULTY_TRANSACTION_INTERNAL_PROPERTIES_H
