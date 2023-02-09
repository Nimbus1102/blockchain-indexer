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
    std::cout << blockchainData.size() << std::endl;
}

void BlockchainReader::addMiddleware(std::shared_ptr<Middleware>& aMiddleware)
{
    middleware = aMiddleware;
}

void BlockchainReader::publish()
{
    Block currBlock;
    json curr = blockchainData[counter];
    convertJsonToBlock(curr, currBlock);
    middleware->publish(currBlock);
    counter++;
}

void BlockchainReader::logic()
{
    while (counter < maxBlocks)
    {
        publish();
        std::this_thread::sleep_for(std::chrono::milliseconds(threadPeriod));
    }
}

}