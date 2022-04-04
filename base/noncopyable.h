#ifndef MUDUO_BASE_NONCOPYABLE_H
#define MUDUO_BASE_NONCOPYABLE_H

namespace muduo
{

    class noncopyable
    {
        public:
            noncopyable(const noncopyable&) = delete;
            void operator=(const noncopyable&) = delete;
            /*
                1.  这里主要运用了c++11的关键字default和delete。
                delete：表示noncopyable类及其子类都不能进行拷贝构造和赋值的操作；
                default：表示使用默认的构造函数和析构函数
            */

        protected:
            noncopyable() = default;
            ~noncopyable() = default;
    };

}  // namespace muduo

#endif  // MUDUO_BASE_NONCOPYABLE_H
