#include "BlockchainReader.h"

namespace BlockchainIndexer
{

BlockchainReader::BlockchainReader()
    : counter(0)
    , threadPeriod(0)
    , blockchainData()
    , fileName("")
    , middleware(nullptr)
{
}

BlockchainReader::~BlockchainReader()
{
    middleware = nullptr;
}

void BlockchainReader::init(std::string& aFilename, int aMaxBlocks, int aThreadPeriod)
{
    fileName = aFilename;
    maxBlocks = aMaxBlocks;
    threadPeriod = aThreadPeriod;

    // get blockchain data
    std::ifstream f(fileName);
    blockchainData = json::parse(f);
}

void BlockchainReader::addMiddleware(std::shared_ptr<Middleware>& aMiddleware)
{
    middleware = aMiddleware;
}

void BlockchainReader::publish()
{
    Block currBlock;
    json curr = blockchainData[counter];
    currBlock.blockHash = curr["hash"];
    currBlock.nextBlockHash = curr["nextblockhash"];
    currBlock.merkleRoot = curr["merkleroot"];
    currBlock.size = curr["size"];
    currBlock.weight = curr["weight"];
    currBlock.height = curr["height"];
    currBlock.confirmations = curr["confirmations"];
    currBlock.timestamp = curr["time"];

    // check for genesis block
    if (counter > 0)
    {
        currBlock.prevBlockHash = curr["previousblockhash"];
    }

    counter++;
    middleware->publish(currBlock);
}

void BlockchainReader::logic()
{
    while (counter < maxBlocks)
    {
        publish();
        std::this_thread::sleep_for(std::chrono::seconds(threadPeriod));
    }
}

}