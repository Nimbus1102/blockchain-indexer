#ifndef BLOCKCHAIN_UTILS_H
#define BLOCKCHAIN_UTILS_H

#include <string>
#include <iostream>
#include <nlohmann/json.hpp>

#include "BlockchainTypes.h"

using json = nlohmann::json;

namespace BlockchainIndexer
{

void convertJsonToBlock(json& aJsonBlock, Block& aBlock);
void checkBlockInformation(Block& block);

}

#endif  // BLOCKCHAIN_UTILS_H