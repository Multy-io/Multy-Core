/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/src/object.h"

namespace multy_core
{
namespace internal
{

Object::Object(const void* magic)
    : m_magic(magic)
{
}

Object::Object(const Object& other)
    : m_magic(other.m_magic)
{
}

Object& Object::operator=(const Object& other)
{
    m_magic = other.m_magic;
    return *this;
}

Object::~Object()
{
    m_magic = nullptr;
}

const void* Object::get_magic() const
{
    return m_magic;
}

} // namespace internal
} // namespace multy_core
