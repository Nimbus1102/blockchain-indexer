#ifndef BLOCKCHAIN_TYPES_H
#define BLOCKCHAIN_TYPES_H

#include <string>
#include <vector>

namespace BlockchainIndexer
{

struct Block
{
    Block()
        : blockHash("")
        , prevBlockHash("")
        , nextBlockHash("")
        , merkleRoot("")
        , size(0)
        , weight(0)
        , height(0)
        , confirmations(0)
        , timestamp(0u)
    {
    }
    
    std::string blockHash;
    std::string prevBlockHash;
    std::string nextBlockHash;
    std::string merkleRoot;
    int size;
    int weight;
    int height;
    int confirmations;
    uint64_t timestamp;
};

}

#endif  // BLOCKCHAIN_TYPES_H