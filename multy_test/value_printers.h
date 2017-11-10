/* Copyright 2017 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef VALUE_PRINTERS_H
#define VALUE_PRINTERS_H

#include "multy_core/account.h"

#include <memory>
#include <ostream>

struct Account;
struct BinaryData;
struct BIP39TestCase;
struct Error;
struct Key;

namespace std
{
void PrintTo(const std::nullptr_t&, std::ostream*);
} // namespace std;

void PrintTo(const Error&, std::ostream*);
void PrintTo(const BinaryData&, std::ostream*);
void PrintTo(const Key&, std::ostream*);
void PrintTo(const ExtendedKey&, std::ostream*);
void PrintTo(const BIP39TestCase& e, std::ostream* out);
void PrintTo(const Account& e, std::ostream* out);
void PrintTo(const HDAccount& e, std::ostream* out);
void PrintTo(const Currency& e, std::ostream* out);
void PrintTo(const AddressType& e, std::ostream* out);

template <typename T, typename D>
inline void PrintTo(const std::unique_ptr<T, D>& up, std::ostream* out)
{
    if (up)
    {
        PrintTo(*up, out);
    }
    else
    {
        PrintTo(nullptr, out);
    }
}

#endif // VALUE_PRINTERS_H
