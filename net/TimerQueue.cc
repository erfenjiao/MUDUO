#ifndef __STDC_LIMIT_MACROS
#define __STDC_LIMIT_MACROS
#endif

#include "TimerQueue.h"

#include "../base/Logging.h"
#include "EventLoop.h"
#include "Timer.h"
#include "TimerId.h"

#include <sys/timerfd.h>
#include <unistd.h>

namespace muduo
{
    namespace net
    {
        namespace detail
        {
            int createTimerfd()
            {
                int timerfd = ::timerfd_create(CLOCK_MONOTONIC,
                                                TFD_NONBLOCK | TFD_CLOEXEC);
                if (timerfd < 0)
                {
                    LOG_SYSFATAL << "Failed in timerfd_create";
                }
                return timerfd;
            }

            struct timespec howMuchTimeFromNow(Timestamp when)
            {
                int64_t microseconds = when.microSecondsSinceEpoch()
                                        - Timestamp::now().microSecondsSinceEpoch();
                if (microseconds < 100)
                {
                    microseconds = 100;
                }
                struct timespec ts;
                ts.tv_sec = static_cast<time_t>(
                    microseconds / Timestamp::kMicroSecondsPerSecond);
                ts.tv_nsec = static_cast<long>(
                    (microseconds % Timestamp::kMicroSecondsPerSecond) * 1000);
                return ts;
            }

            void readTimerfd(int timerfd, Timestamp now)
            {
                uint64_t howmany;
                ssize_t n = ::read(timerfd, &howmany, sizeof howmany);
                LOG_TRACE << "TimerQueue::handleRead() " << howmany << " at " << now.toString();
                if (n != sizeof howmany)
                {
                    LOG_ERROR << "TimerQueue::handleRead() reads " << n << " bytes instead of 8";
                }
            }


            void resetTimerfd(int timerfd, Timestamp expiration)
            {
                // wake up loop by timerfd_settime()
                struct itimerspec newValue;
                struct itimerspec oldValue;
                memZero(&newValue, sizeof newValue);
                memZero(&oldValue, sizeof oldValue);
                newValue.it_value = howMuchTimeFromNow(expiration);            //提供所给时间与现在的差值 大于100微秒
                //expiration到now的时间 大于等于100微秒 也就是说timer_fd触发时一定有可执行的回调 也有一个重置定时器的作用
                int ret = ::timerfd_settime(timerfd, 0, &newValue, &oldValue);  //重新设置定时器,第四个参数可为空
                /*
                    timerfd_settime
                    这是一个定时器是信号驱动还是事件驱动的核心,我们不妨想象,何为信号驱动和事件驱动,
                    TimerWheel和TimerQueue不过是提供了一个添加和执行的功能而已,定时机制仍需我们
                    在代码中展现,而timerfd_settime就是提供这样一个定时机制,用大白话来说,就是告诉
                    系统多长时间后一个给我的eventfd写点东西告诉时间到了!信号驱动呢?无非是使用alarm
                    ,在信号处理函数中再发出一次信号罢了.
                */
                if (ret)
                {
                    LOG_SYSERR << "timerfd_settime()";
                }
            }

            
        }  //namespace detail
    }  //namespace net
}  //namespace muduo

using namespace muduo;
using namespace muduo::net;
using namespace muduo::net::detail;

//只有一个构造函数,即Eventloop类型,原因涉及到muduo的整个事件分发机制
TimerQueue::TimerQueue(EventLoop* loop)
  : loop_(loop),
    timerfd_(createTimerfd()),         //创建一个Timerfd
    timerfdChannel_(loop, timerfd_),   //创建一个channl对象 加入IO线程的Eventloop
    timers_(),
    callingExpiredTimers_(false)       //callingExpiredTimers_,先埋个悬念,下面我们会着重讲这种机制.
{                                      //Channel对象,即==timerfdChannel_==注册回调,涉及到muduo的事件分发机制,
    timerfdChannel_.setReadCallback(
        std::bind(&TimerQueue::handleRead, this));
    // we are always reading the timerfd, we disarm it with timerfd_settime.
    timerfdChannel_.enableReading();
}

TimerQueue::~TimerQueue()
{
    timerfdChannel_.disableAll();
    timerfdChannel_.remove();
    ::close(timerfd_);
    // do not remove channel, since we're in EventLoop::dtor();
    for (const Entry& timer : timers_)
    {
        delete timer.second;
    }
}


TimerId TimerQueue::addTimer(TimerCallback cb,
                             Timestamp when,
                             double interval)
{
    Timer* timer = new Timer(std::move(cb), when, interval);
    loop_->runInLoop(
        std::bind(&TimerQueue::addTimerInLoop, this, timer));
    return TimerId(timer, timer->sequence());
}


void TimerQueue::cancel(TimerId timerId)
{
    //回调
    loop_->runInLoop(
        std::bind(&TimerQueue::cancelInLoop, this, timerId));
}

void TimerQueue::cancelInLoop(TimerId timerId)
{
    loop_->assertInLoopThread();
    assert(timers_.size() == activeTimers_.size());
    ActiveTimer timer(timerId.timer_, timerId.sequence_);
    ActiveTimerSet::iterator it = activeTimers_.find(timer);
    if (it != activeTimers_.end())
    {
        size_t n = timers_.erase(Entry(it->first->expiration(), it->first));
        assert(n == 1); 
        (void)n;
        delete it->first; // FIXME: no delete please
        activeTimers_.erase(it);           //153,156 在timers和activeTimer中直接删除    
    }
    else if (callingExpiredTimers_)
    {
        cancelingTimers_.insert(timer);      //加入删除队列 在被触发时删除
    }
    assert(timers_.size() == activeTimers_.size());
}


void TimerQueue::addTimerInLoop(Timer* timer)
{
    loop_->assertInLoopThread();
    bool earliestChanged = insert(timer);               //插入Timers和activeTimers_(cancel)

    if (earliestChanged)
    {
        resetTimerfd(timerfd_, timer->expiration());   //如果timers中最低的时间限度被更新,就更新一次定时器
    }
}



/*
    关于callingExpiredTimers_,为什么为true才向删除集合中加入呢?我们可以看到在第一个if判断里面实际上已经从现有的集合中删除了事件,
    我们不禁要问,还需要cancelingTimers_吗?答案是肯定的,见下代码,即第三个重要的函数handleRead
*/


/*
    因为触发这个函数的时候我们可以确定Timerfd已经可读,我们可以看到ReadTimerfd先进行读取,因为muduo默认LT,
    然后通过getExpired获取可读集合.下面进入重点,callingExpiredTimers_,为什么要用true和false把这里包起来呢
    ,原因是run中的回调可能执行cancelInLoop, 那时timers中已经没有了要删除的,这样就会丢失信息,所以维护一个
    cancelingTimers_,我们再回到cancelInLoop中,callingExpiredTimers_为true才加入删除集合,即
    cancelingTimers_,什么时候使用呢,我们在handleRead执行完回调执行reset
    原文链接：https://blog.csdn.net/weixin_43705457/article/details/104347817
*/
void TimerQueue::handleRead()
{
    loop_->assertInLoopThread();
    Timestamp now(Timestamp::now());
    readTimerfd(timerfd_, now);    //读取超时时间,因为muduo默认LT 防止多次触发

    std::vector<Entry> expired = getExpired(now);     //获取目前超时的Timer RVO 不必担心效率

    /*
        TimerQueue的工作机制的核心,即getExpired,在触发可读时间后如何取出时间,这也是和时间轮最大的区别之一.
    */
   
    callingExpiredTimers_ = true;
    cancelingTimers_.clear();
    // safe to callback outside critical section
    for (const Entry& it : expired)
    {
        it.second->run();       //执行Timer中回调 ,
    }
    callingExpiredTimers_ = false;

    reset(expired, now);
}


std::vector<TimerQueue::Entry> TimerQueue::getExpired(Timestamp now)
{
    assert(timers_.size() == activeTimers_.size());
    std::vector<Entry> expired;
    Entry sentry(now, reinterpret_cast<Timer*>(UINTPTR_MAX));
    TimerList::iterator end = timers_.lower_bound(sentry);
    assert(end == timers_.end() || now < end->first);
    std::copy(timers_.begin(), end, back_inserter(expired));
    timers_.erase(timers_.begin(), end);

    for (const Entry& it : expired)
    {
        ActiveTimer timer(it.second, it.second->sequence());
        size_t n = activeTimers_.erase(timer);
        assert(n == 1); (void)n;
    }

    assert(timers_.size() == activeTimers_.size());
    return expired;
}


//可重复且不希望删除的的再加入请求队列 
void TimerQueue::reset(const std::vector<Entry>& expired, Timestamp now)
{
    Timestamp nextExpire;

    for (const Entry& it : expired)
    {
        ActiveTimer timer(it.second, it.second->sequence());
        if (it.second->repeat()
            && cancelingTimers_.find(timer) == cancelingTimers_.end())
        {
            it.second->restart(now);
            insert(it.second);
        }
        else
        {
            // FIXME move to a free list
            delete it.second; // FIXME: no delete please
        }
    }

    if (!timers_.empty())
    {
        nextExpire = timers_.begin()->second->expiration();
    }

    if (nextExpire.valid())
    {
        resetTimerfd(timerfd_, nextExpire);
    }
}


bool TimerQueue::insert(Timer* timer)
{
    loop_->assertInLoopThread();
    assert(timers_.size() == activeTimers_.size());
    bool                earliestChanged =    false;
    Timestamp           when            =    timer->expiration();
    TimerList::iterator it              =    timers_.begin();

    /*
        获取队列中定时时间最短的项,即第一个 因为数据结构是set,红黑树有序,
        比较顺序为pair的比较顺序 即先比较first,相同比较second
    */
    if (it == timers_.end() || when < it->first)  //timers中不存在Timer或者定时时间小于最小的那一个
    {
        earliestChanged = true;    //为true说明插入timers后为第一个元素 即更新最小值
    }
    {
        std::pair<TimerList::iterator, bool> result
          =  timers_.insert(Entry(when, timer));  //就算Timestamp一样后面的地址也一定不一样
        assert(result.second); (void)result;      //断言永真
    }  
    {
        std::pair<ActiveTimerSet::iterator, bool> result
          =  activeTimers_.insert(ActiveTimer(timer, timer->sequence()));  // TODO activeTimers是干什么的?
        assert(result.second); (void)result;
    }

    assert(timers_.size() == activeTimers_.size());
    return earliestChanged;
}