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

void BlockchainReader::convertJsonToBlock(json& aJsonBlock, Block& aBlock)
{
    aBlock.blockHash = aJsonBlock["hash"];
    aBlock.nextBlockHash = aJsonBlock["nextblockhash"];
    aBlock.merkleRoot = aJsonBlock["merkleroot"];
    aBlock.size = aJsonBlock["size"];
    aBlock.weight = aJsonBlock["weight"];
    aBlock.height = aJsonBlock["height"];
    aBlock.confirmations = aJsonBlock["confirmations"];
    aBlock.timestamp = aJsonBlock["time"];

    // check for genesis block
    if (counter > 0)
    {
        aBlock.prevBlockHash = aJsonBlock["previousblockhash"];
    }

    // get transactions
    int idx = 0;
    for (auto& tx : aJsonBlock["tx"])
    {
        Transaction trans;
        trans.idx = idx;
        trans.version = tx["version"];
        trans.lockTime = tx["locktime"];
        trans.id = tx["txid"];
        trans.hash = tx["hash"];

        int vin_count = 0;
        for (auto& vin : tx["vin"])
        {
            TransactionInput in;
            in.txIdx = vin_count;
            in.sequence = vin["sequence"];

            if (vin.contains("coinbase"))
            {
                in.coinbase = true;
            }
            else
            {
                in.coinbase = false;
                in.txOutputId = vin["txid"];
                in.txOutputIdx = vin["vout"];
                in.scriptSigAsm = vin["scriptSig"]["asm"];
                in.scriptSigHex = vin["scriptSig"]["hex"];
            }

            trans.inputs.push_back(in);
            vin_count++;
        }

        for (auto& vout : tx["vout"])
        {
            TransactionOutput out;
            out.value = vout["value"];
            out.txOutputIdx = vout["n"];
            out.txOutputId = trans.id;
            out.scriptPubKeyAsm = vout["scriptPubKey"]["asm"];
            out.scriptPubKeyHex = vout["scriptPubKey"]["hex"];
            out.scriptPubKeyType = vout["scriptPubKey"]["type"];

            if (out.scriptPubKeyType != "nulldata")
            {
                out.scriptPubKeyReqSig = vout["scriptPubKey"]["reqSigs"];
                out.scriptPubKeyAddresses = vout["scriptPubKey"]["addresses"][0];
                
                // debug
                if (vout["scriptPubKey"]["addresses"].size() > 1u)
                {
                    std::cout << "WRONG!!" << std::endl;
                }
            }

            trans.outputs.push_back(out);
        }

        aBlock.transactions.push_back(trans);
        idx++;
    }
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
        std::this_thread::sleep_for(std::chrono::seconds(threadPeriod));
    }
}

}