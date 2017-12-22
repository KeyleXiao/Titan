/*******************************************************************
** 文件名:	ViewDef_.h
** 版  权:	(C) 深圳冰川网络股份有限公司
** 创建人:	baoliang.shen
** 日  期:	2017/11/30
** 版  本:	1.0
** 描  述:	观战系统通用宏定义
** 应  用:  	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once
#include "buffer.h"
#include "Common.h"
#include "net.h"
#include "NetMessageDef.h"

#pragma pack(1)

#define MAC_ADDRESS_SIZE			32		// 网卡物理地址长度
#define VIEW_SECRET_KEY_SIZE		64		// 登陆验证码长度
#define VIEW_SECRET_KEY_TIME_OUT	1*60	// 登陆验证码 有效时间（单位：秒）

#define MAX_GATEWAY_COUNT		USHRT_MAX	// 最多能支持的Gate数量
#define MAX_VIEW_SERVER_COUNT	USHRT_MAX	// 最多能支持的View数量
#define MAX_GAME_SERVER_COUNT	USHRT_MAX	// 最多能支持的游戏服数量

#define VIEW_KEEPALIVE_INTERVAL			2000			// 两次KeepAlive探测间的时间间隔
#define VIEW_KEEPALIVE_MAXTIMEOUT		120000			// 最大无响应时间
#define VIEW_LISTENER_KEEPALIVE_TIMEOUT	300*1000		// 监听端口超时时间
#define	VIEW_RECONNECT_INTERVAL			(10*1000)		// 重连周期10秒


typedef	WORD	ViewID;		// ViewServerID
typedef	WORD	GateID;		// 网关ID
typedef	WORD	GameID;		// 游戏服ID

typedef	DWORD	ReplayID;	// 战报ID
typedef DWORD	PlayerID;	// 用来标识观战客户端的唯一ID（目前由请求观战的服务器发来，现在可以用玩家DBID）


//////////////////////////////////////////////////////////////////////////
/* 通用宏*/
//////////////////////////////////////////////////////////////////////////
// 检查val，如果是false，则返回false
#define D_IF_FALSE_RETURN(val)	\
if (!val)						\
{								\
	return false;				\
}


//////////////////////////////////////////////////////////////////////////
/* 通用方法*/
//////////////////////////////////////////////////////////////////////////
// ULONG的ip地址转为char*
inline char* Net_nToAddr(ULONG nIP)
{
	SOCKADDR_IN sock;
	//将字符串转换为in_addr类型  
	sock.sin_addr.S_un.S_addr = nIP;

	return inet_ntoa(sock.sin_addr);
}
//ULONG nIP = inet_addr("192.168.1.111");


#pragma pack()