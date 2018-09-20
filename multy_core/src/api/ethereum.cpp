/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/ethereum.h"
#include "multy_core/error.h"

#include "multy_core/src/account_base.h"
#include "multy_core/src/blockchain_facade_base.h"
#include "multy_core/src/codec.h"
#include "multy_core/src/hash.h"
#include "multy_core/src/api/key_impl.h"
#include "multy_core/src/utility.h"
#include "multy_core/src/u_ptr.h"

#include <memory>
#include <string>
#include <string.h>

namespace
{
using namespace multy_core::internal;

std::string ethereum_personal_sign_impl(
        const char* serialized_private_key,
        const char* hex_encoded_message)
{
    INVARIANT(serialized_private_key != nullptr);
    INVARIANT(hex_encoded_message != nullptr);

    // We can use any Ethereum net type, that makes no difference in signature.
    const BlockchainType blockchain_type{BLOCKCHAIN_ETHEREUM, ETHEREUM_CHAIN_ID_ETC_MAINNET};
    AccountPtr account = get_blockchain(blockchain_type).make_account(blockchain_type, 0, serialized_private_key);

    BinaryDataPtr decoded_message = decode(hex_encoded_message, strlen(hex_encoded_message), CODEC_HEX);
     // Padded from right with zeroes to allow more space for message length.
    const char format[] = "\u0019Ethereum Signed Message:\n%zd";
    char buffer[sizeof(format) + 10] = {'\0'};

    const auto f = snprintf(buffer, sizeof(buffer) - 1, format, decoded_message->len);
    if (f >= static_cast<int>(sizeof(buffer)) || f  < 0)
    {
        THROW_EXCEPTION2(ERROR_GENERAL_ERROR, "Failed to format message for Ethereum personal_sign.");
    }

    BinaryDataPtr message = new_binary_data(strlen(buffer) + decoded_message->len);
    memcpy(const_cast<unsigned char*>(message->data), buffer, strlen(buffer));
    memcpy(const_cast<unsigned char*>(message->data + strlen(buffer)), decoded_message->data, decoded_message->len);

    BinaryDataPtr signature = account->get_private_key()->sign(*message);
    const_cast<unsigned char*>(signature->data)[64] += 27;

    return encode(*signature, CODEC_HEX);
}

} // namespace


Error* ethereum_personal_sign(
        const char* serialized_private_key,
        const char* hex_encoded_message,
        char** signature)
{
    ARG_CHECK(serialized_private_key != nullptr);
    ARG_CHECK(hex_encoded_message != nullptr);
    ARG_CHECK(signature != nullptr);

    try
    {
        *signature = copy_string(ethereum_personal_sign_impl(serialized_private_key, hex_encoded_message));
    }
    CATCH_EXCEPTION_RETURN_ERROR(ERROR_SCOPE_ACCOUNT);

    OUT_CHECK(*signature != nullptr);

    return nullptr;
}
