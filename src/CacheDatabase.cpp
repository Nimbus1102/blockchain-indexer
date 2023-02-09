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
    , addressInputTransactionsCache()
    , addressInputTransactionsMutex()
    , addressOutputTransactionsCache()
    , addressOutputTransactionsMutex()
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

        // remove transaction from address updated transactions
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

        {
            // add to spent transaction
            std::lock_guard<std::mutex> tLock(addressInputTransactionsMutex);
            if (addressInputTransactionsCache.find(address) == addressInputTransactionsCache.end())
            {
                addressInputTransactionsCache[address] = std::vector<TransactionInput>();
            }

            addressInputTransactionsCache[address].push_back(tx);
        }
    }

    for (auto& tx : aTransaction.outputs)
    {        
        if (tx.scriptPubKeyType != "nulldata")
        {
            // add to address updated transactions
            if (addressTransactionsCache.find(tx.scriptPubKeyAddress) == addressTransactionsCache.end())
            {
                addressTransactionsCache[tx.scriptPubKeyAddress] = std::vector<TransactionOutput>();
            }

            addressTransactionsCache[tx.scriptPubKeyAddress].push_back(tx);

            {
                // add to out transaction
                std::lock_guard<std::mutex> tLock(addressOutputTransactionsMutex);

                if (addressOutputTransactionsCache.find(tx.scriptPubKeyAddress) == addressOutputTransactionsCache.end())
                {
                    addressOutputTransactionsCache[tx.scriptPubKeyAddress] = std::vector<TransactionOutput>();
                }

                addressOutputTransactionsCache[tx.scriptPubKeyAddress].push_back(tx);

            }

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

bool CacheDatabase::getAllBlocks(std::vector<Block>& aBlocks)
{
    std::lock_guard<std::mutex> tLock(blockHeightMutex);
    for (int i = highestHeight; i >= 0; --i)
    {
        Block tmp;
        if (getBlock(blockHeightCache[i], tmp))
        {
            aBlocks.push_back(tmp);
        }
    }

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
    if (getBlock(aHash, tmp))
    {
        aTransactions = tmp.transactions;
        return true;
    }

    return false;
}

bool CacheDatabase::getTransactionsWithId(std::string aTransId, Transaction& aTransaction)
{
    std::lock_guard<std::mutex> tLock(transactionMutex);
    if (transactionCache.find(aTransId) != transactionCache.end())
    {
        aTransaction = transactionCache[aTransId];
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

bool CacheDatabase::getAddressInputTransactions(std::string aAddress, std::vector<TransactionInput>& aTransactions)
{
    std::lock_guard<std::mutex> tLock(addressInputTransactionsMutex);
    if (addressInputTransactionsCache.find(aAddress) == addressInputTransactionsCache.end())
    {
        return false;
    }

    aTransactions = addressInputTransactionsCache[aAddress];
    return true;
}

bool CacheDatabase::getAddressOutputTransactions(std::string aAddress, std::vector<TransactionOutput>& aTransactions)
{
    std::lock_guard<std::mutex> tLock(addressOutputTransactionsMutex);
    if (addressOutputTransactionsCache.find(aAddress) == addressOutputTransactionsCache.end())
    {
        return false;
    }

    aTransactions = addressOutputTransactionsCache[aAddress];
    return true;
}

}