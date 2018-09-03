/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/src/eos/eos_transaction_transfer_action.h"

#include "multy_core/eos.h"

#include "multy_core/src/utility.h"
#include "multy_core/src/eos/eos_binary_stream.h"

namespace multy_core
{
namespace internal
{


EosTransactionAction::ActionType EosTransactionTransferAction::get_type() const
{
    return TRANSFER;
}

EosTransactionTransferAction::EosTransactionTransferAction(
        const std::string& from,
        const std::string& to,
        const BigInt amount,
        const BinaryData& memo)
    : m_from(from),
      m_to(to),
      m_amount(std::move(amount)),
      m_memo()
{
    m_memo = make_clone(memo);
    m_authorizations.push_back(EosAuthorization(from, "active"));
}

void EosTransactionTransferAction::write_to_stream(EosBinaryStream* stream) const
{
    *stream << EosName("eosio.token");
    *stream << EosName("transfer");
    *stream << static_cast<uint8_t>(m_authorizations.size());
    for (const auto& authorization: m_authorizations)
    {
        *stream << authorization;
    }
    const auto& data = make_data();
    *stream << static_cast<uint8_t>(data->len);
    *stream << *data;
}

BinaryDataPtr EosTransactionTransferAction::make_data() const
{
    EosBinaryStream data;
    data << m_from;
    data << m_to;
    data << m_amount.get_value_as_uint64();
    data << EOS_PRECISION;
    data << as_binary_data(EOS_TOKEN_NAME);
    data << static_cast<uint8_t>(m_memo->len);
    if (m_memo->len > 0)
    {
        data << *m_memo;
    }

    return make_clone(data.get_content());
}

} // namespace internal
} // namespace multy_core

