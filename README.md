# Blockchain Bitcoin Indexer (C++)

This is a blockchain index project, which is essentially an ETL (Extract, Transform and Load) model that decentralized applications use as a unique method to convert blockchain data and store it in a way that it is highly accessible for the logic of the appplication. The issue that the indexer aims to tackle is fetching transactional / information history that may not be highly inefficient if we were to interact directly with the blockchain.

## 1. Overview

The blockchain indexing application is written in C++, and leverages on LevelDB for on-disk key-value datastore to index blockchain data on disk. The application also implements a key-value database stored in memory, for fast access and retrieval of blockchain data.

The application provides 3 indexes in accessing blockchain data. Mock REST endpoints are included in the description of the indexes and the various callback functions are implemented in the application code (BlockchainIndexerMain.cpp), but the HTTP interfaacing layer is not provided for this project for the sake of simplicity. 

In addition, the indexer is developed such that it only considers a block to be valid when it has x-confirmations, and this value is a configurable parameter. The current configuration is tuned to be at 6, since attackers are unlikely to amass more than 10% of total compute on the Bitcoin network.

## 2. Indexes

The application provides 4 indexes: block index, block transaction index, transaction index and address transaction index.

### 1. Block Index

Block index enables clients to query block data quickly by height and hash, and query blocks sorted by height (decreasing order).

- `/api/blocks`: block[] 
    - BlockchainIndexerMain.cpp::getAllBlocks
- `/api/blocks?maxHeight=`: block
    - BlockchainIndexerMain.cpp::getMaxHeightBlock
- `/api/blocks/{height}`: block
    - BlockchainIndexerMain.cpp::getBlockWithHeight
- `/api/blocks/{hash}`: block 
    - BlockchainIndexerMain.cpp::getBlock

### 2. Block Transactions Index

Block transactions index provides clients the ability to query transactions quickly within a block, given its height or hash.

- `/api/blocks/{hash}/transactions`: transaction[] 
    - BlockchainIndexerMain.cpp::getTransactionsWithHash
- `/api/blocks/{height}/transactions`: transaction[] 
    - BlockchainIndexerMain.cpp::getTransactionsWithHeight

### 3. Transaction Index

Transaction index provides clients the option to query all transactions from the transaction id.

- `/api/transaction/{transaction_id}/transaction`: transaction
    - BlockchainIndexerMain.cpp::getTransactionWithId

### 4. Address Transaction Index

Address transaction index provides clients the option to query transactions using bitcoin addresses. The application provides 3 endpoints, where client can query for the updated UTXO, all Vin from the address, and all Vout from the address.

- `/api/addresses/{address}/transactions`: transaction[]
    - BlockchainIndexerMain.cpp::getAddressTransactions
- `/api/addresses/{address}/input_transactions`: transaction[]
    - BlockchainIndexerMain.cpp::getAddressInputTransactions
- `/api/addresses/{address}/output_transactions`: transaction[]
    - BlockchainIndexerMain.cpp::getAddressOutputTransactions

## 3. System Design

The application have 5 main components:

1. BlockchainReader
2. BlockchainIndexer
3. CacheDatabase
4. SimpleMiddleware
5. TestIndexer

The application is designed to mimick real-time updates received by the bitcoin network. When a new block is added into the blockchain, it is simulated with the **BlockchainReader**. The BlockchainReader digests the blockchain data file, and will persistently and asynchronously send block data using the **SimpleMiddleware** to interested subscribers. The persistence is sent over using a configured period. 

The **BlockIndexer** component and **CacheDatabase** component is instantiated and managed in a separate thread, and processes every new block data they receive. The BlockIndexer leverages on **LevelDB** as our on-disk database, which its underlying implementation is a key-value datastore for fast access and retrieval of blockchain data. The CacheDatabase implements data storage of the blockchain information in memory, and similarly use a key-value implementation to enable fast accessing and retrieval of blockchain information.

The **TestIndexer** component is the application's testing framework, that digests the various test cases to test for block indexing and address indexing accuracies.

The application is compiled and built into a Docker image, which will be executed inside a Docker container.

## 4. Tests

To ensure the application's various functionality, test cases are created and defined from the blockchain data. Tests are also splitfrom address indexing tests and block indexing tests.

For the block indexing tests, there are 12 defined test cases, with edge cases in *test10, test11 and test12* using a non-existent block, block 198 and block 199 respectively. We test our block indexing features by querying for the blocks in our indexing datastore, and comparing the retrieved blocks with our test cases.

For the address indexing tests, there are 6 defined test cases, containing transactions found on the blockchain data. I included edge cases such as non-existent transactions, spent/input transactions and unspent transactions. The tests test our address transaction index for updated transactions, input transactions and output transactions, and ensure that the event sourcing pattern used to accumulate the address transaction history is accurate.

For our test stage to pass, we will require 100% accuracy for both block indexing tests and address indexing tests.

## 5. Continuous Integration

The development of this application leverages on GitHub actions to automate our build and test stages. For our build workflow, ideally we will want to store our built image inside a registry, but we will assume that the pipeline to push the image into a registry will be omitted for this project.

Our test workflow runs the application inside a Docker container and leverages on the TestIndexer to test our application with the various test cases. A successful test stage will log *"Passed block test cases."* and *"Passed address test cases."* at the end.

Author: Niven Sie (sieniven@gmail.com)
