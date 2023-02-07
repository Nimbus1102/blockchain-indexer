#ifndef CACHE_DATABASE_H
#define CACHE_DATABASE_H

#include <mutex>
#include <string>
#include <memory>
#include <iostream>
#include <unordered_map>

#include "BlockchainTypes.h"

// Cache database to index blocks, transactions and address transactions in memory.

namespace BlockchainIndexer
{

class CacheDatabase
{
public:
    CacheDatabase();
    ~CacheDatabase();

    void cacheBlock(Block& aBlock);
    void cacheAddressTransaction(Transaction& aTransaction);

    // block index
    bool getBlockWithHeight(int aHeight, Block& aBlock);
    bool getBlockWithHash(std::string aHash, Block& aBlock);

    // block transaction index
    bool getBlockTransactions(int aHeight, std::vector<Transaction>& aTransactions);
    bool getBlockTransactions(std::string aHash, std::vector<Transaction>& aTransactions);

    // address index
    bool getAddressTransactions(std::string aAddress, std::vector<TransactionOutput>& aTransactions);

private:
    mutable std::mutex blockMutex;
    mutable std::mutex blockHashMutex;
    mutable std::mutex transactionMutex;
    mutable std::mutex addressTransactionsMutex;

    std::unordered_map<int, Block> blockCache;
    std::unordered_map<std::string, Block> blockHashCache;
    std::unordered_map<std::string, Transaction> transactionCache;
    std::unordered_map<std::string, std::vector<TransactionOutput>> addressTransactionsCache;
};

}

#endif  // CACHE_DATABASE_H