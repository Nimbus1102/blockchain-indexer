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
#include "CacheDatabase.h"
#include "BlockchainReader.h"
#include "ConfigFileReader.h"
#include "SimpleMiddleware.h"
#include "BlockchainIndexer.h"

BlockchainIndexer::ProjectConfig config;
std::shared_ptr<BlockchainIndexer::CacheDatabase> cacheDatabase;
std::shared_ptr<BlockchainIndexer::BlockListener> indexSubscriber;
std::shared_ptr<BlockchainIndexer::BlockchainReader> blockchainReader;
std::shared_ptr<BlockchainIndexer::BlockchainIndexer> blockchainIndexer;
std::string configFilePath = "/home/niven/blockchain-indexer/config/ConfigFile.xml";

void runReader()
{
    std::cout << "Starting blockchain reader." << std::endl;
    blockchainReader->logic();
    std::cout << "End of blockchain. Closing reader thread." << std::endl;
}

void runIndexer()
{
    std::cout << "Starting indexing logic thread." << std::endl;
    
    while (true)
    {
        if (indexSubscriber->haveNewMessage())
        {
            BlockchainIndexer::Block tmp;
            indexSubscriber->getMessage(tmp);

            if (tmp.confirmations >= config.xConfirmations)
            {
                // cache block data in memory
                cacheDatabase->cacheBlock(tmp);

                // index block data on disk
                blockchainIndexer->indexBlock(tmp);
                std::cout << "Processed block information." << std::endl;
            }
            else
            {
                std::cout << "Block confirmation below x-confirmation, not processing received block." << std::endl;
            }
        }
    }

    std::cout << "Closing indexer logic thread." << std::endl;
}

void checkBlockInformation(BlockchainIndexer::Block& block)
{
    std::cout << block.blockHash << std::endl;
    std::cout << block.prevBlockHash << std::endl;
    std::cout << block.nextBlockHash << std::endl;
    std::cout << block.merkleRoot << std::endl;
    std::cout << block.size << std::endl;
    std::cout << block.weight << std::endl;
    std::cout << block.height << std::endl;
    std::cout << block.confirmations << std::endl;
    std::cout << block.timestamp << std::endl;

    for (auto& transaction : block.transactions)
    {
        std::cout << "Transaction:" << std::endl;
        std::cout << transaction.idx << std::endl;
        std::cout << transaction.version << std::endl;
        std::cout << transaction.lockTime << std::endl;
        std::cout << transaction.id << std::endl;
        std::cout << transaction.hash << std::endl;

        std::cout << "Inputs:" << std::endl;
        for (auto& in : transaction.inputs)
        {
            std::cout << "Input:" << std::endl;
            std::cout << in.txIdx << std::endl;
            std::cout << in.coinbase  << std::endl;
            std::cout << in.sequence  << std::endl;
            std::cout << in.txOutputIdx  << std::endl;
            std::cout << in.txOutputId  << std::endl;
            std::cout << in.scriptSigAsm  << std::endl;
            std::cout << in.scriptSigHex  << std::endl;
        }

        std::cout << "Outputs:" << std::endl;
        for (auto& out : transaction.outputs)
        {
            std::cout << "Output:" << std::endl;
            std::cout << out.value << std::endl;
            std::cout << out.txOutputIdx << std::endl;
            std::cout << out.txOutputId << std::endl;
            std::cout << out.scriptPubKeyReqSig << std::endl;
            std::cout << out.scriptPubKeyAsm << std::endl;
            std::cout << out.scriptPubKeyHex << std::endl;
            std::cout << out.scriptPubKeyType << std::endl;
            std::cout << out.scriptPubKeyAddress << std::endl;
        }
    }
}

int main(const int argc, const char** const argv)
{
    // get config
    BlockchainIndexer::ConfigFileReader configReader;
    configReader.init(configFilePath);
    configReader.getConfiguration(config);

    // initialize middleware and subscriber
    indexSubscriber = std::make_shared<BlockchainIndexer::BlockListener>();
    std::shared_ptr<BlockchainIndexer::Middleware> middleware = std::make_shared<BlockchainIndexer::Middleware>();
    middleware->addSubscriber(indexSubscriber);

    // initialize reader
    blockchainReader = std::make_shared<BlockchainIndexer::BlockchainReader>();
    blockchainReader->init(config.blockchainFile, config.maxBlocks, config.publishPeriod);
    blockchainReader->addMiddleware(middleware);

    // initialize indexer
    blockchainIndexer = std::make_shared<BlockchainIndexer::BlockchainIndexer>();
    blockchainIndexer->init(config.databaseDirectory);

    // initialize cache database in heap
    cacheDatabase.reset(new BlockchainIndexer::CacheDatabase());

    // start indexing worker thread and reader thread
    std::thread readerThread(runReader);
    std::thread indexerThread(runIndexer);
    readerThread.join();

    if (config.runTestCases)
    {
        // run test cases

    }

    indexerThread.join();

	return 0;
}