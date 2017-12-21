/*******************************************************************
** 文件名:	GuardConnectorService.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2015-08-18
** 版  本:	1.0
** 描  述:	数据监护服务器连接服务			
********************************************************************/

#pragma once

#include "IServiceMgr.h"
#include "IFramework.h"
#include "IServerGlobal.h"

#include "IMessageDispatch.h"
#include "net/Reactor.h"
#include "net/net.h"

#include "Simple_Atom_Lock.h"

using namespace Guard;

//#define GUARD_CONNECTOR_DEBUG

/**
	职责: 
	1.连接数据监护服务器
*/
class GuardConnectorService : public IGuardConnectorService, IGuardMessageHandler, public EventHandler
{
#undef	CONNECTOR_NETWORK_USERTYPE
#define CONNECTOR_NETWORK_USERTYPE		NetworkUserType_User2

protected:
	IGuardConnector *	m_pConnector;
	volatile	LONG	m_mutex;

	// 消息处理者列表
	typedef std::vector<IGuardMessageHandler*>  MESSAGEHANDLERS;
	MESSAGEHANDLERS	m_MsgHandlers[IMAGE_DATA_NAMESPACE_MAX];
	
	// 用于检测用途
	typedef std::map<int, void*> KEY_CONTENT_MAP;
	KEY_CONTENT_MAP		m_arrTest[IMAGE_DATA_NAMESPACE_MAX];
	

public:
	GuardConnectorService() : m_pConnector(),m_mutex(0)
	{
	}

	virtual ~GuardConnectorService()
	{
        char szAddr[64] = {0};
        sprintf_s(szAddr, "0x%p", this);
        EmphasisLn(__FILE__ << ", " << __FUNCTION__ << " destroy, addr=" << szAddr);

	}

	bool Start(const char * ip, int port)
	{
		// 挂接网络事件
		Reactor * reactor = gServerGlobal->getFramework()->get_reactor();
		if ( reactor==0 )
			return false;

		HANDLE hNetEvent = GetNetworkEventHandle(CONNECTOR_NETWORK_USERTYPE);
		reactor->AddEvent(hNetEvent);
        reactor->RegisterEventHandler( hNetEvent,this, __FUNCTION__);

        // 启动会有eventhander内存坏掉的问题,加日志
        char szAddr[64] = {0};
        sprintf_s(szAddr, "0x%p", this);
        EmphasisLn(__FILE__ << ", " << __FUNCTION__ << " event hander=" << szAddr);

		m_pConnector = CreateGuardConnectorProc((TimerAxis *)gServerGlobal->getTimerService()->getAxis());
		if ( m_pConnector==0 )
		{
			ErrorLn("create guard connector object failed!");
			return false;
		}

		DWORD dwGameWorldID = getThisGameWorldID();
		WORD wServerID = gServerGlobal->getServerID();
		char szWorldName[32] = {0};
		ssprintf_s(szWorldName, sizeof(szWorldName), "场景服[%d]", wServerID);

		WarningLn(__FUNCTION__ <<": name="<< szWorldName << ", worldid="<<dwGameWorldID);
		
		if(!m_pConnector->Create(dwGameWorldID,MSG_ENDPOINT_SCENE,wServerID, _GT("场景服务器"), szWorldName))
		{
			ErrorLn("guard connecter create failed!");
			return false;
		}
		if(!m_pConnector->ConnectServer(ip, port, CONNECTOR_NETWORK_USERTYPE))
		{
			ErrorLn("connect guard server failed!");
			return false;
		}
		// 注册模块消息
		m_pConnector->registerMessageHandler(MSG_MODULEID_GUARD, this);

		// 启动成功事件
		IEventEngine * pEventEngine = gServerGlobal->getEventEngine();
		if (pEventEngine)
		{
			pEventEngine->FireExecute(EVENT_SYSTEM_GUARDCONNECTOR_STARTED, SOURCE_TYPE_SYSTEM, 0, NULL, 0);
		}

		return true;
	}

	void Stop()
	{
		// 数据守护连接器服务已关闭
		IEventEngine * pEventEngine = gServerGlobal->getEventEngine();
		if (pEventEngine)
		{
			pEventEngine->FireExecute(EVENT_SYSTEM_GUARDCONNECTOR_STOP, SOURCE_TYPE_SYSTEM, 0, NULL, 0);
		}

		// 挂接网络事件
		Reactor * reactor = gServerGlobal->getFramework()->get_reactor();
		if ( reactor )
		{
			HANDLE hNetEvent = GetNetworkEventHandle(CONNECTOR_NETWORK_USERTYPE);
			reactor->RemoveEvent( hNetEvent );
		}

		if (m_pConnector)
		{
			// 反注册模块消息
			m_pConnector->unregisterMessageHandler(MSG_MODULEID_GUARD);

			EmphasisLn("guard connector is closed!");
			m_pConnector->Release();
			m_pConnector = NULL;
		}

		void *p = NULL;
		for(int i=0; i<IMAGE_DATA_NAMESPACE_MAX; ++i)
		{
			KEY_CONTENT_MAP & debug_map = m_arrTest[i];
			for(KEY_CONTENT_MAP::iterator iter = debug_map.begin(); iter != debug_map.end(); ++iter)
			{
				p = iter->second;
				if(p) free(p);
			}

			debug_map.clear();
		}
	}

	virtual void release()
	{
		delete this;
	}

	////////////// EventHandler ////////////////////////////////////////////////////////////
	virtual void OnEvent( HANDLE event )
	{
		DispatchNetworkEvents(100, CONNECTOR_NETWORK_USERTYPE);
	}
	

	////////////// IGuardConnectorService ////////////////////////////////////////////////////////////
	/** 关闭和主服务器之间的连接
	*/
	virtual void			Close()
	{
		return m_pConnector->Close();
	}

	/** 注册消息处理者
	@param moduleId 模块ID
	@param hander 消息处理者实例
	*/
	virtual bool					RegisterMessageHandler(DWORD dwNameSpace, IGuardMessageHandler* handler)
	{
		if (dwNameSpace<=IMAGE_DATA_NAMESPACE_UNKNOWN || dwNameSpace >= IMAGE_DATA_NAMESPACE_MAX)
		{
			ErrorLn("GuardConnector registerMessageHandler check namespace("<<dwNameSpace<<") range error");
			return false;
		}

		if (handler == NULL)
		{
			return false;
		}

		Simple_Atom_Lock lock(&m_mutex);

		MESSAGEHANDLERS::iterator iter = find(m_MsgHandlers[dwNameSpace].begin(), m_MsgHandlers[dwNameSpace].end(), handler);
		if (iter != m_MsgHandlers[dwNameSpace].end())
		{
			WarningLn("GuardConnector registerMessageHandler check namespace("<<dwNameSpace<<") already registered");
			return false;
		}

		m_MsgHandlers[dwNameSpace].push_back(handler);

		return true;
	}

	/** 注销消息处理者
	@param moduleId 模块ID
	*/
	virtual bool					UnregisterMessageHandler(DWORD dwNameSpace, IGuardMessageHandler* handler)
	{
		if (dwNameSpace <= IMAGE_DATA_NAMESPACE_UNKNOWN || dwNameSpace >= IMAGE_DATA_NAMESPACE_MAX)
		{
			ErrorLn("GuardConnector unregisterMessageHandler check dwNameSpace("<<dwNameSpace<<") range error");
			return false;
		}

		if (handler == NULL)
		{
			return false;
		}

		Simple_Atom_Lock lock(&m_mutex);

		MESSAGEHANDLERS::iterator iter = find(m_MsgHandlers[dwNameSpace].begin(), m_MsgHandlers[dwNameSpace].end(), handler);
		if (iter != m_MsgHandlers[dwNameSpace].end())
		{
			m_MsgHandlers[dwNameSpace].erase(iter);
		}
		return true;
	}


	/** 往数据监护服务器写入数据
	@param nNameSpace    : 镜像数据空间命名 IMAGE_DATA_NAMESPACE
	@param nKey	         : 键值
	@param pData         : 数据
	@param dwLen         : 数据长度
	@return
	*/
	virtual bool					WriteImageDataToServer(IGuardMessageHandler* handler, int nNameSpace, int nKey, const char * pData, DWORD dwLen)
	{
#ifdef GUARD_CONNECTOR_DEBUG
		if(!m_pConnector->WriteImageData(handler,nNameSpace,nKey,pData, dwLen))
		{
			return false;
		}

		KEY_CONTENT_MAP & debug_map = m_arrTest[nNameSpace];
		void * pContext = malloc(dwLen);
		memcpy(pContext, pData, dwLen);
		debug_map[nKey] = pContext;

		return true;
#else
		return m_pConnector->WriteImageData(handler,nNameSpace,nKey,pData, dwLen);
#endif		
	}

	/** 从数据监护服务器异步读取数据
	@param nNameSpace    : 镜像数据空间命名 IMAGE_DATA_NAMESPACE
	@param nKey	         : 键值
	@return
	*/
	virtual bool					ReadImageDataFromServer(IGuardMessageHandler* handler, int nNameSpace, int nKey)
	{
		return m_pConnector->ReadImageData(handler,nNameSpace,nKey);
	}		



	/** 取得本地服务器在中心服务器上的ID
	*/
	virtual DWORD			GetLocalServerCSID()
	{
		return m_pConnector->GetLocalServerCSID();
	}

	/** 是否与服务器断开连接了
	@param   
	@param   
	@return  
	*/
	virtual bool			IsDisconnectToServer(void)
	{
		return m_pConnector->IsDisconnectToServer();
	}

	/** 是否已启动
	@param   
	@param   
	@return  
	*/
	virtual bool			IsStartup() 
	{
		return m_pConnector->IsStartup();
	}

	////////IGuardMessageHandler///////////////////////////////////////////////////////
	/** 数据监护服务器发过来的消息
	@param actionId	:消息码
	@param head		:消息头
	@param data		:消息内容
	@param len		:消息长度
	*/
	virtual void HandleGuardMessage(unsigned long dwMsgID, void* head, void* data, size_t len)
	{
		switch(dwMsgID)
		{
		case MSG_GUARD_READ_DATA_RESULT:
			{
				OnMsgReadImageDataResult(dwMsgID,head,data,len);
			}
			break;
		case MSG_GUARD_WRITE_DATA_RESULT:
			{
				OnMsgWriteImageDataResult(dwMsgID,head,data,len);
			}
			break;
		default:
			{
				ErrorLn("has a message not handle, msg="<<dwMsgID <<", len="<<len);
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////

	void OnMsgReadImageDataResult(unsigned long actionId, void* head, void* data, size_t len)
	{
		if (len < sizeof(SMsgGuardReadDataResult))
		{
			ErrorLn("read guard image data failed because length of message is invalid, len="<< len);
			return;
		}

		SMsgGuardReadDataResult *pMsg = (SMsgGuardReadDataResult*)data;
		if (len != sizeof(SMsgGuardReadDataResult) + pMsg->nDataLen)
		{
			ErrorLn("read guard image data failed because message length is invalid, len="<< len);
			return;
		}

		if (pMsg->nNameSpace <= IMAGE_DATA_NAMESPACE_UNKNOWN || pMsg->nNameSpace >= IMAGE_DATA_NAMESPACE_MAX)
		{
			ErrorLn("read guard image data failed because namespace invalid, ns="<<pMsg->nNameSpace<<", key="<<pMsg->nKey);
			return;
		}

		Simple_Atom_Lock lock(&m_mutex);

		MESSAGEHANDLERS & handles = m_MsgHandlers[pMsg->nNameSpace];
		MESSAGEHANDLERS::iterator iter = find(handles.begin(), handles.end(), (IGuardMessageHandler*)pMsg->nBackPtr);
		if (iter== handles.end())
		{
			return;
		}
		
#	ifdef GUARD_CONNECTOR_DEBUG
		CheckReadImageData(data, len);
#	endif

		// 回调给当前对象执行
		((IGuardMessageHandler*)pMsg->nBackPtr)->HandleGuardMessage(actionId, head, data, len);
	}

	void OnMsgWriteImageDataResult(unsigned long actionId, void* head, void* data, size_t len)
	{
		if (len < sizeof(SMsgGuardWriteDataResult))
		{
			ErrorLn("write guard image data failed because length of message is invalid, len="<< len);
			return;
		}

		SMsgGuardWriteDataResult *pMsg = (SMsgGuardWriteDataResult*)data;
		if (pMsg->nNameSpace <= IMAGE_DATA_NAMESPACE_UNKNOWN || pMsg->nNameSpace >= IMAGE_DATA_NAMESPACE_MAX)
		{
			ErrorLn("write guard image data failed because namespace invalid, namespace="<<pMsg->nNameSpace<<", ken="<<pMsg->nKey);
			return;
		}

		Simple_Atom_Lock lock(&m_mutex);

		MESSAGEHANDLERS & handles = m_MsgHandlers[pMsg->nNameSpace];
		MESSAGEHANDLERS::iterator iter = find(handles.begin(), handles.end(), (IGuardMessageHandler*)pMsg->nBackPtr);
		if (iter== handles.end())
		{
			return;
		}

		// 回调给当前对象执行
		((IGuardMessageHandler*)pMsg->nBackPtr)->HandleGuardMessage(actionId, head, data, len);
	}

	// 检测读取数据有效性
	bool CheckReadImageData(void * data, size_t len)
	{
        if ( len < sizeof(SMsgGuardReadDataResult) )
		{
			ErrorLn( __FUNCTION__ <<": length is invalid, len="<< len <<", sizeof(SMsgGuardReadDataResult)="<<sizeof(SMsgGuardReadDataResult));
			return false;
		}

        SMsgGuardReadDataResult * result = (SMsgGuardReadDataResult *)data;
        if ( len!=sizeof(SMsgGuardReadDataResult)+result->nDataLen )
		{
			ErrorLn( __FUNCTION__ <<": length is invalid, ns="<<result->nNameSpace<<",key="<<result->nKey<<", len="<< len 
				<<", sizeof(SMsgGuardReadDataResult)="<<sizeof(SMsgGuardReadDataResult) <<", nDataLen="<<result->nDataLen);
			return false;
		}
		if( result->nNameSpace<=IMAGE_DATA_NAMESPACE_UNKNOWN || result->nNameSpace>=IMAGE_DATA_NAMESPACE_MAX)
		{
			ErrorLn( __FUNCTION__ <<": namespace is invalid, ns="<< result->nNameSpace);
			return false;
		}
		void * pReturnContext = (void*)(result+1);

		KEY_CONTENT_MAP & debug_map = m_arrTest[result->nNameSpace];
		void * pContext = debug_map[result->nKey];
		if ( memcmp(pReturnContext, pContext, result->nDataLen) == 0 )
		{
            TraceLn( __FUNCTION__ <<": check is ok, ns="<<result->nNameSpace<<",key="<< result->nKey);
			return true;
		}
		else
		{
			ErrorLn( __FUNCTION__ <<": check is failed, ns="<<result->nNameSpace<<",key="<< result->nKey);
			return false;
		}		
	}

};