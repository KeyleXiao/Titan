/*******************************************************************
** 文件名:	WorldMessage.h
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

#ifndef __WORLDMESSAGE_H__
#define __WORLDMESSAGE_H__

#pragma pack(1)
#pragma warning(disable:4200)

/**
@name : 中心服务器通信格式
@brief: 中心服务器在每一个TCP连接上同时支持两类协议
        1.文本形式的命令行协议 (类似Telnet)
		2.二进制形式的消息码通讯形式
		数据包结构是所有服务器通用的: 消息头 = 目标服务器ID(2bytes) + 模块ID(2bytes) + 消息码(2bytes)
*/

namespace WorldServer
{
	typedef     DWORD       WSID;				// 世界服务器上维护的服务器ID
	typedef     DWORD       WGID;				// 服务器所属的群组ID
#	define      INVALID_SERVER_ID   0xFFFFFFFF	// 不合法的服务器ID 
#	define      WS_LISTEN_PORT 8001				// 世界服务器默认监听端口
#	define		MAX_SERVER_NAME        32		// 最大服务器名字

	enum TWServer_State							// 服务器状态定义
	{
		emWServer_Connected = 1,				// 服务器已连接
		emWServer_Update       ,				// 服务器信息更新
		emWServer_Disconnected ,			    // 服务器断开连接
	};

	/// 单个服务器的信息
	struct SWServerData
	{
		WSID    dwServerID;       // 服务器ID
		WGID    dwGroupID;        // 服务器所属的群组ID
		WORD    wServerType;      // 服务器类型,例如是区域服务器还是网关服务器
		char    szName[MAX_SERVER_NAME]; // 服务器名字
		WORD    wServerInfoLen;   // 服务器专有的信息长度
		char  * pServerInfoData;  // 服务器专有的信息内容,该数组的长度是wServerInfoLen的值
	};

	/**
	@name         : 中心服发给世界服的握手消息
	@brief        : 数据包结构见SMsgWorldServerHandshake
	*/
#	define MSG_WORLDSERVER_HANDSHAKE			1
	struct SMsgWorldServer_Handshake_CW
	{
		WGID    dwGroupID;        // 服务器所属的群组ID
		WORD    wServerType;      // 服务器类型,例如是区域服务器还是网关服务器
		char    szName[MAX_SERVER_NAME];// 服务器名称
        WORD    wExternDataLen;   // 服务器专有的扩展数据长度
		char    pExternData[];    // 服务器专有的扩展数据内容
	};

	/**
	@name         : 世界服发回给中心服的握手回复包
	@brief        : 数据包结构见SMsgWorldServer_HandshakeResponse
	*/
#	define MSG_WORLDSERVER_HANDSHAKE_RESPONSE	2
	struct SMsgWorldServer_HandshakeResponse_WC
	{
		WSID   dwServerID;       // 服务器在中心服务器上的ID
		DWORD  dwErrorCode;      // 如果握手失败,例如发现了重复的服务器ID,则错误码不为0
	};

	/**
	@name         : 通知全局服务器列表发生了变化,即有新的服务器加入或断开
	@brief        : 数据包结构见SMsgWorldServer_UpdateServerList
	*/
#	define MSG_WORLDSERVER_SERVER_LIST_UPDATE	3
	struct SMsgWorldServer_UpdateServerList_WC
	{
		struct ServerInfo
		{
			WSID    dwServerID;       // 服务器ID
			WSID    dwGroupID;        // 服务器所属的群组ID
			WORD    wServerType;      // 服务器类型,例如是区域服务器还是网关服务器
			char    szName[MAX_SERVER_NAME];// 服务器名称
			WORD    wExternDataLen;   // 服务器专有的扩展数据长度
			char    pExternData[1];   // 服务器专有的扩展数据内容,该数组的长度是wExternDataLen的值
		};

		WORD        wServerNum;       // 服务器个数
		ServerInfo  wServerArray[];   // 服务器信息数组,注意:每个服务器的信息是不定长的
	};

	/**
	@name        : 服务器信息更新(CW)
	@brief       : 包的内容即ServerInfo结构,不同服务器ServerInfo不同
	*/
#	define MSG_WORLDSERVER_SERVER_INFO			4

	/**
	@name         : 通知服务器信息变化(WC)
	@brief        : 消息结构为 ServerState(int 4) + SMsgWorldServer_UpdateServerList::ServerInfo
	*/
#	define MSG_WORLDSERVER_SERVER_INFO_UPDATE	5
	
	/**
	@name         : 世界服给区域服务器发送消息（WZ）
	@brief        : 数据包结构见SMsgCenterServer_Message_W2Z
	*/
#	define MSG_WORLDSERVER_MESSAGE_WZ		6

	struct SMsgWorldServer_Message_WZ
	{
		ulong   dwSourceServerID;  // 源服务器ID
		WORD   wDataLen;          // 要发送的数据长度
		// 要发送的数据内容 char   pDataContent[];    
	};

	/**
	@name         : 世界服给某种类型区域服务器广播消息（WZ）
	@brief        : 数据包结构见SMsgCenterServer_Message_WBZ
	*/
#	define MSG_WORLDSERVER_BROADCASTMESSAGE_WZ		7

	struct SMsgWorldServer_BroadcastMessage_WZ
	{
		int    dwSourceServerType;  // 源服务器类型
		WORD   wDataLen;            // 要发送的数据长度
		// 要发送的数据内容 char   pDataContent[];
	};

	#	define MSG_WORLDSERVER_MESSAGE_ZW			8

	#	define MSG_WORLDSERVER_ASEVER_CONNECTED		9
	struct SMsgWorldServer_ASeverConnected
	{
		ulong nServerID;			// 区域服务器ID
		int   nConnectedState;      // 连接状态
	};
};

#pragma pack()

#endif//__INTERFACE_WORLD_SERVER_PROTOCOL_H__