/*******************************************************************
** 文件名:      IViewConnector.h 
** 版  权:      (C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:      宋文武 (sww8@163.com)
** 日  期:      2011-05-16
** 版  本:      1.0
** 描  述:      语音服务器连接器
** 应  用:      

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once
#include "Common.h"
#include "VoiceDef.h"

#ifndef  VIEW_API
#	ifdef VIEWMNG_EXPORTS
#		define  VIEW_API  extern "C" __declspec(dllexport)
#	else
#		define  VIEW_API  extern "C" __declspec(dllimport)
#		pragma comment(lib, MAKE_DLL_LIB_NAME(ViewConnector))
#	endif
#endif


#pragma pack(1)
#pragma warning(disable:4200)

namespace View
{

	/**
	@name : 处理语音服务器发过来的消息
	@brief:
	*/
	struct IVoiceMessageHandler
	{
		/**
		@name         : 处理其他服务器通过语音服务器转发的消息
		@param actionId  : 消息码
		@param voiceHead  : 语音消息头
		@param head  : 消息头
		@param data  : 消息内容
		@param len:消息长度
		*/
		virtual void HandleVoiceMessage(unsigned long actionId,SMsgVoiceHead &voiceHead, SGameMsgHead* head, LPSTR data, size_t len) = 0;
	};

	/// 处理处理语音服务器执行事件
	struct IVoiceEventHandler
	{
		/** 
		@param   wEventID ：事件ID
		@param   pszContext : 上下文
		@param   nLen : 上下文长度
		@retval buffer 
		*/
		virtual void OnVoiceEventExecute(WORD wEventID, LPSTR pszContext, size_t nLen) = 0;
	};

	/// 场景服 跨区语音服务器连接器扩展部件接口
	struct IVoicePart
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
	};

	// 游戏世界信息
	struct IVoiceWorldInfo
	{
		// 游戏世界ID
		virtual DWORD GetWorldID(void) = 0;
		// 游戏世界名称
		virtual const char * GetWorldName(void) = 0;
		// 服务器个数
		virtual DWORD GetServerCounts(void) = 0;
	};

	//////////////////////////////////////////////////////////////////////////

	/**
	@name : 语音服务器连接器
	@brief: 该模块负责维护和语音服务器之间的连接,并转发各种命令
	*/
	struct IViewConnector
	{
		virtual bool Create(DWORD dwGameWorldID,WORD wServerType,WORD wSubID,const char * szServerName,const char * szWorldName) = 0;

		/**
		@name         : 连接语音服务器
		@brief        : 启动服务器时连接语音服务器,以后如果连接断开,该模块内部会定时重试
		@param szCSIP : 语音服务器IP 
		@param nCSPort: 语音服务器端口
		@return       : 如果没有网络错误,则返回成功
		*/
		virtual bool Connect(const char * szCSIP,WORD nCSPort,int nThreadMode) = 0;

		/**
		@name                 : 向语音服务器注册本地服务器信息
		@brief                : 如果本地服务器信息更改了,也调用这个接口进行Update 
		@param szName         : 服务器名称,必须小于32
		@param dwGroupID      : 本地服务器所在的群组ID
		@param wServerType    : 本地服务器的类型,例如是区域服务器还是网关服务器
		@return               : 
		*/
		virtual bool RegisterServer(DWORD dwGameWorldID,WORD wServerType,WORD wSubID) = 0;

		/**
		@name         : 取得本地服务器在语音服务器上的ID
		@brief        :
		*/
		virtual DWORD  GetServerID() = 0;

		// 关闭和主服务器之间的连接
		virtual void  Close() = 0;

		virtual void  Release() = 0;

		virtual bool  IsConnected() = 0;

		/**
		@name                : 直接发送数据给语音服务器
		@brief               : 
		@param pData         : 要发送的数据
		@param dwLen         : 数据长度
		*/
		virtual void  SendData(const char * pData,DWORD dwLen) = 0;

		// 主动重连
		virtual bool ReConnect() = 0;

		/** 注册消息处理者
		@param moduleId 模块ID，参考InputMessage.h中的枚举定义
		@param hander 消息处理者实例
		*/
		virtual bool registerMessageHandler(DWORD moduleId, IVoiceMessageHandler* handler) = 0;

		/** 注销消息处理者
		@param moduleId 模块ID，参考DGlobalMessage.h中的枚举定义
		*/
		virtual bool unregisterMessageHandler(DWORD moduleId) = 0;

		/** 注册事件处理者
		@param wEventID 事件ID	参考 VoiceDef.h   EMEventVoice
		@param hander 事件处理者实例
		*/
		virtual bool registerEventHandler(WORD wEventID,IVoiceEventHandler* handler) = 0;

		/** 注销事件处理者
		@param wEventID 事件ID	参考 VoiceDef.h   EMEventVoice
		@param  hander 事件处理者实例
		*/
		virtual bool unregisterEventHandler(WORD wEventID,IVoiceEventHandler* handler) = 0;


		////// 常用发数据接口 ////////////////////////////////////////////////////////////////////

		// 发数据给指定游戏服务器 dwServerID:服务器ID 为0时广播给所有服务器,bZeroBroadcast:为true时打开为0广播,否则无效
		virtual void SendDataToServer(DWORD dwServerID,const char * pData,DWORD dwLen,bool bZeroBroadcast=false) = 0;

		// 广播数据给指定列表游戏服务器 pServerArray:服务器ID列表,wServerNum:列表个数
		virtual void BroadcastDataToServer(DWORD * pServerArray,WORD wServerNum,const char * pData,DWORD dwLen) = 0;


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
		virtual IVoiceWorldInfo * GetGameWorldInfo(DWORD dwWorldID) = 0;

		/** 列出所有游戏世界信息
		@return 
		*/
		virtual void PrintGameWorldListInfo() = 0;

		////// 常用扩展信息类列表相关接口 ////////////////////////////////////////////////////////////////////

		/** 用服务器ID 取得扩展信息类
		@param   DWORD dwServerID		: 服务器ID
		@return  IVoicePart*	:服务器扩展信息类
		*/
		virtual IVoicePart* GetGameServerPart(DWORD dwServerID) = 0;

		/** 用TypeSubID取得扩展信息类
		@param   DWORD dwWorldID		: 游戏世界ID
		@param   WORD wServerType		: 服务器类型
		@param   WORD wSubID			: 服务器序号
		@return  IVoicePart*	:服务器扩展信息类
		*/
		virtual IVoicePart* GetGameServerPartByTypeSubID(DWORD dwWorldID,WORD wServerType,WORD wSubID) = 0;
		
		////// 语音功能类相关接口 ////////////////////////////////////////////////////////////////////
		/** 添加蓝钻
		@param   DWORD dwUDBID		: 用户UDBID
		@param   DWORD dwCount		: 蓝钻数
		@param	 DWORD dwPDBID		: 用户PDBID 可以为0
		@param	 DWORD dwPDBIDSend	: 赠送者PDBID 可以为0
		@param	 DWORD dwSpare1
		@param	 DWORD dwSpare2
		@param	 DWORD dwSpare3 备用
		@param	 const char * szReason : 备注
		*/
		virtual bool AddVoiceBlueDiamond( DWORD dwUDBID, DWORD dwCount, DWORD dwPDBID = 0, DWORD dwPDBIDSend = 0, DWORD dwSpare1 = 0, DWORD dwSpare2 = 0, DWORD dwSpare3 = 0, const char * szReason = NULL ) = 0;

		/** 广播数据到客户端
		@param	pData		: 数据
		@param	dwLen		: 数据长度
		@param	nBroadcastType : 广播类型 EMVoiceChannelType
		@param	dwPDBID1/dwPDBID2 : 用户PDBID
		@param	dwWorldID1/dwWorldID2 : 世界ID，
		@param  dwValueID1/dwValueID2 : ID值,参考SVoiceChannelID
		@param  bRoom : 是否广播用户dwPDBID1所在房间
		*/
		virtual bool BroadcastDataToClient( BYTE nBroadcastType, void * pData, DWORD dwLen, DWORD dwPDBID1 = 0, DWORD dwPDBID2 = 0, DWORD dwWorldID1 = 0, DWORD dwWorldID2 = 0, DWORD dwValueID1 = 0, DWORD dwValueID2 = 0, bool bRoom = false ) = 0;

	};

	/**
	@name            : 创建一个语音服务器连接对象
	@brief           :
	@param pTimerAxis: 时间轴接口,主要用来定时重试
	@return          : 返回语音服务器连接对象指针
	*/
	// IViewConnector * CreateViewConnector(rkt::TimerAxis * pTimerAxis);


}; // namespace

//////////////////////////////////////////////////////////////////////////

// 取得语音服务器连接器接口
VIEW_API View::IViewConnector* GetGlobalViewConnector();

/// 静态库版本
#if defined(_LIB) || defined(VIEW_STATIC_LIB)
#	pragma comment(lib, MAKE_LIB_NAME(ViewConnector))
extern "C" Voice::IViewConnector * CreateViewConnector(rkt::TimerAxis * pTimerAxis);
#	define	CreateViewConnectorProc	CreateViewConnector
#else 
/// 动态库版本
typedef View::IViewConnector* (RKT_CDECL*procCreateViewConnector)(rkt::TimerAxis * pTimerAxis);
#	define	CreateViewConnectorProc	DllApi<procCreateViewConnector>::load(MAKE_DLL_NAME(ViewConnector), "CreateViewConnector")
#endif

#pragma pack()
