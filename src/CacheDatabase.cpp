#include "CacheDatabase.h"

namespace BlockchainIndexer
{

CacheDatabase::CacheDatabase()
    : highestHeight(0)
    , blockCache()
    , blockMutex()
    , blockHeightCache()
    , blockHeightMutex()
    , transactionCache()
    , transactionMutex()
    , addressTransactionsCache()
    , addressTransactionsMutex()
{
}

CacheDatabase::~CacheDatabase()
{
}

void CacheDatabase::cacheBlock(Block& aBlock)
{
    {
        // cache block with block hash as key
        std::lock_guard<std::mutex> tLock(blockMutex);
        blockCache[aBlock.blockHash] = aBlock;
    }

    {
        // cache block hash with block height as key
        std::lock_guard<std::mutex> tLock(blockHeightMutex);
        highestHeight = std::max(highestHeight, aBlock.height); 
        blockHeightCache[aBlock.height] = aBlock.blockHash;
    }

    {
        // cache transactions with transaction ID as key
        std::lock_guard<std::mutex> tLock(transactionMutex);
        for (auto& tx : aBlock.transactions)
        {
            transactionCache[tx.id] = tx;
            cacheAddressTransaction(tx);
        }
    }
}

void CacheDatabase::cacheAddressTransaction(Transaction& aTransaction)
{
    std::lock_guard<std::mutex> tLock(addressTransactionsMutex);

    for (auto& tx : aTransaction.inputs)
    {
        if (tx.coinbase)
        {
            continue;
        }

        // get address of spent transaction input
        std::string address = "";
        if (transactionCache.find(tx.txOutputId) != transactionCache.end())
        {
            address = transactionCache[tx.txOutputId].outputs[tx.txOutputIdx].scriptPubKeyAddress;
        }

        // remove transaction from address transaction container
        if (addressTransactionsCache.find(address) != addressTransactionsCache.end())
        {
            for (int i = 0; i < addressTransactionsCache[address].size(); ++i)
            {
                auto& txo = addressTransactionsCache[address][i];
                if (txo.txOutputId == tx.txOutputId && txo.txOutputIdx == tx.txOutputIdx)
                {
                    addressTransactionsCache[address].erase(addressTransactionsCache[address].begin() + i);
                    break;
                }
            }
        }
    }

    for (auto& tx : aTransaction.outputs)
    {        
        if (tx.scriptPubKeyType != "nulldata")
        {
            if (addressTransactionsCache.find(tx.scriptPubKeyAddress) == addressTransactionsCache.end())
            {
                addressTransactionsCache[tx.scriptPubKeyAddress] = std::vector<TransactionOutput>();
            }

            addressTransactionsCache[tx.scriptPubKeyAddress].push_back(tx);
        }
    }
}

bool CacheDatabase::getBlock(std::string aHash, Block& aBlock)
{
    std::lock_guard<std::mutex> tLock(blockMutex);
    if (blockCache.find(aHash) == blockCache.end())
    {
        return false;
    }

    aBlock = blockCache[aHash];
    return true;
}

bool CacheDatabase::getBlockWithMaxHeight(Block& aBlock)
{
    return getBlockWithHeight(highestHeight, aBlock);
}

bool CacheDatabase::getBlockWithHeight(int aHeight, Block& aBlock)
{
    std::lock_guard<std::mutex> tLock(blockHeightMutex);
    if (blockHeightCache.find(aHeight) == blockHeightCache.end())
    {
        return false;
    }

    return getBlock(blockHeightCache[aHeight], aBlock);
}

bool CacheDatabase::getBlockTransactions(int aHeight, std::vector<Transaction>& aTransactions)
{
    Block tmp;
    if (getBlockWithHeight(aHeight, tmp))
    {
        aTransactions = tmp.transactions;
        return true;
    }

    return false;
}

bool CacheDatabase::getBlockTransactions(std::string aHash, std::vector<Transaction>& aTransactions)
{
    Block tmp;
    if (getBlock(aHash, tmp))
    {
        aTransactions = tmp.transactions;
        return true;
    }

    return false;
}

bool CacheDatabase::getAddressTransactions(std::string aAddress, std::vector<TransactionOutput>& aTransactions)
{
    std::lock_guard<std::mutex> tLock(addressTransactionsMutex);
    if (addressTransactionsCache.find(aAddress) == addressTransactionsCache.end())
    {
        return false;
    }

    aTransactions = addressTransactionsCache[aAddress];
    return true;
}

}