/*******************************************************************
** 文件名:      IBridgeConnector.h 
** 版  权:      (C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:      宋文武 (sww8@163.com)
** 日  期:      2010-6-24
** 版  本:      1.0
** 描  述:      跨区桥服务器连接器
** 应  用:      

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#ifndef __INTERFACE_BRIDGE_SERVER_CONNECTOR_H__
#define __INTERFACE_BRIDGE_SERVER_CONNECTOR_H__

#include "Common.h"
#include "BridgeModuleDef.h"
#include "GlobalServerDef.h"

#ifndef  BRIDGE_API
#	ifdef BRIDGESERVER_EXPORTS
#		define  BRIDGE_API  extern "C" __declspec(dllexport)
#	else
#		define  BRIDGE_API  extern "C" __declspec(dllimport)
#		pragma comment(lib, MAKE_DLL_LIB_NAME(BridgeConnector))
#	endif
#endif


#pragma pack(1)
#pragma warning(disable:4200)

namespace Bridge
{

	/**
	@name : 处理跨区桥服务器发过来的消息
	@brief:
	*/
	struct IBridgeMessageHandler
	{
		/**
		@name         : 处理其他服务器通过跨区桥服务器转发的消息
		@param actionId  : 消息码
		@param bridgeHead  : 桥消息头
		@param head  : 消息头
		@param data  : 消息内容
		@param len:消息长度
		*/
		virtual void HandleBridgeMessage(unsigned long actionId, SMsgBridgeHead &bridgeHead, SGameMsgHead* head, LPSTR data, size_t len) = 0;
	};

	/// 处理处理跨区桥服务器执行事件
	struct IBridgeEventHandler
	{
		/** 
		@param   wEventID ：事件ID
		@param   pszContext : 上下文
		@param   nLen : 上下文长度
		@retval buffer 
		*/
		virtual void OnBridgeEventExecute(WORD wEventID, LPSTR pszContext, size_t nLen) = 0;
	};

	/// 场景服 跨区桥服务器连接器扩展部件接口
	struct IBridgePart
	{
		/** 创建扩展部件
		DWORD dwID			: 服务器ID
		DWORD dwWorldID		: 服务器所在游戏世界ID
		WORD wServerType	: 服务器类型
		WORD wSubID			: 第n号服务器
		bool bIsLocal		: 本地对像
		@return  
		*/
		virtual bool Create(DWORD dwID,DWORD dwWorldID,WORD wServerType,WORD wSubID,bool bIsLocal=false) = 0;

		/** 释放,会释放内存
		@return  
		*/
		virtual void Release(void) = 0;

		// 取服务器ID
		virtual DWORD GetServerID(void) = 0;
		// 取服务器类型
		virtual DWORD GetWorldID(void) = 0;
		// 取服务器所在游戏世界ID
		virtual WORD GetServerType(void) = 0;
		// 取第n号服务器
		virtual WORD GetServerSubID(void) = 0;
		// 创建镜像 DWORD dwServerID:来源服务器ID
		virtual void BuildSelf(DWORD dwServerID) = 0;

		/** 扩展信息消息,就是 SendExtDataToServer 参数
		@return  
		*/
		virtual	void OnExtDateMsg(DWORD dwServerID,DWORD dwMsgCode,DWORD dwUserData1,DWORD dwUserData2,DWORD dwUserData3,const char * pData,DWORD dwLen) = 0;
	};


	// 共享场景信息
	struct IBridgeZone
	{
		// 场景ID
		virtual DWORD GetZoneID(void) = 0;
		// 地图ID
		virtual DWORD GetMapID(void) = 0;
		// 场景状态
		virtual bool IsEnable(void) = 0;
	};

	// 游戏世界信息
	struct IBridgeWorldInfo
	{
		// 游戏世界ID
		virtual DWORD GetWorldID(void) = 0;
		// 游戏世界名称
		virtual const char * GetWorldName(void) = 0;
		// 服务器个数
		virtual DWORD GetServerCounts(void) = 0;
	};

	/// 跨区桥服务器连接器扩展部件接口
	struct IBridgeZonePart : public IBridgePart
	{
		// 取得共享场景信息
		virtual IBridgeZone* GetBridgeZone(DWORD dwZoneID) = 0;
		// 创建场景
		virtual void OnBuildZone(SMsgBridgeBuildZone_B &msgData) = 0;
		// 置场景状态
		virtual void OnSetZoneState(SMsgBridgeSetZoneState_B &msgData) = 0;
		// 消毁场景
		virtual void OnDestroyZone(SMsgBridgeDestroyZone_B &msgData) = 0;
	};
	
	/// 跨区桥服务器连接器扩展部件接口
	struct IBridgeSocialPart : public IBridgePart
	{
	};

	//////////////////////////////////////////////////////////////////////////

	/**
	@name : 跨区桥服务器连接器
	@brief: 该模块负责维护和跨区桥服务器之间的连接,并转发各种命令
	*/
	struct IBridgeConnector
	{
		virtual bool Create(DWORD dwGameWorldID,WORD wServerType,WORD wSubID,const char * szServerName,const char * szWorldName) = 0;

		/**
		@name         : 连接跨区桥服务器
		@brief        : 启动服务器时连接跨区桥服务器,以后如果连接断开,该模块内部会定时重试
		@param szCSIP : 跨区桥服务器IP 
		@param nCSPort: 跨区桥服务器端口
		@return       : 如果没有网络错误,则返回成功
		*/
		virtual bool Connect(const char * szCSIP,WORD nCSPort,int nThreadMode = NetworkUserType_Default) = 0;

		/**
		@name                 : 向跨区桥服务器注册本地服务器信息
		@brief                : 如果本地服务器信息更改了,也调用这个接口进行Update 
		@param szName         : 服务器名称,必须小于32
		@param dwGroupID      : 本地服务器所在的群组ID
		@param wServerType    : 本地服务器的类型,例如是区域服务器还是网关服务器
		@return               : 
		*/
		virtual bool RegisterServer(DWORD dwGameWorldID,WORD wServerType,WORD wSubID) = 0;

		/**
		@name         : 取得本地服务器在跨区桥服务器上的ID
		@brief        :
		*/
		virtual DWORD  GetServerID() = 0;

		// 取得公共游戏世界ID
		virtual DWORD GetPublicWorldID() = 0;

		// 关闭和主服务器之间的连接
		virtual void  Close() = 0;

		virtual void  Release() = 0;

		virtual bool  IsConnected() = 0;

		/**
		@name                : 直接发送数据给跨区桥服务器
		@brief               : 
		@param pData         : 要发送的数据
		@param dwLen         : 数据长度
		*/
		virtual void  SendData(const char * pData,DWORD dwLen, DWORD dwFlag=0) = 0;

		// 设定服务器状态
		// DWORD dwStateFlag:服务器状态类型定义 EMManagerServerStateFlag
		virtual bool SetServerState(DWORD dwStateFlag=0) = 0;

		// 主动重连
		virtual bool ReConnect() = 0;

		/** 注册消息处理者
		@param moduleId 模块ID，参考InputMessage.h中的枚举定义
		@param hander 消息处理者实例
		*/
		virtual bool registerMessageHandler(DWORD moduleId, IBridgeMessageHandler* handler) = 0;

		/** 注销消息处理者
		@param moduleId 模块ID，参考DGlobalMessage.h中的枚举定义
		*/
		virtual bool unregisterMessageHandler(DWORD moduleId) = 0;

		/** 注册事件处理者
		@param wEventID 事件ID	参考"BridgeModuleDef.h"   EMEventBridge
		@param hander 事件处理者实例
		*/
		virtual bool registerEventHandler(WORD wEventID,IBridgeEventHandler* handler) = 0;

		/** 注销事件处理者
		@param wEventID 事件ID	参考"BridgeModuleDef.h"   EMEventBridge
		@param  hander 事件处理者实例
		*/
		virtual bool unregisterEventHandler(WORD wEventID,IBridgeEventHandler* handler) = 0;

		/** 触发事件
		@param wEventID 事件ID	参考"BridgeModuleDef.h"   EMEventBridge
		*/
		virtual bool FireEventExecute(WORD wEventID, LPSTR pszContext, size_t nLen) = 0;

		////// 常用发数据接口 ////////////////////////////////////////////////////////////////////

		// 发数据给指定游戏服务器 dwServerID:服务器ID 为0时广播给所有服务器,bZeroBroadcast:为true时打开为0广播,否则无效
		virtual void SendDataToServer(DWORD dwServerID,const char * pData,DWORD dwLen,bool bZeroBroadcast=false) = 0;

		// 广播数据给指定列表游戏服务器 pServerArray:服务器ID列表,wServerNum:列表个数
		virtual void BroadcastDataToServer(DWORD * pServerArray,WORD wServerNum,const char * pData,DWORD dwLen) = 0;

		// 发服务器扩展信息消息 dwServerID:服务器ID 为0时广播给所有桥服务器连接器,跨区桥服务器连接器同步服务器扩展信息专用
		virtual bool SendExtDataToServer(DWORD dwServerID,DWORD dwMsgCode,DWORD dwUserData1=0,DWORD dwUserData2=0,DWORD dwUserData3=0,const char * pData=0,DWORD dwLen=0) = 0;

		////// 常用服务器列表相关接口 ////////////////////////////////////////////////////////////////////

		/**  取得服务器ID列表
		@param   DWORD* pServerArray	: 保存找到的ID数组
		@param   WORD dwArrayLen		: 数组长度
		@param   WORD &dwGetCounts		: 找到的个数
		@param   DWORD dwWorldID		: 游戏世界ID，0为全部
		@param   WORD wServerType		: 服务器类型,0为全部
		@return  找到的个数
		*/
		virtual DWORD GetGameServerIDList(DWORD * pServerArray,DWORD dwArrayLen,DWORD &dwGetCounts,DWORD dwWorldID=0,WORD wServerType=0) = 0;

		/**  取得指定游戏世界的指定类型的服务器ID
		@param   DWORD dwWorldID		: 游戏世界ID
		@param   WORD wServerType		: 服务器类型
		@param   WORD wSubID			: 服务器序号
		@return  找到的服务器ID,为0表示没找到
		*/
		virtual DWORD GetGameServerIDByTypeSubID(DWORD dwWorldID,WORD wServerType,WORD wSubID) = 0;

		/** 列出所有服务器信息
		@param   DWORD dwWorldID		: 游戏世界ID
		@param   WORD wServerType		: 服务器类型
		@return 
		*/
		virtual void PrintGameServerListInfo(DWORD dwWorldID=0,WORD wServerType=0) = 0;

		////// 常用游戏世界列表相关接口 ////////////////////////////////////////////////////////////////////

		/**  取得游戏世界ID列表
		@param   DWORD* pWorldArray	: 保存找到的ID数组
		@param   WORD dwArrayLen		: 数组长度
		@param   WORD &dwGetCounts		: 找到的个数
		@return  找到的个数
		*/
		virtual DWORD GetGameWorldIDList(DWORD * pWorldArray,DWORD dwArrayLen,DWORD &dwGetCounts) = 0;

		/**  取得指定游戏世界信息
		@param   DWORD dwWorldID		: 游戏世界ID
		@return  找到的服务器ID,为0表示没找到
		*/
		virtual IBridgeWorldInfo * GetGameWorldInfo(DWORD dwWorldID) = 0;

		/** 列出所有游戏世界信息
		@return 
		*/
		virtual void PrintGameWorldListInfo() = 0;

		////// 常用扩展信息类列表相关接口 ////////////////////////////////////////////////////////////////////

		/** 用服务器ID 取得扩展信息类
		@param   DWORD dwServerID		: 服务器ID
		@return  IBridgePart*	:服务器扩展信息类
		*/
		virtual IBridgePart* GetGameServerPart(DWORD dwServerID) = 0;

		/** 用TypeSubID取得扩展信息类
		@param   DWORD dwWorldID		: 游戏世界ID
		@param   WORD wServerType		: 服务器类型
		@param   WORD wSubID			: 服务器序号
		@return  IBridgePart*	:服务器扩展信息类
		*/
		virtual IBridgePart* GetGameServerPartByTypeSubID(DWORD dwWorldID,WORD wServerType,WORD wSubID) = 0;

		/** 本服务器扩展信息类指针
		@return  IBridgePart*	:服务器扩展信息类
		*/
		virtual IBridgePart* GetLocalBridgePart() = 0;

		// 通过指定服务器发中转消息数据给目标游戏服务器 dwServerID:服务器ID 为0时广播给所有服务器,DWORD dwTransitServerID 中转到服务器ID
		virtual void SendTransitDataToServer(DWORD dwServerID,DWORD dwTransitServerID,const char * pData,DWORD dwLen) = 0;

		/** 是否已启动
		@param   
		@param   
		@return  
		*/
		virtual bool IsStartup() = NULL;

		/** 启动服务器性能测试
		@param   
		@param   
		@return  
		*/
		virtual void  StartCapabilityTest(int nCheckInterval) = NULL;

		/** 停止服务器性能测试
		@param   
		@param   
		@return  
		*/
		virtual void  StopCapabilityTest() = NULL;

		/** 取完成性能测试的结点
		@param   
		@param   
		@return  
		*/
		virtual TCAPABILITYNODE& GetCapabilityNode() = NULL;

		/** 取最新监控结点数据
		@param   
		@param   
		@return  
		*/
		virtual TMONITORNODE& GetMonitorNode() = NULL;

		/** 处理心跳
		@param   
		@param   
		@return  
		*/
		virtual void HandleKeepAlive() = NULL;
	};

	/**
	@name            : 创建一个跨区桥服务器连接对象
	@brief           :
	@param pTimerAxis: 时间轴接口,主要用来定时重试
	@return          : 返回跨区桥服务器连接对象指针
	*/
	// IBridgeConnector * CreateBridgeConnector(rkt::TimerAxis * pTimerAxis);


}; // namespace

//////////////////////////////////////////////////////////////////////////

// 取得跨区桥服务器连接器接口
BRIDGE_API Bridge::IBridgeConnector* GetGlobalBridgeConnector();

/// 静态库版本
#if defined(_LIB) || defined(BRIDGE_STATIC_LIB)
#	pragma comment(lib, MAKE_LIB_NAME(BridgeConnector))
extern "C" Bridge::IBridgeConnector * CreateBridgeConnector(rkt::TimerAxis * pTimerAxis);
#	define	CreateBridgeConnectorProc	CreateBridgeConnector
#else 
/// 动态库版本
typedef Bridge::IBridgeConnector* (RKT_CDECL*procCreateBridgeConnector)(rkt::TimerAxis * pTimerAxis);
#	define	CreateBridgeConnectorProc	DllApi<procCreateBridgeConnector>::load(MAKE_DLL_NAME(BridgeConnector), "CreateBridgeConnector")
#endif

#pragma pack()



#endif//__INTERFACE_BRIDGE_SERVER_CONNECTOR_H__