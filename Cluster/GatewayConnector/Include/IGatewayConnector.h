/*******************************************************************
** 文件名:	IGatewayConnector.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	02/19/2008
** 版  本:	1.0
** 描  述:	网关连接模块
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#ifndef __INTERFACE_GATEWAY_CONNECTOR_H__
#define __INTERFACE_GATEWAY_CONNECTOR_H__

#include "IGatewayService.h"
#include "GatewayServiceDef.h"
#include "TimerAxis.h"
#include "net/net.h"

namespace Gateway{

	/// 重连周期10秒
#	define RECONNECT_INTERVAL     (10*1000)

	/**
	@name : 网关数据处理器
	@brief: 处理网关发来的数据
	*/
	struct IGatewayMessageHandler
	{
		/**
		@name         : 网关服务器通知游戏服务器有一个新的客户端连进来
		@param  client: 该客户端在网关服务器上的ID
		@return       :
		*/
		virtual void HandleClientIncoming(ClientID client) = 0;
		// GM Chat Client
		virtual void HandleClientIncoming(SMsgGatewayHandshakeGMChatClient& mp){};

		/**
		@name         : 网关服务器通知游戏服务器有新的数据包到达
		@brief        : 该数据包是客户端发给该游戏服务器的,通过数据包包头的wKeyServer字节指定
		@param client : 发送数据的客户端ID
		@param pData  : 数据缓冲区位置
		@param wDataLen:数据长度
		*/
		virtual void HandleReceivedData(ClientID client,LPVOID pData,DWORD wDataLen) = 0;

		/**
		@name         : 网关服务器通知游戏服务器有连接断开
		@param client : 断开连接的客户端ID
		@param dwReason:断开的原因或错误码
		*/
		virtual void HandleClientDisconnect(ClientID client,DWORD dwReason) = 0;

		/**
		@name          : 通知客户端迁移结果
		@param client  : 被迁移的客户端
		@param result  : 迁移的结果现场
		*/
		virtual void HandleMigrateResult(ClientID client,SMsgGatewayMigrateResult * result){}
	};

	/**
	@name : 维护游戏服务器与多个网关服务器之间的联系
	@brief: 具体负责的事物有:
			1.维护和多个网关服务器之间的连接
			2.发送和处理与网关之间交互的数据
	*/
	struct IGatewayConnectorManager
	{
		/**
		@name         : 连接网关服务器
		@brief        : 这个函数可以调用多次,因为可能存在多个网关服务器
		@note         : 在最终的系统的,网关服务器的ip是从中心服务器获得的.发现一个网关就调一下这个函数
		@param   ip   : 网关服务器ip
		@param port   : 网关服务器端口
		@return       : 连接过程中是否出错
		*/
		virtual bool ConnectGatewayServer(const char * ip,WORD port,int nThreadMode = NetworkUserType_Default) = 0;

		/**
		@name         : 向网关服务器注册本地服务器的信息 
		@brief        : 网关会根据该注册信息进行数据转发.
		@note         : (这个函数最好一开始就调用.紧随SetMessageHandler)
		@param wServerType: 服务器的类型.是区域服务器还是帐号服务器等.
		@param dwServerKey: 该服务器对应的中心服务器ID
		@return       : 
		*/
		virtual bool RegisterServer(WORD wServerType,DWORD dwServerKey) = 0;

		/**
		@name         : 发送数据给客户端
		@brief        : 
		@param client : 客户端ID
		@param pData  : 数据缓冲区地址
		@param wDataLen:数据长度
        @param nFlag: SEND_DATA_FLAG
		@return       : 如果该客户端不存在或者对应的网关服务器已断开则返回false,其他返回true
		*/
		virtual bool SendData(ClientID client,LPVOID pData,DWORD dwDataLen,int nFlag = 0) = 0;

		/**
		@name         : 向多个客户端广播数据,即使这些客户端不在同一个网关上
		@brief        : 使用这个函数可以显著减少服务器间的数据传输量,因为如果在同一个网关上的客户端只需发送一条消息
		@param pClientArray: 要发送的客户端ID数组
		@param wClientNum  : 客户端数目
		@param pData       : 数据缓冲区地址
		@param wDataLen    : 数据长度
		@return       : 
		*/
		virtual void Broadcast(ClientID * pClientArray,WORD wClientNum,LPVOID pData,WORD wDataLen) = 0;

		/**
		@name         : 通知网关服务器将客户端连接断开
 		@brief        : 例如用来踢人
		@param client : 客户端ID
		@param dwReason:断开原因,见EKickoutClient_Reason_PersonDataInvalid定义
		@return       : 如果该客户端不存在或者对应的网关服务器已断开则返回false,其他返回true
		*/
		virtual bool KickoutClient(ClientID client,DWORD dwReason) = 0;

        // 通知网关客户端迁移
        virtual bool MigrateClient(ClientID client,SessionID session,int nZoneServerID, int nSceneID) = 0;

		/**
		@name         : 通过网关服务器告诉客户端转连其他服务器
		@brief        :
		@param client : 客户端ID
		@param ip     : 要转连的其他服务器IP
		@param port   : 要转连的其他服务器端口
		@return       : 如果该客户端不存在或者对应的网关服务器已断开则返回false,其他返回true
		*/
		virtual bool RedirectClient(ClientID client,const char * ip,WORD port) = 0;
		
		/**
		@name         : 获取玩家数据
		@brief        : 这个函数主要是为了方便应用层编程而提供的,类似ListCtrl的SetItemData功能
		@param client : 客户端ID
		*/
		virtual DWORD GetClientData(ClientID client) = 0;

		/**
		@name         : 设置玩家数据
		@brief        : 这个函数主要是为了方便应用层编程而提供的,类似ListCtrl的SetItemData功能
		@param client : 客户端ID
		@param dwData : 设置和该客户端关联的数据,内部并不知道这个数据是什么意义
		*/
		virtual void  SetClientData(ClientID client,DWORD dwData) = 0;

		/**
		@name         : 取得客户端IP
		@return       :
		*/
		virtual const char *  GetClientIP(ClientID client) = 0;

		/**
		@name         : 取得客户端连接的网关网络类型
		@return       :
		*/
		virtual DWORD GetClientNetworkType(ClientID client) = 0;

		virtual void  Release() = 0;

		/**
		@name         : 取得客户端IP(DWORD)
		@return       :
		*/
		virtual DWORD GetIP(ClientID client) = 0;

        // 获取sessionID
        virtual SessionID getSessionID(ClientID client) = 0;

        // 某个session是否使用过。
        virtual bool isSessionUsed(SessionID session) = 0;

        // 获取某个客户端的场景服节点位置
        virtual bool getNodeLocOfClient(ClientID client, int& nZoneServerID, int& nSceneID) = 0;
	};

	/**
	@name            : 创建一个网关连接管理对象
	@brief           :
	@param pHandler  : 数据处理接口
	@param pTimerAxis: 时间轴接口,主要用来定时重试
	@return          : 返回网关管理接口指针
	*/
	//GATEWAY_API IGatewayConnectorManager * CreateGatewayManager(IGatewayMessageHandler * pHandler,rkt::TimerAxis * pTimerAxis);
};

/// 静态库版本
#if defined(_LIB) || defined(GATEWAY_STATIC_LIB)
#	pragma comment(lib, MAKE_LIB_NAME(GatewayConnector))
extern "C" Gateway::IGatewayConnectorManager * CreateGatewayManager(Gateway::IGatewayMessageHandler * pHandler,rkt::TimerAxis * pTimerAxis);
#	define	CreateGatewayManagerProc  CreateGatewayManager
#else 
/// 动态库版本
typedef Gateway::IGatewayConnectorManager* (RKT_CDECL*procCreateGatewayManager)(Gateway::IGatewayMessageHandler * pHandler,rkt::TimerAxis * pTimerAxis);
#	define	CreateGatewayManagerProc	DllApi<procCreateGatewayManager>::load(MAKE_DLL_NAME(GatewayConnector), "CreateGatewayManager")
#endif

#endif//__INTERFACE_GATEWAY_CONNECTOR_H__