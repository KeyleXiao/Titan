#pragma  once;

#include "Thread.h"

///// �����ж���
//struct IRunnable
//{
//    virtual void run() = 0;
//    virtual void release() = 0;
//};

// ������
struct IProducter : public ::rkt::IRunnable
{

};

// ������
struct IConsumer: public ::rkt::IRunnable
{

};