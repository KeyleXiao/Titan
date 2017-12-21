/*******************************************************************
** 文件名:	GatewayAcceptorService.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	11/28/2014
** 版  本:	1.0
** 描  述:	

			用户连接接受服务
********************************************************************/

#pragma once

#include "IServiceMgr.h"
#include "IFramework.h"
#include "ITimerService.h"
#include "TimerHandler.h"
#include "TimerAxis.h"
#include "ICenterConnectorService.h"
#include "IGatewayAcceptorService.h"
#include "IGatewayConnector.h"
#include "ISocialGlobal.h"
#include "net/producer_consumer_queue.h"
#include "net/Reactor.h"
#include "net/net.h"
#include "Alert.h"
#include "INetMessageHandler.h"
using namespace Gateway;

extern "C" __declspec(dllimport) IServiceStub * CreateActorService();

/**
	职责: 
	1.接受用户连接，创建Actor服务
	2.将收到的网络消息，转发给Actor处理
	3.根据角色ID(PDBID),获取Actor代理接口
*/
class GatewayAcceptorService : public IGatewayAcceptorService,public IGatewayMessageHandler,public EventHandler
{
private:
	IGatewayConnectorManager *				m_pGatewayConnectors;


public:
	GatewayAcceptorService()
	{
		m_pGatewayConnectors = 0;
	}

	void release()
	{
		stop();
		delete this;
	}

	bool start()
	{
		m_pGatewayConnectors = CreateGatewayManagerProc(this,(TimerAxis*)gSocialGlobal->getTimerService()->getAxis());
		if ( m_pGatewayConnectors==0 )
			return false;

		// 挂接网络事件
		Reactor * reactor = gSocialGlobal->getFramework()->get_reactor();
        if ( reactor==0 )
            return false;

		HANDLE hNetEvent = GetNetworkEventHandle();
		reactor->AddEvent(hNetEvent);
		reactor->RegisterEventHandler( hNetEvent,this );

		// 网关连接器服务已启动
		IEventEngine* pEventEngine = gSocialGlobal->getEventEngine();
		if(pEventEngine)
		{
			pEventEngine->FireExecute(EVENT_SYSTEM_GATEWAYCONNECTOR_STARTED, SOURCE_TYPE_SYSTEM, 0, NULL, 0);
		}

		return true;
	}

	bool stop()
	{
		// 网关连接器服务已关闭
		IEventEngine* pEventEngine = gSocialGlobal->getEventEngine();
		if(pEventEngine)
		{
			pEventEngine->FireExecute(EVENT_SYSTEM_GATEWAYCONNECTOR_STOP, SOURCE_TYPE_SYSTEM, 0, NULL, 0);
		}

		if ( m_pGatewayConnectors )
		{
			m_pGatewayConnectors->Release();
			m_pGatewayConnectors = 0;
		}

		// 都是临时做法
		Reactor * reactor = gSocialGlobal->getFramework()->get_reactor();
		if ( reactor ) 
		{
			reactor->RemoveEvent( GetNetworkEventHandle() );
		}
		return true;
	}


	/////////// IGatewayAcceptorService ///////////////////////////////////////////////////////////////
	/**
	连接某个网关 : 这个函数可以多次调用，因为存在多个网关
	通常调用的时机是:连接中心服成功后，发现了网关服务器，就调用一下连接
	*/
	virtual bool connectGateway( string ip,int port )
	{
		m_pGatewayConnectors->RegisterServer(MSG_ENDPOINT_SOCIAL,gSocialGlobal->getCenterConnectorService()->getLocalServerCSID());

		return m_pGatewayConnectors->ConnectGatewayServer(ip.c_str(),port);
	}

	virtual void sendData( ClientID client,PACKAGE_PTR package )
	{
		m_pGatewayConnectors->SendData(client,(void*)package->c_str(),package->size());
	}

	/**
	@name         : 向多个客户端广播数据,即使这些客户端不在同一个网关上
	@brief        : 使用这个函数可以显著减少服务器间的数据传输量,因为如果在同一个网关上的客户端只需发送一条消息
	@param pClientArray: 要发送的客户端ID数组
	@param wClientNum  : 客户端数目
	@param pData       : 数据缓冲区地址
	@param wDataLen    : 数据长度
	@return       : 
	*/
	virtual void broadcast(ClientID * pClientArray,WORD wClientNum,LPVOID pData,WORD wDataLen)
	{
		m_pGatewayConnectors->Broadcast(pClientArray, wClientNum, pData, wDataLen);
	}

    virtual SessionID getSession(ClientID client)
    {
        return m_pGatewayConnectors->getSessionID(client);
    }

    virtual SSceneNodeLoc getSceneNodeLoc(ClientID client)
    {
        SSceneNodeLoc loc;
        if(m_pGatewayConnectors->getNodeLocOfClient(client, loc.nZoneServerID, loc.nSceneID))
        {
            return loc;
        }

        return SSceneNodeLoc(); 
    }

	// 获取IP地址
	virtual string getIpAddress(ClientID client)
	{
		return m_pGatewayConnectors ? m_pGatewayConnectors->GetClientIP(client) : "";
	}
	
    /////////// IGatewayMessageHandler ///////////////////////////////////////////////////////////////

	/**
	@name         : 网关服务器通知游戏服务器有一个新的客户端连进来
	@param  client: 该客户端在网关服务器上的ID
	@return       :
	*/
	virtual void HandleClientIncoming(ClientID client)
	{
		gSocialGlobal->getNetMessageHandler()->handleClientIncoming(client);
	}
	

	/**
	@name         : 网关服务器通知游戏服务器有连接断开
	@param client : 断开连接的客户端ID
	@param dwReason:断开的原因或错误码
	*/
	virtual void HandleClientDisconnect(ClientID client,DWORD dwReason)
	{
		gSocialGlobal->getNetMessageHandler()->handleClientDisconnect(client, dwReason);
	}
	
	/**
	@name         : 网关服务器通知游戏服务器有新的数据包到达
	@brief        : 该数据包是客户端发给该游戏服务器的,通过数据包包头的wKeyServer字节指定
	@param client : 发送数据的客户端ID
	@param pData  : 数据缓冲区位置
	@param wDataLen:数据长度
	*/
	virtual void HandleReceivedData(ClientID client,LPVOID pData,DWORD wDataLen)
	{
		gSocialGlobal->getNetMessageHandler()->handleReceivedData(client,pData,wDataLen);		
	}

    /////////// EventHandler ///////////////////////////////////////////////////////////////
    virtual void OnEvent( HANDLE event )
    {
        DispatchNetworkEvents(100,NetworkUserType_Default);
    }

};