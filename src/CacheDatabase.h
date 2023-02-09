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

    // method for endpoint to get block from block hash
    bool getBlock(std::string aHash, Block& aBlock);

    // method for endpoint to get block from block height
    bool getBlockWithMaxHeight(Block& aBlock);
    bool getBlockWithHeight(int aHeight, Block& aBlock);

    // method for endpoint to get all blocks
    bool getAllBlocks(std::vector<Block>& aBlocks);
    
    // method for endpoint to get transactions in block from block height
    bool getBlockTransactions(int aHeight, std::vector<Transaction>& aTransactions);

    // method for endpoint to get transactions in block from block hash
    bool getBlockTransactions(std::string aHash, std::vector<Transaction>& aTransactions);

    // method for endpoint to get transaction from transaction id
    bool getTransactionsWithId(std::string aTransId, Transaction& aTransaction);

    // method for endpoint to get all updated transactions from address 
    bool getAddressTransactions(std::string aAddress, std::vector<TransactionOutput>& aTransactions);

    // method for endpoint to get all input transactions from address 
    bool getAddressInputTransactions(std::string aAddress, std::vector<TransactionInput>& aTransactions);

    // method for endpoint to get all output transactions from address
    bool getAddressOutputTransactions(std::string aAddress, std::vector<TransactionOutput>& aTransactions);

private:
    mutable std::mutex blockMutex;
    mutable std::mutex blockHeightMutex;
    mutable std::mutex transactionMutex;
    mutable std::mutex addressTransactionsMutex;
    mutable std::mutex addressInputTransactionsMutex;
    mutable std::mutex addressOutputTransactionsMutex;

    int highestHeight;
    std::unordered_map<std::string, Block> blockCache;
    std::unordered_map<int, std::string> blockHeightCache;
    std::unordered_map<std::string, Transaction> transactionCache;
    std::unordered_map<std::string, std::vector<TransactionOutput>> addressTransactionsCache;
    std::unordered_map<std::string, std::vector<TransactionInput>> addressInputTransactionsCache;
    std::unordered_map<std::string, std::vector<TransactionOutput>> addressOutputTransactionsCache;
};

}

#endif  // CACHE_DATABASE_H