/*******************************************************************
** 文件名:	BridgeConnectorService.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2015-07-09
** 版  本:	1.0
** 描  述:	桥服务器连接服务			
********************************************************************/

#pragma once

#include "IServiceMgr.h"
#include "IFramework.h"
#include "IServerGlobal.h"

#include "IMessageDispatch.h"
#include "net/Reactor.h"
#include "net/net.h"


using namespace Bridge;


/**
	职责: 
	1.连接桥服务器
*/
class BridgeConnectorService : public IBridgeConnectorService, public IBridgeMessageHandler, public IBridgeEventHandler, public EventHandler
{
#undef	CONNECTOR_NETWORK_USERTYPE
#define CONNECTOR_NETWORK_USERTYPE		NetworkUserType_User3


protected:
	IBridgeConnector *  m_pConnector;

public:
	BridgeConnectorService() : m_pConnector()
	{
	}

	virtual ~BridgeConnectorService()
	{
        char szAddr[64] = {0};
        sprintf_s(szAddr, "0x%p", this);
        EmphasisLn(__FILE__ << ", " << __FUNCTION__ << " destroy, addr=" << szAddr);

		Stop();
	}

	bool Start(const char * ip, int port, bool bOpenBridge)
	{
        if (!bOpenBridge)
        {
            WarningLn(_GT("跨区桥服务器连接禁用!"));
        }
        else
        {
            // 挂接网络事件
            Reactor * reactor = gServerGlobal->getFramework()->get_reactor();
            if ( reactor==0 )
                return false;

            //HANDLE hNetEvent = GetNetworkEventHandle(CONNECTOR_NETWORK_USERTYPE);
            //reactor->AddEvent(hNetEvent);
            //reactor->RegisterEventHandler( hNetEvent,this);

            //////////////////////////////////////////////////////////////////////////
            DWORD dwGameWorldID = getThisGameWorldID();
            string strWorldName;
            strWorldName = "";
            WORD wSenceID = gServerGlobal->getServerID();

            m_pConnector = CreateBridgeConnectorProc((TimerAxis *)gServerGlobal->getTimerService()->getAxis());
            if ( m_pConnector==0 )
            {
                ErrorLn(_GT("创建桥服务器连接器失败!"));
                return false;
            }
            m_pConnector->Create(dwGameWorldID,MSG_ENDPOINT_SCENE,wSenceID, "SceneServer", strWorldName.c_str());

            HANDLE hNetEvent = GetNetworkEventHandle(CONNECTOR_NETWORK_USERTYPE);
            reactor->AddEvent(hNetEvent);
            reactor->RegisterEventHandler( hNetEvent,this, __FUNCTION__);

            // 启动会有eventhander内存坏掉的问题,加日志
            char szAddr[64] = {0};
            sprintf_s(szAddr, "0x%p", this);
            EmphasisLn(__FILE__ << ", " << __FUNCTION__ << " event hander=" << szAddr);

            // 跨区桥服务器连接器连接
            TraceLn(_GT("连接跨区桥服务器:游戏世界ID=")<< dwGameWorldID <<":"<<strWorldName.c_str()<<","<<ip<<":"<<port);
            if(!m_pConnector->Connect(ip, port, CONNECTOR_NETWORK_USERTYPE))
            {
                TraceLn(_GT("连接跨区桥服务器失败!"));
                return false;
            }

            // 全部游戏世界加入事件
            m_pConnector->registerEventHandler(EVENT_BRIDGE_WORLDLIST_ALL,static_cast<IBridgeEventHandler *>(this));
            // 新增游戏世界事件
            m_pConnector->registerEventHandler(EVENT_BRIDGE_WORLDLIST_ADD,static_cast<IBridgeEventHandler *>(this));
            // 修改游戏世界事件
            m_pConnector->registerEventHandler(EVENT_BRIDGE_WORLDLIST_EDIT,static_cast<IBridgeEventHandler *>(this));
        }
		
		// 桥连接器服务已启动
		IEventEngine* pEventEngine = gServerGlobal->getEventEngine();
		if(pEventEngine)
		{
			pEventEngine->FireExecute(EVENT_SYSTEM_BRIDGECONNECTOR_STARTED, SOURCE_TYPE_SYSTEM, 0, NULL, 0);
		}

		return true;
	}

	void Stop()
	{
		// 桥连接器服务已关闭
		IEventEngine* pEventEngine = gServerGlobal->getEventEngine();
		if(pEventEngine)
		{
			pEventEngine->FireExecute(EVENT_SYSTEM_BRIDGECONNECTOR_STOP, SOURCE_TYPE_SYSTEM, 0, NULL, 0);
		}

		// 挂接网络事件
		Reactor * reactor = gServerGlobal->getFramework()->get_reactor();
		if ( reactor )
		{
			HANDLE hNetEvent = GetNetworkEventHandle(CONNECTOR_NETWORK_USERTYPE);
			reactor->RemoveEvent( hNetEvent );
		}

		// 关闭跨区桥服务器连接器
		if (m_pConnector)
		{
			// 全部游戏世界加入事件
			m_pConnector->unregisterEventHandler(EVENT_BRIDGE_WORLDLIST_ALL,static_cast<IBridgeEventHandler *>(this));
			// 新增游戏世界事件
			m_pConnector->unregisterEventHandler(EVENT_BRIDGE_WORLDLIST_ADD,static_cast<IBridgeEventHandler *>(this));
			// 修改游戏世界事件
			m_pConnector->unregisterEventHandler(EVENT_BRIDGE_WORLDLIST_EDIT,static_cast<IBridgeEventHandler *>(this));

			EmphasisLn(_GT("关闭跨区桥服务器连接器!"));
			m_pConnector->Release();
			m_pConnector = NULL;
		}
	}

	virtual void release()
	{
		delete this;
	}

	virtual void OnEvent( HANDLE event )
	{
		DispatchNetworkEvents(100, CONNECTOR_NETWORK_USERTYPE);
	}
	

	////////////// IBridgeConnectorService ////////////////////////////////////////////////////////////
	/** 取得本地服务器在跨区桥服务器上的ID
	*/
	virtual DWORD  GetServerID()
	{
		return m_pConnector ? m_pConnector->GetServerID() : 0;
	}

	/** 取得公共游戏世界ID
	*/
	virtual DWORD GetPublicWorldID()
	{
		return m_pConnector ? m_pConnector->GetPublicWorldID() : 0;
	}

	/** 直接发送数据给跨区桥服务器
	@param pData         : 要发送的数据
	@param dwLen         : 数据长度
	*/
	virtual void  SendData(ibuffer & msg)
	{
		return m_pConnector ? m_pConnector->SendData( msg.data(), msg.size()) : 0;
	}

	////// 常用发数据接口 ////////////////////////////////////////////////////////////////////

	/** 发数据给指定游戏服务器 dwServerID:服务器ID 为0时广播给所有服务器,bZeroBroadcast:为true时打开为0广播,否则无效
	*/
	virtual void SendDataToServer(DWORD dwServerID,ibuffer & msg,bool bZeroBroadcast)
	{
		return m_pConnector ? m_pConnector->SendDataToServer(dwServerID, msg.data(), msg.size(), bZeroBroadcast) : 0;
	}

	/** 广播数据给指定列表游戏服务器 pServerArray:服务器ID列表,wServerNum:列表个数
	*/
	virtual void BroadcastDataToServer(DWORD * pServerArray,WORD wServerNum,ibuffer & msg)
	{
		return m_pConnector ? m_pConnector->BroadcastDataToServer(pServerArray, wServerNum, msg.data(), msg.size()) : 0;
	}

	/**
	@name         : 发服务器扩展信息消息 dwServerID:服务器ID 为0时广播给所有桥服务器连接器,跨区桥服务器连接器同步服务器扩展信息专用
	@brief        :
	*/
	virtual bool SendExtDataToServer(DWORD dwServerID,DWORD dwMsgCode,DWORD dwUserData1,DWORD dwUserData2,DWORD dwUserData3,const char * pData,DWORD dwLen)
	{
		return m_pConnector ? m_pConnector->SendExtDataToServer(dwServerID, dwMsgCode, dwUserData1, dwUserData2, dwUserData3, pData, dwLen) : false;
	}
		
	////// 常用服务器列表相关接口 ////////////////////////////////////////////////////////////////////
	/**  取得服务器ID列表
	@param   DWORD* pServerArray	: 保存找到的ID数组
	@param   WORD dwArrayLen		: 数组长度
	@param   WORD &dwGetCounts		: 找到的个数
	@param   DWORD dwWorldID		: 游戏世界ID，0为全部
	@param   WORD wServerType		: 服务器类型,0为全部
	@return  找到的个数
	*/
	virtual DWORD GetGameServerIDList(DWORD * pServerArray,DWORD dwArrayLen,DWORD &dwGetCounts,DWORD dwWorldID,WORD wServerType)
	{
		return m_pConnector ? m_pConnector->GetGameServerIDList(pServerArray, dwArrayLen, dwGetCounts, dwWorldID, wServerType) : 0;
	}

	/**  取得指定游戏世界的指定类型的服务器ID
	@param   DWORD dwWorldID		: 游戏世界ID
	@param   WORD wServerType		: 服务器类型
	@param   WORD wSubID			: 服务器序号
	@return  找到的服务器ID,为0表示没找到
	*/
	virtual DWORD GetGameServerIDByTypeSubID(DWORD dwWorldID,WORD wServerType,WORD wSubID)
	{
		return m_pConnector ? m_pConnector->GetGameServerIDByTypeSubID(dwWorldID,wServerType,wSubID) : 0;
	}

	////// 常用游戏世界列表相关接口 ////////////////////////////////////////////////////////////////////

	/**  取得游戏世界ID列表
	@param   DWORD* pWorldArray	: 保存找到的ID数组
	@param   WORD dwArrayLen		: 数组长度
	@param   WORD &dwGetCounts		: 找到的个数
	@return  找到的个数
	*/
	virtual DWORD GetGameWorldIDList(DWORD * pWorldArray,DWORD dwArrayLen,DWORD &dwGetCounts)
	{
		return m_pConnector ? m_pConnector->GetGameWorldIDList(pWorldArray,dwArrayLen,dwGetCounts) : 0;
	}

	/**  取得指定游戏世界信息
	@param   DWORD dwWorldID		: 游戏世界ID
	@return  找到的服务器ID,为0表示没找到
	*/
	virtual IBridgeWorldInfo * GetGameWorldInfo(DWORD dwWorldID)
	{
		return m_pConnector ? m_pConnector->GetGameWorldInfo(dwWorldID) : 0;
	}

	////// 常用扩展信息类列表相关接口 ////////////////////////////////////////////////////////////////////

	/** 用服务器ID 取得扩展信息类
	@param   DWORD dwServerID		: 服务器ID
	@return  IBridgePart*	:服务器扩展信息类
	*/
	virtual IBridgePart* GetGameServerPart(DWORD dwServerID)
	{
		return m_pConnector ? m_pConnector->GetGameServerPart(dwServerID) : 0;
	}

	/** 用TypeSubID取得扩展信息类
	@param   DWORD dwWorldID		: 游戏世界ID
	@param   WORD wServerType		: 服务器类型
	@param   WORD wSubID			: 服务器序号
	@return  IBridgePart*	:服务器扩展信息类
	*/
	virtual IBridgePart* GetGameServerPartByTypeSubID(DWORD dwWorldID,WORD wServerType,WORD wSubID)
	{
		return m_pConnector ? m_pConnector->GetGameServerPartByTypeSubID(dwWorldID,wServerType,wSubID) : 0;
	}

	/** 本服务器扩展信息类指针
	@return  IBridgePart*	:服务器扩展信息类
	*/
	virtual IBridgePart* GetLocalBridgePart()
	{
		return m_pConnector->GetLocalBridgePart();
	}

	// 通过指定服务器发中转消息数据给目标游戏服务器 dwServerID:服务器ID 为0时广播给所有服务器,DWORD dwTransitServerID 中转到服务器ID
	virtual void SendTransitDataToServer(DWORD dwServerID,DWORD dwTransitServerID,const char * pData,DWORD dwLen)
	{
		m_pConnector ? m_pConnector->SendTransitDataToServer(dwServerID,dwTransitServerID,pData,dwLen) : 0;
	}

	///////// IBridgeMessageHandler /////////////////////////////////////////////////////////////////
	/**
	@name         : 处理其他服务器通过跨区桥服务器转发的消息
	@param actionId  : 消息码
	@param bridgeHead  : 桥消息头
	@param head  : 消息头
	@param data  : 消息内容
	@param len:消息长度
	*/
	virtual void HandleBridgeMessage(unsigned long actionId, SMsgBridgeHead &bridgeHead, SGameMsgHead* head, LPSTR data, size_t len)
	{

	}

	///////// IBridgeEventHandler /////////////////////////////////////////////////////////////////
	/** 
	@param   wEventID ：事件ID
	@param   pszContext : 上下文
	@param   nLen : 上下文长度
	@retval buffer 
	*/
	virtual void OnBridgeEventExecute(WORD wEventID, LPSTR pszContext, size_t nLen)
	{

	}


};