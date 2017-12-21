/*******************************************************************
** 文件名:	ICenterServerConnector.h 
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

#ifndef __INTERFACE_CENTER_SERVER_CONNECTOR_H__
#define __INTERFACE_CENTER_SERVER_CONNECTOR_H__

#include "DGlobalMessage.h"
#include "ICenterServerProtocol.h"
#include "net/net.h"
#include "GlobalServerDef.h"

struct ICommandFactory;

#pragma pack(1)
#pragma warning(disable:4200)

namespace CenterServer
{
	/// 最大服务器个数
#	define  MAX_SERVER_COUNT       2048
	/// 重连周期
#	define  RECONNECT_INTERVAL     (10*1000)

	struct ServerData;
	//class TCAPABILITYNODE;
	//class TMONITORNODE;

	/**
	@name : 处理中心服务器发过来的消息
	@brief:
	*/
	struct ICenterServerMessageHandler
	{
		/**
		@name         : 通知服务器列表已经更新
		@brief        : 服务器列表更新只会在有服务器进入退出时触发
		@note         : 具体的列表调用ICenterServerConnector::GetServerList获得
		*/
		virtual void HandleServerListUpdated() = 0;

		/**
		@name         : 通知服务器信息更新
		@brief        : 例如服务器人数变化等等
		*/
		virtual void HandleServerInfoUpdated(CSID ServerID,SERVER_STATE nState,ServerData * pInfo) = 0;

		/**
		@name         : 处理其他服务器通过中心服务器转发的消息
		@brief        : 具体内容不清楚
		@param server : 该消息是哪个服务器发过来的
		@param pData  : 消息内容
		@param wDataLen:消息长度
		*/
		virtual void HandlePostMessage(CGID server,const char * pData,WORD wDataLen) = 0;

		/**
		@name         : 收到请求,执行一个命令
		@brief        : 
		@param server : 发送命令的服务器ID
		@param szCommand : 命令行
		@return       : 执行后调用ICenterServerConnector::SendCommandResponse返回命令执行结果
		*/
		virtual void HandleCommandRequest(CGID server,const char * szCommand) = 0;

		/**
		@name          : 处理命令执行的结果
		@brief         :
		@param szCommand:执行的命令字符串
		@param bSuccess: 命令是否执行成功
		@param szOutput: 输出的提示信息
		@return        :
		*/
		virtual void HandleCommandResponse(const char * szCommand,bool bSuccess,const char * szOutput )= 0;

		/**
		@name			: 与中心服握手成功
		@brief			:
		@param			:
		@return			:
		*/
		virtual void OnHandleHandshake() {}
	};

	/**
	@name : 中心服务器连接器
	@brief: 该模块负责维护和中心服务器之间的连接,并转发各种命令
	*/
	struct ICenterServerConnector
	{
		/**
		@name         : 连接中心服务器
		@brief        : 启动服务器时连接中心服务器,以后如果连接断开,该模块内部会定时重试
		@param szCSIP : 中心服务器IP 
		@param nCSPort: 中心服务器端口
		@return       : 如果没有网络错误,则返回成功
		*/
		virtual bool ConnectCenterServer(const char * szCSIP,WORD nCSPort,int nThreadMode = NetworkUserType_Default) = 0;

		/**
		@name                 : 向中心服务器注册本地服务器信息
		@brief                : 如果本地服务器信息更改了,也调用这个接口进行Update 
		@param szName         : 服务器名称,必须小于MAX_SERVER_NAME
		@param dwGroupID      : 本地服务器所在的群组ID
		@param wServerType    : 本地服务器的类型,例如是区域服务器还是网关服务器
		@return               : 
		*/
		virtual bool RegisterServer(const char * szName,CGID dwGroupID,WORD wServerType) = 0;

		/**
		@name                 : 向中心服务器更新服务器信息
		@brief                :
		@param  pServerInfo   : 服务器信息内容,不同的服务器有不同的扩展信息,例如区域服务器就有该服务器负责的区域ID等
		@param  wLen          : 服务器信息长度
		@return               :
		*/
		virtual bool UpdateServerInfo(LPVOID pServerInfo,WORD wLen) = 0;

		/**
		@name         : 取得本地服务器在中心服务器上的ID
		@brief        :
		*/
		virtual CSID  GetLocalServerCSID() = 0;

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
		@note                : DWORD count=ICenterServerConnector::GetServerList(serverPtrArray,1024,1,MSG_SERVER_GATE);
		@return              :
		*/
		virtual DWORD GetServerList(ServerData * serverPtrArray[],DWORD dwArratSize,CGID GroupFilter=INVALID_SERVER_ID,WORD wTypeFilter=MSG_ENDPOINT_UNKNOW) = 0;

		/**
		@name                : 取得服务器信息
		@brief               :
		@param  ServerID     : 服务器ID
		@return              : 如果目标服务器存在则返回服务器信息,否则返回0
		*/
		virtual ServerData * GetServerInfo(CSID ServerID) = 0;

		/**
		@name                : 向某个服务器发送一条消息
		@brief               : 服务器之间的消息转发
		@param DestServerID  : 目标服务器ID
		@param pMessage      : 消息内容
		@param wMessageLen   : 消息长度
		@return              : 是否成功发到了中心服务器
		*/
		virtual bool  PostMessage(CSID DestServerID,const char * pMessage,WORD wMessageLen) = 0;

		/**
		@name                : 向多个服务器广播消息
		@brief               :
		@param DestServerArray:目标服务器数组
		@param wDestServerCount:目标服务器个数
		@param pMessage      : 消息内容
		@param wMessageLen   : 消息长度
		@return              : 是否成功发到了中心服务器
		*/
		virtual bool  BroadcastMessage(CSID * DestServerArray,WORD wDestServerCount,const char * pMessage,WORD wMessageLen) = 0;

		/**
		@name                : 请求远端服务器执行一个命令语句
		@brief               :
		@param DestServerID  : 目标服务器ID
		@oaram szCommandStr  : 命令语句,注意必须是0结尾的字符串
		@return              : 是否成功发到了中心服务器
		*/
		virtual bool  RemoteCommandRequest(CSID DestServerID,const char * szCommandStr) = 0;

		/**
		@name                : 发送命令执行的结果
		@brief               :
		@param DestServerID  : 目标服务器ID
		@param bSuccess      : 命令是否执行成功
		@param szCommandStr  : 命令语句,null-terminated string
		@param szResponse    : 执行的结果,null-terminated string
		@return              : 
		*/
		virtual bool  SendCommandResponse(CSID DestServerID,bool bSuccess,const char * szCommandStr,const char * szResponse) = 0;

		virtual void  Release() = 0;

		/**
		@name                : 直接发送数据给中心服务器
		@brief               : 因为中心服务器上挂接了部分游戏逻辑模块,这些模块可能需要以自己的格式进行通讯
		@param pData         : 要发送的数据
		@param dwLen         : 数据长度
		*/
		virtual void  SendDataToCenterServer(const char * pData,DWORD dwLen) = 0;

		/**
		@name                : 直接发送数据给世界服务器
		@brief               : 因为世界服务器上挂接了部分游戏逻辑模块,这些模块可能需要以自己的格式进行通讯
		@param pData         : 要发送的数据
		@param dwLen         : 数据长度
		*/
		virtual void  SendDataToWorldServer(const char * pData,DWORD dwLen) = 0;

		/** 是否与中心服断开连接了
		@param   
		@param   
		@return  
		*/
		virtual bool  IsDisconnectToCenter(void) = 0;

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

		/** 开始消息统计
		@param   
		@param   
		@return  
		*/
		virtual void StartMessageStat() = NULL;

		/** 打印当前消息统计信息
		@param   
		@param   
		@return  
		*/
		virtual void DumpMessageStat() = NULL;

		/** 结束当前消息统计信息
		@param   
		@param   
		@return  
		*/
		virtual void EndMessageStat() = NULL;

        // 获取静态地图所在的场景服ID
        virtual ServerData* GetZoneServerOfStaticMap(int nMapID) = 0;

        // 获取某个场景的序列号
        virtual int GetSceneSerialNo(CSID server, int nSceneID) = 0;

        // 获取当前服务器类型
    	virtual WORD GetServerType() = 0;
	};

	/// 单个服务器的信息
	struct ServerData
	{
		CSID    dwServerID;       // 服务器ID
		CGID    dwGroupID;        // 服务器所属的群组ID
		WORD    wServerType;      // 服务器类型,例如是区域服务器还是网关服务器
		char    szName[MAX_SERVER_NAME]; // 服务器名字
		WORD    wServerInfoLen;   // 服务器专有的信息长度
		char  * pServerInfoData;  // 服务器专有的信息内容,该数组的长度是wServerInfoLen的值

        ServerData()
        {
            memset(this, 0, sizeof(*this));
        }
	};

	/// 网关服务器信息
	struct ServerInfo_Gateway
	{
		DWORD   dwGatewayIP;      // 网关IP,网络字节序,因为网关一般都在局域网,所以这个一般是内网IP
		WORD    wGatewayPort;     // 网关端口,网络字节序
		WORD    wGatewayID;       // 网关ID
		WORD	 wClientPort;			//网关侦听客户端连接的端口	add by lgf 10.04.26
		WORD		wReserved;			//保留字段 目的为了4字节对齐 add by lgf 10.04.28
		DWORD dwGatewayClientNum;	//网关上的玩家人数
		DWORD   dwGatewayOuterIP;	//add by lgf 10.07.13 网关外网IP
		DWORD   dwNetworkType;			//add by lgf 12.02.27 网络类型
	};

    /// 区域服务器信息
    struct ServerInfo_Zone
    {
        struct  MapZone
        {
            DWORD   dwZoneID;               // 场景ID
            WORD    dwMapID;	            // 地图ID
            int     nSerialNo;              // 序列号
            WORD    dwMapPlayerCount;       // 该地图玩家数量
            WORD	dwPreEnterPlayerCount;  // 预进入该地图的玩家数量
        };

        DWORD	dwZoneServerID;	  // 场景服务器ID
        DWORD   dwMaxUserCount;   // 最大玩家个数
        DWORD   dwUserCount;      // 玩家个数
        WORD    wMapNum;          // 该服务器负责的完整地图个数 
        WORD    wStaticMapNum;    // 静态地图数量(pMapArray前wStaticMapNum个是静态地图)
        DWORD   dwFlags;          // 用于附加数据，0x0=为单线程模式，0x1=为多线程模式
        WORD    wZoneServerIndex; // 场景序号
        MapZone pMapArray[];      // 该服务器负责的完整地图数组
    };



	/**
	@name            : 创建一个中心服务器连接对象
	@brief           :
	@param pHandler  : 处理接口指针
	@param pTimerAxis: 时间轴接口,主要用来定时重试
	@param pCmdFactory:命令工厂,用来执行远程服务器请求执行的命令行语句
	@return          : 返回中心服务器连接对象指针
	*/
	//CSCAPI ICenterServerConnector * CreateCenterServerConnector(ICenterServerMessageHandler * pHandler,rkt::TimerAxis * pTimerAxis);
};

/// 静态库版本
#if defined(_LIB) || defined(CENTER_STATIC_LIB)
#	pragma comment(lib, MAKE_LIB_NAME(CenterServerConnector))
extern "C" CenterServer::ICenterServerConnector * CreateCenterServerConnector(CenterServer::ICenterServerMessageHandler * pHandler,rkt::TimerAxis * pTimerAxis);
#	define	CreateCenterServerConnectorProc	CreateCenterServerConnector
#else 
/// 动态库版本
typedef CenterServer::ICenterServerConnector* (RKT_CDECL*procCreateCenterServerConnector)(CenterServer::ICenterServerMessageHandler * pHandler,rkt::TimerAxis * pTimerAxis);
#	define	CreateCenterServerConnectorProc	DllApi<procCreateCenterServerConnector>::load(MAKE_DLL_NAME(CenterServerConnector), "CreateCenterServerConnector")
#endif

#pragma pack()

#endif//__INTERFACE_CENTER_SERVER_CONNECTOR_H__