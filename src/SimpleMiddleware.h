#ifndef SIMPLE_MIDDLEWARE_H
#define SIMPLE_MIDDLEWARE_H

// simple middleware class to help in publishing and subscribing to events / messages.

#include <deque>
#include <atomic>
#include <vector>
#include <memory>

#include "BlockchainTypes.h"

namespace BlockchainIndexer
{

// add message topics here
enum class Topics
{
    NOT_APPLICABLE = 0,
    BLOCKS = 1,
};

class BlockListener
{
public:
    BlockListener();
    ~BlockListener();
    
    void notify(Block& aBlock);
    void getMessage(Block& aBlock);

private:
    std::atomic<bool> newMessage;
    std::deque<Block> blocks;
};

class Middleware
{
public:
    Middleware(Topics aTopic);
    ~Middleware();

    void publish(Block& aBlock);
    void addSubscriber(std::shared_ptr<BlockListener>& aListener);

private:
    Topics topic;
    std::vector<std::shared_ptr<BlockListener>> subscribers;
};

}

#endif  // SIMPLE_MIDDLEWARE_H