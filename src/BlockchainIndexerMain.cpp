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

std::string configFilePath = "/home/niven/blockchain-indexer/config/ConfigFile.xml";

int main(const int argc, const char** const argv)
{
    // get config
    BlockchainIndexer::ProjectConfig config;
    BlockchainIndexer::ConfigFileReader configReader;
    configReader.init(configFilePath);
    configReader.getConfiguration(config);

	return 0;
}
