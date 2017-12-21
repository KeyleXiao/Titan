/*******************************************************************
** 文件名:	Simple_Atom_Lock.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	10/16/2014
** 版  本:	1.0
** 描  述:	极简原子锁

			
********************************************************************/

#pragma once

#include "thread.h"

// 用原子操作实现的锁，优点是不需要进入内核态，所以速度会非常快
// MODIFY BY CHEN.TAO! 2016.6.6
// 这种原子锁发现有个问题，就是假设AB2个线程都在竞争该锁，可能会出现A一直活得权限，而B一直在等待
class Simple_Atom_Lock
{
protected:
	volatile	LONG * m_MutexBlock;		// 互斥标志
	
	Simple_Atom_Lock(){}

public:
	Simple_Atom_Lock(volatile	LONG * mutex)
	{
		m_MutexBlock = mutex;
		acquire_permission();
	}

	virtual ~Simple_Atom_Lock()
	{
		release_permission();
	}

	// 取得权限
	bool acquire_permission()
	{
		register int  i = 0;
		register int  j = 1000;    // 经测试1000的值最好，少了Sleep的次数会过多，严重影响效率,多了会导致忙等，其他线程拿不到总线，最大等待时间会增加

		DWORD dwThread = ::GetCurrentThreadId();
		if ( dwThread==*m_MutexBlock )				// 允许重入
			return true;

		while ( ::InterlockedCompareExchange(m_MutexBlock,dwThread,0)!=0 )
		{
			if ( ++i>j )
			{
				j+=1000;           // Sleep一次就增加饥渴度，让他下次更容易获得
				i=0;
                WarningLn(__FUNCTION__":sleep");
				Sleep(1);
			}
		}

		return true;
	}

	// 释放权限
	void release_permission()
	{
		*m_MutexBlock = 0;
	}
};

class CAtomLock
{
public:
    CAtomLock():m_mutex(0){}

    void lock()
    {
        int  i = 0;
        while(::InterlockedCompareExchange(&m_mutex, 1, 0) != 0)
        {
            if(++i >= 1000)
            {
                i = -1000;
                WarningLn(__FUNCTION__":sleep");
                Sleep(1);
            }
        }
    }

    void unlock()
    {
        m_mutex = 0;
    }

private:
    volatile LONG m_mutex;
};