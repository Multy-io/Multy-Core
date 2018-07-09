/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license
 *
 * See LICENSE for details
 */

#include "value_printers.h"

#include "multy_test/bip39_test_cases.h"
#include "multy_test/utility.h"

#include "multy_core/account.h"
#include "multy_core/common.h"
#include "multy_core/error.h"

#include "multy_core/src/account_base.h"
#include "multy_core/src/api/big_int_impl.h"
#include "multy_core/src/api/key_impl.h"
#include "multy_core/src/api/properties_impl.h"
#include "multy_core/src/api/transaction_impl.h"
#include "multy_core/src/api/transaction_builder_impl.h"
#include "multy_core/src/utility.h"

#include "wally_elements.h"

namespace
{
using namespace test_utility;
using namespace multy_core::internal;

std::ostream& operator<<(std::ostream& ostr, HDPath const& path)
{
    ostr << "m";
    for (const auto i : path)
    {
        ostr << "/" << i;
    }
    return ostr;
}

std::ostream& operator<<(std::ostream& ostr, Blockchain blockchain)
{
    ostr << to_string(blockchain);

    return ostr;
}

std::ostream& operator<<(std::ostream& ostr, const BlockchainType& blockchain_type)
{
    ostr << to_string(blockchain_type.blockchain);

    return ostr;
}

} // namespace

namespace std
{

void PrintTo(const std::nullptr_t&, std::ostream* out)
{
    *out << "<nullptr>";
}

} // namespace std

void PrintTo(const Error& e, std::ostream* out)
{
    *out << "Error{ " << e.code << ", \"" << e.message << "\" }";
    if (e.location.file)
    {
        *out << " @ " << e.location.file << " : " << e.location.line;
    }
    if (e.backtrace && strlen(e.backtrace) > 0)
    {
        *out << "\nBacktrace:\n" << e.backtrace;
    }
}

void PrintTo(const BinaryData& data, std::ostream* out)
{
    *out << "BinaryData{ " << (data.data ? to_hex(data) : std::string("<null>"))
         << ", " << data.len << " }";
}

void PrintTo(const Key& key, std::ostream* out)
{
    *out << "Key{ " << key.to_string() << " }";
}

void PrintTo(const ExtendedKey& key, std::ostream* out)
{
    *out << "ExtendedKey{" << key.to_string() << " }";
}

void PrintTo(const BIP39TestCase& e, std::ostream* out)
{
    *out << "BIP39TestCase{\n"
         << "\tenropy: " << e.entropy << ",\n"
         << "\tmnemonic: \"" << e.mnemonic << "\",\n"
         << "\tseed: " << e.seed << ",\n"
         << "\troot key: " << e.root_key << "\n"
         << "}";
}

void PrintTo(const Account& a, std::ostream* out)
{
    *out << "Account{\n"
         << "\tBlockchain: " << a.get_blockchain_type() << ",\n"
         << "\tpath: " << a.get_path() << "\n"
         << "}";
}

void PrintTo(const HDAccount& a, std::ostream* out)
{
    *out << "HDAccount{\n"
         << "\tBlockchain: " << a.get_blockchain_type() << ",\n"
         << "\tpath: " << a.get_path() << "\n"
         << "}";
}

void PrintTo(const Blockchain& blockchain, std::ostream* out)
{
    *out << blockchain;
}

void PrintTo(const BlockchainType& blockchain_type, std::ostream* out)
{
    *out << blockchain_type;
}

void PrintTo(const AddressType& a, std::ostream* out)
{
    switch (a)
    {
        case ADDRESS_EXTERNAL:
            *out << "ADDRESS_EXTERNAL";
            break;
        case ADDRESS_INTERNAL:
            *out << "ADDRESS_INTERNAL";
            break;
        default:
            *out << "unknown address " << a;
            break;
    }
}

void PrintTo(const Transaction& t, std::ostream* out)
{
    *out << "Transaction {" << t.get_blockchain_type() << "}";
}

void PrintTo(const TransactionBuilder& builder, std::ostream* out)
{
    *out << "TransactionBuilder { 0x" << static_cast<const void*>(&builder) << "}";
}

void PrintTo(const BigInt& a, std::ostream* out)
{
    *out << "BigInt {" << a.get_value() << "}";
}
