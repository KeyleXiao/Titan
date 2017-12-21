/*******************************************************************
** 文件名:	IWorldServer.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	06/04/2008
** 版  本:	1.0
** 描  述:	
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#pragma once

#include "DGlobalMessage.h"
#include "WorldMessage.h"

// 事件引擎接口
namespace rkt
{
	struct IEventEngine;
}

struct IAntiAddictGlobal;
struct IDateTriggerManager;
struct IMessageDispatch;

namespace WorldServer
{
	///// 单个服务器的信息
	//struct ServerData
	//{
	//	WSID    dwServerID;       // 服务器ID
	//	WGID    dwGroupID;        // 服务器所属的群组ID
	//	WORD    wServerType;      // 服务器类型,例如是区域服务器还是网关服务器
	//	char    szName[MAX_SERVER_NAME]; // 服务器名字
	//	WORD    wServerInfoLen;   // 服务器专有的信息长度
	//	char  * pServerInfoData;  // 服务器专有的信息内容,该数组的长度是wServerInfoLen的值
	//};

	///// 网关服务器信息
	//struct ServerInfo_Gateway
	//{
	//	DWORD   dwGatewayIP;      // 网关IP,网络字节序,因为网关一般都在局域网,所以这个一般是内网IP
	//	WORD    wGatewayPort;     // 网关端口,网络字节序
	//};

	///// 区域服务器信息
	//struct ServerInfo_Zone
	//{
	//	struct  MapZone
	//	{
	//		DWORD   dwMapID;          // 地图ID
	//		RECT    rcRect;           // 区域坐标
	//	};

	//	DWORD   dwUserCount;         // 玩家个数
	//	DWORD   wMapNum;             // 该服务器负责的完整地图个数 
	//	DWORD   pMapArray[];         // 该服务器负责的完整地图数组
	//};

	/**
	@name : 中心服务器接口 
	@brief: 该接口主要提供给需要挂接再中心服务器上的游戏逻辑模块使用
	*/
	struct IWorldServer
	{
		/**
		@name                : 世界服向某个服务器发送一条消息
		@brief               : 世界服向区域服务器发送消息
		@param dwCenterSeverID:目标所在的中心服务器ID
		@param DestServerID  : 目标服务器ID
		@param pMessage      : 消息内容
		@param wMessageLen   : 消息长度
		@return              : 是否成功发到了中心服务器
		*/
		virtual bool  PostMessage(WSID dwCenterSeverID, ulong dwDestServerID, const char* szMessage, int nMessageLen) = 0;

		/**
		@name                : 向中心服旗下的某种类型服务器广播消息
		@brief               :
		@param dwCenterSeverID:目标所在的中心服务器ID
		@param nServerType   : 目标服务器类型
		@param pMessage      : 消息内容
		@param wMessageLen   : 消息长度
		@return              : 是否成功发到了中心服务器
		*/
		virtual bool  BroadcastMessage(int nServerType, const char* szMessage, int nMessageLen, WSID dwCenterSeverID = INVALID_SERVER_ID) = 0;

		/**
		@brief                : 取得事件引擎接口
		*/
		virtual IEventEngine* GetEventEngine() const = 0;

		/**
		@brief                : 取得时间轴
		*/
		virtual TimerAxis* GetTimerAxis() const = 0;

		/**
		@brief                : 取得消息分发器
		*/
		virtual IMessageDispatch* GetMessageDispatch() = 0;

		/**
		@brief                : 取得数据库引擎
		*/
		virtual IDBEngine* GetDBEngine() const = 0;
		
		/**
		@brief                : 取得防沉迷模块
		*/
		virtual IDateTriggerManager* GetDateTriggerManager() = 0;

		/**
		@brief                : 取得防沉迷模块
		*/
		virtual IAntiAddictGlobal* GetAntiAddictGlobal() const = 0;
		
		////////////////////////////////////////////////////////////////////////////////
		///**
		//@name         : 取得网络上所有服务器的个数
		//@brief        :
		//*/
		//virtual DWORD GetAllServersCount() = 0;

		///**
		//@name                : 取得服务器列表
		//@brief               :
		//@param serverPtrArray: 服务器指针数组,用来返回指向服务器信息结构的指针
		//@param dwArratSize   : 指针数组的大小,防止越界
		//@param GroupFilter   : 按组ID进行过滤,如果GroupFilter为INVALID_SERVER_ID则表示不进行组过滤
		//@param wTypeFilter   : 按服务器类型过滤,如果wTypeFilter为MSG_SERVERID_UNKNOW则表示不进行服务器类型过滤
		//@note                : 例如返回组ID等于1的所有网关服务器:
		//@note                : ServerInfo * serverPtrArray[MAX_SERVER_COUNT];
		//@note                : DWORD count=IWorldServerConnector::GetServerList(serverPtrArray,1024,1,MSG_SERVER_GATE);
		//@return              :
		//*/
		//virtual DWORD GetServerList(WSID * serverPtrArray,DWORD dwArratSize,WGID GroupFilter=INVALID_SERVER_ID,WORD wTypeFilter=MSG_ENDPOINT_UNKNOW) = 0;

		///**
		//@name                : 取得服务器信息
		//@brief               :
		//@param  ServerID     : 服务器ID
		//@param  info         : 返回服务器信息
		//@return              : 如果目标服务器存在则返回服务器信息,否则返回false
		//*/
		//virtual bool GetServerInfo(WSID ServerID,ServerData & info) = 0;

		///**
		//@name                : 查找场景服务器
		//@brief               :
		//@param    groupID    : 区ID
		//@param    dwMapID    : 地图ID
		//@param    x          : 位置 (因为同一个地图可能由多个服务器共同维护,所以还需要传入位置)
		//@param    y          : 位置
		//@return              :
		//*/
		//virtual WSID FindZoneServer(WGID groupID,DWORD wMapID,DWORD x,DWORD y) = 0;

		///**
		//@name                : 向某个服务器发送一条消息
		//@brief               : 服务器之间的消息转发
		//@param DestServerID  : 目标服务器ID
		//@param pMessage      : 消息内容
		//@param wMessageLen   : 消息长度
		//@return              : 是否成功发到了中心服务器
		//*/
		//virtual bool  PostMessage(WSID DestServerID,const char * pMessage,WORD wMessageLen) = 0;

		///**
		//@name                : 向多个服务器广播消息
		//@brief               :
		//@param DestServerArray:目标服务器数组
		//@param wDestServerCount:目标服务器个数
		//@param pMessage      : 消息内容
		//@param wMessageLen   : 消息长度
		//@return              : 是否成功发到了中心服务器
		//*/
		//virtual bool  BroadcastMessage(WSID * DestServerArray,WORD wDestServerCount,const char * pMessage,WORD wMessageLen) = 0;

		///**
		//@name                : 请求远端服务器执行一个命令语句
		//@brief               :
		//@param DestServerID  : 目标服务器ID
		//@oaram szCommandStr  : 命令语句,注意必须是0结尾的字符串
		//@return              : 是否成功发到了中心服务器
		//*/
		//virtual bool  RemoteCommandRequest(WSID DestServerID,const char * szCommandStr) = 0;

		
	};

	/// 快捷宏
	#define gWorldServer		   ((IWorldServer*)getGlobal())
}