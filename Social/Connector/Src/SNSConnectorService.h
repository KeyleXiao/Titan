/*******************************************************************
** 文件名:	SNSConnectorService.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李有红
** 日  期:	2016-11-30
** 版  本:	1.0
** 描  述:	SNS服务器连接服务			
********************************************************************/

#pragma once

#include "IServiceMgr.h"
#include "IFramework.h"
#include "ISocialGlobal.h"

#include "IMessageDispatch.h"
#include "net/Reactor.h"
#include "net/net.h"

#include "SNSDef.h"
#include "ISNSConnectorService.h"
#include "SocialGlobalHelper.h"
#include "Net/netdef.h"
#include "Intl.h"
#include "ISNSService.h"
#include "ISocialGlobal.h"
#include "ISNSManager.h"
#include "SNSHelper.h"

using namespace SNS;
using namespace rkt;

/**
	职责: 
	1.SNS服务器连接服务
*/
class SNSConnectorService : public ISNSConnectorService, public ISNSMessageHandler, public rkt::EventHandler
{
//#undef	CONNECTOR_NETWORK_USERTYPE
//#define CONNECTOR_NETWORK_USERTYPE		NetworkUserType_User4

protected:
	ISNSConnector *  m_pConnector;

public:
	SNSConnectorService() : m_pConnector()
	{
	}

	virtual ~SNSConnectorService()
	{
		Stop();
	}

	bool Start(const char * ip, int port)
	{
		// 挂接网络事件
		Reactor * reactor = gSocialGlobal->getFramework()->get_reactor();
		if ( reactor==0 )
			return false;

		HANDLE hNetEvent = GetNetworkEventHandle(NetworkUserType_User1);
		reactor->AddEvent(hNetEvent);
		reactor->RegisterEventHandler( hNetEvent,this);

		m_pConnector = CreateSNSConnectorProc((TimerAxis*)gSocialGlobal->getTimerService()->getAxis());
		if ( m_pConnector==0 )
		{
			ErrorLn(_GT("创建SNS连接器失败!"));
			return false;
		}

		m_pConnector->Create(getThisGameWorldID(), MSG_ENDPOINT_SOCIAL, gSocialGlobal->getServerID(),_GT("社会服务器"));
		if(!m_pConnector->ConnectServer(ip,port, NetworkUserType_User1))
		{
			TraceLn(_GT("连接SNS服务器失败!"));
			return false;
		}
		TraceLn(_GT("连接SNS服务器成功!"));

		// 订阅 要求更新实时数据事件
		//m_pConnector->registerEventHandler(EVENT_MG_REAL_TIME_DATA,static_cast<IManagerEventHandler *>(this));
		
		m_pConnector->registerMessageHandler(MSG_MODULEID_SNS, dynamic_cast<ISNSMessageHandler*>(this));

		// SNS连接器服务已启动
		//IEventEngine* pEventEngine = gSocialGlobal->getEventEngine();
		//if(pEventEngine)
		//{
			//pEventEngine->FireExecute(EVENT_SYSTEM_MANAGECONNECTOR_STARTED, SOURCE_TYPE_SYSTEM, 0, NULL, 0);
		//}

		return true;
	}

	void Stop()
	{
		// 管理器连接器服务已关闭
		/*IEventEngine* pEventEngine = gSocialGlobal->getEventEngine();
		if(pEventEngine)
		{
			pEventEngine->FireExecute(EVENT_SYSTEM_MANAGECONNECTOR_STOP, SOURCE_TYPE_SYSTEM, 0, NULL, 0);
		}*/

		// 挂接网络事件
		Reactor * reactor = gSocialGlobal->getFramework()->get_reactor();
		if ( reactor )
		{
			HANDLE hNetEvent = GetNetworkEventHandle(NetworkUserType_User1);
			reactor->RemoveEvent( hNetEvent );
		}

		// 关闭连接器
		if (m_pConnector)
		{
			//m_pConnector->SetServerState(MG_SERVERSTATE_STOPING);// 正在关闭

			// 取消订阅
			m_pConnector->unregisterMessageHandler(MSG_MODULEID_SNS);

			EmphasisLn(_GT("关闭子服务器管理器连接!"));
			m_pConnector->Release();
			m_pConnector = NULL;
		}
	}

	virtual void release()
	{
		Stop();
		delete this;
	}

	virtual void OnEvent( HANDLE event )
	{
		DispatchNetworkEvents(100, NetworkUserType_User1);
	}
	

	//////////////// ISNSConnectorService ////////////////////////////////////////////////////////////
	
	
	//////////////////////ISNSMessageHandler//////////////////////////////////////

	/** SNS服务器发过来的消息
	@param msgId	:消息码
	@param head		:消息头
	@param data		:消息内容
	@param len		:消息长度
	*/
	virtual void HandleSNSMessage(unsigned long msgId, void* head, void* data, size_t len)
	{
		SNetMsgHead* pHead = (SNetMsgHead*)head;

		SNSHelper helper;
		ISNSService* pService = helper.m_ptr;
		if (NULL != pService)
		{
			pService->recv_message(msgId, (const char*)data, len);
		}
	}

	/** 直接发送数据给SNS服务器
	@param msg: 要发送的数据
	*/
	virtual void sendMessage(int nMessageID, void * pData, int nDataLen)
	{
		m_pConnector->SendMessage(nMessageID, pData, nDataLen);
	}
};