#ifndef PROJECT_CONFIG_H
#define PROJECT_CONFIG_H

#include <string>

namespace BlockchainIndexer
{

struct ProjectConfig
{
    int maxBlocks;
    int publishPeriod;  // in seconds
    int xConfirmations;
    std::string blockchainFile;
    std::string databaseDirectory;
};

}

#endif  // PROJECT_CONFIG_H