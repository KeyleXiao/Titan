/*******************************************************************
** 文件名:	CallbackProxy.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	04/29/2015
** 版  本:	1.0
** 描  述:	跨协程回调代理类

			
********************************************************************/

#pragma once

#include "Framework.h"
#include "IServiceContainer.h"
#include "WorkThreadScheduler.h"
#include <list>
#include <string>
using namespace rkt;

//struct ICallbackProxy
//{
//	virtual void Do(SERVICE_MESSAGE * pMsg) = 0;
//};

/**
	跨协程回调代理类
	1.定时器回调事件回调，以前在单线程框架下可以自由使用，而如果需要跨线程回调，则需要用这个类进行包装
*/

template<typename InterfaceType/*回调接口类型*/>
class CallbackProxy : public InterfaceType,public ICallbackProxy
{
protected:
	volatile LONG		m_dwRef;				// 这个变量的地址据说要32位对齐

	IServiceContainer *  m_pContainer;          // 因为容器释放时，必须保证Kill所有Timer,所以这里存放指针是安全的
	InterfaceType     *  m_pRealHandler;        // 当应用层删除回调接口后，必须把m_pRealHandler设置成0

#	ifdef FRAMEWORK_DEBUG
	std::string          m_strDebugInfo;
#endif
	//SERVICE_MESSAGE      m_Message;				// 不用每次都创建Message
    SERVICE_MESSAGE*     m_pMessage;            // do_message打印信息里需要消息有debug信息

    const char* m_strPrefix;

public:
	void Release()
	{
		if ( ::InterlockedDecrement(&m_dwRef)==0 )
		{
            free(m_pMessage);
			delete this;
		}
	}

	void AddRef()
	{
		::InterlockedIncrement(&m_dwRef);
	}

    void DecRef()
    {
        ::InterlockedDecrement(&m_dwRef);
    }

	CallbackProxy() : m_dwRef(0),m_pContainer(0),m_pRealHandler(0), m_strPrefix("callback-")
	{
		ICallbackProxy * pCallback = dynamic_cast<ICallbackProxy *>(this);

        m_pMessage = (SERVICE_MESSAGE*)malloc(sizeof(SERVICE_MESSAGE) + 128);
        memset(m_pMessage, 0, sizeof(SERVICE_MESSAGE) + 128);
        m_pMessage->from = INVALID_SERVICE_ID;
        m_pMessage->to = (SERVICE_ID)(INT_PTR)pCallback;
        m_pMessage->session = MSG_SESSION_CALLBACK;
        m_pMessage->is_request = true;
        m_pMessage->need_response = false;
        m_pMessage->context_len = 128;
        memcpy(m_pMessage->context, m_strPrefix, strlen(m_strPrefix));

        /*
		m_Message.from = INVALID_SERVICE_ID;
		m_Message.to   = (SERVICE_ID)(INT_PTR)pCallback;
		m_Message.session = MSG_SESSION_CALLBACK;
		m_Message.is_request = true;
		m_Message.need_response = false;
		m_Message.context_len = 0;
        */
	}

	virtual ~CallbackProxy(){}

	bool Init( InterfaceType * pRealHandler,const char * szDebugInfo )
	{
		RUNNING_CONTEXT * pRunning = WorkThreadScheduler::getInstance().get_running();
		SERVICE_PTR pServiceCall = ServiceMgr::getInstance().get_service(pRunning->call_service);
		if ( pServiceCall==0 )
		{
			ErrorLn("Create Callback Proxy Failed! can not get container info");
			return false;
		}

		m_pContainer   = pServiceCall;
		m_pRealHandler = pRealHandler;

        if(!szDebugInfo)
        {
            szDebugInfo = typeid(*pRealHandler).name(); 
        }

		if ( szDebugInfo ) 
        {
            size_t nPrefixLen = strlen(m_strPrefix);
            size_t nSize = strlen(szDebugInfo);
            if(nSize > 128 - nPrefixLen)
            {
                nSize = 128 - nPrefixLen; // message 的 context其实有129字节
            }
            memcpy(m_pMessage->context + nPrefixLen, szDebugInfo, nSize);

            m_strDebugInfo = szDebugInfo;
        }

#if defined(FRAMEWORK_DEBUG) && defined(VERSEION_DEBUG)
        sstrcpyn(m_pMessage->szName, m_strDebugInfo.c_str(), sizeof(m_pMessage->szName));
#endif

		AddRef();

		return true;
	}


	// 发送回调
	void PostCallback()
	{
		if ( m_pRealHandler==0 )   // 该定时器已经删除
			return; 

		AddRef();

		m_pContainer->recv_message( /*&m_Message*/m_pMessage,false );
	}

    std::string GetDebugString()
    {
        return m_strDebugInfo;
    }

	//virtual void Do()
	//{
	//	// 已删除
	//	if ( m_pRealHandler==0 )
	//		return;

	//	m_pRealHandler->OnTimer( m_dwTimerID );
	//	Release();
	//}
};