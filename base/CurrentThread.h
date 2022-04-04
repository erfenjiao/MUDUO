#ifndef MUDUO_BASE_CURRENTTHREAD_H
#define MUDUO_BASE_CURRENTTHREAD_H

#include "Types.h"


namespace muduo
{
    namespace CurrentThread
    {
        // internal
        //线程真实pid缓存,如果每次都调用getpid()获得pid,效率较低
        extern __thread int t_cachedTid;  
        //这是tid的字符串表示形式
        extern __thread char t_tidString[32];
        extern __thread int t_tidStringLength;
        extern __thread const char* t_threadName;
        void cacheTid();

        inline int tid()
        {
            if (__builtin_expect(t_cachedTid == 0, 0))
            {
                cacheTid();
            }
            return t_cachedTid;
        }

        inline const char* tidString()  // for logging
        {
            return t_tidString;
        }

        inline int tidStringLength()    // for logging
        {
            return t_tidStringLength;
        }

        inline const char* name()
        {
            return t_threadName;
        }

        bool isMainThread();                //判断是否是主线程

        void sleepUsec(int64_t usec);       // for testing

        string stackTrace(bool demangle);   //异常类中使用,主要是返回函数栈信息
    }  //namespace CurrenThread
}  //namespace muduo

#endif