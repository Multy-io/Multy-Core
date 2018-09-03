/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_CORE_SRC_EOS_TRANSACTION_ACTION_H
#define MULTY_CORE_SRC_EOS_TRANSACTION_ACTION_H

#include "multy_core/src/u_ptr.h"
#include "multy_core/src/eos/eos_name.h"
#include "multy_core/src/api/big_int_impl.h"

#include <string>
#include <stdint.h>
#include <vector>

namespace multy_core
{
namespace internal
{
class EosBinaryStream;

class EosTransactionAction
{
public:
    virtual ~EosTransactionAction();

    enum ActionType
    {
        TRANSFER
    };

    virtual ActionType get_type() const = 0;
    virtual void write_to_stream(EosBinaryStream* /*stream*/) const = 0;
    virtual BinaryDataPtr make_data() const = 0;

    std::string get_type_name() const;
};

class EosAuthorization
{
public:
    EosAuthorization(const std::string& actor, const std::string& permission);

    EosName get_actor() const;
    EosName get_permission() const;

private:
    const EosName m_actor;
    const EosName m_permission;
};


} // namespace internal
} // namespace multy_core


#endif // MULTY_CORE_SRC_EOS_TRANSACTION_ACTION_H
