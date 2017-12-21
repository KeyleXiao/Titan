/*******************************************************************
** 文件名:	EventHandlerProxy.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	4/29/2015
** 版  本:	1.0
** 描  述:	事件回调

			
********************************************************************/

#pragma once

#include "CallbackProxy.h"
#include "IEventEngine.h"
#include "buffer.h"
using namespace rkt;

#include "EventRefBase.h"

/**
	时间轴回调时，需要使用容器的现场回调，这就很麻烦，需要用一个代理对象切换一下
*/
class EventHandlerProxy : public CallbackProxy<IEventExecuteSink>
{
protected:
	WORD m_wEventID;
	BYTE m_bSrcType;
	DWORD m_dwSrcID;

	typedef std::list<EventHandlerProxy*>		PROXY_ITEM_LIST;
	typedef std::map<INT_PTR, PROXY_ITEM_LIST>	PROXY_LIST;
	typedef CallbackProxy<IEventExecuteSink>	BASE_TYPE;

public:
	EventHandlerProxy() : m_wEventID(0),m_bSrcType(0),m_dwSrcID(0){}

	bool Init(INT_PTR dwEventCenterID, IEventExecuteSink * pRealHandler,WORD wEventID,BYTE bSrcType,DWORD dwSrcID,const char * szDebugInfo )
	{
		void **ppProxyInfo = pRealHandler->__GetProxyInfoPtr();
		if ( ppProxyInfo==0 )
			return false;

		PROXY_LIST* pProxyList = *(PROXY_LIST**)ppProxyInfo;
		if ( pProxyList==0 )
		{
			pProxyList = new PROXY_LIST;
			*ppProxyInfo = pProxyList;
		}

		PROXY_LIST::iterator iter = pProxyList->find(dwEventCenterID);
		if (iter != pProxyList->end())
		{
			PROXY_ITEM_LIST & itemList = iter->second;
			// 已经有这个定时器了
			PROXY_ITEM_LIST::iterator it = itemList.begin();
			for ( ; it!=itemList.end(); ++it )
			{
				EventHandlerProxy* pProxy = *it;
				if ( pProxy->m_wEventID==wEventID && pProxy->m_bSrcType==bSrcType && pProxy->m_dwSrcID==dwSrcID )
					return false;
			}
		}

		if ( !BASE_TYPE::Init( pRealHandler,szDebugInfo ) )
			return false;

		m_wEventID = wEventID;
		m_bSrcType = bSrcType;
		m_dwSrcID = dwSrcID;

		//pProxyList->push_back(this);
		(*pProxyList)[dwEventCenterID].push_back(this);
		return true;
	}

	static EventHandlerProxy * CloseProxy(INT_PTR dwEventCenterID, IEventExecuteSink * pRealHandler,WORD wEventID,BYTE bSrcType,DWORD dwSrcID)
	{
		void **ppProxyInfo = pRealHandler->__GetProxyInfoPtr();
		if ( ppProxyInfo==0 )
			return 0;

		PROXY_LIST* pProxyList = *(PROXY_LIST**)ppProxyInfo;
		if ( pProxyList==0 )
			return 0;

		EventHandlerProxy * pReturnProxy = 0;

		PROXY_LIST::iterator iter = pProxyList->find(dwEventCenterID);
		if (iter != pProxyList->end())
		{
			PROXY_ITEM_LIST & itemList = iter->second;

			PROXY_ITEM_LIST::iterator it = itemList.begin();
			for ( ; it!=itemList.end(); ++it )
			{
				EventHandlerProxy* pProxy = *it;
				if ( pProxy->m_wEventID==wEventID && pProxy->m_bSrcType==bSrcType && pProxy->m_dwSrcID==dwSrcID )
				{
					pProxy->m_pRealHandler = 0;
					pReturnProxy = pProxy;
					itemList.erase(it);
					break;
				}
			}

            if(itemList.empty())
            {
                pProxyList->erase(dwEventCenterID);
            }
		}

		if ( pProxyList->empty() )
		{
			delete pProxyList;
			*ppProxyInfo = 0;
		}

		return pReturnProxy;
	}

	virtual void OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
	{
		if ( m_pRealHandler==0 )   // 该定时器已经删除
			return; 

		// 类型不符合直接跳出
		if ( m_bSrcType>0 && bSrcType!=m_bSrcType )
			return;

		// 源不符合直接跳出
		if ( m_dwSrcID >0 && dwSrcID!=m_dwSrcID )
			return;

		AddRef();

    	// 执行补充逻辑
		if ( nLen > sizeof(event_base))
		{
			event_base * pbase = (event_base*)pszContext;
			if(pbase->ptrRef)
			{
				pbase->ptrRef->AddRef();
			}
		}

        // 压入事件内容
        size_t nBuffSize = sizeof(WORD) + sizeof(BYTE) + sizeof(DWORD) + sizeof(int) + nLen;
        BUILD_MSG_BUFFER_LEN(IEventExecuteSink::OnExecute, nBuffSize);
        memcpy( pMsg,m_pMessage,sizeof(SERVICE_MESSAGE) );
        t_data << wEventID << bSrcType << dwSrcID << nLen;
        t_data.push_back( pszContext,nLen ); 

		m_pContainer->recv_message( pMsg,false );
	}

	virtual void Do(SERVICE_MESSAGE * pMsg)
	{
		ibuffer in( pMsg->context,pMsg->context_len );

		WORD wEventID = 0;
		BYTE bSrcType = 0;
		DWORD dwSrcID = 0;
		int nLen      = 0;

		in >> wEventID >> bSrcType >> dwSrcID >> nLen;
		char * pContext = in.current();

		// 弹出事件内容
		if(m_pRealHandler)
		{
			m_pRealHandler->OnExecute(wEventID,bSrcType,dwSrcID,pContext,nLen);
		}

		// 执行补充逻辑
		if ( nLen > sizeof(event_base))
		{
			event_base * pbase = (event_base*)pContext;
			if(pbase->ptrRef)
			{
				pbase->ptrRef->Release();
			}
		}

		free(pMsg);

		Release();
	}
};