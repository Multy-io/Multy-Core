/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/src/api/json_api_impl.h"

#include "multy_core/blockchain.h"
#include "multy_core/src/blockchain_facade_base.h"
#include "multy_core/src/api/account_impl.h"
#include "multy_core/src/api/properties_impl.h"
#include "multy_core/src/api/transaction_impl.h"
#include "multy_core/src/api/transaction_builder_impl.h"

#include "multy_core/src/codec.h"
#include "multy_core/src/exception.h"
#include "multy_core/src/exception_stream.h"
#include "multy_core/src/json_helpers.h"
#include "multy_core/src/utility.h"

#include <json/json.h>
#include <string.h>
#include <sstream>

namespace
{
using namespace multy_core::internal;

void from_json(const Json::Value& value, const BlockchainType&, int32_t* int_value)
{
    INVARIANT(int_value != nullptr);

    *int_value = value.asInt();
}

void from_json(const Json::Value& value, const BlockchainType&, std::string* string_value)
{
    INVARIANT(string_value != nullptr);

    *string_value = value.asString();
}

void from_json(const Json::Value& value, const BlockchainType&, BigInt* big_int_value)
{
    INVARIANT(big_int_value != nullptr);

    if (value.isInt64())
    {
        *big_int_value = BigInt(value.asInt64());
    }
    else if (value.isUInt64())
    {
        *big_int_value = BigInt(value.asUInt64());
    }
    else if (value.type() == Json::stringValue)
    {
        big_int_value->set_value(value.asCString());
    }
    else
    {
        THROW_EXCEPTION2(ERROR_INVALID_ARGUMENT, "Invalid JSON value type for BigInt value.");
    }
}

void from_json(const Json::Value& value, const BlockchainType&, BinaryDataPtr* binary_data)
{
    INVARIANT(binary_data != nullptr);

    const CodecType codec = CODEC_HEX;
    const char* encoded = value.asCString();

    struct
    {
        const char* str;
        const size_t length;
        const CodecType codec;
    } prefixes[] = {
        {"hex:",    4, CODEC_HEX},
        {"base32:", 7, CODEC_BASE32},
        {"base58:", 7, CODEC_BASE58},
    };

    const CodecType* codec_ptr = &codec;
    for (const auto& prefix : prefixes)
    {
        if (strncmp(encoded, prefix.str, prefix.length) == 0)
        {
            codec_ptr = &prefix.codec;
            encoded += prefix.length;
            break;
        }
    }

    decode(encoded, *codec_ptr).swap(*binary_data);
}

void from_json(const Json::Value& value, const BlockchainType& blockchain_type, PrivateKeyPtr* private_key)
{
    INVARIANT(private_key != nullptr);

    const char* serialized_private_key = value.asCString();
    const auto account_type = ACCOUNT_TYPE_DEFAULT;

    const auto& blockchain_facade = get_blockchain(blockchain_type.blockchain);
    const auto& account = blockchain_facade.make_account(
                blockchain_type, account_type, serialized_private_key);
    account->get_private_key().swap(*private_key);
}

template <typename T>
const T& to_binder_argument(const T& argument)
{
    return argument;
}

template <typename T, typename D>
const T& to_binder_argument(const std::unique_ptr<T, D>& argument)
{
    INVARIANT(argument.get() != nullptr);

    return *argument;
}

template <typename T>
void set_value(const Json::Value& value, const BlockchainType& blockchain_type, Properties::Binder* binder)
{
    INVARIANT(binder != nullptr);

    T read_value;
    from_json(value, blockchain_type, &read_value);
    binder->set_value(to_binder_argument(read_value));

    INVARIANT(binder->is_set());
}

void set_properties(const Json::Value& values, const BlockchainType& blockchain_type, Properties* properties)
{
    INVARIANT(properties != nullptr);

    for (auto i = values.begin(), e = values.end(); i != e; ++i)
    {
        try
        {
            switch (i->type())
            {
                case Json::arrayValue:
                case Json::objectValue:
                case Json::realValue:
                case Json::nullValue:
                {
                    THROW_EXCEPTION2(ERROR_INVALID_ARGUMENT,
                            "Unsupported Json value type.");
                }
                default:
                    break;
            }
            Properties::Binder& binder = properties->get_property(i.name());
            switch (binder.get_value_type())
            {
                case Properties::Binder::VALUE_TYPE_INT32:
                    set_value<int32_t>(*i, blockchain_type, &binder);
                    break;
                case Properties::Binder::VALUE_TYPE_BIG_INT:
                    set_value<BigInt>(*i, blockchain_type, &binder);
                    break;
                case Properties::Binder::VALUE_TYPE_STRING:
                    set_value<std::string>(*i, blockchain_type, &binder);
                    break;
                case Properties::Binder::VALUE_TYPE_BINARY_DATA:
                    set_value<BinaryDataPtr>(*i, blockchain_type, &binder);
                    break;
                case Properties::Binder::VALUE_TYPE_PRIVATE_KEY:
                    set_value<PrivateKeyPtr>(*i, blockchain_type, &binder);
                    break;
                default:
                    INVARIANT(false && "Unhandled Property type.");
            }
        }
        catch(const Exception& e)
        {
            e << " Setting properties to: " << properties->get_name() << ", "
                    << " Value name:" << i.name() << ", type: " << i->type();
            throw;
        }
        catch(const std::exception& e)
        {
            std::stringstream message;
            message << e.what()
                    << " Setting properties to: " << properties->get_name() << ", "
                    << " Value name:" << i.name() << ", type: " << i->type() << ".";

            THROW_EXCEPTION2(ERROR_INVALID_ARGUMENT, "Invalid input JSON.")
                    << message.str();
        }
    }
}

} // namespace

namespace multy_core
{
namespace internal
{

std::string make_transaction_from_json(const std::string& json_string)
{
    const Json::Value root = parse_json(json_string);

    const auto blockchain_type = BlockchainType
    {
        from_string<Blockchain>(root["blockchain"].asCString()),
        static_cast<size_t>(root["net_type"].asUInt())
    };

    const auto& facade = get_blockchain(blockchain_type);

    const auto& account_json = root["account"];
    AccountPtr account = facade.make_account(blockchain_type,
            account_json["type"].asUInt(),
            account_json["private_key"].asCString());

    const auto& builder_json = root["builder"];
    TransactionBuilderPtr builder = facade.make_transaction_builder(
            *account,
            builder_json["type"].asUInt(),
            builder_json.get("action", std::string()).asCString());

    // stuff builder with properties:
    set_properties(builder_json["payload"], blockchain_type, &builder->get_properties());

    TransactionPtr transaction = builder->make_transaction();
    if (auto tx_json = root["transaction"])
    {
        if (const auto& fee_json = tx_json["fee"])
        {
            set_properties(fee_json, blockchain_type, &transaction->get_fee());
        }
        tx_json.removeMember("fee");
        set_properties(tx_json, blockchain_type, &transaction->get_transaction_properties());
    }

    return R"json({"transaction":{"serialized":")json" + facade.encode_serialized_transaction(transaction.get()) + "\"}}";
}

} // namespace internal
} // namespace multy_core

