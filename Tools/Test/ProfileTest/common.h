#pragma  once
// Windows 头文件:
#include <windows.h>
#include <assert.h>
#include <math.h>


typedef unsigned int			uint,uint32;
typedef unsigned long           ulong;

#define ASSERT                  assert

#define TEST_BUFF_SIZE          2
#define PRODUCT_THREAD_COUNT    16
#define CONSUME_THREAD_COUNT    16
#define THREAD_RUN_EXPIRED      30000



inline DWORD getTickCount()
{
   return ::GetTickCount();
}



inline int rkt_rand(ulong* _seek)
{
    // [0, 0x7fff]
    if (_seek)
        return ((( (*_seek) = (*_seek) * 214013L + 2531011L) >> 16) & 0x7fff); // 微软的随机算法
    return ::rand();
}

inline int rkt_range_rand(int _min, int _max, ulong* _seek)
{
    // [_min, _max]
    if (_max > _min)
    {
        ulong rnd = rkt_rand(_seek);
        //double r = (double)rnd / (RAND_MAX+1);
        double r = (double)rnd * 0.000030517578125;
        return (int)::floor(r * (_max - _min + 1)) + _min;
    }
    else if (_max < _min)
    {
        ulong rnd = rkt_rand(_seek);
        //double r = (double)rnd / (RAND_MAX+1);
        double r = (double)rnd * 0.000030517578125;
        return (int)::floor(r * (_min - _max + 1)) + _max;
    }

    return _max;
}