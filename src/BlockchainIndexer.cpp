#include "BlockchainIndexer.h"

namespace BlockchainIndexer
{

BlockchainIndexer::BlockchainIndexer()
    : levelDB()
    , cacheDB()
{
}

BlockchainIndexer::~BlockchainIndexer()
{
    levelDB = nullptr;
    cacheDB = nullptr;
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
    cacheDB = std::make_unique<CacheDatabase>();
}

void BlockchainIndexer::indexBlock(Block& aBlock)
{
    // cache block data in memory
    cacheDB->cacheBlock(aBlock);

    // block-height : block-hash
    std::stringstream height_ss;
    height_ss << std::setfill('0') << std::setw(8) << aBlock.height;
    levelDB->Put(leveldb::WriteOptions(), height_ss.str(), aBlock.blockHash);

    // block-hash : block-height
    std::stringstream hash_height_ss;
    hash_height_ss << "block-" << aBlock.blockHash << "-height";
    levelDB->Put(leveldb::WriteOptions(), hash_height_ss.str(), height_ss.str());

    // block-hash : block-size
    std::stringstream hash_size_ss;
    hash_size_ss << "block-" << aBlock.blockHash << "-size";
    std::stringstream block_size_ss;
    block_size_ss << std::setfill('0') << std::setw(8) << aBlock.size;
    levelDB->Put(leveldb::WriteOptions(), hash_height_ss.str(), block_size_ss.str());

    // block-hash : number of transactions
    std::stringstream hash_trans_ss;
    hash_size_ss << "block-" << aBlock.blockHash << "-num-transactions";
    std::stringstream block_trans_ss;
    block_trans_ss << std::setfill('0') << std::setw(8) << int(aBlock.transactions.size());
    levelDB->Put(leveldb::WriteOptions(), hash_trans_ss.str(), block_trans_ss.str());

    for (auto& transaction : aBlock.transactions)
    {
        // transaction-id : block-hash-transaction-idx
        std::stringstream transaction_idx_ss;
        transaction_idx_ss << "block-id-" << aBlock.blockHash << "-trans-idx-" << transaction.idx;
        levelDB->Put(leveldb::WriteOptions(), transaction.id, transaction_idx_ss.str());

        for (auto& output : transaction.outputs)
        {
            if (output.scriptPubKeyType == "nulldata")
            {
                continue;
            }

            // transaction-id-idx : value
            std::stringstream transaction_output_idx_val_ss;
            transaction_output_idx_val_ss << "trans-id-" << output.txOutputId << "-idx-" << output.txOutputIdx << "-value";
            std::stringstream transaction_output_val_ss;
            transaction_output_val_ss << std::setfill('0') << std::setw(8) << output.value;
            levelDB->Put(leveldb::WriteOptions(), transaction_output_idx_val_ss.str(), transaction_output_val_ss.str());

            // transaction-id-idx : address
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
                else
                {
                    std::cout << "Failed to fetch transactions from address." << std::endl;
                }
            }
            else
            {
                std::cout << "Failed to fetch addres from transaction id and idx." << std::endl;
            }            
        }        
    }    
}

bool BlockchainIndexer::getBlockWithHeight(int aHeight, Block& aBlock)
{
    return true;
}

bool BlockchainIndexer::getBlockWithHash(std::string aHash, Block& aBlock)
{

    return true;
}

bool BlockchainIndexer::getBlockTransactions(int aHeight, std::vector<Transaction>& aTransactions)
{

    return true;
}

bool BlockchainIndexer::getBlockTransactions(std::string ahash, std::vector<Transaction>& aTransactions)
{

    return true;
}

bool BlockchainIndexer::getAddressTransactions(std::string aAddress, std::vector<TransactionOutput>& aTransactions)
{

    return true;
}

}