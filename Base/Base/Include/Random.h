/*******************************************************************
** 文件名:	Random.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2016-8-9
** 版  本:	1.0
** 描  述:	多线程安全随机类

			
********************************************************************/

#ifndef __THREAD_SAFE_RANDOM_H__
#define __THREAD_SAFE_RANDOM_H__

#include <random>
#include "Singleton.h"
#include "net/producer_consumer_queue.h"

namespace rkt
{  
    class Random : public Singleton<Random>
    {
    typedef producer_consumer_queue_ex<std::mt19937* > GENERATOR_LIST;
    public:
        Random();
        ~Random();

        // Random integer
        int value();

        void seed(unsigned long nSeed = std::mt19937::default_seed);    
        void randomSeed();

        // Random float between min and max
        float nextFloat(const float & min, const float & max);
        // Random float between 0 and 1
        float nextFloat();

        // Random double between min and max
        double nextDouble(const double & min, const double & max);
        // Random double between 0 and 1
        double nextDouble();

        // Normally distributed random number
        double nextGaussian(const double & mean, const double & sigma);
        // Random int between min and max
        int nextInt(const int & min, const int & max);

        // Random long between min and max
        long nextLong(const long & min, const long & max);
        // Random bool
        bool nextBool();
    

    private:        
        std::mt19937* getEngine();

    private:        
        GENERATOR_LIST          m_generators;
        DWORD                   m_dwTLSIndex;			// 线程专有存储索引  
    };

};

#endif // __THREAD_SAFE_RANDOM_H__