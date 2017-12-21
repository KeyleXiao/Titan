/*******************************************************************
** 文件名:	GuardDef.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2015-08-14
** 版  本:	1.0
** 描  述:	数据监护通用结构定义

********************************************************************/

#pragma once

// 输出DEBUG信息
#define GUARD_PRINT_DEBUG_INF

#define SERVER_NAME_MAXSIZE				32			// 名称最大长度,32
#define GUARD_KEEPALIVE_MAXTIMEOUT		120000		// 最大无响应时间
#define GUARD_BROADCASTALL_NUM			0xFFFF		// WORD 的最大数,0xFFFF


/////// 公共枚举定义 ///////////////////////////////////////////////////////////////////
// 是否支持Int类型
//#define GUARD_SUPPORT_IMAGE_INT_UNIT

// 镜像数据命名类型
enum IMAGE_DATA_NAMESPACE
{
	IMAGE_DATA_NAMESPACE_UNKNOWN = 0,		// 镜像数据：未知
    IMAGE_DATA_NAMESPACE_ACTOR_WAR,			// 角色战斗数据
	//IMAGE_DATA_NAMESPACE_ACCOUNT,			// 帐号数据
	//IMAGE_DATA_NAMESPACE_ACTOR,				// 角色数据
	IMAGE_DATA_NAMESPACE_MAX,				// 最大镜像
};


// 服务器跨线程执行操作代码定义
enum EMGuardServerControlCode
{
	GS_ControlCode_None = 0,		// 无操作
	GS_ControlCode_StartService,	// 启动服务
	GS_ControlCode_StopService,		// 停止服务
	GS_ControlCode_ShutDown,		// 关闭程序,停止服务后要关闭进程
	// 最大
	GS_ControlCode_Max
};

/***************************************************************/
///////////////////// 数据监护服务器模块 的消息码////////////////
// 上层的消息码必定是 MSG_MODULEID_GUARD
/***************************************************************/

///////////////////////////////////////////////////////////////////
// 消息码枚举定义: ENUM + 消息码名
enum EMMsgGuard
{
	MSG_GUARD_INVALID = 0,
	MSG_GUARD_HANDSHAKE,						// 握手消息
	MSG_GUARD_HANDSHAKE_RESPONSE,				// 回应握手消息
	MSG_GUARD_KEEPALIVE,						// 心跳检查
	MSG_GUARD_ANSWER_KEEPALIVE,					// 回应心跳检查
	MSG_GUARD_SERVERCHECKREQ,					// 发送服务器性能检测请求
	MSG_GUARD_SERVERCHECKRES,					// 响应服务器性能检测请求

	MSG_GUARD_READ_DATA,						// 从服务器中读镜像数据
	MSG_GUARD_READ_DATA_RESULT,					// 从服务器中读镜像数据结果
	MSG_GUARD_WRITE_DATA,						// 写镜像数据到服务器
	MSG_GUARD_WRITE_DATA_RESULT,				// 写镜像数据到服务器结果

	MSG_GUARD_MAXID,							// 最大消息码
};

// MSG_GUARD_HANDSHAKE		数据监护服务器连接器与数据监护服务器握手消息数据结构
struct SMsgGuardHandshake_SG
{
	DWORD	dwWorldID;							// 服务器所在游戏世界ID
	WORD	nServerType;						// 服务器类型
	WORD	wSubID;								// 第n号服务器
	char	szName[SERVER_NAME_MAXSIZE];		// 服务器名
	char	szWorldName[SERVER_NAME_MAXSIZE];	// 游戏世界名

	SMsgGuardHandshake_SG(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

// MSG_GUARD_HANDSHAKE_RESPONSE		数据监护服务器回应握手消息数据结构
struct SMsgGuardHandshakeResponse	
{
	DWORD	dwID;								// 连接ID
	DWORD	dwServerTime;						// 服务器时间

	SMsgGuardHandshakeResponse(void)
	{
		memset(this, 0, sizeof(*this));
	}
};


// MSG_GUARD_SERVERCHECKREQ			发送服务器性能检测请求数据结构
struct SMsgGuardServerCheck
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

	SMsgGuardServerCheck()
	{
		memset(this, 0, sizeof(*this));
	}
};


// MSG_GUARD_READ_DATA,				从服务器中读镜像数据
struct SMsgGuardReadData
{
	int			nNameSpace;
	int			nKey;
	UINT_PTR	nBackPtr;
	SMsgGuardReadData()
	{
		memset(this, 0, sizeof(*this));
	}
};
//MSG_GUARD_READ_DATA_RESULT,		从服务器中读镜像数据结果
struct SMsgGuardReadDataResult
{
	int			nNameSpace;			// 镜像数据命名类型 IMAGE_DATA_NAMESPACE
	int			nKey;				// 键值
	UINT_PTR	nBackPtr;
	int			nDataLen;			// 数据长度
	// char* data;				// 数据
	SMsgGuardReadDataResult()
	{
		memset(this, 0, sizeof(*this));
	}
};

// MSG_GUARD_WRITE_DATA,			写镜像数据到服务器
struct SMsgGuardWriteData
{
	int			nNameSpace;
	int			nKey;
	UINT_PTR	nBackPtr;
	DWORD		dwDataLen;
	// char*  data
	SMsgGuardWriteData()
	{
		memset(this, 0, sizeof(*this));
	}
};

//MSG_GUARD_WRITE_DATA_RESULT,		写镜像数据到服务器结果
struct SMsgGuardWriteDataResult
{
	int			nNameSpace;
	int			nKey;
	UINT_PTR	nBackPtr;
	bool		bResult;

	SMsgGuardWriteDataResult()
	{
		memset(this, 0, sizeof(*this));
	}
};
