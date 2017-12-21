/*******************************************************************
** 文件名:	TimerHandlerProxy.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	4/29/2015
** 版  本:	1.0
** 描  述:	时间轴回调

			
********************************************************************/


#pragma once

#include "CallbackProxy.h"
#include "ITimerService.h"
#include "Simple_Atom_Lock.h"

/**
	时间轴回调时，需要使用容器的现场回调，这就很麻烦，需要用一个代理对象切换一下
*/
class TimerHandlerProxy : public CallbackProxy<ITimerHandler>
{
protected:
	DWORD               m_dwTimerID;
	DWORD				m_dwCallTimes;
	void * _InfoPtr; // 指向临时数据的指针

    typedef std::list<TimerHandlerProxy*>   PROXY_LIST;
    struct SProxyList
    {
        volatile LONG  lMutex;
        PROXY_LIST listProxy;
        
        SProxyList():lMutex(0)
        {
            listProxy.clear();
        }
    };

public:
	TimerHandlerProxy() : m_dwTimerID(0),m_dwCallTimes(0),_InfoPtr(0){}

	bool Init( ITimerHandler * pRealHandler,DWORD dwTimerID,DWORD dwCallTimes,const char * szDebugInfo )
	{
		void **ppTimerInfo = pRealHandler->GetTimerInfoPtr();
		if ( ppTimerInfo==0 )
        {
            ErrorLn(__FUNCTION__": ppTimerInfo==0 dwTimerID ="<<dwTimerID);
            return false;
        }
			

        SProxyList* pProxyList = new SProxyList;
        if(::InterlockedCompareExchange((ULONG_PTR*)ppTimerInfo, (ULONG_PTR)pProxyList, 0) != 0)
        {
            delete pProxyList;
            pProxyList = (SProxyList*)*ppTimerInfo;
        }

        Simple_Atom_Lock(&pProxyList->lMutex);

		// 已经有这个定时器了
		TimerHandlerProxy* proxy = NULL;
		PROXY_LIST::iterator it = pProxyList->listProxy.begin();
		for ( ;it!=pProxyList->listProxy.end();++it )
		{
			proxy = (*it);
			if ( proxy && proxy->m_dwTimerID==dwTimerID )
            {
                ErrorLn(__FUNCTION__": proxy && proxy->m_dwTimerID==dwTimerID dwTimerID ="<<dwTimerID);
                return false;
            }
				
		}

		if ( !CallbackProxy<ITimerHandler>::Init( pRealHandler,szDebugInfo ) )
        {            
            ErrorLn(__FUNCTION__": CallbackProxy<ITimerHandler>::Init( pRealHandler,szDebugInfo ) dwTimerID ="<<dwTimerID);
            return false;
        }

		m_dwTimerID = dwTimerID;
		m_dwCallTimes = dwCallTimes;

		pProxyList->listProxy.push_back(this);
		return true;
	}

	static TimerHandlerProxy * CleanProxy(ITimerHandler * pRealHandler,DWORD dwTimerID)
	{
		TimerHandlerProxy * pReturn  = 0;

		void **ppTimerInfo = pRealHandler->GetTimerInfoPtr();
		if ( ppTimerInfo==0 )
			return pReturn;

		SProxyList* pProxyList = *(SProxyList**)ppTimerInfo;
		if ( pProxyList==0 )
			return pReturn;

        Simple_Atom_Lock* pLock = new Simple_Atom_Lock(&pProxyList->lMutex);

		PROXY_LIST::iterator it = pProxyList->listProxy.begin();
		for ( ;it!=pProxyList->listProxy.end(); )
		{
			TimerHandlerProxy* pProxy = *it;
            if(pProxy == NULL)
            {
                ErrorLn("timer handler proxy list, element error: pProxy == NULL");
                pProxyList->listProxy.erase(it++);
                continue;
            }
			if (pProxy->m_dwTimerID==dwTimerID )
			{
				pReturn = pProxy;
				pProxy->m_pRealHandler = 0;
				pProxyList->listProxy.erase(it);
				break;
			}
            ++it;
		}

		if ( pProxyList->listProxy.empty() )
		{
            delete pLock; // 在pProxyList->lMutex释放之前解锁。
			delete pProxyList;
			*ppTimerInfo = 0;
		}
        else
        {
            delete pLock;
        }

		return pReturn;
	}

	virtual void ** GetTimerInfoPtr()
	{
		return &_InfoPtr;
	}

	virtual void OnTimer( unsigned long dwTimerID )
	{
		if ( m_dwRef>1 )		   // 上一次的还没执行完，不要再调
			return;

		CallbackProxy<ITimerHandler>::PostCallback();
	}

	virtual void Do(SERVICE_MESSAGE * pMsg);
};