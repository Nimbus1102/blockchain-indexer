#ifndef BLOCKCHAIN_INDEXER_H
#define BLOCKCHAIN_INDEXER_H

// Indexer to index blocks read from the Bitcoin blockchain, including its transactions. The indexer 
// will store the blockchain information in memory (CacheDatabase) and on disk  using a key-value 
// datastore (LevelDB). BlockIndexer class helps the client to read and write blockchain information, 
// handling queries from various http endpoints that the indexer application may offer.
//
// It will index the elements such that we are able to:
// 1. Block index: indexing the block in height order, with the option to query blocks by their hash.
// 2. Block transactions index: query transactions within a block, given its height or hash.
// 3. Address transaction index: every transaction will have its accompanying address on both Vin and
//    Vout. This index provides the ability to query all transactions scoped to the address.
//
// When storing in disk, the blockchain indexer will store key-value pairs of the block attributes.
// The attributes are:
// 1. block-height : block-hash
// 2. block-hash : block-height
// 3. block-hash : block-size
// 4. block-hash : number of transactions
// 
// The indexer will also store transactions and address transactions in disk.
// 1. transaction-id : block-hash-transaction-idx
// 2. transaction-id-idx : value
// 2. transaction-id-idx : address
// 3. address : spent and unspent transaction-ids

#include <mutex>
#include <string>
#include <vector>
#include <memory>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <leveldb/db.h>
#include <leveldb/cache.h>
#include <leveldb/write_batch.h>
#include <leveldb/filter_policy.h>

#include "BlockchainTypes.h"
#include "CacheDatabase.h"

namespace BlockchainIndexer
{

class BlockchainIndexer
{
public:
    BlockchainIndexer();
    ~BlockchainIndexer();

    void init(std::string aDatabaseDirectory);
    void indexBlock(Block& aBlock);

    // block index
    bool getBlockWithHeight(int aHeight, Block& aBlock);
    bool getBlockWithHash(std::string aHash, Block& aBlock);

    // block transaction index
    bool getBlockTransactions(int aHeight, std::vector<Transaction>& aTransactions);
    bool getBlockTransactions(std::string aHash, std::vector<Transaction>& aTransactions);

    // address index
    bool getAddressTransactions(std::string aAddress, std::vector<TransactionOutput>& aTransactions);

private:
    mutable std::mutex dbMutex;
    std::unique_ptr<leveldb::DB> levelDB;
    std::unique_ptr<CacheDatabase> cacheDB;
};

}

#endif   // BLOCKCHAIN_INDEXER_H