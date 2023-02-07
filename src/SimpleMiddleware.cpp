#include "SimpleMiddleware.h"

namespace BlockchainIndexer
{

BlockListener::BlockListener()
    : newMessage(false)
    , blocks()
    , blocksMutex()
{
}

BlockListener::~BlockListener()
{
    blocks.clear();
}

void BlockListener::notify(Block& aBlock)
{
    {
        std::lock_guard<std::mutex> tLock(blocksMutex);
        blocks.push_back(aBlock);
    }

    newMessage.store(true);
}

void BlockListener::getMessage(Block& aBlock)
{
    std::lock_guard<std::mutex> tLock(blocksMutex);
    aBlock = blocks.front();
    blocks.pop_front();

    if (blocks.size() == 0u)
    {
        newMessage.store(false);
    }
}

bool BlockListener::haveNewMessage()
{
    return newMessage.load();
}

Middleware::Middleware()
    : subscribers()
{
}

Middleware::~Middleware()
{
    subscribers.clear();
}

void Middleware::publish(Block& aBlock)
{
    for (auto& sub : subscribers)
    {
        sub->notify(aBlock);
    }
}

void Middleware::addSubscriber(std::shared_ptr<BlockListener>& aListener)
{
    subscribers.push_back(aListener);
}

}