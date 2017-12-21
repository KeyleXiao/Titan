/*******************************************************************
** 文件名:	IWorldServerConnector.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	02/21/2008
** 版  本:	1.0
** 描  述:	
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#ifndef __INTERFACE_WORLD_SERVER_CONNECTOR_H__
#define __INTERFACE_WORLD_SERVER_CONNECTOR_H__

#include "DGlobalMessage.h"
#include "WorldMessage.h"

#pragma pack(1)
#pragma warning(disable:4200)

namespace WorldServer
{
	/// 最大服务器个数
#	define  MAX_SERVER_COUNT       2048
	/// 重连周期
#	define  RECONNECT_INTERVAL     (10 * 1000)

	struct SWServerData;

	/**
	@name : 处理世界服务器发过来的消息
	@brief:
	*/
	struct IWorldServerMessageHandler
	{
		/**
		@brief        : 自己和世界服握手成功返回
		*/
		virtual void HandleHandshakeResponse() = 0;

		/**
		@name         : 通知服务器列表已经更新
		@brief        : 服务器列表更新只会在有服务器进入退出时触发
		@note         : 具体的列表调用IWorldServerConnector::GetServerList获得
		*/
		virtual void HandleServerListUpdated() = 0;

		/**
		@name         : 通知服务器信息更新
		@brief        : 例如服务器人数变化等等
		*/
		virtual void HandleServerInfoUpdated(WSID ServerID, TWServer_State nState, SWServerData * pInfo) = 0;

		/**
		@name         : 世界服向中心服下某个服务器转发消息
		@brief        : 具体内容不清楚
		@param server : 该消息是哪个服务器发过来的
		@param pData  : 消息内容
		@param wDataLen:消息长度
		*/
		virtual void HandlePostMessage(ulong server,const char * pData,WORD wDataLen) = 0;

		/**
		@name                : 世界服向多个服务器广播消息
		@brief               :
		@param SeverType     : 服务器的类型
		@param pMessage      : 消息内容
		@param wMessageLen   : 消息长度
		*/
		virtual bool  HandleBroadcastMessage(int SeverType, const char * pMessage,WORD wMessageLen) = 0;
	};

	/**
	@name : 世界服务器连接器
	@brief: 该模块负责维护和世界服务器之间的连接,并转发各种命令
	*/
	struct IWorldServerConnector
	{
		/**
		@name         : 连接世界服务器
		@brief        : 启动服务器时连接世界服务器,以后如果连接断开,该模块内部会定时重试
		@param szCSIP : 世界服务器IP 
		@param nCSPort: 世界服务器端口
		@return       : 如果没有网络错误,则返回成功
		*/
		virtual bool ConnectWorldServer(const char* szCSIP, WORD nCSPort) = 0;

		/**
		@name                 : 向世界服务器注册本地服务器信息
		@brief                : 如果本地服务器信息更改了,也调用这个接口进行Update 
		@param szName         : 服务器名称,必须小于MAX_SERVER_NAME
		@param dwGroupID      : 本地服务器所在的群组ID
		@param wServerType    : 本地服务器的类型,例如是区域服务器还是网关服务器
		@return               : 
		*/
		virtual bool RegisterServer(const char* szName, WGID dwGroupID, WORD wServerType) = 0;

		/**
		@name                 : 向世界服务器更新服务器信息
		@brief                :
		@param  pServerInfo   : 服务器信息内容,不同的服务器有不同的扩展信息,例如区域服务器就有该服务器负责的区域ID等
		@param  wLen          : 服务器信息长度
		@return               :
		*/
		virtual bool UpdateServerInfo(LPVOID pServerInfo,WORD wLen) = 0;

		/**
		@name         : 取得本地服务器在世界服务器上的ID
		@brief        :
		*/
		virtual WSID  GetLocalServerCSID() = 0;

		/**
		@name         : 取得网络上所有服务器的个数
		@brief        :
		*/
		virtual DWORD GetAllServersCount() = 0;

		/**
		@name                : 取得服务器列表
		@brief               :
		@param serverPtrArray: 服务器指针数组,用来返回指向服务器信息结构的指针
		@param dwArratSize   : 指针数组的大小,防止越界
		@param GroupFilter   : 按组ID进行过滤,如果GroupFilter为INVALID_SERVER_ID则表示不进行组过滤
		@param wTypeFilter   : 按服务器类型过滤,如果wTypeFilter为MSG_SERVERID_UNKNOW则表示不进行服务器类型过滤
		@note                : 例如返回组ID等于1的所有网关服务器:
		@note                : ServerInfo * serverPtrArray[MAX_SERVER_COUNT];
		@note                : DWORD count=IWorldServerConnector::GetServerList(serverPtrArray,1024,1,MSG_SERVER_GATE);
		@return              :
		*/
		virtual DWORD GetServerList(SWServerData * serverPtrArray[],DWORD dwArratSize,WGID GroupFilter=INVALID_SERVER_ID,WORD wTypeFilter=MSG_ENDPOINT_UNKNOW) = 0;

		/**
		@name                : 取得服务器信息
		@brief               :
		@param  ServerID     : 服务器ID
		@return              : 如果目标服务器存在则返回服务器信息,否则返回0
		*/
		virtual SWServerData* GetServerInfo(WSID ServerID) = 0;

		/**
		@name                : 直接发送数据给世界服务器
		@brief               : 因为世界服务器上挂接了部分游戏逻辑模块,这些模块可能需要以自己的格式进行通讯
		@param pData         : 要发送的数据
		@param dwLen         : 数据长度
		*/
		virtual void  SendDataToWorldServer(const char* pData, DWORD dwLen) = 0;

		/**
		@brief                 : 通知世界服某个区域服务器断开连接
		@param nServerID       : 区域服务器ID
		@param nConnectedState : 连接状态
		*/
		virtual void  ASeverConnected(ulong nServerID, int nServerType, int nConnectedState) = 0;

		virtual void  Release() = 0;
	};
};

/// 静态库版本
#if defined(_LIB) || defined(WORLD_STATIC_LIB)
#	pragma comment(lib, MAKE_LIB_NAME(WorldServerConnector))
extern "C" WorldServer::IWorldServerConnector * CreateWorldServerConnector(WorldServer::IWorldServerMessageHandler * pHandler,rkt::TimerAxis * pTimerAxis);
#	define	CreateWorldServerConnectorProc	CreateWorldServerConnector
#else 
/// 动态库版本
typedef WorldServer::IWorldServerConnector* (RKT_CDECL*procCreateWorldServerConnector)(WorldServer::IWorldServerMessageHandler* pHandler,rkt::TimerAxis* pTimerAxis);
#	define	CreateWorldServerConnectorProc	DllApi<procCreateWorldServerConnector>::load(MAKE_DLL_NAME(WorldServerConnector), "CreateWorldServerConnector")
#endif

#pragma pack()

#endif//__INTERFACE_WORLD_SERVER_CONNECTOR_H__