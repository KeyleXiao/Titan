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

//#include "NetMessageDef.h"
#include "IServiceMgr.h"
#include "IFramework.h"
#include "ITimerService.h"
#include "TimerHandler.h"
#include "TimerAxis.h"
#include "ICenterConnectorService.h"
#include "IGatewayAcceptorService.h"
#include "IGatewayConnector.h"
#include "IServerGlobal.h"
#include "fast_id_map.h"
#include <map>

#include "IEntity.h"
#include "IActorService.h"
#include "net/producer_consumer_queue.h"
#include "net/Reactor.h"
#include "net/net.h"
#include "Alert.h"
#include "IMatchScene.h"
#include "ServerGlobalHelper.h"
#include "Simple_Atom_Lock.h"
#include "MigrateDef.h"

#include "TraceRouteMessageDef.h"
#include "INetMessageHandler.h"
#include "IMigrateService.h"
#include "ISceneManager.h"
#include "IActorMgr.h"

using namespace Gateway;

extern "C" __declspec(dllimport) IServiceStub * CreateActorService();

/**
	职责: 
	1.接受用户连接，创建Actor服务
	2.将收到的网络消息，转发给Actor处理
	3.根据UID,获取Actor代理接口
*/
class GatewayAcceptorService : public IGatewayAcceptorService,public IGatewayMessageHandler,public EventHandler
{
private:
	IGatewayConnectorManager *				m_pGatewayConnectors;
	
	typedef std::map<ClientID,SERVICE_ID>	CLIENT_MAP;
	CLIENT_MAP                              m_clientMap;
    string                                  m_strIP;

public:
	GatewayAcceptorService() : m_pGatewayConnectors(0)
	{
        m_strIP.clear();
	}

	void release()
	{
        char szAddr[64] = {0};
        sprintf_s(szAddr, "0x%p", this);
        EmphasisLn(__FILE__ << ", " << __FUNCTION__ << " destroy, addr=" << szAddr);

		delete this;
	}

	bool Start(string strIP)
	{
        m_strIP = strIP;

		m_pGatewayConnectors = CreateGatewayManagerProc(this,(TimerAxis*)gServerGlobal->getTimerService()->getAxis());
		if ( m_pGatewayConnectors==0 )
			return false;

		// 挂接网络事件
		Reactor * reactor = gServerGlobal->getFramework()->get_reactor();
		if ( reactor==0 )
			return false;

		HANDLE hNetEvent = GetNetworkEventHandle();
		reactor->AddEvent(hNetEvent);
        reactor->RegisterEventHandler( hNetEvent,this, __FUNCTION__);

        // 启动会有eventhander内存坏掉的问题,加日志
        char szAddr[64] = {0};
        sprintf_s(szAddr, "0x%p", this);
        EmphasisLn(__FILE__ << ", " << __FUNCTION__ << " event hander=" << szAddr);

		// 网关连接器服务已启动
		IEventEngine* pEventEngine = gServerGlobal->getEventEngine();
		if(pEventEngine)
		{
			pEventEngine->FireExecute(EVENT_SYSTEM_GATEWAYCONNECTOR_STARTED, SOURCE_TYPE_SYSTEM, 0, NULL, 0);
		}

		return true;
	}

	bool Stop()
	{
		// 网关连接器服务已关闭
		IEventEngine* pEventEngine = gServerGlobal->getEventEngine();
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
		Reactor * reactor = gServerGlobal->getFramework()->get_reactor();
		if ( reactor ) 
		{
			reactor->RemoveEvent( GetNetworkEventHandle() );
		}
		return true;
	}

	virtual void OnEvent( HANDLE event )
	{
		DispatchNetworkEvents(100,NetworkUserType_Default);
	}

	/////////// IGatewayAcceptorService ///////////////////////////////////////////////////////////////
	/**
	连接某个网关 : 这个函数可以多次调用，因为存在多个网关
	通常调用的时机是:连接中心服成功后，发现了网关服务器，就调用一下连接
	*/
	virtual bool connectGateway( string ip,int port )
	{
        if(m_strIP.size() <  2)
            m_strIP = ip;

		m_pGatewayConnectors->RegisterServer(MSG_ENDPOINT_SCENE,gServerGlobal->getCenterConnectorService()->getLocalServerCSID());

		return m_pGatewayConnectors->ConnectGatewayServer(m_strIP.c_str(),port);
	}

    virtual void sendData( ClientID client,PACKAGE_PTR package,int nFlag )
    {
        SNetMsgHead* pHead = (SNetMsgHead*)package->c_str();

#		ifdef ENTITY_MESSAGE_ROUTE_TIME_HACK
        if(pHead->byKeyModule == MSG_MODULEID_ENTITY)
		{
			SMsgEntityHead *pEHead = (SMsgEntityHead*) (pHead+1);
			pEHead->dwGateConnectTickSC = getTickCount();
		}
#		endif

		m_pGatewayConnectors->SendData(client,(void*)package->c_str(),package->size(),nFlag);
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

	/////////// GatewayAcceptorService ///////////////////////////////////////////////////////////////
	/**
	@name         : 网关服务器通知游戏服务器有一个新的客户端连进来
	@param  client: 该客户端在网关服务器上的ID
	@return       :
	*/
	virtual void HandleClientIncoming(ClientID client)
	{
		// 暂时把这个玩家设置到当前场景
		//m_pGatewayConnectors->MigrateClient(client,0,0,0,0);
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
		if ( pData==0 || wDataLen<sizeof(SNetMsgHead) )
			return;

		SNetMsgHead * pHead = (SNetMsgHead *)pData;

        // 实体模块相关消息统一到IActorService里面处理
        if ( pHead->byKeyModule==MSG_MODULEID_ENTITY )
        {
            if(pHead->byKeyAction == MSG_TRACE_ROUTE)
            {
                SMsgTraceRoute* pMsg = (SMsgTraceRoute*)((char*)pData + sizeof(SGameMsgHead) + sizeof(SMsgEntityHead));
                pMsg->aTick[TR_SERVER_ARRIVED] = getTickCount();
            }

#		ifdef ENTITY_MESSAGE_ROUTE_TIME_HACK
			SMsgEntityHead *pEHead = (SMsgEntityHead*)(pHead + 1);
			pEHead->dwGateConnectTickCS = getTickCount();
#		endif

            IActorMgr* pActorMgr = gServerGlobal->getActorMgr();
            assert(pActorMgr != nullptr);
			if( !pActorMgr )
				return;

            SERVICE_PTR pContainer = pActorMgr->getActorByClientID(client);
            if ( pContainer!=0 )
            {
                IActorService * pActor = (IActorService*)pContainer->query_interface();
                if ( pActor!=0 )
                {
					// 对移动数据包进行特别的去重投递
					if ( pHead->bySrcEndPoint==MSG_ENDPOINT_CLIENT && pHead->byKeyModule == MSG_MODULEID_ENTITY )
					{
							SMsgEntityHead * pEntityHead = (SMsgEntityHead*)(pHead+1);
							if ( pEntityHead->byPartID == PART_MOVE )
							{
								unsigned long ulKey = PART_MOVE;
								ulKey = (ulKey << 8) | (pHead->byKeyAction);
								pActor->handleReplaceableMessage( ulKey,pData,wDataLen);
								return;
							}
					}

                    pActor->handleMessage( pData,wDataLen );
                }
            }
        }
        else
        {
            // 非实体模块消息处理
			gServerGlobal->getNetMessageHandler()->handleReceivedData(client, pData, wDataLen);
        }
	}

	/**
	@name         : 网关服务器通知游戏服务器有连接断开
	@param client : 断开连接的客户端ID
	@param dwReason:断开的原因或错误码
	*/
	virtual void HandleClientDisconnect(ClientID client,DWORD dwReason)
	{
		IActorMgr* pActorMgr = gServerGlobal->getActorMgr();
		assert(pActorMgr != nullptr);
		if( !pActorMgr )
			return;

        SERVICE_PTR pContainer = pActorMgr->getActorByClientID(client);
        if(pContainer != 0)
        {
            IActorService* pActorService = (IActorService*)pContainer->query_interface();
            if(pActorService != NULL)
            {
                pActorService->onClientDisconnect();
            }
        }

        rkt::IEventEngine* pEventEngine = gServerGlobal->getEventEngine();
        if (pEventEngine)
        {
            pEventEngine->FireExecute(EVENT_SYSTEM_CLIENT_DISCONNECT, SOURCE_TYPE_SYSTEM, client, NULL, 0);
        }
	}


	void onMessage_ExitGame( ClientID client)
	{
		TraceLn("ClientID="<< client <<" begin to logout！");
		kickOutClient(client, 0);
	}

	/**
	踢玩家下线
	*/
	virtual void kickOutClient( ClientID clientID, int nReason )
	{
		if (clientID == 0)
		{
			return;
		}

		m_pGatewayConnectors->KickoutClient(clientID, nReason);
	}

    // 获取sessionID(为了判断客户端clientID是否已经复用)
    virtual SessionID getSession(ClientID client)
    {
        return m_pGatewayConnectors->getSessionID(client);
    }

    virtual bool isSessionUsed(SessionID session)
    {
        return m_pGatewayConnectors->isSessionUsed(session);
    }

    virtual void migrateClient(ClientID client, SessionID session, int nZoneServerID, int nSceneID)
    {
        m_pGatewayConnectors->MigrateClient(client, session, nZoneServerID, nSceneID);
    }

    // 获取IP地址
    virtual string getIpAddress(ClientID client)
    {
        return m_pGatewayConnectors ? m_pGatewayConnectors->GetClientIP(client) : "";
    }

    virtual void plug(ClientID client, SERVICE_ID id)
    {
        m_clientMap[client] = id;
    }

    virtual void unplug(ClientID client)
    {
        m_clientMap.erase(client);
    }
};
