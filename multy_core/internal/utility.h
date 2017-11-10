/* Copyright 2017 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_CORE_UTILITY_H
#define MULTY_CORE_UTILITY_H

/** Set of utility functions/classes/etc.
 * Not part of the interface, and should not be used by clients.
 */

#include "multy_core/error.h"
#include "multy_core/internal/u_ptr.h"

#include <memory>

struct Error;

#define ARG_CHECK(arg)                                                         \
    do                                                                         \
    {                                                                          \
        if (!(arg))                                                            \
        {                                                                      \
            return make_error(                                                 \
                    ERROR_INVALID_ARGUMENT, "Argument check failed: " #arg);   \
        }                                                                      \
    } while (false)

#define OUT_CHECK(arg)                                                         \
    do                                                                         \
    {                                                                          \
        if (!(arg))                                                            \
        {                                                                      \
            return make_error(                                                 \
                    ERROR_INTERNAL, "Failed to create output value: " #arg);   \
        }                                                                      \
    } while (false)

namespace wallet_core
{

namespace internal
{
inline void throw_if_error(Error* err)
{
    if (err)
    {
        throw err;
    }
}

void throw_if_wally_error(int err_code, const char* message);

template <typename T, size_t N>
constexpr size_t array_size(T (&)[N])
{
    return N;
}

template <typename T, size_t N>
constexpr size_t array_size(const std::array<T, N>&)
{
    return N;
}

/// Converts exception to a Error*, to be used inside a catch(...) block.
Error* exception_to_error();

/** Convenience function to copy a string.
 * @param str - string to copy, must not be null.
 * @return - copy of a string, must be freed with free_string(), can be null on
 * error.
 */
char* copy_string(const char* str);

char* copy_string(const std::string& str);

/** Convenience to simplify passing C++ smart_pointers (like std::unique_ptr<T>)
 * to C-like functions than take T** and store address of new object there.
 * Should be used in conjunction with reset_sp() function.
 * Resets value of smart pointer if it was requested to be converted to
 * pointer-to-pointer (T**)
 * and that value was modified during lifetime of the object.
 */
template <typename SP>
class UniquePointerUpdater
{
    typedef typename SP::pointer Pointer;
    SP& sp;
    mutable Pointer p;

public:
    // TODO: change from reference to pointer
    explicit UniquePointerUpdater(SP& sp) : sp(sp), p(sp.get())
    {
    }
    ~UniquePointerUpdater()
    {
        if (p != sp.get())
        {
            sp.reset(p);
        }
    }
    operator Pointer*() const
    {
        p = sp.get();
        return &p;
    }
};

/// See UniquePointerUpdater.
template <typename SP>
inline UniquePointerUpdater<SP> reset_sp(SP& sp)
{
    return UniquePointerUpdater<SP>(sp);
}

template <typename T>
UPtr<T> make_clone(const T& original)
{
    return UPtr<T>(new T(original));
}

} // namespace internal
} // namespace wallet_core

#endif // MULTY_CORE_UTILITY_H
