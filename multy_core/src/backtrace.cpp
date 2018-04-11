/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/src/backtrace.h"

#include <algorithm>
#include <memory>
#include <sstream>

namespace
{

const int MAX_STACK_DEPTH = 10;

} // namespace

#ifdef MULTY_BUILD_FOR_ANDROID
// this entire block is based on:
// https://stackoverflow.com/a/35585744
// And probably belongs to Public Domain, since there is no explicit mentioning of license.

#include <unwind.h>
#include <dlfcn.h>
#include <cxxabi.h>

namespace
{
struct android_backtrace_state
{
    void **current;
    void **end;
};

_Unwind_Reason_Code android_unwind_callback(struct _Unwind_Context* context,
        void* arg)
{
    android_backtrace_state* state = reinterpret_cast<android_backtrace_state*>(arg);
    uintptr_t pc = _Unwind_GetIP(context);
    if (pc)
    {
        if (state->current == state->end)
        {
            return _URC_END_OF_STACK;
        }
        else
        {
            *state->current++ = reinterpret_cast<void*>(pc);
        }
    }

    return _URC_NO_REASON;
}

std::string get_platform_backtrace(size_t skip_frames)
{
    std::stringstream sstr;

    const int max = MAX_STACK_DEPTH * 10;
    void* buffer[max];

    android_backtrace_state state;
    state.current = buffer;
    state.end = buffer + max;

    _Unwind_Backtrace(android_unwind_callback, &state);

    int count = (int)(state.current - buffer);
    const int last_frame = std::min(count, static_cast<int>(MAX_STACK_DEPTH + skip_frames));
    for (int idx = skip_frames; idx < last_frame; ++idx)
    {
        const void* addr = buffer[idx];
        const char* symbol = "";

        Dl_info info;
        if (dladdr(addr, &info) && info.dli_sname)
        {
            symbol = info.dli_sname;
        }
        int status = 0;
        std::unique_ptr<char, decltype(free)*> demangled(
                __cxxabiv1::__cxa_demangle(symbol, 0, 0, &status), &free);

        sstr << idx << ": 0x" << addr << " "
                << ((demangled && status != 0) ? demangled.get() : symbol)
                << '\n';
    }

    return sstr.str();
}
} // namespace

#elif defined(MULTY_BUILD_FOR_IOS) || defined(MULTY_BUILD_FOR_OSX) || defined(MULTY_BUILD_FOR_UNIX)
// This entire block is based on `man backtrace`
#include <execinfo.h>

namespace
{

std::string get_platform_backtrace(size_t skip_frames)
{
    std::stringstream sstr;

    const int max = MAX_STACK_DEPTH * 10;
    void* callstack[max];
    const int frames = backtrace(callstack, max);
    std::unique_ptr<char*[], decltype(free)*> strs(
            backtrace_symbols(callstack, frames),
            &free);

    const int last_frame = std::min(frames, static_cast<int>(MAX_STACK_DEPTH + skip_frames));
    for (int i = skip_frames; i < last_frame; ++i)
    {
        sstr << strs[i] << '\n';
    }

    return sstr.str();
}

} // namespace

#else
std::string get_platform_backtrace(size_t /*skip_frames*/)
{
    (void)(MAX_STACK_DEPTH);
    static_assert(false, "backtrace not supported.");
}
#endif

std::string get_backtrace(size_t skip_frames)
{
    // +1 is for get_backtrace()
    // +1 is for get_platform_backtrace()
    // + 1 is for native backtrace taking function.
    const size_t new_skip_frames =
            std::min(static_cast<int>(skip_frames + 3), MAX_STACK_DEPTH - 1);

    return get_platform_backtrace(new_skip_frames);
}
