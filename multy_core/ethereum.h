/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_CORE_ETHEREUM_H
#define MULTY_CORE_ETHEREUM_H

#include "multy_core/api.h"
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

struct Error;

enum EthereumChainId
{
    // Default chain id value from Ethereum sources.
    ETHEREUM_CHAIN_ID_PRE_EIP155 = -4,

    // https://github.com/ethereum/EIPs/blob/master/EIPS/eip-155.md
    ETHEREUM_CHAIN_ID_MAINNET = 1, // Ethereum mainnet
    ETHEREUM_CHAIN_ID_MORDEN = 2, // 	Morden (disused), Expanse mainnet
    ETHEREUM_CHAIN_ID_ROPSTEN = 3, // 	Ropsten
    ETHEREUM_CHAIN_ID_RINKEBY = 4, // 	Rinkeby
    ETHEREUM_CHAIN_ID_ROOTSTOCK_MAINNET = 30, // 	Rootstock mainnet
    ETHEREUM_CHAIN_ID_ROOTSTOCK_TESTNET = 31, // 	Rootstock testnet
    ETHEREUM_CHAIN_ID_KOVAN = 42, // 	Kovan
    ETHEREUM_CHAIN_ID_ETC_MAINNET = 61, // 	Ethereum Classic mainnet
    ETHEREUM_CHAIN_ID_ETC_TESTNET = 62, //	Ethereum Classic testnet
};

enum EthereumTransactionBuilderType
{
//    ETHEREUM_TRANSACTION_BUILDER_ERC20,
    ETHEREUM_TRANSACTION_BUILDER_MULTISIG,
};

const size_t ETHEREUM_BINARY_ADDRESS_SIZE = 20;

/** The sign method calculates an Ethereum specific signature
 * compatitable with geth/partity 'personal_sign' and web3.js.

 @param serialized_private_key - serialized Ethereum private key.
 @param hex_encoded_message - hex-encoded message to sign (may contain 0x prefix).
 @param signature - OUT hex-encoded signature, with recovery_id set to 27 or 28.
 @return nullptr if no error Error object otherwise.
*/
MULTY_CORE_API struct Error* ethereum_personal_sign(
        const char* serialized_private_key,
        const char* hex_encoded_message,
        char** signature);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* MULTY_CORE_ETHEREUM_H */
