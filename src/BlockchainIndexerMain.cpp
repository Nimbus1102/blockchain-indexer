#include <leveldb/db.h>
#include <leveldb/write_batch.h>
#include <leveldb/filter_policy.h>
#include <leveldb/cache.h>

#include <mutex>
#include <array>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>

#include "ProjectConfig.h"
#include "BlockchainReader.h"
#include "ConfigFileReader.h"
#include "SimpleMiddleware.h"

std::shared_ptr<leveldb::DB> db;
std::shared_ptr<BlockchainIndexer::BlockchainReader> blockchainReader;
std::shared_ptr<BlockchainIndexer::BlockListener> indexSubscriber;
std::string configFilePath = "/home/niven/blockchain-indexer/config/ConfigFile.xml";

void runBlockchainReader()
{
    std::cout << "Starting blockchain reader." << std::endl;
    blockchainReader->logic();
    std::cout << "End of blockchain!" << std::endl;
}

void runIndexer()
{
    while (true)
    {
        if (indexSubscriber->haveNewMessage())
        {
            BlockchainIndexer::Block block;
            indexSubscriber->getMessage(block);

            std::cout << block.blockHash << std::endl;
            std::cout << block.nextBlockHash << std::endl;
            std::cout << block.prevBlockHash << std::endl;
            std::cout << block.merkleRoot << std::endl;
            std::cout << block.size << std::endl;
            std::cout << block.weight << std::endl;
            std::cout << block.height << std::endl;
            std::cout << block.confirmations << std::endl;
            std::cout << block.timestamp << std::endl;
        }
    }
    
}

int main(const int argc, const char** const argv)
{
    // get config
    BlockchainIndexer::ProjectConfig config;
    BlockchainIndexer::ConfigFileReader configReader;
    configReader.init(configFilePath);
    configReader.getConfiguration(config);

    indexSubscriber = std::make_shared<BlockchainIndexer::BlockListener>();
    std::shared_ptr<BlockchainIndexer::Middleware> middleware = std::make_shared<BlockchainIndexer::Middleware>();
    middleware->addSubscriber(indexSubscriber);

    blockchainReader = std::make_shared<BlockchainIndexer::BlockchainReader>();
    blockchainReader->init(config.blockchainFile, config.maxBlocks, config.publishPeriod);
    blockchainReader->addMiddleware(middleware);

    std::thread blockchainReaderThread(runBlockchainReader);
    std::thread indexerThread(runIndexer);
    blockchainReaderThread.join();

	return 0;
}
