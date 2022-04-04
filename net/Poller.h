#ifndef MUDUO_NET_POLLER_H
#define MUDUO_NET_POLLER_H

/*
poller 是 EventLoop 的间接成员,只供其 owner EventLoop 在 IO 线程内调用,因此无需加锁.
其生命值与 EventLoop 相等.
*/
#include <map>
#include <vector>

#include "../base/Timestamp.h"
#include "EventLoop.h"


#include <vector>
#include <map>

struct poller;

namespace muduo
{
    namespace net
    {
        class Channel;

        class Poller
        {
            public:
                typedef std::vector<Channel*> ChannelList;
                Poller(EventLoop* loop);
                virtual ~Poller();

                /// Polls the I/O events.
                /// Must be called in the loop thread.
                virtual Timestamp poll(int timeoutMs, ChannelList* activeChannels) = 0;

                /// Changes the interested I/O events.
                /// Must be called in the loop thread.
                virtual void updateChannel(Channel* channel) = 0;

                /// Remove the channel, when it destructs.
                /// Must be called in the loop thread.
                virtual void removeChannel(Channel* channel) = 0;

                virtual bool hasChannel(Channel* channel) const;

                static Poller* newDefaultPoller(EventLoop* loop);

                void assertInLoopThread() const
                {
                    ownerloop_->assertInLoopThread();
                }
            private:
                EventLoop* ownerloop_;

            protected:
                typedef std::map<int, Channel*> ChannelMap;
                ChannelMap channels_;
                
                
        };
    
        
    }
}




#endif