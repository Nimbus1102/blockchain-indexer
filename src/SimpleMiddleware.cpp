#include "SimpleMiddleware.h"

namespace BlockchainIndexer
{

BlockListener::BlockListener()
    : newMessage(false)
    , blocks()
{
}

BlockListener::~BlockListener()
{
    blocks.clear();
}

void BlockListener::notify(Block& aBlock)
{
    blocks.push_back(aBlock);
    newMessage.store(true);
}

void BlockListener::getMessage(Block& aBlock)
{
    aBlock = blocks.front();
    blocks.pop_front();
}

Middleware::Middleware(Topics aTopic)
    : topic(aTopic)
    , subscribers()
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