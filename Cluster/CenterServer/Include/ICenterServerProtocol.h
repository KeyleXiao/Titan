/*******************************************************************
** 文件名:	ICenterServerProtocol.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	02/20/2008
** 版  本:	1.0
** 描  述:	
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#ifndef __INTERFACE_CENTER_SERVER_PROTOCOL_H__
#define __INTERFACE_CENTER_SERVER_PROTOCOL_H__

#pragma pack(1)
#pragma warning(disable:4200)

/**
@name : 中心服务器通信格式
@brief: 中心服务器在每一个TCP连接上同时支持两类协议
        1.文本形式的命令行协议 (类似Telnet)
		2.二进制形式的消息码通讯形式
		数据包结构是所有服务器通用的: 消息头 = 目标服务器ID(2bytes) + 模块ID(2bytes) + 消息码(2bytes)
*/

namespace CenterServer
{
	typedef     DWORD       CSID;				/// 中心服务器上维护的服务器ID
	typedef     DWORD       CGID;				/// 服务器所属的群组ID
#	define      INVALID_SERVER_ID   0xFFFFFFFF 	/// 不合法的服务器ID 
#	define      DEFAULT_LISTEN_PORT 8001		/// 中心服务器默认监听端口
#	define		MAX_SERVER_NAME        32		/// 最大服务器名字

	enum SERVER_STATE							/// 服务器状态定义
	{
		SERVER_CONNECTED = 1,		// 服务器已连接
		SERVER_UPDATE       ,       // 服务器信息更新
		SERVER_DISCONNECTED ,       // 服务器断开连接
	};

	//////////////////////////////////////////////////////////////////////////
	enum EMCENTERSERVERMESSAGE
	{
		EMMSG_CENTERSERVER_HANDSHAKE = 1,			// 游戏服务器发给中心服务器的握手消息
		EMMSG_CENTERSERVER_HANDSHAKE_RESPONSE,		// 中心服务器发回给游戏服务器的握手回复包
		EMMSG_CENTERSERVER_SERVER_LIST_UPDATE,		// 通知全局服务器列表发生了变化,即有新的服务器加入或断开
		EMMSG_CENTERSERVER_POST_MESSAGE,			// 转发消息给指定服务器
		EMMSG_CENTERSERVER_BROADCAST_MESSAGE,		// 广播消息
		EMMSG_CENTERSERVER_EXECUTE_COMMAND,			// 执行一个命令行语句
		EMMSG_CENTERSERVER_COMMAND_RESPONSE,		// 返回命令执行的结果
		EMMSG_CENTERSERVER_SERVER_INFO,				// 服务器信息更新
		EMMSG_CENTERSERVER_SERVER_INFO_UPDATE,		// 通知服务器信息变化
        EMMSG_CENTERSERVER_TO_WORLDMESSAGE,         // 发消息给给世界服(ZW)
		EMMSG_CENTERSERVER_SERVERCHECKREQ,			// 发检测请求给中心服，由中心服转发至其它服务器，同时中心服自己对消息做出回应
		EMMSG_CENTERSERVER_SERVERCHECKRES,			// 回应检测请求给中心服连接器
	};

	/**
	@name         : 游戏服务器发给中心服务器的握手消息
	@brief        : 数据包结构见SMsgCenterServerHandshake
	*/
#	define MSG_CENTERSERVER_HANDSHAKE				EMMSG_CENTERSERVER_HANDSHAKE

	struct SMsgCenterServer_Handshake
	{
		CGID    dwGroupID;        // 服务器所属的群组ID
		WORD    wServerType;      // 服务器类型,例如是区域服务器还是网关服务器
		char    szName[MAX_SERVER_NAME];// 服务器名称
        WORD    wExternDataLen;   // 服务器专有的扩展数据长度
		char    pExternData[];    // 服务器专有的扩展数据内容
	};

	/**
	@name         : 中心服务器发回给游戏服务器的握手回复包
	@brief        : 数据包结构见SMsgCenterServer_HandshakeResponse
	*/
#	define MSG_CENTERSERVER_HANDSHAKE_RESPONSE		EMMSG_CENTERSERVER_HANDSHAKE_RESPONSE

	struct SMsgCenterServer_HandshakeResponse
	{
		CSID   dwServerID;       // 服务器在中心服务器上的ID
		DWORD  dwErrorCode;      // 如果握手失败,例如发现了重复的服务器ID,则错误码不为0
	};

	/**
	@name         : 通知全局服务器列表发生了变化,即有新的服务器加入或断开
	@brief        : 数据包结构见SMsgCenterServer_UpdateServerList
	*/
#	define MSG_CENTERSERVER_SERVER_LIST_UPDATE		EMMSG_CENTERSERVER_SERVER_LIST_UPDATE

	struct SMsgCenterServer_UpdateServerList
	{
		struct ServerInfo
		{
			CSID    dwServerID;       // 服务器ID
			CGID    dwGroupID;        // 服务器所属的群组ID
			WORD    wServerType;      // 服务器类型,例如是区域服务器还是网关服务器
			char    szName[MAX_SERVER_NAME];// 服务器名称
			WORD    wExternDataLen;   // 服务器专有的扩展数据长度
			char    pExternData[1];   // 服务器专有的扩展数据内容,该数组的长度是wExternDataLen的值
		};

		WORD        wServerNum;       // 服务器个数
		ServerInfo  wServerArray[];   // 服务器信息数组,注意:每个服务器的信息是不定长的
	};

	/**
	@name         : 转发消息给指定服务器
	@brief        : 数据包结构见SMsgCenterServer_PostMessage
	*/
#	define MSG_CENTERSERVER_POST_MESSAGE			EMMSG_CENTERSERVER_POST_MESSAGE

	struct SMsgCenterServer_PostMessage
	{
		CSID   dwDestServerID;    // 目标服务器ID       
		CSID   dwSourceServerID;  // 源服务器ID
		WORD   wDataLen;          // 要发送的数据长度
		char   pDataContent[];    // 要发送的数据内容
	};

	/** 广播消息
	@brief: 数据包结构见SMsgCenterServer_BroadcastMessage
	*/
#	define MSG_CENTERSERVER_BROADCAST_MESSAGE		EMMSG_CENTERSERVER_BROADCAST_MESSAGE
	struct SMsgCenterServer_BroadcastMessage
	{
		WORD   wDestServerNum;    // 目标服务器个数
		CSID   DestServerArray[1];// 目标服务器数组,数组个数为wDestServerNum指定的值
		CSID   dwSourceServerID;  // 源服务器ID
		WORD   wDataLen;          // 要发送的数据长度
		char   pDataContent[1];   // 要发送的数据内容
	};

	/** 执行一个命令行语句
	@brief: 数据包结构见SMsgCenterServer_ExecuteCommand
	*/
#	define MSG_CENTERSERVER_EXECUTE_COMMAND			EMMSG_CENTERSERVER_EXECUTE_COMMAND
	struct SMsgCenterServer_ExecuteCommand
	{
		CSID  RequesterID;        // 请求执行命令的服务器ID
		WORD  wCommandLen;        // 命令字符串长度
		char  szCommandStr[];     // 要执行的命令,注意该字符串必须以0结尾
	};

	/** 返回命令执行的结果
	@brief: 数据包结构见SMsgCenterServer_CommandResponse
	*/
#	define MSG_CENTERSERVER_COMMAND_RESPONSE		EMMSG_CENTERSERVER_COMMAND_RESPONSE
	struct SMsgCenterServer_CommandResponse
	{
		bool  bSuccess;           // 改语句是否执行成功
		WORD  wCommandLen;        // 命令字符串长度
		char  szCommand[1];       // 命令字符串
		WORD  wOutputLen;         // 错误提示字符串长度
		char  szOutput[];         // 错误提示,主语该字符穿必须以0结尾
	};

	/** 服务器信息更新
	@brief: 包的内容即ServerInfo结构,不同服务器ServerInfo不同
	*/
#	define MSG_CENTERSERVER_SERVER_INFO				EMMSG_CENTERSERVER_SERVER_INFO

	/** 通知服务器信息变化
	@brief: 消息结构为 ServerState(int 4) + SMsgCenterServer_UpdateServerList::ServerInfo
	*/
#	define MSG_CENTERSERVER_SERVER_INFO_UPDATE		EMMSG_CENTERSERVER_SERVER_INFO_UPDATE

	/**
	@name         : 发消息给给世界服(ZW)
	@brief        : 数据包结构见SMsgCenterServer_PostMessage
	*/
#	define MSG_CENTERSERVER_TO_WORLDMESSAGE         EMMSG_CENTERSERVER_TO_WORLDMESSAGE
	struct SMsgCenterServer_ToWorldMessage
	{
		CSID   dwSourceServerID;  // 源服务器ID
		WORD   wDataLen;          // 要发送的数据长度
		// 要发送的数据内容 char   pDataContent[];
	};

	/** 发检测请求给中心服，由中心服转发至其它服务器，同时中心服自己对消息做出回应
	*/
#	define MSG_CENTERSERVER_SERVERCHECKREQ			EMMSG_CENTERSERVER_SERVERCHECKREQ
	struct SMsgCenterServer_ServerCheck
	{
		int nCheckTime;		// 开始检测采集的时间点
		int nSerial;		// 序号
		int nGameWorldID;	// 游戏世界ID
		int nBeginTick;		// 检测开始的TICK数
		int nEndTick;		// 检测结数的TICK数
		WORD wServerType;	// 服务器类型
		WORD wServerNo;		// 服务器编号（同类型时）
		int nFromServerID;	// 来源服务器ID
		int nToServerID;	// 到达服务器ID
		SMsgCenterServer_ServerCheck()
		{
			memset(this, 0, sizeof(*this));
		}
	};

	/** 回应检测请求给中心服连接器
	*/
#	define MSG_CENTERSERVER_SERVERCHECKRES			EMMSG_CENTERSERVER_SERVERCHECKRES
};

#pragma pack()

#endif//__INTERFACE_CENTER_SERVER_PROTOCOL_H__