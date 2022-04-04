#include "Poller.h"
#include "Channel.h"

using namespace muduo;
using namespace muduo::net;

Poller::Poller(EventLoop* loop) : ownerloop_(loop)
{
}

Poller::~Poller()
{
}