/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_CORE_SRC_OBJECT_H
#define MULTY_CORE_SRC_OBJECT_H

#include "multy_core/api.h"

namespace multy_core
{
namespace internal
{

/** A Base class for all objects that leave boundaries of the multy_core.
 *
 * Provides object validity checks, some other facilities can be added later.
 */
class MULTY_CORE_API Object
{
    // In order to support CHECK_OBJECT, AGR_CHECK_OBJECT, OUT_CHECK_OBJECT,
    // derived class has to pass some custom value to the constructor of
    // Object() and implement function taht checks that current magic value
    // matches one object was initialized with:
    // bool is_valid() const;
protected:
    explicit Object(const void* magic);

    Object(const Object&);
    Object& operator=(const Object&);
    // TODO: add a free() function that does de-allocation of Object.
    // It is going to do so by invoking function provided by subclass.
    // Do not forget to make destructor virtual!
    // Rationale: some day we are to use shared-libraries plugins as
    // base for expanding  blockchain support. In order to avoid issues with
    // using different CRT instances/versions, all objects has to
    // be de-allocated within same library they were created.
    ~Object();

    const void* get_magic() const;

private:
    const void* m_magic;
};

/** Helper that simplifies creating Object subclasses.
 */
template <typename T>
class ObjectBase : public Object
{
public:
    // Returns false if this is null or was already destroyed.
    bool is_valid() const
    {
        /// (void*) is to suppress clang's warning about non-null `this`.
        return (void*)(this) && Object::get_magic() == get_type_magic();
    }

protected:
    ObjectBase() : Object(get_type_magic())
    {
    }

    ~ObjectBase()
    {
    }

    // This is static to enforce derived classes to have get_object_magic() static too.
    inline static const void* get_type_magic()
    {
        // get_object_magic() has to be static to avoid accessing potentially
        // dead or dereferencing null-ptr object through 'this'.
        return T::get_object_magic();
    }
};

/** To be used inside T::get_object_magic().
 * Returns some value, that should be unique for all subclass of Object.
 * NOTE that this MUST NOT be used in header files.
 */
#define RETURN_MAGIC()                                                         \
    do                                                                         \
    {                                                                          \
        static const int MULTY_CORE_MAGIC = __LINE__;                          \
        return &MULTY_CORE_MAGIC;                                              \
    } while (false)

} // namespace internal
} // namespace multy_core

#endif // MULTY_CORE_SRC_OBJECT_H
