/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_CORE_SRC_EOS_NAME_H
#define MULTY_CORE_SRC_EOS_NAME_H

#include <string>
#include <stdint.h>

namespace multy_core
{
namespace internal
{

// String name in EOS, packed 5-bytes per character into uint64_t, max 12 chars long.
class EosName
{
public:
    EosName();
    explicit EosName(const std::string& name);
    ~EosName();

    uint64_t get_data() const;
    std::string get_string() const;

    static EosName from_string(const std::string& string);
    static std::string to_string(const EosName& name);

private:
    // Non-const to make it copyable.
    uint64_t m_data;
    std::string m_data_string;
};

typedef EosName EosAddress;

} // namespace internal
} // namespace multy_core


#endif // MULTY_CORE_SRC_EOS_NAME_H
