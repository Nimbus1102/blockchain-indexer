#ifndef BLOCKCHAIN_TYPES_H
#define BLOCKCHAIN_TYPES_H

#include <string>
#include <vector>

namespace BlockchainIndexer
{

struct TransactionInput
{
    TransactionInput()
        : txIdx(-1)
        , coinbase(false)
        , sequence(0u)
        , txOutputIdx(-1)
        , txOutputId("")
        , scriptSigAsm("")
        , scriptSigHex("")
    {
    }

    int txIdx;
    bool coinbase;
    uint64_t sequence;

    // UTXO that is being spent
    int txOutputIdx;
    std::string txOutputId;

    // scriptSig
    std::string scriptSigAsm;
    std::string scriptSigHex;
};

struct TransactionOutput
{
    TransactionOutput()
        : value(-1)
        , txOutputIdx(-1)
        , txOutputId("")
        , scriptPubKeyReqSig(-1)
        , scriptPubKeyAsm("")
        , scriptPubKeyHex("")
        , scriptPubKeyType("")
        , scriptPubKeyAddress()
    {
    }

    double value;
    int txOutputIdx;
    std::string txOutputId;

    // scriptPubKey
    int scriptPubKeyReqSig;
    std::string scriptPubKeyAsm;
    std::string scriptPubKeyHex;
    std::string scriptPubKeyType;
    std::string scriptPubKeyAddress;
};

struct Transaction
{
    Transaction()
        : version(-1)
        , lockTime(-1)
        , id("")
        , hash("")
        , inputs()
        , outputs()        
    {
    }

    int idx;
    int version;
    int lockTime;
    std::string id;
    std::string hash;
    std::vector<TransactionInput> inputs;
    std::vector<TransactionOutput> outputs;
};

struct Block
{
    Block()
        : blockHash("")
        , prevBlockHash("")
        , nextBlockHash("")
        , merkleRoot("")
        , size(0)
        , weight(0)
        , height(0)
        , confirmations(0)
        , timestamp(0u)
        , transactions()
    {
    }
    
    std::string blockHash;
    std::string prevBlockHash;
    std::string nextBlockHash;
    std::string merkleRoot;
    int size;
    int weight;
    int height;
    int confirmations;
    uint64_t timestamp;
    std::vector<Transaction> transactions;
};

}

#endif  // BLOCKCHAIN_TYPES_H