/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_CORE_BITCOIN_H
#define MULTY_CORE_BITCOIN_H

#ifdef __cplusplus
extern "C" {
#endif

enum BitcoinNetType
{
    BITCOIN_NET_TYPE_MAINNET = 0,
    BITCOIN_NET_TYPE_TESTNET = 1,
};

enum BitcoinAddressType
{
    BITCOIN_ADDRESS_P2PKH = 0,
    BITCOIN_ADDRESS_P2SH = 1,
    BITCOIN_ADDRESS_P2SH_P2WPKH = 2
};

enum BitcoinAccountType
{
    BITCOIN_ACCOUNT_DEFAULT = 0,
    BITCOIN_ACCOUNT_P2PKH = BITCOIN_ACCOUNT_DEFAULT,
    BITCOIN_ACCOUNT_SEGWIT = 1,
};

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* MULTY_CORE_BITCOIN_H */
