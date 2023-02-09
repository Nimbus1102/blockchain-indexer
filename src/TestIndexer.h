#ifndef TEST_INDEXER_H
#define TEST_INDEXER_H

#include <string>
#include <limits>
#include <fstream>
#include <sstream>
#include <iostream>
#include <nlohmann/json.hpp>

#include "CacheDatabase.h"
#include "BlockchainTypes.h"
#include "BlockchainUtils.h"
#include "BlockchainIndexer.h"

using json = nlohmann::json;

namespace BlockchainIndexer
{

static double DOUBLE_EPSILON = 0.01;

class TestIndexer
{
public:
    TestIndexer(int aNumBlockTests, int aNumAddressTests, std::string aBlockDir, std::string aAddressDir);
    ~TestIndexer();

    void init(std::shared_ptr<CacheDatabase>& aCacheDB, std::shared_ptr<BlockchainIndexer>& aIndexer);
    bool runBlockTests();
    bool runAddressTests();

private:
    // methods for block tests with cache database
    bool getCachedBlockFromBlockHeight(int aBlockHeight, Block& tBlock);
    bool getCachedBlockFromBlockHash(std::string aBlockHash, Block& tBlock);
    bool getCachedTransactionsFromBlockHeight(int aBlockHeight, Block& tBlock);
    bool getCachedTransactionsFromBlockHash(std::string aBlockHash, Block& tBlock);

    // methods for block tests with leveldb database
    bool getBlockInfoFromBlockHeight(int aBlockHeight, std::string aTest);
    bool getBlockInfoFromBlockHash(std::string aBlockHash, std::string aTest);
    bool getValueFromTransactionIdxAndId(std::string aTransactionId, int aIdx, double aTest);

    // method for address test with cache database
    bool getCachedTransactionsFromAddress(std::string& aAddress, json& aTest);
    bool getCachedInputTransactionsFromAddress(std::string& aAddress, json& aTest);
    bool getCachedOutputTransactionsFromAddress(std::string& aAddress, json& aTest);

    // method for address test with leveldb database
    bool getAddressTransactionsFromAddress(std::string& aAddress, json& aTest);

    int numBlockTests;
    int numAddressTests;
    std::string blockTestDir;
    std::string addressTestDir;
    std::shared_ptr<CacheDatabase> db;
    std::shared_ptr<BlockchainIndexer> indexer;
};

}

#endif  // TEST_INDEXER_H