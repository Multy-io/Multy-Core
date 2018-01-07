/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_CORE_INTERNAL_UNIVERSAL_DELETER_H
#define MULTY_CORE_INTERNAL_UNIVERSAL_DELETER_H

#include "multy_core/api.h"

#include <memory>

struct Account;
struct BinaryData;
struct Error;
struct ExtendedKey;
struct HDAccount;
struct Key;
struct PrivateKey;
struct PublicKey;

namespace wallet_core
{
namespace internal
{
class UniversalDeleter;

template <typename T>
using UPtr = std::unique_ptr<T, UniversalDeleter>;

/// Deletes every X with corresponding multy_core public API call: free_X().
class MULTY_CORE_API UniversalDeleter
{
public:
    void operator()(char*) const;
    void operator()(const char*) const;
    void operator()(Account*) const;
    void operator()(BinaryData*) const;
    void operator()(Error*) const;
    void operator()(ExtendedKey*) const;
    void operator()(HDAccount*) const;
    void operator()(Key*) const;
    void operator()(PublicKey*) const;
    void operator()(PrivateKey*) const;
};

typedef UPtr<Account> AccountPtr;
typedef UPtr<BinaryData> BinaryDataPtr;
typedef UPtr<char> CharPtr;
typedef UPtr<const char> ConstCharPtr;
typedef UPtr<Error> ErrorPtr;
typedef UPtr<ExtendedKey> ExtendedKeyPtr;
typedef UPtr<HDAccount> HDAccountPtr;
typedef UPtr<Key> KeyPtr;
typedef UPtr<PrivateKey> PrivateKeyPtr;
typedef UPtr<PublicKey> PublicKeyPtr;

} // namespace wallet_core
} // namespace internal

#endif // MULTY_CORE_INTERNAL_UNIVERSAL_DELETER_H
