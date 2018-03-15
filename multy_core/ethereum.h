/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_CORE_ETHEREUM_H
#define MULTY_CORE_ETHEREUM_H

#ifdef __cplusplus
extern "C" {
#endif

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

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* MULTY_CORE_ETHEREUM_H */
