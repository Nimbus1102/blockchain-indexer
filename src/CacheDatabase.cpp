#include "CacheDatabase.h"

namespace BlockchainIndexer
{

CacheDatabase::CacheDatabase()
    : blockCache()
    , blockMutex()
    , blockHashCache()
    , blockHashMutex()
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
        // cache block with block height as key
        std::lock_guard<std::mutex> tLock(blockMutex);
        blockCache[aBlock.height] = aBlock;
    }

    {
        // cache block with block hash as key
        std::lock_guard<std::mutex> tLock(blockHashMutex);
        blockHashCache[aBlock.blockHash] = aBlock;
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
    for (auto& tx : aTransaction.inputs)
    {
        
        std::string address = "";

        {
            // get address of spent transaction input
            std::lock_guard<std::mutex> tLock(transactionMutex);
            if (transactionCache.find(tx.txOutputId) != transactionCache.end())
            {
                address = transactionCache[tx.txOutputId].outputs[tx.txOutputIdx].scriptPubKeyAddress;
            }
            else
            {
                std::cout << "here" << std::endl;
            }
        }

        {
            // remove transaction from address transaction container
            std::lock_guard<std::mutex> tLock(addressTransactionsMutex);
            if (addressTransactionsCache.find(address) != addressTransactionsCache.end())
            {
                for (int i = 0; i < addressTransactionsCache[address].size(); ++i)
                {
                    auto& txo = addressTransactionsCache[address][i];
                    if (txo.txOutputId == tx.txOutputId && txo.txOutputIdx == tx.txOutputIdx)
                    {
                        addressTransactionsCache[address].erase(addressTransactionsCache[address].begin() + i);
                        std::cout << "Caching spent transaction successful" << std::endl;
                        break;
                    }
                }
            }
            else
            {
                std::cout << "here2" << std::endl;
            }
        }
    }

    for (auto& tx : aTransaction.outputs)
    {
        if (tx.scriptPubKeyType != "nulldata")
        {
            std::lock_guard<std::mutex> tLock(addressTransactionsMutex);
            if (addressTransactionsCache.find(tx.scriptPubKeyAddress) == addressTransactionsCache.end())
            {
                addressTransactionsCache[tx.scriptPubKeyAddress] = std::vector<TransactionOutput>();
            }

            addressTransactionsCache[tx.scriptPubKeyAddress].push_back(tx);            
            std::cout << "Caching unspend transaction successful" << std::endl;
        }
    }
}

bool CacheDatabase::getBlockWithHeight(int aHeight, Block& aBlock)
{
    std::lock_guard<std::mutex> tLock(blockMutex);
    if (blockCache.find(aHeight) == blockCache.end())
    {
        return false;
    }

    aBlock = blockCache[aHeight];
    return true;
}

bool CacheDatabase::getBlockWithHash(std::string aHash, Block& aBlock)
{
    std::lock_guard<std::mutex> tLock(blockHashMutex);
    if (blockHashCache.find(aHash) == blockHashCache.end())
    {
        return false;
    }

    aBlock = blockHashCache[aHash];
    return true;
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
    if (getBlockWithHash(aHash, tmp))
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