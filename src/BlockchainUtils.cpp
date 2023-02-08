#include "BlockchainUtils.h"

namespace BlockchainIndexer
{

void convertJsonToBlock(json& aJsonBlock, Block& aBlock)
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
    if (aBlock.height > 0)
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
                out.scriptPubKeyAddress = vout["scriptPubKey"]["addresses"][0];
                
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

void checkBlockInformation(Block& block)
{
    std::cout << block.blockHash << std::endl;
    std::cout << block.prevBlockHash << std::endl;
    std::cout << block.nextBlockHash << std::endl;
    std::cout << block.merkleRoot << std::endl;
    std::cout << block.size << std::endl;
    std::cout << block.weight << std::endl;
    std::cout << block.height << std::endl;
    std::cout << block.confirmations << std::endl;
    std::cout << block.timestamp << std::endl;

    for (auto& transaction : block.transactions)
    {
        std::cout << "Transaction:" << std::endl;
        std::cout << transaction.idx << std::endl;
        std::cout << transaction.version << std::endl;
        std::cout << transaction.lockTime << std::endl;
        std::cout << transaction.id << std::endl;
        std::cout << transaction.hash << std::endl;

        std::cout << "Inputs:" << std::endl;
        for (auto& in : transaction.inputs)
        {
            std::cout << "Input:" << std::endl;
            std::cout << in.txIdx << std::endl;
            std::cout << in.coinbase  << std::endl;
            std::cout << in.sequence  << std::endl;
            std::cout << in.txOutputIdx  << std::endl;
            std::cout << in.txOutputId  << std::endl;
            std::cout << in.scriptSigAsm  << std::endl;
            std::cout << in.scriptSigHex  << std::endl;
        }

        std::cout << "Outputs:" << std::endl;
        for (auto& out : transaction.outputs)
        {
            std::cout << "Output:" << std::endl;
            std::cout << out.value << std::endl;
            std::cout << out.txOutputIdx << std::endl;
            std::cout << out.txOutputId << std::endl;
            std::cout << out.scriptPubKeyReqSig << std::endl;
            std::cout << out.scriptPubKeyAsm << std::endl;
            std::cout << out.scriptPubKeyHex << std::endl;
            std::cout << out.scriptPubKeyType << std::endl;
            std::cout << out.scriptPubKeyAddress << std::endl;
        }
    }
}
   
}