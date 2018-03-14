/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_TEST_SUPPORTED_BLOCKCHAINS_H
#define MULTY_TEST_SUPPORTED_BLOCKCHAINS_H

#include "multy_core/account.h"
#include "multy_core/src/bitcoin/bitcoin_account.h"
#include "multy_core/src/ethereum/ethereum_account.h"
#include "multy_core/src/golos/golos_account.h"

#include <vector>

const BlockchainType SUPPORTED_BLOCKCHAINS[] =
{
    { BLOCKCHAIN_BITCOIN, BITCOIN_NET_TYPE_MAINNET },
    { BLOCKCHAIN_BITCOIN, BITCOIN_NET_TYPE_TESTNET },
    { BLOCKCHAIN_ETHEREUM, ETHEREUM_CHAIN_ID_MAINNET },
    { BLOCKCHAIN_ETHEREUM, ETHEREUM_CHAIN_ID_RINKEBY },
    { BLOCKCHAIN_GOLOS, GOLOS_NET_TYPE_MAINNET },
    { BLOCKCHAIN_GOLOS, GOLOS_NET_TYPE_TESTNET }
};

const BlockchainType SUPPORTED_BLOCKCHAIN_TESTNETS[] =
{
    { BLOCKCHAIN_BITCOIN, BITCOIN_NET_TYPE_TESTNET },
    { BLOCKCHAIN_ETHEREUM, ETHEREUM_CHAIN_ID_RINKEBY },
    { BLOCKCHAIN_GOLOS, GOLOS_NET_TYPE_TESTNET }
};

#endif // MULTY_TEST_SUPPORTED_BLOCKCHAINS_H
