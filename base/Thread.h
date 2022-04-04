#ifndef MUDUO_BASE_THREAD_H
#define MUDUO_BASE_THREAD_H

#include "CountDownLatch.h"
#include "Types.h"
#include "Atomic.h"

#include <functional>
#include <memory>
#include <pthread.h>

namespace muduo
{
    class Thread : noncopyable
    {
         public:
            typedef std::function<void()> Threadfunc;

            explicit Thread(Threadfunc , const string& name = string());
            ~Thread();
            
            void start();
            int join();  //return pthread_join()

            bool started() const 
            {
                return started_;
            }
            pid_t tid() const
            {
                return tid_;
            }

            const string& name() const
            {
                return name_;
            }
            // static int numCreated()
            // {
            //     return numCreated_.get();
            // }

        private:
            void           setDefaultName();
            bool           started_;
            pid_t          tid_;
            string         name_;
            //static         AtomicInt32 numCreated_;
            bool           joined_;
            pthread_t      pthreadId_;
            Threadfunc     func_;
            CountDownLatch latch_;

    };
}

#endif