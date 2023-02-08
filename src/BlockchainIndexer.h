#ifndef BLOCKCHAIN_INDEXER_H
#define BLOCKCHAIN_INDEXER_H

// Indexer to index blocks read from the Bitcoin blockchain, including its transactions. The indexer 
// will store the blockchain information on disk using a key-value datastore, using LevelDB. The
// BlockchainIndexer class helps the client to read and write blockchain information, handling queries
// from various http endpoints that the indexer application may offer.
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
// 2. block-hash : block-info (block-prevHash-nextHash-merkleRoot-size-weight-height-confirmations-timestamp)
// 
// The indexer will also store transactions and address transactions in disk.
// 1. unspent-transaction-id-idx : value
// 2. unspent-transaction-id-idx : address
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

namespace BlockchainIndexer
{

class BlockchainIndexer
{
public:
    BlockchainIndexer();
    ~BlockchainIndexer();

    void init(std::string aDatabaseDirectory);
    void indexBlock(Block& aBlock);

    // method to get block information of the max height block
    bool getBlockWithMaxHeight(std::string& aBlockInfo);

    // method to get block information from block height
    bool getBlockWithHeight(int aHeight, std::string& aBlockInfo);

    // method to get block information from block hash
    bool getBlockInformation(std::string& aBlockHash, std::string& aBlockInfo);

    // method to get value from transaction id and idx
    bool getTransactionValue(std::string aTransactionId, int aIdx, double& aValue);

    // method to get transaction history from address
    bool getAddressTransactions(std::string aAddress, std::string& aHistory);

private:
    int highestHeight;
    mutable std::mutex dbMutex;
    std::unique_ptr<leveldb::DB> levelDB;
};

}

#endif   // BLOCKCHAIN_INDEXER_H