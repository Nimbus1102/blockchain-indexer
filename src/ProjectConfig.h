#ifndef PROJECT_CONFIG_H
#define PROJECT_CONFIG_H

#include <string>

namespace BlockchainIndexer
{

struct ProjectConfig
{
    int publishPeriod;  // in seconds
    std::string blockchainFile;
};

}

#endif  // PROJECT_CONFIG_H