/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_TEST_VALUE_PRINTERS_H
#define MULTY_TEST_VALUE_PRINTERS_H

#include "multy_core/account.h"

#include <memory>
#include <ostream>

struct Account;
struct BigInt;
struct BinaryData;
struct BIP39TestCase;
struct Error;
struct Key;
struct Transaction;
struct TransactionBuilder;

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
void PrintTo(const Blockchain& e, std::ostream* out);
void PrintTo(const BlockchainType& e, std::ostream* out);
void PrintTo(const AddressType& e, std::ostream* out);
void PrintTo(const Transaction& e, std::ostream* out);
void PrintTo(const TransactionBuilder& e, std::ostream* out);
void PrintTo(const BigInt& a, std::ostream* out);

inline std::ostream& operator<<(std::ostream& out, const BinaryData& value)
{
    PrintTo(value, &out);
    return out;
}

inline std::ostream& operator<<(std::ostream& out, const Error& e)
{
    PrintTo(e, &out);

    return out;
}

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

std::ostream& operator<<(std::ostream& ostr, Blockchain blockchain);
std::ostream& operator<<(std::ostream& ostr, const BlockchainType& blockchain_type);

#endif // MULTY_TEST_VALUE_PRINTERS_H
