#include "../base/Logging.h"
#include "../base/Mutex.h"
#include "Channel.h"
#include "Poller.h"
#include "SocketsOps.h"
#include "TimerQueue.h"
#include "EventLoop.h"

#include <algorithm>

#include <signal.h>
#include <sys/eventfd.h>
#include <unistd.h>
#include <assert.h>

using namespace muduo;
using namespace muduo::net;

namespace
{
    __thread EventLoop* t_loopInThisThread = 0;

    const int kPollTimeMs = 10000;

    int createEventfd()
    {
        int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
        if (evtfd < 0)
        {
            LOG_SYSERR << "Failed in eventfd";
            abort();
        }
        return evtfd;
    }

    #pragma GCC diagnostic ignored "-Wold-style-cast"
    class IgnoreSigPipe
    {
        public:
            IgnoreSigPipe()
            {
                ::signal(SIGPIPE, SIG_IGN);
                // LOG_TRACE << "Ignore SIGPIPE";
            }
    };
    #pragma GCC diagnostic error "-Wold-style-cast"

    IgnoreSigPipe initObj;
}  // namespace

EventLoop* EventLoop::getEventLoopOfCurrentThread()
{
    return t_loopInThisThread;
}


EventLoop::EventLoop() 
                    : looping_(false) , 
                    threadId_(CurrentThread::tid()),
                    quit_(false),
                    eventHandling_(false),
                    callingPendingFunctors_(false),
                    iteration_(0),
                    //poller_(Poller::newDefaultPoller(this)),
                    timerQueue_(new TimerQueue(this)),
                    wakeupFd_(createEventfd()),
                    wakeupChannel_(new Channel(this, wakeupFd_)),  //注册了回调
                    currentActiveChannel_(NULL)
{
    LOG_DEBUG << "EventLoop created " << this << " in thread " << threadId_;
    //每个线程只能有一个EventLoop对象,因此需要检查当前线程是否已经创建了其他对象
    /*t_loopInThisThread是什么,这实际上是一个__Thread的对象*/
    if (t_loopInThisThread)
    {
        LOG_FATAL << "Another EventLoop " << t_loopInThisThread
                << " exists in this thread " << threadId_;
    }
    else
    {
        t_loopInThisThread = this;
    }
}

    EventLoop::~EventLoop()
    {
        // LOG_DEBUG << "EventLoop " << this << " of thread " << threadId_
        //           << " destructs in thread " << CurrentThread::tid();
        // wakeupChannel_->disableAll();
        // wakeupChannel_->remove();
        // ::close(wakeupFd_);
        assert(!looping_);
        t_loopInThisThread = NULL;
    }

    void EventLoop::loop()
    {
        assert(!looping_);
        assertInLoopThread();
        looping_ = true;

        ::poll(NULL , 0 ,5*1000);

        // while (!quit_)
        // {
        //     activeChannels_.clear();
        //     pollReturnTime_ = poller_->poll(kPollTimeMs, &activeChannels_);
        //     ++iteration_;
        //     if (Logger::logLevel() <= Logger::TRACE)
        //     {
        //     printActiveChannels();
        //     }
        //     // TODO sort channel by priority
        //     eventHandling_ = true;
        //     for (Channel* channel : activeChannels_)
        //     {
        //     currentActiveChannel_ = channel;
        //     currentActiveChannel_->handleEvent(pollReturnTime_);
        //     }
        //     currentActiveChannel_ = NULL;
        //     eventHandling_ = false;
        //     doPendingFunctors();
        // }

        LOG_TRACE << "EventLoop" << this << "stop looping";
        looping_ = false;
    }

    EventLoop* EventLoop::getEventLoopOfCurrentThread()
    {
        return t_loopInThisThread;
    }
