#ifndef PROJECT_CONFIG_H
#define PROJECT_CONFIG_H

#include <string>

namespace BlockchainIndexer
{

struct ProjectConfig
{
    // application parameters
    int maxBlocks;
    int publishPeriod;  // in seconds
    int xConfirmations;
    std::string blockchainFile;
    std::string databaseDirectory;

    // test parameters
    int numBlockTestCases;
    int numAddressTestCases;
    std::string blockTestDirectory;
    std::string addressTestDirectory;
};

}

#endif  // PROJECT_CONFIG_H