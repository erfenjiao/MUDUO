#ifndef MUDUO_BASE_ATOMIC_H
#define MUDUO_BASE_ATOMIC_H

#include "noncopyable.h"

#include <stdint.h>

namespace muduo
{
    namespace detail
    {
        template<typename T>
        class AtomicIntegerT : noncopyable
        {
            public:
                AtomicIntegerT()
                    : value_(0)
                {
                }

                T get()
                {
                    /*
                        原子性比较还有设置操作
                        判断value的值是否为零，如果为0则赋值0
                        返回的是未赋值之前的值，其实就是获取value的值
                    */
                    return __sync_val_compare_and_swap(&value_, 0, 0);
                }

                T getAndAdd(T x)
                {
                    //先获取然后执行加的操作，返回的是未修改的值
                    return __sync_fetch_and_add(&value_, x);
                }

                T addAndGet(T x)
                {
                    //先加然后获取
                    //这里是调用getAndAdd函数，返回之后再加上x
                    return getAndAdd(x) + x;
                }

                T incrementAndGet()
                {
                    //自加1
                    return addAndGet(1);
                }

                T decrementAndGet()
                {
                    //自减1
                    return addAndGet(-1);
                }

                void add(T x)
                {
                    //加法
                    getAndAdd(x);
                }

                void increment()
                {
                    //先加后获取
                    incrementAndGet();
                }

                void decrement()
                {
                    //先减后获取
                    decrementAndGet();
                }

                T getAndSet(T newValue)
                {
                    //返回原来的值然后设置成新值
                    // in gcc >= 4.7: __atomic_exchange_n(&value_, newValue, __ATOMIC_SEQ_CST)
                    return __sync_lock_test_and_set(&value_, newValue);
                }

            private:
                volatile T value_;
                /*
                    volatile关键字确保指令不会因编译器的优化而省略(防止编译器对代码进行优化)
                    每次都从内存读取数据而不是使用保存在寄存器中的备份
                    是为了避免其他线程已经将该值修改
                */
        };
    }  //namespace detail

    //两个实例化，32位和64位的原子性操作
    typedef detail::AtomicIntegerT<int32_t> AtomicInt32;
    typedef detail::AtomicIntegerT<int64_t> AtomicInt64;
}//namespace muduo

#endif