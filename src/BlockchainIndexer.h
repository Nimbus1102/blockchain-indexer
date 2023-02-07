#ifndef BLOCKCHAIN_INDEXER_H
#define BLOCKCHAIN_INDEXER_H

// Indexer to index blocks read from the Bitcoin blockchain, including its transactions. It will
// index the elements such that we are able to:
// 1. Block index: indexing the block in height order, with the option to query blocks by their hash.
// 2. Block transactions index: query transactions within a block, given its height or hash.
// 3. Address transaction index: every transaction will have its accompanying address on both Vin and
//    Vout. This index provides the ability to query all transactions scoped to the address.

#include <string>
#include <vector>
#include <memory>
#include <leveldb/db.h>
#include <leveldb/write_batch.h>

#include "BlockchainTypes.h"
#include "CacheDatabase.h"

namespace BlockchainIndexer
{

class BlockchainIndexer
{
public:
    BlockchainIndexer(std::shared_ptr<leveldb::DB>& aDB, std::shared_ptr<CacheDatabase>& aCacheDB);
    ~BlockchainIndexer();

private:

};

}

#endif   // BLOCKCHAIN_INDEXER_H