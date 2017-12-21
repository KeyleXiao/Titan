#pragma  once;

#include "Thread.h"

///// 可运行对象
//struct IRunnable
//{
//    virtual void run() = 0;
//    virtual void release() = 0;
//};

// 生产者
struct IProducter : public ::rkt::IRunnable
{

};

// 消费者
struct IConsumer: public ::rkt::IRunnable
{

};