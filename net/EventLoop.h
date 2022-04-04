#ifndef MUDUO_NET_EVENTLOOP_H
#define MUDUO_NET_EVENTLOOP_H

#include <atomic>
#include <functional>
#include <vector>

#include <boost/any.hpp>

#include "../base/Mutex.h"
#include "../base/CurrentThread.h"
#include "../base/Timestamp.h"

#include "Callbacks.h"
#include "TimerId.h"

namespace muduo
{
    namespace net
    {
        class Channel;
        class Poller;
        class TimerQueue;   //时间

        /// Reactor, at most one per thread.
        ///
        /// This is an interface class, so don't expose too much details.
        //one loop per thread 每个线程只能有一个EventLoop对象

        class EventLoop :noncopyable
        {
            public:
                typedef std::function<void()> Functor;
            
                EventLoop();
                ~EventLoop();

                /*
                    Loops forever.
                    Must be called in the same thread as creation of the object.
                */
                void loop();

                /*
                    Quits loop.
                    This is not 100% thread safe, if you call through a raw pointer,
                    better to call through shared_ptr<EventLoop> for 100% safety.
                */
                void quit();

                
                Timestamp pollReturnTime() const { return pollReturnTime_; }

                int64_t iteration() const { return iteration_; }


                void runInLoop(Functor cb);
                
                void queueInLoop(Functor cb);

                size_t queueSize() const;

                
                TimerId runAt(Timestamp time, TimerCallback cb);
                
                TimerId runAfter(double delay, TimerCallback cb);
                
                TimerId runEvery(double interval, TimerCallback cb);
                
                void cancel(TimerId timerId);

                // internal usage
                void wakeup();
                void updateChannel(Channel* channel);
                void removeChannel(Channel* channel);
                bool hasChannel(Channel* channel);

                
                void assertInLoopThread()
                {
                    if(!isInLoopThread())
                    {
                        abortNotInLoopThread();
                    }
                }

                bool isInLoopThread() const
                {
                    return threadId_ == CurrentThread::tid();
                }

                bool eventHandling() const { return eventHandling_; }

                void setContext(const boost::any& context)
                { context_ = context; }

                const boost::any& getContext() const
                { return context_; }

                boost::any* getMutableContext()
                { return &context_; }


                static EventLoop* getEventLoopOfCurrentThread();
                
            private:
                void abortNotInLoopThread();
                void handleRead();  // waked up
                void doPendingFunctors();

                void printActiveChannels() const; // DEBUG

                typedef std::vector<Channel*> ChannelList;

                bool looping_; /* atomic */
                std::atomic<bool> quit_;
                bool eventHandling_; /* atomic */
                bool callingPendingFunctors_; /* atomic */
                int64_t iteration_;
                const pid_t threadId_;     //本线程所属的线程
                Timestamp pollReturnTime_;
                std::unique_ptr<Poller> poller_;
                std::unique_ptr<TimerQueue> timerQueue_;
                int wakeupFd_;
                // unlike in TimerQueue, which is an internal class,
                // we don't expose Channel to client.
                std::unique_ptr<Channel> wakeupChannel_;
                boost::any context_;

                // scratch variables
                ChannelList activeChannels_;
                Channel* currentActiveChannel_;

                mutable MutexLock mutex_;
                std::vector<Functor> pendingFunctors_ GUARDED_BY(mutex_);
        };
    }
}

#endif