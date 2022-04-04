#include "Thread.h"
#include "CurrentThread.h"
#include "Exception.h"
#include "Logging.h"

#include <type_traits>

#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/prctl.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <linux/unistd.h>

namespace muduo
{
    namespace detail
    {
        pid_t gettid()
        {
            return static_cast<pid_t>(::syscall(SYS_gettid));
        }

        struct ThreadData
        {
            typedef muduo::Thread::Threadfunc Threadfunc;
            Threadfunc  func_;
            string      name_;
            pid_t*      tid_;
            CountDownLatch*  latch_;

            ThreadData(Threadfunc func , string& name , pid_t* tid , CountDownLatch* latch)
                :func_(std::move(func)) , 
                 name_(name),
                 tid_(tid),
                 latch_(latch)
            { }
             
        };

        void* startThread(void* obj)
        {

        }
    };

    void Thread::start()
    {
        assert(!started_);
        started_ = true;
        detail::ThreadData* data = new detail::ThreadData(func_ , name_ , &tid_ , &latch_);
        if(pthread_create(&pthreadId_ , NULL , &detail::startThread , data))
        {
            started_ = false;
            delete data;
            LOG_SYSFATAL << "Thread.cc(Thread::start()) Failed in pthread_create";
        }
        else
        {
            latch_.wait();
            assert(tid_ > 0);
        }
    }

    int Thread::join()
    {
        assert(started_);
        assert(!joined_);
        joined_ = true;
        return pthread_join(pthreadId_ , NULL);

    }
}