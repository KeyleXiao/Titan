/*******************************************************************
** 文件名:	e:\Rocket\Common\"BridgeModuleDef.h"
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	宋文武 (sww8@163.com)
** 日  期:	2009/8/20
** 版  本:	1.0
** 描  述:	跨区桥服务器模块定义	
** 应  用:  	
	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once


#ifndef __BRIDGEMODULE_DEF_H__
#define __BRIDGEMODULE_DEF_H__


// 打印调试信息,并设为测试模式
//#define BRIDGE_PRINT_DEBUG_INF

#ifndef SafeRelease
#	define SafeRelease(ptr)		if ((ptr)) {(ptr)->Release(); (ptr) = 0;}
#endif


#pragma pack(1)

/////// 公共宏定义 ///////////////////////////////////////////////////////////////////

/// 重连周期10秒
#define  BRIDGE_RECONNECT_INTERVAL     (10*1000)

#define  BRIDGE_BROADCASTALL_NUM	0xFFFF		//WORD 的最大数,0xFFFF

#define  BRIDGE_NAME_MAXSIZE		32		//名称最大长度,32
#define  BRIDGE_MSGSTR_MAXSIZE		64		//信息文字最大长度,64
#define  BRIDGE_TITLE_MAXSIZE		128		//标题字串的长度,128
#define  BRIDGE_LONGSTR_MAXSIZE		256		//长字串的长度,256
#define  BRIDGE_TEXTSTR_MAXSIZE		512		//大字串的长度,512


#define  BRIDGE_KEEPALIVE_INTERVAL		2000	// 两次KeepAlive探测间的时间间隔
#define  BRIDGE_KEEPALIVE_TIMES			5000	// 开始首次KeepAlive探测前的TCP空闭时间
#define  BRIDGE_KEEPALIVE_TIMEOUT		3000	// 无响应时间
#define  BRIDGE_KEEPALIVE_MAXTIMEOUT	120000	// 最大无响应时间

/////// 公共枚举定义 ///////////////////////////////////////////////////////////////////

// 服务器跨线程执行操作代码定义
enum EMBridgeServerControlCode
{
	BS_ControlCode_None = 0,		// 无操作
	BS_ControlCode_StartService,	// 启动服务
	BS_ControlCode_StopService,		// 停止服务
	BS_ControlCode_ShutDown,		// 关闭程序,停止服务后要关闭进程
	// 最大
	BS_ControlCode_Max
};


// 服务器列表更新类型定义
enum EMBridgeServerListUpdateType
{
	BS_SvrListUpdateType_All = 0,		// 全部
	BS_SvrListUpdateType_Add,			// 新增
	BS_SvrListUpdateType_Edit,			// 修改
	BS_SvrListUpdateType_Del,			// 删除
	// 最大
	BS_SvrListUpdateType_Max
};


// 服务器扩展信息子消息代码定义,小于LocalMaxID为本地部件处理
enum EMBridgeServerExtDataMsgCode
{
	BS_SvrExtDataMsg_RequestAll = 0,		// 请求全部数据
	BS_SvrExtDataMsg_LocalMaxID,			// 小于LocalMaxID为本地部件处理
	BS_SvrExtDataMsg_SendAll,				// 发送全部数据
	BS_SvrExtDataMsg_BuildZone,				// 创建场景
	BS_SvrExtDataMsg_SetZoneState,			// 设置场景状态
	BS_SvrExtDataMsg_DestroyZone,			// 消毁场景
	BS_SvrExtDataMsg_UpdateCountry,			// 更新国家
	// 最大
	BS_SvrExtDataUpdateCode_Max
};


///// 公用数据结构定义 //////////////////////////////////////////////

// 通过跨区桥服务器中转消息必须加入此消息体
struct SMsgBridgeHead
{
	DWORD dwWorldID;		// 消息来源游戏世界ID
	DWORD dwServerID;		// 消息来源服务器ID
};

// 游戏服务器信息数据结构
struct SBridgeGameServerInfo
{
	DWORD	dwID;				// 服务器ID
	DWORD	dwWorldID;			// 服务器所在游戏世界ID
	WORD	wServerType;		// 服务器类型
	WORD	wSubID;				// 第n号服务器
	DWORD	dwStateFlag;		// 服务器状态
	void*	pExtPart;			// 连接器扩展部件接口

	SBridgeGameServerInfo(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

// 共享场景信息数据结构
struct SBridgeZoneProperty
{
	DWORD dwZoneID;
	DWORD dwMapID;
	bool bEnable;
	SBridgeZoneProperty(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

// 共享国家信息数据结构
struct SBridgeCountryProperty
{
	DWORD dwID;							// 国家ID
	DWORD nLevel;						// 国家发展等级
	DWORD dwPower;						// 国家强大程度
	DWORD dwPiont;						// 国家文明点数
	DWORD dwMoney;						// 国家金钱资源
	DWORD dwWood;						// 国家木材资源
	DWORD dwFood;						// 国家粮草资源
	DWORD dwWorldAttack;				// 国家综合攻击力
	DWORD dwWorldDefend;				// 国家综合防御力
	DWORD dwLastWinTimes;				// 最近32次国战赢的次数
	DWORD dwWorldOrder;					// 全共公服排名
	DWORD dwWarOnline;					// 国战时最高参战人数
	DWORD dwKingPlayerID;				// 国王的角色ID
	DWORD dwCivilGrade;					// 世界文明等级
	DWORD dwCivilDot;					// 世界文明点数

	DWORD dwDBWorldID;					// 所属数据库游戏世界ID
	DWORD dwGroupID;					// 群组号
	DWORD dwFlag;						// 国家基础数据标识 EMCountryBaseDBFlag
	char szName[BRIDGE_NAME_MAXSIZE];	// 本区国家名称
	char szWorldName[BRIDGE_NAME_MAXSIZE];		// 跨区国家名称
	char szDBWorldName[BRIDGE_NAME_MAXSIZE];	// 所属数据库游戏世界名称

	SBridgeCountryProperty(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

///////////////////////////////////////////////////////////////////
// 事件码枚举定义: ENUM_EVENT_BRIDGE + 消息码名
enum EMEventBridge
{
	ENUM_EVENT_BRIDGE_INVALID = 0,
	ENUM_EVENT_BRIDGE_HANDSHAKE,					// 与跨区桥服务器握手成功事件
	ENUM_EVENT_BRIDGE_BUILD_PART,					// 创建扩展信息数据事件
	ENUM_EVENT_BRIDGE_SERVERLIST_ALL,				// 全部服务器加入事件
	ENUM_EVENT_BRIDGE_SERVERLIST_ADD,				// 新增服务器事件
	ENUM_EVENT_BRIDGE_SERVERLIST_DEL,				// 删除服务器事件
	ENUM_EVENT_BRIDGE_WORLDLIST_ALL,				// 全部游戏世界加入事件
	ENUM_EVENT_BRIDGE_WORLDLIST_ADD,				// 新增游戏世界事件
	ENUM_EVENT_BRIDGE_WORLDLIST_EDIT,				// 修改游戏世界事件
	ENUM_EVENT_BRIDGE_COUNTRYLIST_ALL,				// 全部游共享国家信息加入事件
	ENUM_EVENT_BRIDGE_COUNTRYLIST_UPDATE,			// 更新共享国家信息事件

	ENUM_EVENT_BRIDGE_MAXID,						// 最大事件码
};

// 最大事件码
#define EVENT_BRIDGE_MAXID						ENUM_EVENT_BRIDGE_MAXID

//////////////////////////////////////////////////////////////////////////
// 与跨区桥服务器握手成功事件
#define EVENT_BRIDGE_HANDSHAKE				ENUM_EVENT_BRIDGE_HANDSHAKE
// 事件数据结构
struct SEventBridgeHandshake
{
	DWORD	dwServerID;				// 服务器ID
	DWORD	dwPublicWorldID;		// 公共游戏世界ID

	SEventBridgeHandshake(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

//////////////////////////////////////////////////////////////////////////
// 创建扩展信息数据事件
#define EVENT_BRIDGE_BUILD_PART				ENUM_EVENT_BRIDGE_BUILD_PART
// 事件数据结构
struct SEventBridgeBuildPart
{
	DWORD	dwServerID;				// 来源服务器ID

	SEventBridgeBuildPart(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

//////////////////////////////////////////////////////////////////////////
// 全部服务器加入事件
#define EVENT_BRIDGE_SERVERLIST_ALL				ENUM_EVENT_BRIDGE_SERVERLIST_ALL
// 事件数据结构
struct SEventBridgeServerListAll
{
	DWORD	dwServerCounts;				// 收到服务器个数

	SEventBridgeServerListAll(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

//////////////////////////////////////////////////////////////////////////
// 新增服务器事件
#define EVENT_BRIDGE_SERVERLIST_ADD				ENUM_EVENT_BRIDGE_SERVERLIST_ADD
// 事件数据结构
struct SEventBridgeServerListAdd
{
	DWORD	dwID;				// 服务器ID
	DWORD	dwWorldID;			// 服务器所在游戏世界ID
	WORD	wServerType;		// 服务器类型
	WORD	wSubID;				// 第n号服务器
	DWORD	dwStateFlag;		// 服务器状态

	SEventBridgeServerListAdd(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

//////////////////////////////////////////////////////////////////////////
// 删除服务器事件
#define EVENT_BRIDGE_SERVERLIST_DEL				ENUM_EVENT_BRIDGE_SERVERLIST_DEL
// 事件数据结构
struct SEventBridgeServerListDel
{
	DWORD dwServerID;				// 服务器ID

	SEventBridgeServerListDel(void)
	{
		memset(this, 0, sizeof(*this));
	}
};


//////////////////////////////////////////////////////////////////////////
// 全部游戏世界加入事件
#define EVENT_BRIDGE_WORLDLIST_ALL				ENUM_EVENT_BRIDGE_WORLDLIST_ALL
// 事件数据结构
struct SEventBridgeWorldListAll
{
	DWORD	dwWorldCounts;				// 收到游戏世界个数

	SEventBridgeWorldListAll(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

//////////////////////////////////////////////////////////////////////////
// 新增游戏世界事件
#define EVENT_BRIDGE_WORLDLIST_ADD				ENUM_EVENT_BRIDGE_WORLDLIST_ADD
// 事件数据结构
struct SEventBridgeWorldListAdd
{
	DWORD	dwWorldID;			// 游戏世界ID

	SEventBridgeWorldListAdd(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

//////////////////////////////////////////////////////////////////////////
// 修改游戏世界事件
#define EVENT_BRIDGE_WORLDLIST_EDIT				ENUM_EVENT_BRIDGE_WORLDLIST_EDIT
// 事件数据结构
struct SEventBridgeWorldListEdit
{
	DWORD dwWorldID;				// 游戏世界ID

	SEventBridgeWorldListEdit(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

//////////////////////////////////////////////////////////////////////////
// 全部游共享国家信息加入事件
#define EVENT_BRIDGE_COUNTRYLIST_ALL				ENUM_EVENT_BRIDGE_COUNTRYLIST_ALL
// 事件数据结构
struct SEventBridgeCountryListAll
{
	DWORD dwWorldID;				// 游戏世界ID
	DWORD dwServerID;				// 服务器ID
	DWORD dwCountryID;				// 国家ID

	SEventBridgeCountryListAll(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

//////////////////////////////////////////////////////////////////////////
// 更新共享国家信息事件
#define EVENT_BRIDGE_COUNTRYLIST_UPDATE				ENUM_EVENT_BRIDGE_COUNTRYLIST_UPDATE
// 事件数据结构
struct SEventBridgeCountryListUpdate
{
	DWORD dwWorldID;				// 游戏世界ID
	DWORD dwServerID;				// 服务器ID
	DWORD dwCountryID;				// 国家ID

	SEventBridgeCountryListUpdate(void)
	{
		memset(this, 0, sizeof(*this));
	}
};
//////////////////////////////////////////////////////////////////////////
// 消息码与数据结构

/***************************************************************/
///////////////////// 跨区桥服务器模块 的消息码/////////////////
// 上层的消息码必定是 MSG_MODULEID_BRIDGE
/***************************************************************/

///////////////////////////////////////////////////////////////////
// 消息码枚举定义: ENUM + 消息码名
enum EMMsgBridge
{
	ENUM_MSG_BRIDGE_INVALID = 0,
	ENUM_MSG_BRIDGE_HANDSHAKE,						// 握手消息
	ENUM_MSG_BRIDGE_HANDSHAKE_RESPONSE,				// 回应握手消息
	ENUM_MSG_BRIDGE_KEEPALIVE,						// 心跳检查
	ENUM_MSG_BRIDGE_ANSWER_KEEPALIVE,				// 回应心跳检查
	ENUM_MSG_BRIDGE_SENDDATA,						// 请求桥服务器网关服务发送数据给各软件服务器
	ENUM_MSG_BRIDGE_BROADCASTDATA,					// 请求桥服务器网关服务广播数据给各软件服务器
	ENUM_MSG_BRIDGE_WORLDLIST_UPDATE,				// 游戏世界列表更新
	ENUM_MSG_BRIDGE_SERVERLIST_UPDATE,				// 服务器列表更新
	ENUM_MSG_BRIDGE_SERVEREXTDATE_MSG,				// 服务器扩展信息消息
	ENUM_MSG_BRIDGE_TRANSITDATA,					// 中转消息
	ENUM_MSG_BRIDGE_SERVERCHECKREQ,					// 发送服务器性能检测请求
	ENUM_MSG_BRIDGE_SERVERCHECKRES,					// 响应服务器性能检测请求

	ENUM_MSG_BRIDGE_MAXID,							// 最大消息码
};

// 最大消息码
#define MSG_BRIDGE_MAXID						ENUM_MSG_BRIDGE_MAXID



//////////////////////////////////////////////////////////////////////////
// 握手消息
#define MSG_BRIDGE_HANDSHAKE				ENUM_MSG_BRIDGE_HANDSHAKE
// 桥服务器连接器与桥服务器握手消息数据结构
struct SMsgBridgeHandshake_SB
{
	DWORD	dwWorldID;						// 服务器所在游戏世界ID
	WORD	nServerType;					// 服务器类型
	WORD	wSubID;							// 第n号服务器
	bool	bIsPublic;						// 是否是公共区的
	char	szName[BRIDGE_NAME_MAXSIZE];	// 服务器名
	char	szWorldName[BRIDGE_NAME_MAXSIZE];	// 游戏世界名

	SMsgBridgeHandshake_SB(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

//////////////////////////////////////////////////////////////////////////
// 回应握手消息
#define MSG_BRIDGE_HANDSHAKE_RESPONSE		ENUM_MSG_BRIDGE_HANDSHAKE_RESPONSE

// 桥服务器回应握手消息数据结构
struct SMsgBridgeHandshakeResponse	
{
	DWORD	dwID;				// 连接ID
	DWORD	dwServerTime;		// 服务器时间
	DWORD	dwPublicWorldID;	// 公共游戏世界ID

	SMsgBridgeHandshakeResponse(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

//////////////////////////////////////////////////////////////////////////
// 心跳检查
#define MSG_BRIDGE_KEEPALIVE				ENUM_MSG_BRIDGE_KEEPALIVE


//////////////////////////////////////////////////////////////////////////
// 回应心跳检查
#define MSG_BRIDGE_ANSWER_KEEPALIVE			ENUM_MSG_BRIDGE_ANSWER_KEEPALIVE


//////////////////////////////////////////////////////////////////////////
// 请求桥服务器网关服务发送数据给各软件服务器消息
#define MSG_BRIDGE_SENDDATA					ENUM_MSG_BRIDGE_SENDDATA

// 请求桥服务器网关服务发送数据给各软件服务器数据结构
struct SMsgBridgeSendData
{
	DWORD	dwServerID;			// 软件服务器ID
	DWORD	nDataLens;			// 数据长度
	//BYTE	pData[nDataLens];	// 数据
};


//////////////////////////////////////////////////////////////////////////
// 请求桥服务器网关服务广播数据给各软件服务器
#define MSG_BRIDGE_BROADCASTDATA					ENUM_MSG_BRIDGE_BROADCASTDATA

// 请求桥服务器网关服务广播数据给各软件服务器数据结构
struct SMsgBridgeBroadcastData
{
	DWORD	dwServerCounts;		// 软件服务器ID列表个数
	DWORD	nDataLens;			// 数据长度
	//DWORD	dwServerID[dwServerCounts];			// 软件服务器ID列表
	//BYTE	pData[nDataLens];					// 数据
};


//////////////////////////////////////////////////////////////////////////
// 游戏世界列表更新
#define MSG_BRIDGE_WORLDLIST_UPDATE					ENUM_MSG_BRIDGE_WORLDLIST_UPDATE

// 消息用 游戏世界数据结构
struct SMsgBridgeGameWorldNode
{
	DWORD	dwWorldID;			// 游戏世界ID
	BYTE	nNameStrlens;		// 游戏世界名称字串长度
	
	SMsgBridgeGameWorldNode(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

// 游戏世界列表更新数据结构
struct SMsgBridgeWorldListUpdate
{
	BYTE	nUpdateType;		// 更新类型,EMBridgeServerListUpdateType
	DWORD	dwWorldCounts;		// 游戏世界个数
	//dwWorldCounts* SMsgBridgeGameWorldNode;	// 软件服务器信息列表
	// dwWorldCounts* char szWorldName[nNameStrlens];	// 游戏世界名称列表

	SMsgBridgeWorldListUpdate(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

//////////////////////////////////////////////////////////////////////////
// 服务器列表更新
#define MSG_BRIDGE_SERVERLIST_UPDATE					ENUM_MSG_BRIDGE_SERVERLIST_UPDATE

// 消息用 游戏服务器信息数据结构
struct SMsgBridgeGameServerNode
{
	DWORD	dwID;				// 服务器ID
	DWORD	dwWorldID:16;		// 服务器所在游戏世界ID
	WORD	wServerType:8;		// 服务器类型
	WORD	wSubID:8;			// 第n号服务器
	DWORD	dwStateFlag:8;		// 服务器状态

	SMsgBridgeGameServerNode(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

// 服务器列表更新数据结构
struct SMsgBridgeServerListUpdate
{
	BYTE	nUpdateType;		// 更新类型,EMBridgeServerListUpdateType
	DWORD	dwServerCounts;		// 软件服务器个数
	//dwServerCounts* SMsgBridgeGameServerNode;	// 软件服务器信息列表

	SMsgBridgeServerListUpdate(void)
	{
		memset(this, 0, sizeof(*this));
	}
};


//////////////////////////////////////////////////////////////////////////
// 服务器扩展信息消息
#define MSG_BRIDGE_SERVEREXTDATE_MSG					ENUM_MSG_BRIDGE_SERVEREXTDATE_MSG

// 服务器扩展信息消息数据结构
struct SMsgBridgeServerExtDataMsg
{
	DWORD dwServerID;						// 来源服务器ID
	DWORD dwMsgCode;						// 子消息代码  EMBridgeServerExtDataMsgCode
	DWORD dwUserData1;						// 用户数据1
	DWORD dwUserData2;						// 用户数据2
	DWORD dwUserData3;						// 用户数据3
	// 附加数据
};

// BS_SvrExtDataMsg_SendAll,				// 发送全部数据
// 创建场景消息体
//struct SMsgBridgeZoneSendAll_B
//{
//	 DWORD dwZoneCounts;		// 场景个数 存在dwUserData1中
//	// dwZoneCounts * SMsgBridgeBuildZone_B
//};
// BS_SvrExtDataMsg_BuildZone,				// 创建场景
// 创建场景消息体
struct SMsgBridgeBuildZone_B
{
	DWORD dwZoneID;				// 场景ID
	DWORD dwMapID;				// 地图ID
	bool bEnable;				// 是否有效
};

// BS_SvrExtDataMsg_SetZoneState,			// 设置场景状态
// 设置场景状态消息体
struct SMsgBridgeSetZoneState_B
{
	DWORD dwZoneID;				// 场景ID
	bool bEnable;				// 是否有效
};
// BS_SvrExtDataMsg_DestroyZone,			// 消毁场景
// 消毁场景消息体
struct SMsgBridgeDestroyZone_B
{
	DWORD dwZoneID;				// 场景ID
};


// BS_SvrExtDataMsg_UpdateCountry,				// 更新国家信息
struct SMsgBridgeUpdateCountry_B
{
	SBridgeCountryProperty data;
};



//////////////////////////////////////////////////////////////////////////
// 中转消息
#define MSG_BRIDGE_TRANSITDATA					ENUM_MSG_BRIDGE_TRANSITDATA

// 连接器中转消息数据结构
struct SMsgBridgeTransitData
{
	DWORD dwServerID;						// 目标服务器ID
	DWORD dwDataLens;						// 中转消息数据长度
	// 附加数据
};

//////////////////////////////////////////////////////////////////////////
// 发送服务器性能检测请求
#define MSG_BRIDGE_SERVERCHECKREQ					ENUM_MSG_BRIDGE_SERVERCHECKREQ

// 发送服务器性能检测请求数据结构
struct SMsgBridgeServerCheck
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
	SMsgBridgeServerCheck()
	{
		memset(this, 0, sizeof(*this));
	}
};

//////////////////////////////////////////////////////////////////////////
// 响应服务器性能检测请求
#define MSG_BRIDGE_SERVERCHECKRES					ENUM_MSG_BRIDGE_SERVERCHECKRES

// 响应服务器性能检测请求数据结构 同 发送服务器性能检测请求数据结构

//////////////////////////////////////////////////////////////////////////

#pragma pack()


#endif	// __BRIDGEMODULE_DEF_H__