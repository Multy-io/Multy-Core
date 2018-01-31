/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_CORE_PROPERTIES_H
#define MULTY_CORE_PROPERTIES_H

#include "multy_core/api.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct BigInt;
struct BinaryData;
struct PrivateKey;
struct Properties;

/** Properties system.
 *
 * In order to configure varios aspects of objects in library, we've desided to
 * utilize propery system. Properties object is a collection of name:value pairs.
 *  * Property is strongly typed and attempt to set value to type other that
 *    supported would fail.
 *  * Every property might have additional retrictions on type range and\or
 *    value, setting a property might fail if those restictions are not met.
 *  * Setting a property by non-existing key would fail.
 * In case of failure Error object is non-null and contains detailed
 * description, it must be disposed with free_error().
 */

/** Set property int value.
 *
 * Sets property value to a copy of given int.
 * @param properties - non null valid properties object.
 * @param name - name of the property to reset.
 * @param value - int to set as value.
 * @return null if no error, Error object otherwise.
 */
MULTY_CORE_API struct Error* properties_set_int32_value(
        struct Properties* properties, const char* name, int32_t value);

/** Set property string value.
 *
 * Sets property value to a copy of given string.
 * @param properties - non null valid properties object.
 * @param name - name of the property to reset.
 * @param value - string to set as value.
 * @return null if no error, Error object otherwise.
 */
MULTY_CORE_API struct Error* properties_set_string_value(
        struct Properties* properties, const char* name, const char* value);

/** Set property BigInt value.
 *
 * Sets property value to a copy of given BigInt.
 * @param properties - non null valid properties object.
 * @param name - name of the property to reset.
 * @param value - BigInt to set as value.
 * @return null if no error, Error object otherwise.
 */
MULTY_CORE_API struct Error* properties_set_big_int_value(
        struct Properties* properties,
        const char* name,
        const struct BigInt* value);

/** Set property BinaryData value.
 *
 * Sets property value to a copy of given binary data.
 * @param properties - non null valid properties object.
 * @param name - name of the property to reset.
 * @param value - BinaryData to set as value.
 * @return null if no error, Error object otherwise.
 */
MULTY_CORE_API struct Error* properties_set_binary_data_value(
        struct Properties* properties,
        const char* name,
        const struct BinaryData* value);

/** Set property PrivateKey value.
 *
 * Sets property value to a copy of given private key.
 * @param properties - non null valid properties object.
 * @param name - name of the property to reset.
 * @param value - private key to set as value.
 * @return null if no error, Error object otherwise.
 */
MULTY_CORE_API struct Error* properties_set_private_key_value(
        struct Properties* properties,
        const char* name,
        const struct PrivateKey* value);


/** Reset property value by key name.
 *
 * Resets given property by name.
 * @param properties - non null valid properties object.
 * @param name - name of the property to reset.
 * @return null if no error, Error object otherwise.
 */
MULTY_CORE_API struct Error* properties_reset_value(
        struct Properties* properties, const char* name);

/** Validate properties instance.
 *
 * Checks that all required properties are set.
 * @param properties - non null valid properties object.
 * @return null if no error, Error object otherwise.
 */
MULTY_CORE_API struct Error* properties_validate(
        const struct Properties* properties);

/** Get properties specification string.
 *
 * Allows client to introspect all properties keys and expected types.
 * @param properties - non null valid properties object.
 * @param out_specification - specification string, must be freed with free_string().
 * @return null if no error, Error object otherwise.
 */
MULTY_CORE_API struct Error* properties_get_specification(
        const struct Properties* properties, const char** out_specification);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* MULTY_CORE_PROPERTIES_H */
