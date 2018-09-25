/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_CORE_SRC_EOS_TRANSACTION_TRANSFER_ACTION_H
#define MULTY_CORE_SRC_EOS_TRANSACTION_TRANSFER_ACTION_H

#include "multy_core/src/eos/eos_transaction_action.h"
#include "multy_core/src/u_ptr.h"

namespace multy_core
{
namespace internal
{

class EosBinaryStream;
class EosTransactionTransferAction;


class EosTransactionTransferAction : public EosTransactionAction
{
public:
    EosTransactionTransferAction(
            const std::string& from,
            const std::string& to,
            const BigInt amount,
            const BinaryData& memo);

    ActionType get_type() const override;

    void write_to_stream(EosBinaryStream* stream) const override;

    BinaryDataPtr make_data() const override;
public:
    EosName m_from;
    EosName m_to;
    BigInt m_amount;
    BinaryDataPtr m_memo;
    std::vector<EosAuthorization> m_authorizations;
};


} // namespace internal
} // namespace multy_core


#endif // MULTY_CORE_SRC_EOS_TRANSACTION_TRANSFER_ACTION_H
