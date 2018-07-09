/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/properties.h"

#include "multy_core/src/api/properties_impl.h"
#include "multy_core/src/utility.h"

Error* properties_set_int32_value(
        Properties* properties, const char* name, int32_t value)
{
    ARG_CHECK_OBJECT(properties);
    ARG_CHECK(name);
    try
    {
        properties->set_property_value(name, value);
    }
    CATCH_EXCEPTION_RETURN_ERROR(ERROR_SCOPE_GENERIC);

    return nullptr;
}

Error* properties_set_string_value(
        Properties* properties, const char* name, const char* value)
{
    ARG_CHECK_OBJECT(properties);
    ARG_CHECK(name);
    ARG_CHECK(value);
    try
    {
        properties->set_property_value(name, value);
    }
    CATCH_EXCEPTION_RETURN_ERROR(ERROR_SCOPE_GENERIC);

    return nullptr;
}

Error* properties_set_big_int_value(
        Properties* properties, const char* name, const BigInt* value)
{
    ARG_CHECK_OBJECT(properties);
    ARG_CHECK(name);
    ARG_CHECK(value);
    try
    {
        properties->set_property_value(name, *value);
    }
    CATCH_EXCEPTION_RETURN_ERROR(ERROR_SCOPE_GENERIC);

    return nullptr;
}

Error* properties_set_binary_data_value(
        Properties* properties, const char* name, const BinaryData* value)
{
    ARG_CHECK_OBJECT(properties);
    ARG_CHECK(name);
    ARG_CHECK(value);
    try
    {
        properties->set_property_value(name, *value);
    }
    CATCH_EXCEPTION_RETURN_ERROR(ERROR_SCOPE_GENERIC);

    return nullptr;
}

Error* properties_set_private_key_value(
        Properties* properties, const char* name, const PrivateKey* value)
{
    ARG_CHECK_OBJECT(properties);
    ARG_CHECK(name);
    ARG_CHECK(value);
    try
    {
        properties->set_property_value(name, *value);
    }
    CATCH_EXCEPTION_RETURN_ERROR(ERROR_SCOPE_GENERIC);

    return nullptr;
}

Error* properties_reset_value(Properties* properties, const char* name)
{
    ARG_CHECK_OBJECT(properties);
    ARG_CHECK(name);
    try
    {
        properties->reset_property(name);
    }
    CATCH_EXCEPTION_RETURN_ERROR(ERROR_SCOPE_GENERIC);

    return nullptr;
}

Error* properties_validate(const Properties* properties)
{
    ARG_CHECK_OBJECT(properties);
    try
    {
        properties->validate(MULTY_CODE_LOCATION);
    }
    CATCH_EXCEPTION_RETURN_ERROR(ERROR_SCOPE_GENERIC);

    return nullptr;
}

Error* properties_get_specification(const Properties* properties, const char** out_specification)
{
    ARG_CHECK_OBJECT(properties);
    ARG_CHECK(out_specification);
    try
    {
        *out_specification = multy_core::internal::copy_string(properties->get_specification());
    }
    CATCH_EXCEPTION_RETURN_ERROR(ERROR_SCOPE_GENERIC);
    OUT_CHECK(out_specification);

    return nullptr;
}
