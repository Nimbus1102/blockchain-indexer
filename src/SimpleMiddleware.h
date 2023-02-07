#ifndef SIMPLE_MIDDLEWARE_H
#define SIMPLE_MIDDLEWARE_H

// simple middleware class to help in publishing and subscribing to events / messages.

#include <mutex>
#include <deque>
#include <atomic>
#include <vector>
#include <memory>

#include "BlockchainTypes.h"

namespace BlockchainIndexer
{

class BlockListener
{
public:
    BlockListener();
    ~BlockListener();
    
    void notify(Block& aBlock);
    void getMessage(Block& aBlock);
    bool haveNewMessage();

private:
    std::atomic<bool> newMessage;
    std::deque<Block> blocks;
    mutable std::mutex blocksMutex;
};

class Middleware
{
public:
    Middleware();
    ~Middleware();

    void publish(Block& aBlock);
    void addSubscriber(std::shared_ptr<BlockListener>& aListener);

private:
    std::vector<std::shared_ptr<BlockListener>> subscribers;
};

}

#endif  // SIMPLE_MIDDLEWARE_H