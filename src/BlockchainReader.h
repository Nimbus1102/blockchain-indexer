#ifndef BLOCKCHAIN_READER_H
#define BLOCKCHAIN_READER_H

// file reader class that reads blockchain information and persistently sends out the block
// information in a configured time period. 

#include <string>
#include <thread>
#include <memory>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

#include "BlockchainTypes.h"
#include "BlockchainUtils.h"
#include "SimpleMiddleware.h"

using json = nlohmann::json;

namespace BlockchainIndexer
{

class BlockchainReader
{
public:
    BlockchainReader();
    ~BlockchainReader();

    void init(std::string& aFilename, int aMaxBlocks, int aThreadPeriod);
    void addMiddleware(std::shared_ptr<Middleware>& aMiddleware);
    void publish();
    void logic();

private:
    int counter;
    int maxBlocks;
    int threadPeriod;
    json blockchainData;
    std::string fileName;
    std::shared_ptr<Middleware> middleware;
};

}

#endif  // BLOCKCHAIN_READER_H