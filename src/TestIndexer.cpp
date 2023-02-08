#include "TestIndexer.h"

namespace BlockchainIndexer
{

TestIndexer::TestIndexer(int aNumBlockTests, int aNumAddressTests, std::string aBlockDir, std::string aAddressDir)
    : numBlockTests(aNumBlockTests)
    , numAddressTests(aNumAddressTests)
    , blockTestDir(aBlockDir)
    , addressTestDir(aAddressDir)
    , db(nullptr)
    , indexer(nullptr)
{
}

TestIndexer::~TestIndexer()
{
}

void TestIndexer::init(std::shared_ptr<CacheDatabase>& aCacheDB, std::shared_ptr<BlockchainIndexer>& aIndexer)
{
    db = aCacheDB;
    indexer = aIndexer;
}

bool TestIndexer::runBlockTests()
{
    std::string answerFilePath = blockTestDir + "answer.json";
    std::ifstream fAns(answerFilePath);
    json tAnswer = json::parse(fAns);

    for (int idx = 1; idx <= numBlockTests; ++idx)
    {
        std::string filePath = blockTestDir + "test" + std::to_string(idx) + ".json";
        std::cout << "Running block test" + std::to_string(idx) + "..." << std::endl;
        std::ifstream f(filePath);
        json tBlock_json = json::parse(f);
        Block tBlock;
        convertJsonToBlock(tBlock_json, tBlock);
        
        bool result = true;

        // run cache database tests
        result = (getCachedBlockFromBlockHeight(tBlock.height, tBlock) && result);
        result = (getCachedBlockFromBlockHash(tBlock.blockHash, tBlock) && result);
        result = (getCachedTransactionsFromBlockHeight(tBlock.height, tBlock) && result);
        result = (getCachedTransactionsFromBlockHash(tBlock.blockHash, tBlock) && result);

        // run leveldb databasae tests
        result = (getBlockInfoFromBlockHeight(tBlock.height, tAnswer[std::to_string(idx)]["block-info"]) && result);
        result = (getBlockInfoFromBlockHash(tBlock.blockHash, tAnswer[std::to_string(idx)]["block-info"]) && result);

        for (auto& transaction : tBlock.transactions)
        {
            for  (auto& out : transaction.outputs)
            {
                if (out.scriptPubKeyType != "nulldata")
                {
                    result = (getValueFromTransactionIdxAndId(out.txOutputId, out.txOutputIdx, out.value) && result);
                }
            }
        }

        if (result != tAnswer[std::to_string(idx)]["answer"])
        {
            std::cout << "Block test" + std::to_string(idx) + " failed." << std::endl;
            return false;
        }

        std::cout << "Block test" + std::to_string(idx) + " passed." << std::endl;
    }

    return true;
}

bool TestIndexer::runAddressTests()
{
    for (int idx = 1; idx <= numAddressTests; ++idx)
    {
        std::string filePath = addressTestDir + "test" + std::to_string(idx);
        std::ifstream f(filePath);
        json tAnswer = json::parse(f);
        std::string address = tAnswer["address"];
        
        if (!getCachedTransactionsFromAddress(address, tAnswer))
        {
            return false;
        }
    }

    return true;
}

bool TestIndexer::getCachedBlockFromBlockHeight(int aBlockHeight, Block& tBlock)
{
    Block tmp;
    if (db->getBlockWithHeight(aBlockHeight, tmp))
    {
        if (tmp.blockHash != tBlock.blockHash ||
            tmp.prevBlockHash != tBlock.prevBlockHash || 
            tmp.nextBlockHash != tBlock.nextBlockHash || 
            tmp.merkleRoot != tBlock.merkleRoot ||
            tmp.size != tBlock.size ||
            tmp.weight != tBlock.weight ||
            tmp.height != tBlock.height ||
            tmp.confirmations != tBlock.confirmations ||
            tmp.timestamp != tBlock.timestamp)
        {
            return false;
        }

        return true;
    }

    return false;
}

bool TestIndexer::getCachedBlockFromBlockHash(std::string aBlockHash, Block& tBlock)
{
    Block tmp;
    if (db->getBlock(aBlockHash, tmp))
    {
        if (tmp.blockHash != tBlock.blockHash ||
            tmp.prevBlockHash != tBlock.prevBlockHash || 
            tmp.nextBlockHash != tBlock.nextBlockHash || 
            tmp.merkleRoot != tBlock.merkleRoot ||
            tmp.size != tBlock.size ||
            tmp.weight != tBlock.weight ||
            tmp.height != tBlock.height ||
            tmp.confirmations != tBlock.confirmations ||
            tmp.timestamp != tBlock.timestamp)
        {
            return false;
        }

        return true;
    }

    return false;
}

bool TestIndexer::getCachedTransactionsFromBlockHeight(int aBlockHeight, Block& tBlock)
{
    std::vector<Transaction> tmp;
    if (db->getBlockTransactions(aBlockHeight, tmp))
    {
        for (int idx = 0; idx < tmp.size(); ++idx)
        {
            auto& cacheTransaction = tmp[idx];
            auto& testTransaction = tBlock.transactions[idx];

            // compare input transactions
            if (cacheTransaction.inputs.size() != testTransaction.inputs.size())
            {
                return false;
            }

            for (int input_idx = 0; input_idx < cacheTransaction.inputs.size(); ++input_idx)
            {
                auto& cacheInput = cacheTransaction.inputs[input_idx];
                auto& testInput = testTransaction.inputs[input_idx];

                if (cacheInput.txIdx != testInput.txIdx ||
                    cacheInput.coinbase != testInput.coinbase ||
                    cacheInput.sequence != testInput.sequence ||
                    cacheInput.txOutputId != testInput.txOutputId ||
                    cacheInput.txOutputIdx != testInput.txOutputIdx)
                {
                    return false;
                }
            }

            // compare output transactions
            if (cacheTransaction.outputs.size() != testTransaction.outputs.size())
            {
                return false;
            }

            for (int output_idx = 0; output_idx < cacheTransaction.outputs.size(); ++output_idx)
            {
                auto& cacheOutput = cacheTransaction.outputs[output_idx];
                auto& testOutput = testTransaction.outputs[output_idx];

                if (cacheOutput.value != testOutput.value ||
                    cacheOutput.txOutputId != testOutput.txOutputId ||
                    cacheOutput.txOutputIdx != testOutput.txOutputIdx ||
                    cacheOutput.scriptPubKeyType != testOutput.scriptPubKeyType ||
                    cacheOutput.scriptPubKeyAddress != testOutput.scriptPubKeyAddress)
                {
                    return false;
                }
            }
        }

        return true;
    }

    return false;
}

bool TestIndexer::getCachedTransactionsFromBlockHash(std::string aBlockHash, Block& tBlock)
{
    std::vector<Transaction> tmp;
    if (db->getBlockTransactions(aBlockHash, tmp))
    {
        for (int idx = 0; idx < tmp.size(); ++idx)
        {
            auto& cacheTransaction = tmp[idx];
            auto& testTransaction = tBlock.transactions[idx];

            // compare input transactions
            if (cacheTransaction.inputs.size() != testTransaction.inputs.size())
            {
                return false;
            }

            for (int input_idx = 0; input_idx < cacheTransaction.inputs.size(); ++input_idx)
            {
                auto& cacheInput = cacheTransaction.inputs[input_idx];
                auto& testInput = testTransaction.inputs[input_idx];

                if (cacheInput.txIdx != testInput.txIdx ||
                    cacheInput.coinbase != testInput.coinbase ||
                    cacheInput.sequence != testInput.sequence ||
                    cacheInput.txOutputId != testInput.txOutputId ||
                    cacheInput.txOutputIdx != testInput.txOutputIdx)
                {
                    return false;
                }
            }

            // compare output transactions
            if (cacheTransaction.outputs.size() != testTransaction.outputs.size())
            {
                return false;
            }

            for (int output_idx = 0; output_idx < cacheTransaction.outputs.size(); ++output_idx)
            {
                auto& cacheOutput = cacheTransaction.outputs[output_idx];
                auto& testOutput = testTransaction.outputs[output_idx];

                if (cacheOutput.value != testOutput.value ||
                    cacheOutput.txOutputId != testOutput.txOutputId ||
                    cacheOutput.txOutputIdx != testOutput.txOutputIdx ||
                    cacheOutput.scriptPubKeyType != testOutput.scriptPubKeyType ||
                    cacheOutput.scriptPubKeyAddress != testOutput.scriptPubKeyAddress)
                {
                    return false;
                }
            }
        }

        return true;
    }

    return false;
}

bool TestIndexer::getBlockInfoFromBlockHeight(int aBlockHeight, std::string aAnswer)
{
    std::string info;
    if (indexer->getBlockWithHeight(aBlockHeight, info))
    {
        if (info == aAnswer)
        {
            return true;
        }

        return false;
    }

    return false;
}

bool TestIndexer::getBlockInfoFromBlockHash(std::string aBlockHash, std::string aAnswer)
{
    std::string info;
    if (indexer->getBlockInformation(aBlockHash, info))
    {
        if (info == aAnswer)
        {
            return true;
        }

        return false;
    }

    return false;
}

bool TestIndexer::getValueFromTransactionIdxAndId(std::string aTransactionId, int aIdx, double aAnswer)
{
    double val;
    if (indexer->getTransactionValue(aTransactionId, aIdx, val))
    {
        if (std::abs(val - aAnswer) < DOUBLE_EPSILON)
        {
            return true;
        }

        return false;
    }

    return false;
}

bool TestIndexer::getCachedTransactionsFromAddress(std::string& aAddress, json& tAnswer)
{
    return true;
}

bool TestIndexer::getAddressTransactionsFromAddress(std::string aTransactionId, int aIdx, std::string aAnswer)
{
    return true;
}

}