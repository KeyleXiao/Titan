/*******************************************************************
** 文件名:	ControllerDef.h 
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	李界华
** 日  期:	11/24/2016
** 版  本:	1.0
** 描  述:	中控服务器（RCMS-Root Controller Manage Server)模块定义	
** 应  用:  	
	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#ifndef __CONTROLLER_DEF_H__
#define __CONTROLLER_DEF_H__


/************************************************************************/
/* 调试信息输出宏定义                                                   */
/************************************************************************/
// 输出DEBUG信息
#ifdef VERSEION_DEBUG
#   define CONTROLLER_DEBUG_OUTPUT
#endif
#ifdef CONTROLLER_DEBUG_OUTPUT
#	define CONTROLLER_TRACELN(x)			TraceLn(x)
#	define CONTROLLER_EMPHASISLN(x)		    EmphasisLn(x)
#	define CONTROLLER_ERRORLN(x)			ErrorLn(x)
#	define CONTROLLER_WARNINGLN(x)			WarningLn(x)

#else
#	define CONTROLLER_TRACELN(x)
#	define CONTROLLER_EMPHASISLN(x)
#	define CONTROLLER_ERRORLN(x)
#	define CONTROLLER_WARNINGLN(x)
#endif

#define     SERVER_NAME_MAXSIZE             32
#define     CONTROL_DEFAULT_LISTEN_PORT     8001    // 中控服务器默认监听端口
#define     DATABASE_SVR_TYPE               1       // 定义一个数据库ID
#define     CONTROL_ROOT_DBID               1       // 服务器管理器的数据库ID


//////////////////////////////////////////////////////////////////////////
// 资源类型
enum CONTROL_RESOURCE_TYPE
{
    RESOURCE_TYPE_NONE = 0x0,                   // 无效
    RESOURCE_TYPE_DBSERVER = 0x1,               // DBServer
    RESOURCE_TYPE_DLL = 0x2,                    // DLL
    RESOURCE_TYPE_SCRIPT = 0x4,                 // 脚本

    RESOURCE_TYPE_SERVER_EXCLUE_SCRIPT      = RESOURCE_TYPE_DBSERVER | RESOURCE_TYPE_DLL,
    RESOURCE_TYPE_SERVER_EXCLUE_DBSERVER    = RESOURCE_TYPE_DLL | RESOURCE_TYPE_SCRIPT,
    RESOURCE_TYPE_SERVER_ALL    = RESOURCE_TYPE_DBSERVER|RESOURCE_TYPE_DLL|RESOURCE_TYPE_SCRIPT,  // 服务器全部资源

    RESOURCE_TYPE_MAX,                          // 最大
};

// 加载DBServer类型
enum LOAD_DATABASE_DBS_TYPE
{
    LOAD_DATABASE_DBS_TYPE_NORMAL = 0,          // 普通类
    LOAD_DATABASE_DBS_TYPE_GLOBAL = 2,          // 全局中心组（用于读取防沉迷、语音、好友等全局性配置）
};

/***************************************************************/
// 上层的消息码必定是 MSG_MODULEID_CONTROLLER
/***************************************************************/
#pragma pack(1)
// 中控所有消息的消息头
struct SControlMsgHead
{
    // 源端点码
    BYTE		SrcEndPoint;

    // 目标端点码
    BYTE		DestEndPoint;	

    // 目标模块消息码
    BYTE		KeyModule;

    // 行为消息码
    BYTE		KeyAction;
};

///////////////////////////////////////////////////////////////////
// 消息码枚举定义: ENUM + 消息码名
enum EM_MSG_CONTROLLER
{
    MSG_CONTROLLER_INVALID = 0,
    MSG_CONTROLLER_HANDSHAKE,						// 握手消息
    MSG_CONTROLLER_HANDSHAKE_RESPONSE,				// 回应握手消息
    MSG_CONTROLLER_KEEPALIVE,						// 心跳检查
    MSG_CONTROLLER_ANSWER_KEEPALIVE,				// 回应心跳检查

    MSG_CONTROLLER_REQUEST_LOCALRUN_AREALIST,       // 请求本地资源启服游戏世界列表
    MSG_CONTROLLER_REQUEST_LOCALRUN_AREALIST_RESULT,// 请求本地资源启服游戏世界列表回答
    MSG_CONTROLLER_REQUEST_RESOURCE,                // 请求资源
    MSG_CONTROLLER_REQUEST_RESULT,                  // 资源请求回答
    
    MSG_CONTROLLER_RELOAD_RESOURCE,                 // 重加载资源

    MSG_CONTROLLER_MAXID,							// 最大消息码
};

// MSG_CONTROLLER_HANDSHAKE		            连接器与服务器握手消息数据结构
struct SMsgControllerHandshake
{
    char	szName[SERVER_NAME_MAXSIZE];		// 服务器名

    SMsgControllerHandshake(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

// MSG_CONTROLLER_HANDSHAKE_RESPONSE		 服务器回应握手消息数据结构
struct SMsgControllerHandshakeResponse	
{
    DWORD	dwID;								// 连接ID
    DWORD	dwServerTime;						// 服务器时间

    SMsgControllerHandshakeResponse(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

// MSG_CONTROLLER_REQUEST_LOCALRUN_AREALIST          请求本地资源启服游戏世界列表
struct SMsgControlRequestLocalRunAreaList
{
    DWORD    dwGameID;                   // 游戏ID

    SMsgControlRequestLocalRunAreaList()
    {
        memset(this, 0, sizeof(*this));
    }
};

// MSG_CONTROLLER_REQUEST_LOCALRUN_AREALIST_RESULT   请求本地资源启服游戏世界列表回答
struct SMsgControlRequestLocalRunAreaListResult
{
    DWORD    dwGameID;                  // 游戏ID
    WORD    wCount;                     // 游戏世界ID个数
    // wCount * DWORD

    SMsgControlRequestLocalRunAreaListResult()
    {
        memset(this, 0, sizeof(*this));
    }
};


// MSG_CONTROLLER_REQUEST_RESOURCE           请求资源
struct SMsgControlRequestResource
{
    DWORD dwGameID;                     // 游戏ID
    DWORD dwWorldID;					// 游戏世界ID
    BYTE nSourceServerType;				// 服务器类型
    DWORD dwResourceType;               // 资源类型 CONTROL_RESOURCE_TYPE

    SMsgControlRequestResource()
    {
        memset(this, 0, sizeof(*this) );
    }
};

// MSG_CONTROLLER_REQUEST_RESULT               资源请求回答
struct SMsgControlRequestResourceResult
{
    BYTE nEncry;                        // 加密方式
    BYTE nResourceType;                 // 资源类型
    DWORD dwScriptGroupID;			    // 脚本文件组ID
    BYTE nMaxSchemeIndex;				// 最大配置号
    BYTE nSchemeIndex;					// 配置号（从一号脚本开始读取）
    DWORD dwDataLens;					// 本配置号数据长度
    // BYTE * pData;					// 配置数据

    SMsgControlRequestResourceResult(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

//MSG_CONTROLLER_RELOAD_RESOURCE                 重加载资源
//SMsgControlRequestResource

#pragma pack()


#endif //__CONTROLLER_DEF_H__
