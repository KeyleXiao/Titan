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

#ifndef __SNS_DEF_H__
#define __SNS_DEF_H__


/************************************************************************/
/* 调试信息输出宏定义                                                   */
/************************************************************************/
// 输出DEBUG信息
#ifdef VERSEION_DEBUG
#   define SNS_DEBUG_OUTPUT
#endif
#ifdef SNS_DEBUG_OUTPUT
#	define SNS_TRACELN(x)			TraceLn(x)
#	define SNS_EMPHASISLN(x)		EmphasisLn(x)
#	define SNS_ERRORLN(x)			ErrorLn(x)
#	define SNS_WARNINGLN(x)			WarningLn(x)
#else
#	define SNS_TRACELN(x)
#	define SNS_EMPHASISLN(x)
#	define SNS_ERRORLN(x)
#	define SNS_WARNINGLN(x)
#endif


#define     SERVER_NAME_MAXSIZE             32
#define     SNS_DEFAULT_LISTEN_PORT         8001		// 中控服务器默认监听端口

/***************************************************************/
// 上层的消息码必定是 MSG_MODULEID_SNS
/***************************************************************/

///////////////////////////////////////////////////////////////////
// 消息码枚举定义: ENUM + 消息码名
enum EM_MSG_SNS
{
    MSG_SNS_INVALID = 0,
    MSG_SNS_HANDSHAKE,						// 握手消息
    MSG_SNS_HANDSHAKE_RESPONSE,				// 回应握手消息
    MSG_SNS_KEEPALIVE,						// 心跳检查
    MSG_SNS_ANSWER_KEEPALIVE,				// 回应心跳检查
    

    MSG_SNS_MAXID,							// 最大消息码
};

// MSG_SNS_HANDSHAKE		            连接器与服务器握手消息数据结构
struct SMsgSNSHandshake_SR
{
    DWORD	dwWorldID;							// 服务器所在游戏世界ID
    WORD	nServerType;						// 服务器类型
    WORD	wSubID;								// 第n号服务器
    char	szName[SERVER_NAME_MAXSIZE];		// 服务器名

    SMsgSNSHandshake_SR(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

// MSG_SNS_HANDSHAKE_RESPONSE		 服务器回应握手消息数据结构
struct SMsgSNSHandshakeResponse	
{
    DWORD	dwID;								// 连接ID
    DWORD	dwServerTime;						// 服务器时间

    SMsgSNSHandshakeResponse(void)
    {
        memset(this, 0, sizeof(*this));
    }
};


#endif //__SNS_DEF_H__
