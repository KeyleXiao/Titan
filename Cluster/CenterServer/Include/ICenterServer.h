/*******************************************************************
** 文件名:	ICenterServer.h
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
#include "ICenterServerProtocol.h"

// 事件引擎接口
namespace rkt
{
	struct IEventEngine;
}

namespace CenterServer
{
	struct ICSMessageHandler;  // 中心服务器消息处理器
	struct ICenterServerPlugin;// 中心服务器插件接口
	struct ServerData;         // 服务器信息
 
	/**
	@name : 中心服务器接口 
	@brief: 该接口主要提供给需要挂接再中心服务器上的游戏逻辑模块使用
	*/

	struct ICenterServer
	{
		//////////////////////////////////////////////////////////////////////////////
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
		virtual DWORD GetServerList(CSID * serverPtrArray,DWORD dwArratSize,CGID GroupFilter=INVALID_SERVER_ID,WORD wTypeFilter=MSG_ENDPOINT_UNKNOW) = 0;

		/**
		@name                : 取得服务器信息
		@brief               :
		@param  ServerID     : 服务器ID
		@param  info         : 返回服务器信息
		@return              : 如果目标服务器存在则返回服务器信息,否则返回false
		*/
		virtual bool GetServerInfo(CSID ServerID,ServerData & info) = 0;


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

		//////////////////////////////////////////////////////////////////////////////
		/**
		@name                : 注册消息处理器
		@brief               : 各模块解释自己的消息
		@param pHandler      : 消息处理接口
		@oaram wModuleID     : 要处理的模块ID（只接收该模块的消息,如果传MSG_MODULEID_UNKNOW则订阅所有消息）
		@return              : 
		*/
		virtual bool RegisterMessageHandler(ICSMessageHandler * pHandler,WORD wModuleID=MSG_MODULEID_UNKNOW) = 0;

		/**
		@name                : 注销消息处理器
		@param pHandler      : 消息处理接口
		@return              : 
		*/
		virtual bool UnregisterMessageHandler(ICSMessageHandler * pHandler) = 0;

		/**
		@name                : 取得插件数目
		@return              : 
		*/
		virtual int GetPluginCount() = 0;

		/**
		@name                : 根据插件名称查找指定插件
		@brief               : 中心服务器启动时读取插件脚本,然后载入插件
		@brief               : 中心服务器上的各插件之间可能通过该方法查找对方
		@param szPluginName  : 插件名称
		@return              : 返回插件接口指针
		*/
		virtual ICenterServerPlugin * GetPlugin(const char * szPluginName ) = 0;

		/**
		@name                : 返回所有插件列表
		@param pPluginList   : 用来返回插件列表的缓冲区
		@param nMaxReturnSize: 最多返回的个数，根据缓冲区大小决定
		@return              : 实际返回的个数
		*/
		virtual int GetAllPlugins(ICenterServerPlugin ** pPluginList,int nMaxReturnSize ) = 0;

		/**
		@name                : 取得事件引擎接口
		@return              : 
		*/
		virtual rkt::IEventEngine * GetEventEngine() = 0;
	};

	/**
	@name : 中心服务器事件处理接口
	@brief:
	*/
	struct ICSMessageHandler
	{
		/**
		@name                : 收到消息
		@param pHeader       : 消息头
		@param pData         : 数据内容
		@param dwDataLen     : 数据长度
		@return              : 
		*/
		virtual void OnMessage(SGameMsgHead * pHeader,const char * pData,DWORD dwDataLen) = 0;
	};

	/**
	@name : 中心服务器插件接口
	@brief: 插件的配置方法:
	        1.编译DLL,并导出形式为CenterServerPluginCreator的创建函数
			2.配置中心服务器启动目录下的plugin.xml
			3.在plugin.xml中填写dll路径及创建函数名
	*/
	struct ICenterServerPlugin
	{
		// 插件名称
		virtual const char * GetName() = 0;

		// 插件初始化
		virtual void Initialize(ICenterServer * pCenterServer) = 0;

		// 销毁插件
		virtual void Release() = 0;
	};

	/**
	@name : 插件创建函数形式
	@brief: 请在插件DLL中导出该创建函数
	*/
	typedef  ICenterServerPlugin *(*CenterServerPluginCreator)();

	/// 单个服务器的信息
	struct ServerData
	{
		CSID    dwServerID;       // 服务器ID
		CGID    dwGroupID;        // 服务器所属的群组ID
		WORD    wServerType;      // 服务器类型,例如是区域服务器还是网关服务器
		char    szName[MAX_SERVER_NAME]; // 服务器名字
		WORD    wServerInfoLen;   // 服务器专有的信息长度
		char  * pServerInfoData;  // 服务器专有的信息内容,该数组的长度是wServerInfoLen的值
	};

	/// 网关服务器信息
	struct ServerInfo_Gateway
	{
		DWORD   dwGatewayIP;      // 网关IP,网络字节序,因为网关一般都在局域网,所以这个一般是内网IP
		WORD    wGatewayPort;     // 网关端口,网络字节序
	};

	/// 快捷宏
	#define gCenterServer		   ((ICenterServer*)getGlobal())
}