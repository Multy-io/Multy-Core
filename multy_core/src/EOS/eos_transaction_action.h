/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_CORE_SRC_EOS_TRANSACTION_ACTION_H
#define MULTY_CORE_SRC_EOS_TRANSACTION_ACTION_H

#include "multy_core/src/u_ptr.h"

#include <string>
#include <stdint.h>

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

} // namespace internal
} // namespace multy_core


#endif // MULTY_CORE_SRC_EOS_TRANSACTION_ACTION_H
