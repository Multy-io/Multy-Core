/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/src/EOS/eos_name.h"

#include "multy_core/EOS.h"

#include "multy_core/src/exception.h"
#include "multy_core/src/exception_stream.h"

#include <string>
#include <stdint.h>

namespace
{
uint8_t encode_symbol(char value)
{
    if (value == '.')
    {
        return 0;
    }

    if (value >= 'a' && value <= 'z')
    {
        return ((value - 'a') + 6);
    }

    if (value >= '1' && value <= '5')
    {
        return ((value - '1') + 1);
    }
    THROW_EXCEPTION2(ERROR_INVALID_ARGUMENT, "Unsupported symbol in EOS name.")
            << " Symbol: '" << static_cast<char>(value) << "'.";
}

uint64_t name_string_to_uint64(const std::string& name)
{
    if (name.empty())
    {
        return 0L;
    }

    const size_t size = name.size();
    uint64_t result = 0;

    if (size > EOS_ADDRESS_MAX_SIZE)
    {
        THROW_EXCEPTION2(ERROR_INVALID_ARGUMENT,
                "String is too big for EOS name.")
                << " Max length: " << EOS_ADDRESS_MAX_SIZE << ", "
                << " got length: " << size << ".";
    }

    for (size_t i = 0; i < size; i++)
    {
        uint64_t c = 0;
        c = encode_symbol(name[i]);

        // See https://github.com/OracleChain/chainkit/blob/master/chain/typename.cpp#L56
        c &= 0x1f;
        c <<= 64 - 5 * (i + 1);

        result |= c;
    }

    return result;
}

} // namespace

namespace multy_core
{
namespace internal
{

EosName::EosName()
    : m_data(0)
{
}

EosName::EosName(const std::string& name)
    : m_data(name_string_to_uint64(name))
{
}

EosName::~EosName()
{
}

uint64_t EosName::get_data() const
{
    return m_data;
}

std::string EosName::get_string() const
{
    THROW_EXCEPTION2(ERROR_FEATURE_NOT_IMPLEMENTED_YET, __FUNCTION__);
}

EosName EosName::from_string(const std::string& string)
{
    return EosName(string);
}

std::string EosName::to_string(const EosName& name)
{
    return name.get_string();
}

} // namespace internal
} // namespace multy_core
