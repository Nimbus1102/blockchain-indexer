#include "BlockchainIndexer.h"

namespace BlockchainIndexer
{

BlockchainIndexer::BlockchainIndexer()
    : highestHeight(0)
    , levelDB()
{
}

BlockchainIndexer::~BlockchainIndexer()
{
    levelDB = nullptr;
}

void BlockchainIndexer::init(std::string aDatabaseDirectory)
{
    leveldb::DB* db;
    leveldb::Options options;
    options.create_if_missing = true;
    options.block_cache = leveldb::NewLRUCache(100 * 1048576);
    options.filter_policy = leveldb::NewBloomFilterPolicy(10);
    leveldb::Status status = leveldb::DB::Open(options, aDatabaseDirectory, &db);
    assert(status.ok());

    // initialize databases
    levelDB.reset(db);
}

void BlockchainIndexer::indexBlock(Block& aBlock)
{
    // get current max height
    highestHeight = std::max(highestHeight, aBlock.height); 
    
    // block-height : block-hash
    std::stringstream height_ss;
    height_ss << std::setfill('0') << std::setw(8) << aBlock.height;
    levelDB->Put(leveldb::WriteOptions(), height_ss.str(), aBlock.blockHash);

    // block-hash : block-prevHash-nextHash-merkleRoot-size-weight-height-confirmations-timestamp
    std::stringstream hash_info_ss;
    std::stringstream block_info_ss;
    block_info_ss << 
        "prevhash-" << aBlock.prevBlockHash << 
        "-nexthash-" << aBlock.nextBlockHash <<
        "-merkleroot-" << aBlock.merkleRoot << 
        "-size-" << aBlock.size << 
        "-weight-" << aBlock.weight <<
        "-height-" << aBlock.height << 
        "-confirmations-" << aBlock.confirmations << 
        "-transactionnum-" << aBlock.transactions.size() <<
        "-ts-" << aBlock.timestamp;
    
    hash_info_ss << "block-" << aBlock.blockHash << "-info";
    levelDB->Put(leveldb::WriteOptions(), hash_info_ss.str(), block_info_ss.str());

    for (auto& transaction : aBlock.transactions)
    {
        for (auto& output : transaction.outputs)
        {
            if (output.scriptPubKeyType == "nulldata")
            {
                continue;
            }

            // unspent-transaction-id-idx : value
            std::stringstream transaction_output_idx_val_ss;
            transaction_output_idx_val_ss << "trans-id-" << output.txOutputId << "-idx-" << output.txOutputIdx << "-value";
            std::stringstream transaction_output_val_ss;
            transaction_output_val_ss << std::setfill('0') << std::setw(8) << output.value;
            levelDB->Put(leveldb::WriteOptions(), transaction_output_idx_val_ss.str(), transaction_output_val_ss.str());

            // unspent-transaction-id-idx : address
            std::stringstream transaction_output_idx_address_ss;
            transaction_output_idx_address_ss << "trans-id-" << output.txOutputId << "-idx-" << output.txOutputIdx << "-address";
            levelDB->Put(leveldb::WriteOptions(), transaction_output_idx_address_ss.str(), output.scriptPubKeyAddress);

            // address : transactions
            std::stringstream transaction_output_ss;
            transaction_output_ss << "txo-trans-id-" << output.txOutputId << "-idx-" << output.txOutputIdx;
            std::string currTransactions;
            leveldb::Status status = levelDB->Get(leveldb::ReadOptions(), output.scriptPubKeyAddress, &currTransactions);
            if (status.ok())
            {
                currTransactions = currTransactions + "-" + transaction_output_ss.str();
                levelDB->Put(leveldb::WriteOptions(), output.scriptPubKeyAddress, currTransactions);
            }
            else
            {
                levelDB->Put(leveldb::WriteOptions(), output.scriptPubKeyAddress, transaction_output_ss.str());
            }
        }

        for (auto& input : transaction.inputs)
        {
            if (input.coinbase)
            {
                continue;
            }

            // update address transactions
            std::stringstream transaction_input_ss;
            transaction_input_ss << "spent-trans-id-" << input.txOutputId << "-idx-" << input.txOutputIdx;

            // find address
            std::stringstream transaction_output_idx_address_ss;
            transaction_output_idx_address_ss << "trans-id-" << input.txOutputId << "-idx-" << input.txOutputIdx << "-address";
            std::string address;
            leveldb::Status status = levelDB->Get(leveldb::ReadOptions(), transaction_output_idx_address_ss.str(), &address);
            if (status.ok())
            {
                std::string currTransactions;
                leveldb::Status addressStatus = levelDB->Get(leveldb::ReadOptions(), address, &currTransactions);

                if (addressStatus.ok())
                {
                    currTransactions = currTransactions + "-" + transaction_input_ss.str();
                    levelDB->Put(leveldb::WriteOptions(), address, currTransactions);
                }
            }
        }        
    }    
}

bool BlockchainIndexer::getBlockWithMaxHeight(std::string& aBlockInfo)
{
    return getBlockWithHeight(highestHeight, aBlockInfo);
}

bool BlockchainIndexer::getBlockWithHeight(int aHeight, std::string& aBlockInfo)
{
    std::stringstream height_ss;
    height_ss << std::setfill('0') << std::setw(8) << aHeight;
    std::string blockHash;
    leveldb::Status status = levelDB->Get(leveldb::ReadOptions(), height_ss.str(), &blockHash);
    if (status.ok())
    {
        return getBlockInformation(blockHash, aBlockInfo);
    }
    else
    {
        return false;
    }
}

bool BlockchainIndexer::getBlockInformation(std::string& aBlockHash, std::string& aBlockInfo)
{
    std::stringstream hash_info_ss;
    hash_info_ss << "block-" << aBlockHash << "-info";
    leveldb::Status status = levelDB->Get(leveldb::ReadOptions(), hash_info_ss.str(), &aBlockInfo);
    return status.ok();
}

bool BlockchainIndexer::getTransactionValue(std::string aTransactionId, int aIdx, double& aValue)
{
    std::stringstream transaction_output_idx_val_ss;
    transaction_output_idx_val_ss << "trans-id-" << aTransactionId << "-idx-" << aIdx << "-value";
    std::string val;
    leveldb::Status status = levelDB->Get(leveldb::ReadOptions(), transaction_output_idx_val_ss.str(), &val);

    if (status.ok())
    {
        for (int i = 0; i < val.length(); i++)
        {
            if (val[i] != '0')
            {
                val = val.substr(i);
                break;
            }
        }

        aValue = std::stoi(val);
        return true;
    }
    else
    {
        return false;
    }
}

bool BlockchainIndexer::getAddressTransactions(std::string aAddress, std::string& aHistory)
{
    leveldb::Status status = levelDB->Get(leveldb::ReadOptions(), aAddress, &aHistory);
    return status.ok();
}

}