/*******************************************************************
** 文件名:	SwappingList.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	01/25/2008
** 版  本:	1.0
** 描  述:	翻转队列
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#ifndef __SWAPPING_LIST_H__
#define __SWAPPING_LIST_H__

#include "Common.h"

#ifdef SUPPORT_NET

#include <list>
#include "Thread.h"
#include "Trace.h"
using namespace rkt;

/**
@name : 翻转队列
@brief: 在多线程生产者-消费者模式中生产者线程向队列添加对象，消费者线程从队列取走对象
        因为涉及到多线程访问，这个队列必须在添加/取走对象时加锁。这个模版实现这种多线程
		安全的队列，另外使用两个队列翻转的技术，可以极大的减少锁互斥的几率
*/

template<typename _OBJ>
class SwappingList
{
public:
	size_t Add(_OBJ & obj )
	{
		::InterlockedIncrement(&m_nCount);
		MultiThread::scopelock lock(m_frontMutex);

		m_frontList->push_back(obj);
		return m_frontList->size();
	}

	bool Get(_OBJ & obj )
	{
		if ( m_nCount==0 )
		{
			return false;
		}

		static unsigned long s_last_trace = 0;
		if ( m_nCount>1000 && GetTickCount()-s_last_trace>10000 )
		{
			s_last_trace = GetTickCount();
			if ( sizeof(_OBJ)==4 )
			{
				WarningLn("The queues that received packet blocking"<<m_nCount );
			}
			else
			{
				WarningLn("The queues that sent packet blocking"<<m_nCount );
			}
		}

		if ( m_backList->empty() )
		{
			Swap();
		}

		if ( m_backList->empty() )
		{
			return false;
		}

		::InterlockedDecrement(&m_nCount);
		obj = m_backList->front();
		m_backList->pop_front();
		return true;
	}

	int Count() 
	{
		return m_nCount;
	}

	void Swap()
	{
		MultiThread::scopelock lock(m_frontMutex);
		
		PRODUCT_LIST * p = m_backList;
		m_backList = m_frontList;
		m_frontList = p;
	}
	
	SwappingList()
	{
		m_frontList = new PRODUCT_LIST;
		m_backList  = new PRODUCT_LIST;

		m_nCount = 0;
	}

	virtual ~SwappingList()
	{
		if ( m_frontList )
		{
			delete m_frontList;
			m_frontList = 0;
		}

		if ( m_backList )
		{
			delete m_backList;
			m_backList = 0;
		}
	}

protected:
	typedef  std::list<_OBJ>   PRODUCT_LIST;
	PRODUCT_LIST    *        m_frontList;
	PRODUCT_LIST    *        m_backList;

	MultiThread::mutex       m_frontMutex;
	volatile LONG            m_nCount;
};

#endif // #ifdef SUPPORT_NET

#endif//__SWAPPING_LIST_H__