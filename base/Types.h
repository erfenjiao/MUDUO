#ifndef MUDUO_BASE_TYPES_H
#define MUDUO_BASE_TYPES_H

#include <stdint.h>
#include <string.h>    // memset
#include <string>

#ifndef NDEBUG
#include <assert.h>
#endif

namespace muduo
{
    using std::string;

    inline void memZero(void* p, size_t n)
    {
        memset(p, 0, n);
    }

    /*
        类型强转,只能执行up-cast,派生类->基类
        这里模拟了boost库里面的implicit_cast
    */
    template<typename To, typename From>
    inline To implicit_cast(From const &f)
    {
        return f;
    }

    /*
        基类->派生类 , 并检查转换是否安全
    */
    template<typename To, typename From>     // use like this: down_cast<T*>(foo);
    inline To down_cast(From* f)                     // so we only accept pointers
    {
        if(false)
        {
            implicit_cast<From*, To>(0);
        }

        #if !defined(NDEBUG) && !defined(GOOGLE_PROTOBUF_NO_RTTI)
        //dynamic_cast 稽查该类型转换是否安全
        assert(f == NULL || dynamic_cast<To>(f) != NULL);  // RTTI: debug mode only!
        #endif
        return static_cast<To>(f);
    }

}

#endif