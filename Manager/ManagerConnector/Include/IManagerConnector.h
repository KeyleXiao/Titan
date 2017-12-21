/*******************************************************************
** 文件名:      IManagerConnector.h 
** 版  权:      (C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:      宋文武 (sww8@163.com)
** 日  期:      2009/8/25
** 版  本:      1.0
** 描  述:      
** 应  用:      

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#ifndef __INTERFACE_MANAGER_SERVER_CONNECTOR_H__
#define __INTERFACE_MANAGER_SERVER_CONNECTOR_H__

#include "DManagerModule.h"
using namespace rkt;


#ifndef  MANAGER_API
#	ifdef MANAGERSERVER_EXPORTS
#		define  MANAGER_API  extern "C" __declspec(dllexport)
#	else
#		define  MANAGER_API  extern "C" __declspec(dllimport)
#		pragma comment(lib, MAKE_DLL_LIB_NAME(ManagerConnector))
#	endif
#endif


#pragma pack(1)
#pragma warning(disable:4200)

namespace Manager
{
	/// 最大服务器个数
#define  MAX_SERVER_COUNT       2048
	/// 重连周期
//#define  MG_RECONNECT_INTERVAL     (60*1000)

	/**
	@name : 处理子服务器发过来的消息
	@brief:
	*/
	struct IManagerMessageHandler
	{

		/**
		@name         : 处理其他服务器通过子服务器转发的消息
		@brief        : 具体内容不清楚
		@param actionId  : 消息码
		@param head  : 消息头
		@param data  : 消息内容
		@param len:消息长度
		*/
		virtual void HandleManagerMessage(rkt::ulong actionId, SManagerMsgHead* head, LPSTR data, size_t len) = 0;

	};

	/// 处理子服务器连接器执行事件
	struct IManagerEventHandler
	{
		/** 
		@param   wEventID ：事件ID
		@param   pszContext : 上下文
		@param   nLen : 上下文长度
		@retval buffer 
		*/
		virtual void OnManagerEventExecute(WORD wEventID, LPSTR pszContext, size_t nLen) = 0;
	};

	/**
	@name : 子服务器连接器
	@brief: 该模块负责维护和子服务器之间的连接,并转发各种命令
	*/
	struct IManagerConnector
	{
		virtual bool Create(WORD wServerType,WORD wSubID,const char * szServerName) = 0;

		/**
		@name         : 连接子服务器
		@brief        : 启动服务器时连接子服务器,以后如果连接断开,该模块内部会定时重试
		@param szCSIP : 子服务器IP 
		@param nCSPort: 子服务器端口
		@return       : 如果没有网络错误,则返回成功
		*/
		virtual bool Connect(const char * szCSIP,WORD nCSPort,int nThreadMode = NetworkUserType_Default) = 0;

		/**
		@name                 : 向子服务器注册本地服务器信息
		@brief                : 如果本地服务器信息更改了,也调用这个接口进行Update 
		@param szName         : 服务器名称,必须小于MANAGER_NAME_MAXSIZE
		@param dwGroupID      : 本地服务器所在的群组ID
		@param wServerType    : 本地服务器的类型,例如是区域服务器还是网关服务器
		@return               : 
		*/
		virtual bool RegisterServer(WORD wServerType,WORD wSubID) = 0;

		/**
		@name         : 取得本地服务器在子服务器上的ID
		@brief        :
		*/
		virtual DWORD  GetLocalServerID() = 0;


		virtual void  Release() = 0;

		/**
		@name                : 直接发送数据给子服务器
		@brief               : 
		@param pData         : 要发送的数据
		@param dwLen         : 数据长度
		*/
		virtual void  SendData(const char * pData,DWORD dwLen, DWORD dwFlag=0) = 0;
		// 发给主服务器数据
		virtual void  SendDataToMain(const char * pData,DWORD dwLen) = 0;
		// 发给根服务器数据
		virtual void  SendDataToRoot(const char * pData,DWORD dwLen) = 0;
		// 发给客户端器数据 dwClientID = 0 为广播所有
		virtual void  SendDataToClient(DWORD dwClientID,const char * pData,DWORD dwLen) = 0;

		// 请求启动 DWORD dwStartFlag:启动标识 EMManagerStartServerFlag ,DWORD dwUserData: 用户数据
		virtual bool RequestStart(DWORD dwStartFlag=0,DWORD dwUserData=0) = 0;

		// 请求关闭,如果没有请求，就关了服务器，会当作当机处理
		// DWORD dwCloseFlag:关闭标识 EMManagerCloseServerFlag ,DWORD dwUserData: 用户数据
		virtual bool RequestClose(DWORD dwCloseFlag=0,DWORD dwUserData=0) = 0;

		// 设定服务器状态
		// DWORD dwStateFlag:服务器状态类型定义 EMManagerServerStateFlag
		virtual bool SetServerState(DWORD dwStateFlag=0) = 0;

		/* 加入日志
		@param   DWORD dwType  日志类型,参考日志类型定义　EMManagerLogType
		@param   DWORD dwTime  时间
		@param   DWORD dwUserID  操作管理员帐号ID,服务器的日志时为0
		@param   DWORD dwWorldID  游戏世界ID 
		@param   DWORD dwComputerID  硬件服务器ID 
		@param   DWORD dwServerID  软件服务器ID
		@param   char* szUserName  管理员登录帐号
		@param   char* szName  管理员姓名
		@param   char* szLogTxt  日志内容
		@param   DWORD dwSpare1 备用1
		@param   DWORD dwSpare2  备用2
		@param   DWORD dwSpare3  备用3
		*/
		virtual void AddLog(DWORD dwType,const char* szLogTxt,const char* szUserName=0,const char* szName=0,DWORD dwUserID=0,DWORD dwWorldID=0,DWORD dwComputerID=0,DWORD dwServerID=0,WORD dwSpare1=0,WORD dwSpare2=0,WORD dwSpare3=0) = 0;

		// 硬件服务器输出数据到客户端
		virtual bool SendSoftServerDataToClient(DWORD dwClientID,DWORD dwDataType,DWORD dwUserData1=0,DWORD dwUserData2=0,DWORD dwUserData3=0,const char * pData=0,DWORD dwLen=0) = 0;

		// 取得在主服务器上的UID
		virtual DWORD  GetServerUID() = 0;

		// 取得游戏世界ID
		virtual DWORD  GetWorldID() = 0;

		// 取得游戏世界ID
		virtual const SGameWorldConnectorInfo * GetGameWorldInfo() = 0;

		/* 加入软件服务器报警信息
		@param   DWORD dwAlertLevel		报警等级  EMGameServerAlertLevel
		@param   DWORD dwAlertType		报警类型  EMGameServerAlertSrcType
		@param   const char *  szAlertTxt		报警内容
		*/
		virtual void AddAlertInfo(DWORD dwAlertLevel,DWORD dwAlertType,const char * szAlertTxt) = 0;

		// 取得子服务器定义的Ftp根目录路径
		virtual const char * GetFtpRootPath() = 0;

		// 主动重连
		virtual bool ReConnect() = 0;

		/** 注册事件处理者
		@param wEventID 事件ID	参考 EMEventManager
		@param hander 事件处理者实例
		*/
		virtual bool registerEventHandler(WORD wEventID,IManagerEventHandler* handler) = 0;

		/** 注销事件处理者
		@param wEventID 事件ID	参考 EMEventManager
		@param  hander 事件处理者实例
		*/
		virtual bool unregisterEventHandler(WORD wEventID,IManagerEventHandler* handler) = 0;

		/** 触发事件
		@param wEventID 事件ID	参考  EMEventManager
		*/
		virtual bool FireEventExecute(WORD wEventID, LPSTR pszContext, size_t nLen) = 0;

		/** 设置实时数据
		DWORD dwDataID		: 实时数据类型ID,不同的服务器类型用不用的ID 参考 EMManagerRealTimeDataLogin
		int nValue			: 数值
		@return  是否成功设置
		*/
		virtual bool SetRealTimeData(DWORD dwDataID,int nValue) = 0;

		/** 取得实时数据
		DWORD dwDataID		: 实时数据类型ID
		@return  int nValue	: 数值
		*/
		virtual int GetRealTimeData(DWORD dwDataID) = 0;

		/** 设置状态监听数据
		DWORD dwRTVisitorType	: 软件服务器状态监听类型 参考 EMManagerRealTimeVisitorType
		LPSTR pszContext	: 设置buffer
		int nLen			: buffer长度
		@return  是否成功设置
		*/
		virtual bool SetVisitorData(DWORD dwRTVisitorType,LPSTR pszContext, int nLen) = 0;

		/** 是否有新状态监听用户
		DWORD dwRTVisitorType	: 软件服务器状态监听类型 参考 EMManagerRealTimeVisitorType
		@return bool 是否
		*/
		virtual bool IsNewVisitorIncoming(DWORD dwRTVisitorType) = 0;
	};

	/**
	@name            : 创建一个子服务器连接对象
	@brief           :
	@param pHandler  : 处理接口指针
	@param pTimerAxis: 时间轴接口,主要用来定时重试
	@param pCmdFactory:命令工厂,用来执行远程服务器请求执行的命令行语句
	@return          : 返回子服务器连接对象指针
	*/
	// IManagerConnector * CreateManagerConnector(IManagerMessageHandler * pHandler,rkt::TimerAxis * pTimerAxis);


}; // namespace

//////////////////////////////////////////////////////////////////////////

// 取得管理器接口
MANAGER_API Manager::IManagerConnector* GetGlobalManagerConnector();

/// 静态库版本
#if defined(_LIB) || defined(MANAGER_STATIC_LIB)
#	pragma comment(lib, MAKE_LIB_NAME(ManagerConnector))
extern "C" Manager::IManagerConnector * CreateManagerConnector(Manager::IManagerMessageHandler * pHandler,rkt::TimerAxis * pTimerAxis);
#	define	CreateManagerConnectorProc	CreateManagerConnector
#else 
/// 动态库版本
typedef Manager::IManagerConnector* (RKT_CDECL*procCreateManagerConnector)(Manager::IManagerMessageHandler * pHandler,rkt::TimerAxis * pTimerAxis);
#	define	CreateManagerConnectorProc	DllApi<procCreateManagerConnector>::load(MAKE_DLL_NAME(ManagerConnector), "CreateManagerConnector")
#endif

#pragma pack()

#endif//__INTERFACE_MANAGER_SERVER_CONNECTOR_H__