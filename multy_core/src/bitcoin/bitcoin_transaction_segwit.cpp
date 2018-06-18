/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/src/bitcoin/bitcoin_transaction_segwit.h"

#include "multy_core/src/bitcoin/bitcoin_account.h"
#include "multy_core/src/bitcoin/bitcoin_opcode.h"
#include "multy_core/src/bitcoin/bitcoin_stream.h"
#include "multy_core/src/bitcoin/bitcoin_transaction_base.h"
#include "multy_core/src/property_predicates.h"
#include "multy_core/src/api/key_impl.h"
#include "multy_core/src/utility.h"
#include "multy_core/src/codec.h"
#include "multy_core/src/hash.h"
#include "multy_core/binary_data.h"

#include "wally_crypto.h"

#include <array>

namespace {
using namespace multy_core::internal;

const unsigned char BITCOIN_LOCK_TIME[] = {0x00, 0x00, 0x00, 0x00};
const int32_t BITCOIN_SEGWIT_TRANSACTION_VERSION = 2;
const uint8_t SIZE_SCRIPT_PUBKEY_SOURCE_P2SH_P2WPKH = 0x19;
const uint32_t SIGHASH_ALL = 1;

} // namespace

namespace multy_core
{
namespace internal
{

BitcoinTransactionSegWitDestination::BitcoinTransactionSegWitDestination(BitcoinNetType net_type)
    : BitcoinTransactionDestinationBase(net_type)
{
}

BitcoinTransactionSegWitSource::BitcoinTransactionSegWitSource()
    : BitcoinTransactionSourceBase(),
      script_witness()
{
}

BitcoinTransactionSegWitSource::~BitcoinTransactionSegWitSource()
{
}

void BitcoinTransactionSegWitSource::sign(const hash<256>& hash_prevouts, const hash<256>& hash_sequence, const hash<256>& hash_outputs, const BinaryDataPtr& lock_time)
{
    // https://github.com/bitcoin/bips/blob/master/bip-0143.mediawiki

    const auto segwit_script_P2WPKH = do_hash<BITCOIN_HASH, 160>(private_key.get_value()->make_public_key()->get_content());
    BinaryData public_key_hash_data = as_binary_data(segwit_script_P2WPKH);

    BinaryDataPtr signature;
    {
        BitcoinDataStream sign_data;
        sign_data << BITCOIN_SEGWIT_TRANSACTION_VERSION;
        sign_data << as_binary_data(hash_prevouts);
        sign_data << as_binary_data(hash_sequence);
        sign_data << reverse(**prev_transaction_hash);
        sign_data << prev_transaction_out_index;
        sign_data << SIZE_SCRIPT_PUBKEY_SOURCE_P2SH_P2WPKH; // size script_pubkey in SegWit source
        sign_data << OP_DUP;
        sign_data << OP_HASH160;
        sign_data << as_compact_size(public_key_hash_data.len);
        sign_data << public_key_hash_data;
        sign_data << OP_EQUALVERIFY;
        sign_data << OP_CHECKSIG;
        sign_data << *amount;
        sign_data << *sequence;
        sign_data << as_binary_data(hash_outputs);
        sign_data << *lock_time;
        sign_data << SIGHASH_ALL; //signature version

        signature = private_key.get_value()->sign(sign_data.get_content());
    }

    BitcoinDataStream sig_script_witness_stream;
    sig_script_witness_stream << uint8_t(2); // elements count  (signature, public key)
    sig_script_witness_stream << as_compact_size(signature->len + 1); // one byte for write sighash type
    sig_script_witness_stream << *signature;
    sig_script_witness_stream << static_cast<uint8_t>(SIGHASH_ALL); // hash-code type

    const PublicKeyPtr& public_key = private_key.get_value()->make_public_key();
    const BinaryData& public_key_data = public_key->get_content();
    sig_script_witness_stream << as_compact_size(public_key_data.len);
    sig_script_witness_stream << public_key_data;

    script_witness = make_clone(sig_script_witness_stream.get_content());
}

BinaryDataPtr BitcoinTransactionSegWitSource::serialize_with_script_pub_key() const
{
    BitcoinDataStream result;

    result << reverse(**prev_transaction_hash);
    result << prev_transaction_out_index;

    BinaryDataPtr script_pubkey(make_script_sig());
    result << as_compact_size(script_pubkey->len);
    result << *script_pubkey;
    result << *sequence;

    return make_clone(result.get_content());
}

BinaryDataPtr BitcoinTransactionSegWitSource::preview_transaction() const
{
    BitcoinDataStream result;

    result << reverse(**prev_transaction_hash);
    result << prev_transaction_out_index;

    return make_clone(result.get_content());
}

BinaryDataPtr BitcoinTransactionSegWitSource::make_script_sig() const
{
    BitcoinDataStream result;

    const PublicKeyPtr& public_key = private_key.get_value()->make_public_key();
    std::array<uint8_t, HASH160_LEN + 2> segwit_script_P2WPKH;
    BinaryData public_key_hash_data = power_slice(as_binary_data(segwit_script_P2WPKH), 2, -2);
    segwit_script_P2WPKH[0] = OP_0; // Version byte witness
    segwit_script_P2WPKH[1] = HASH160_LEN; // Witness program is 20 bytes
    bitcoin_hash_160(public_key->get_content(), &public_key_hash_data);

    result << static_cast<uint8_t>(segwit_script_P2WPKH.size()); // size script to P2SH
    result << as_binary_data(segwit_script_P2WPKH);

    return make_clone(result.get_content());
}

BitcoinTransactionSegWit::BitcoinTransactionSegWit(BlockchainType blockchain_type)
    : BitcoinTransactionBase(blockchain_type),
      m_version(BITCOIN_SEGWIT_TRANSACTION_VERSION),
      lock_time(make_clone(BinaryData{BITCOIN_LOCK_TIME, 4}),
                    m_properties, "nlocktime", Property::OPTIONAL)
{
    register_properties("", m_fee->get_properties());
}

Properties& BitcoinTransactionSegWit::add_source()
{
    BitcoinTransactionSegWitSourcePtr source(new BitcoinTransactionSegWitSource());
    m_sources.emplace_back(std::move(source));

    return register_properties(
            make_id("#", m_sources.size() - 1),
            m_sources.back()->get_properties());
}

Properties& BitcoinTransactionSegWit::add_destination()
{
    BitcoinTransactionSegWitDestinationPtr destination(
            new BitcoinTransactionSegWitDestination(get_net_type()));
    m_destinations.emplace_back(std::move(destination));

    return register_properties(
            make_id("#", m_destinations.size() - 1),
            m_destinations.back()->get_properties());
}

void BitcoinTransactionSegWit::set_message(const BinaryData& /*value*/)
{
    THROW_EXCEPTION("Not implemented yet");
}

BigInt BitcoinTransactionSegWit::estimate_total_fee(size_t /*sources_count*/, size_t /*destinations_count*/) const
{
    THROW_EXCEPTION("Not implemented yet");
}

BigInt BitcoinTransactionSegWit::get_total_spent() const
{
    THROW_EXCEPTION("Not implemented yet");
}

void BitcoinTransactionSegWit::update()
{
    this->verify();
    sign();

}

BinaryDataPtr BitcoinTransactionSegWit::serialize()
{
    update();

    BitcoinDataStream data_stream;
    data_stream << m_version;
    data_stream << static_cast<uint8_t>(0x00); // marker
    data_stream << static_cast<uint8_t>(0x01); // flag
    data_stream << as_compact_size(m_sources.size());
    for (const auto& source : m_sources)
    {
        data_stream << *(static_cast<const BitcoinTransactionSegWitSource*>(source.get())->serialize_with_script_pub_key());
    }
    data_stream << as_compact_size(m_destinations.size());
    for (const auto& destination : m_destinations)
    {
        data_stream << *destination;
    }

    for (const auto& source : m_sources)
    {
        data_stream << *(static_cast<const BitcoinTransactionSegWitSource*>(source.get())->script_witness);
    }

    data_stream << **lock_time;

    return make_clone(data_stream.get_content());
}

BitcoinNetType BitcoinTransactionSegWit::get_net_type() const
{
    return static_cast<BitcoinNetType>(get_blockchain_type().net_type);
}

void BitcoinTransactionSegWit::sign()
{
    for (auto& source : m_sources)
    {
        source->script_signature.reset();
    }

    BitcoinDataStream preview_inputs;
    for (const auto& source : m_sources)
    {
        preview_inputs << *(static_cast<const BitcoinTransactionSegWitSource*>(source.get())->preview_transaction());
    }
    BitcoinDataStream preview_outputs;
    for (const auto& destination : m_destinations)
    {
        preview_outputs << *destination;
    }

    BitcoinDataStream data;
    for (const auto& source : m_sources)
    {
        data << *source->sequence;
    }


    hash<256> hash_prevouts = do_hash<SHA2_DOUBLE, 256>(preview_inputs.get_content());
    hash<256> hash_sequence = do_hash<SHA2_DOUBLE, 256>(data.get_content());
    hash<256> hash_outpusts = do_hash<SHA2_DOUBLE, 256>(preview_outputs.get_content());

    for (auto& source : m_sources)
    {
        static_cast<BitcoinTransactionSegWitSource*>(
                source.get())->sign(hash_prevouts, hash_sequence, hash_outpusts, *lock_time);
    }
}

void BitcoinTransactionSegWit::verify() const
{
    BitcoinTransactionBase::verify();

    size_t source_index = 0;
    for (const auto& s: m_sources)
    {
        const auto& public_key = s->private_key->make_public_key();
        BinaryData public_key_hash_data;
        std::array<uint8_t, HASH160_LEN> public_key_hash;
        public_key_hash_data = as_binary_data(public_key_hash);
        unsigned char segwit_script[HASH160_LEN+2] = {'\0'};

        BinaryData hash_pub_key = power_slice(as_binary_data(segwit_script), 2, -2);

        bitcoin_hash_160(public_key->get_content(), &hash_pub_key);
        segwit_script[0] = 0x00;
        segwit_script[1] = HASH160_LEN;
        bitcoin_hash_160(as_binary_data(segwit_script), &public_key_hash_data);

        BinaryDataPtr sig_script = make_script_pub_key(public_key_hash_data, BITCOIN_ADDRESS_P2SH);

        if (*sig_script != **s->prev_transaction_out_script_pubkey)
        {
            AccountPtr error_account = make_bitcoin_account(
                        s->private_key->to_string().c_str(),
                        BITCOIN_ACCOUNT_SEGWIT);

            THROW_EXCEPTION2(ERROR_TRANSACTION_INVALID_PRIVATE_KEY,
                    "Source can't be spent using given private key.")
                    << " Source index: " << source_index
                    << ", corresponding address: "<< error_account->get_address()
                    << ", given prev_tx_out_script_pubkey: "
                    << encode(**s->prev_transaction_out_script_pubkey, CODEC_HEX)
                    << ".";
        }
        ++source_index;
    }
}


} // namespace internal
} // namespace multy_core
