/*******************************************************************
** 文件名:	DPassportModule.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	宋文武 (sww8@163.com)
** 日  期:	2011-05-16
** 版  本:	1.0
** 描  述:	通行证服务器模块定义	
** 应  用:  	
	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once


#ifndef __D_PassportModule_H__
#define __D_PassportModule_H__


// 打印调试信息,并设为测试模式
//#define PASSPORT_PRINT_DEBUG_INF

#ifndef SafeRelease
#	define SafeRelease(ptr)		if ((ptr)) {(ptr)->Release(); (ptr) = 0;}
#endif


#pragma pack(1)

/////// 公共宏定义 ///////////////////////////////////////////////////////////////////

/// 重连周期10秒
#define  PASSPORT_RECONNECT_INTERVAL     (10*1000)

#define  PASSPORT_BROADCASTALL_NUM	0xFFFF		//WORD 的最大数,0xFFFF

#define  PASSPORT_NAME_MAXSIZE			32		//名称最大长度,32
#define  PASSPORT_MSGSTR_MAXSIZE		64		//信息文字最大长度,64
#define  PASSPORT_TITLE_MAXSIZE			128		//标题字串的长度,128
#define  PASSPORT_LONGSTR_MAXSIZE		256		//长字串的长度,256
#define  PASSPORT_TEXTSTR_MAXSIZE		512		//大字串的长度,512


#define  PASSPORT_KEEPALIVE_INTERVAL		2000	// 两次KeepAlive探测间的时间间隔
#define  PASSPORT_KEEPALIVE_TIMES			5000	// 开始首次KeepAlive探测前的TCP空闭时间
#define  PASSPORT_KEEPALIVE_TIMEOUT			3000	// 无响应时间
#define  PASSPORT_KEEPALIVE_MAXTIMEOUT		120000	// 最大无响应时间

/////// 公共枚举定义 ///////////////////////////////////////////////////////////////////

// 服务器跨线程执行操作代码定义
enum EMPassportServerControlCode
{
	PS_ControlCode_None = 0,		// 无操作
	PS_ControlCode_StartService,	// 启动服务
	PS_ControlCode_StopService,		// 停止服务
	PS_ControlCode_ShutDown,		// 关闭程序,停止服务后要关闭进程
	// 最大
	PS_ControlCode_Max
};

// 服务器列表更新类型定义
enum EMPassportServerListUpdateType
{
	PS_SvrListUpdateType_All = 0,		// 全部
	PS_SvrListUpdateType_Add,			// 新增
	PS_SvrListUpdateType_Edit,			// 修改
	PS_SvrListUpdateType_Del,			// 删除
	// 最大
	PS_SvrListUpdateType_Max
};


///// 公用数据结构定义 //////////////////////////////////////////////

// 通过跨区通行证服务器中转消息必须加入此消息体
struct SMsgPassportHead
{
	DWORD dwWorldID;		// 消息来源游戏世界ID
	DWORD dwServerID;		// 消息来源服务器ID
};

// 游戏服务器信息数据结构
struct SPassportGameServerInfo
{
	DWORD	dwID;				// 服务器ID
	DWORD	dwWorldID;			// 服务器所在游戏世界ID
	WORD	wServerType;		// 服务器类型
	WORD	wSubID;				// 第n号服务器
	DWORD	dwStateFlag;		// 服务器状态
	void*	pExtPart;			// 连接器扩展部件接口

	SPassportGameServerInfo(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

///////////////////////////////////////////////////////////////////
// 事件码枚举定义: ENUM_EVENT_PASSPORT + 消息码名
enum EMEventPassport
{
	ENUM_EVENT_PASSPORT_INVALID = 0,
	ENUM_EVENT_PASSPORT_HANDSHAKE,					// 与通行证服务器握手成功事件
	ENUM_EVENT_PASSPORT_SERVERLIST_ALL,				// 全部服务器加入事件
	ENUM_EVENT_PASSPORT_SERVERLIST_ADD,				// 新增服务器事件
	ENUM_EVENT_PASSPORT_SERVERLIST_DEL,				// 删除服务器事件
	ENUM_EVENT_PASSPORT_WORLDLIST_ALL,				// 全部游戏世界加入事件
	ENUM_EVENT_PASSPORT_WORLDLIST_ADD,				// 新增游戏世界事件
	ENUM_EVENT_PASSPORT_WORLDLIST_EDIT,				// 修改游戏世界事件

	ENUM_EVENT_PASSPORT_MAXID,						// 最大事件码
};

// 最大事件码
#define EVENT_PASSPORT_MAXID						ENUM_EVENT_PASSPORT_MAXID

//////////////////////////////////////////////////////////////////////////
// 与通行证服务器握手成功事件
#define EVENT_PASSPORT_HANDSHAKE				ENUM_EVENT_PASSPORT_HANDSHAKE
// 事件数据结构
struct SEventPassportHandshake
{
	DWORD	dwServerID;				// 服务器ID

	SEventPassportHandshake(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

//////////////////////////////////////////////////////////////////////////
// 全部服务器加入事件
#define EVENT_PASSPORT_SERVERLIST_ALL				ENUM_EVENT_PASSPORT_SERVERLIST_ALL
// 事件数据结构
struct SEventPassportServerListAll
{
	DWORD	dwServerCounts;				// 收到服务器个数

	SEventPassportServerListAll(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

//////////////////////////////////////////////////////////////////////////
// 新增服务器事件
#define EVENT_PASSPORT_SERVERLIST_ADD				ENUM_EVENT_PASSPORT_SERVERLIST_ADD
// 事件数据结构
struct SEventPassportServerListAdd
{
	DWORD	dwID;				// 服务器ID
	DWORD	dwWorldID;			// 服务器所在游戏世界ID
	WORD	wServerType;		// 服务器类型
	WORD	wSubID;				// 第n号服务器
	DWORD	dwStateFlag;		// 服务器状态

	SEventPassportServerListAdd(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

//////////////////////////////////////////////////////////////////////////
// 删除服务器事件
#define EVENT_PASSPORT_SERVERLIST_DEL				ENUM_EVENT_PASSPORT_SERVERLIST_DEL
// 事件数据结构
struct SEventPassportServerListDel
{
	DWORD dwServerID;				// 服务器ID

	SEventPassportServerListDel(void)
	{
		memset(this, 0, sizeof(*this));
	}
};


//////////////////////////////////////////////////////////////////////////
// 全部游戏世界加入事件
#define EVENT_PASSPORT_WORLDLIST_ALL				ENUM_EVENT_PASSPORT_WORLDLIST_ALL
// 事件数据结构
struct SEventPassportWorldListAll
{
	DWORD	dwWorldCounts;				// 收到游戏世界个数

	SEventPassportWorldListAll(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

//////////////////////////////////////////////////////////////////////////
// 新增游戏世界事件
#define EVENT_PASSPORT_WORLDLIST_ADD				ENUM_EVENT_PASSPORT_WORLDLIST_ADD
// 事件数据结构
struct SEventPassportWorldListAdd
{
	DWORD	dwWorldID;			// 游戏世界ID

	SEventPassportWorldListAdd(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

//////////////////////////////////////////////////////////////////////////
// 修改游戏世界事件
#define EVENT_PASSPORT_WORLDLIST_EDIT				ENUM_EVENT_PASSPORT_WORLDLIST_EDIT
// 事件数据结构
struct SEventPassportWorldListEdit
{
	DWORD dwWorldID;				// 游戏世界ID

	SEventPassportWorldListEdit(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

//////////////////////////////////////////////////////////////////////////
// 消息码与数据结构

/***************************************************************/
///////////////////// 通行证服务器模块 的消息码/////////////////
// 上层的消息码必定是 MSG_MODULEID_PASSPORT
/***************************************************************/

///////////////////////////////////////////////////////////////////
// 消息码枚举定义: ENUM + 消息码名
enum EMMsgPassport
{
	ENUM_MSG_PASSPORT_INVALID = 0,
	ENUM_MSG_PASSPORT_HANDSHAKE,						// 握手消息
	ENUM_MSG_PASSPORT_HANDSHAKE_RESPONSE,				// 回应握手消息
	ENUM_MSG_PASSPORT_KEEPALIVE,						// 心跳检查
	ENUM_MSG_PASSPORT_ANSWER_KEEPALIVE,					// 回应心跳检查
	ENUM_MSG_PASSPORT_PARTNERCHECK_PASSWORD,			// 盛大请求帐号验证
	ENUM_MSG_PASSPORT_SENDSMS,							// 发送短信
	ENUM_MSG_PASSPORT_SENDDATA,							// 请求通行证服务器网关服务发送数据给各软件服务器
	ENUM_MSG_PASSPORT_BROADCASTDATA,					// 请求通行证服务器网关服务广播数据给各软件服务器
	ENUM_MSG_PASSPORT_WORLDLIST_UPDATE,					// 游戏世界列表更新
	ENUM_MSG_PASSPORT_SERVERLIST_UPDATE,				// 服务器列表更新
	ENUM_MSG_PASSPORT_MAXID,							// 最大消息码
};

// 最大消息码
#define MSG_PASSPORT_MAXID						ENUM_MSG_PASSPORT_MAXID


// 合作商ID定义
enum
{
	PASSPORT_ID_PARTNER_SELF = 0,		// 我们自己
	PASSPORT_ID_PARTNER_PPS,			// PPS
	PASSPORT_ID_PARTNER_7BAO,			// 7宝
	PASSPORT_ID_PARTNER_SDOA,			// 盛大
	PASSPORT_ID_PARTNER_KUAIW,			// 快玩
	PASSPORT_ID_PARTNER_YY,				// YY
	PASSPORT_ID_PARTNER_KUAIBO,			// 快播
	PASSPORT_ID_PARTNER_Q1,			    // Q1，我们自己的网页版
	PASSPORT_ID_PARTNER_ZY,			    // 中国游戏中心
	PASSPORT_ID_PARTNER_XL,				//迅雷
	PASSPORT_ID_PARTNER_3366,			//3366
	PASSPORT_ID_PARTNER_NewYY,			//新YY
	PASSPORT_ID_PARTNER_WEB = 100,			//web共享登录专用,qq, msn, 支付宝...
};

//////////////////////////////////////////////////////////////////////////
// 握手消息
#define MSG_PASSPORT_HANDSHAKE				ENUM_MSG_PASSPORT_HANDSHAKE
// 通行证服务器连接器与通行证服务器握手消息数据结构
struct SMsgPassportHandshake_SB
{
	DWORD	dwWorldID;						// 服务器所在游戏世界ID
	WORD	nServerType;					// 服务器类型
	WORD	wSubID;							// 第n号服务器
	bool	bIsPublic;						// 是否是公共区的
	char	szName[PASSPORT_NAME_MAXSIZE];	// 服务器名
	char	szWorldName[PASSPORT_NAME_MAXSIZE];	// 游戏世界名

	SMsgPassportHandshake_SB(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

//////////////////////////////////////////////////////////////////////////
// 回应握手消息
#define MSG_PASSPORT_HANDSHAKE_RESPONSE		ENUM_MSG_PASSPORT_HANDSHAKE_RESPONSE

// 通行证服务器回应握手消息数据结构
struct SMsgPassportHandshakeResponse	
{
	DWORD	dwID;				// 连接ID
	DWORD	dwServerTime;		// 服务器时间

	SMsgPassportHandshakeResponse(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

//////////////////////////////////////////////////////////////////////////
// 心跳检查
#define MSG_PASSPORT_KEEPALIVE				ENUM_MSG_PASSPORT_KEEPALIVE


//////////////////////////////////////////////////////////////////////////
// 回应心跳检查
#define MSG_PASSPORT_ANSWER_KEEPALIVE			ENUM_MSG_PASSPORT_ANSWER_KEEPALIVE

//////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////
// 登录服来的联运帐号检查信息
#define MSG_PASSPORT_PARTNERCHECK_PASSWORD			ENUM_MSG_PASSPORT_PARTNERCHECK_PASSWORD
struct SMsgPartnerServer_CheckTokenInfo
{
	int	nPartnerID;
	DWORD	nClientID;
	DWORD	dwSize;		// 数据长度
	SMsgPartnerServer_CheckTokenInfo(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

/////////////////////////////////////////////////////////////////////////
// 登录服来的联运帐号检查信息
#define MSG_PASSPORT_SENDSMS						ENUM_MSG_PASSPORT_SENDSMS
struct SMsgPartnerServer_SendSms
{
	char szMobile[15];
	char szContent[128];
	SMsgPartnerServer_SendSms(void)
	{
		memset(this, 0, sizeof(*this));
	}
};


//////////////////////////////////////////////////////////////////////////
// 请求通行证服务器网关服务发送数据给各软件服务器消息
#define MSG_PASSPORT_SENDDATA					ENUM_MSG_PASSPORT_SENDDATA

// 请求通行证服务器网关服务发送数据给各软件服务器数据结构
struct SMsgPassportSendData
{
	DWORD	dwServerID;			// 软件服务器ID
	DWORD	nDataLens;			// 数据长度
	//BYTE	pData[nDataLens];	// 数据
};


//////////////////////////////////////////////////////////////////////////
// 请求通行证服务器网关服务广播数据给各软件服务器
#define MSG_PASSPORT_BROADCASTDATA					ENUM_MSG_PASSPORT_BROADCASTDATA

// 请求通行证服务器网关服务广播数据给各软件服务器数据结构
struct SMsgPassportBroadcastData
{
	DWORD	dwServerCounts;		// 软件服务器ID列表个数
	DWORD	nDataLens;			// 数据长度
	//DWORD	dwServerID[dwServerCounts];			// 软件服务器ID列表
	//BYTE	pData[nDataLens];					// 数据
};


//////////////////////////////////////////////////////////////////////////
// 游戏世界列表更新
#define MSG_PASSPORT_WORLDLIST_UPDATE					ENUM_MSG_PASSPORT_WORLDLIST_UPDATE

// 消息用 游戏世界数据结构
struct SMsgPassportGameWorldNode
{
	DWORD	dwWorldID;			// 游戏世界ID
	BYTE	nNameStrlens;		// 游戏世界名称字串长度

	SMsgPassportGameWorldNode(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

// 游戏世界列表更新数据结构
struct SMsgPassportWorldListUpdate
{
	BYTE	nUpdateType;		// 更新类型,EMPassportServerListUpdateType
	DWORD	dwWorldCounts;		// 游戏世界个数
	//dwWorldCounts* SMsgPassportGameWorldNode;	// 软件服务器信息列表
	// dwWorldCounts* char szWorldName[nNameStrlens];	// 游戏世界名称列表

	SMsgPassportWorldListUpdate(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

//////////////////////////////////////////////////////////////////////////
// 服务器列表更新
#define MSG_PASSPORT_SERVERLIST_UPDATE					ENUM_MSG_PASSPORT_SERVERLIST_UPDATE

// 消息用 游戏服务器信息数据结构
struct SMsgPassportGameServerNode
{
	DWORD	dwID;				// 服务器ID
	DWORD	dwWorldID:16;		// 服务器所在游戏世界ID
	WORD	wServerType:8;		// 服务器类型
	WORD	wSubID:8;			// 第n号服务器
	DWORD	dwStateFlag:8;		// 服务器状态

	SMsgPassportGameServerNode(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

// 服务器列表更新数据结构
struct SMsgPassportServerListUpdate
{
	BYTE	nUpdateType;		// 更新类型,EMPassportServerListUpdateType
	DWORD	dwServerCounts;		// 软件服务器个数
	//dwServerCounts* SMsgPassportGameServerNode;	// 软件服务器信息列表

	SMsgPassportServerListUpdate(void)
	{
		memset(this, 0, sizeof(*this));
	}
};



#pragma pack()


#endif	// __D_PassportModule_H__