#ifndef MUDUO_NET_TIMERQUEUE_H
#define MUDUO_NET_TIMERQUEUE_H

#include <set>
#include <vector>

#include "../base/Mutex.h"
#include "../base/Timestamp.h"
#include "Callbacks.h"
#include "Channel.h"

namespace muduo
{
    namespace net
    {
        class EventLoop;
        class Timer;   
        class TimerId;

        /*
            A best efforts timer queue.
            No guarantee that the callback will be on time.
        */
       class TimerQueue : noncopyable
       {
           public:
                //只有一个构造函数,即Eventloop类型,原因涉及到muduo的整个事件分发机制
                explicit TimerQueue(EventLoop * loop);
                ~TimerQueue();
                
                //addTimer实际上就是注册了一个回调
                TimerId addTimer(TimerCallback cb ,  //用户自定义回调
                                 Timestamp when   ,  //何时触发
                                 double interval);   //重复触发间隔 小于0不重复触发
                void cancel(TimerId timerId);
            private:
            /*
                FIXME: use unique_ptr<Timer> instead of raw pointers.
                This requires heterogeneous comparison lookup (N3465) from C++14
                so that we can find an T* in a set<unique_ptr<T>>.
            */
            typedef std::pair<Timestamp, Timer*> Entry;
            typedef std::set<Entry> TimerList;
            typedef std::pair<Timer*, int64_t> ActiveTimer;
            typedef std::set<ActiveTimer> ActiveTimerSet;

            void addTimerInLoop(Timer* timer);
            void cancelInLoop(TimerId timerId);
            // called when timerfd alarms
            void handleRead();
            // move out all expired timers
            std::vector<Entry> getExpired(Timestamp now);
            void reset(const std::vector<Entry>& expired, Timestamp now);

            bool insert(Timer* timer);

            EventLoop* loop_;
            const int timerfd_;
            Channel timerfdChannel_;
            // Timer list sorted by expiration
            TimerList timers_;

            // for cancel()
            ActiveTimerSet activeTimers_;
            bool callingExpiredTimers_; /* atomic */
            ActiveTimerSet cancelingTimers_;
       };
    }  //namespace net
} //namespace muduo

#endif