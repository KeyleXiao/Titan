/*******************************************************************
** 文件名:	f:\Rocket20100601\Common\DStarTrekMsg.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	彭智锋 （9671690@qq.com）
** 日  期:	2010/6/29 17:24
** 版  本:	1.0
** 描  述:	跨区消息定义头文件
** 应  用:  	
	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once

#include "DStarTrekDef.h"

#pragma pack(1)
// tolua_begin

/*****************************************************************/
///////////////////////////////////////////////////////////////////
// 上层的消息码必定是MSG_MODULEID_STARTREK
/*****************************************************************/
///////////////////////////////////////////////////////////////////
struct SMsgStarTrekHead
{
	BYTE		nDir;
};

// 征询迁入世界是否同意
#define MSG_STARTREK_CONSULTINWORLD								1
// 消息体
struct SMsgStarTrekConsultInWorld_SS
{ 
	STID		stid;						// STID

	DWORD		PDBID;						// PDBID
	DWORD		UDBID;						// UDBID

	int			nFromGameWorldID;			// 角色原游戏世界ID
	int			nOutGameWorldID;			// 迁出世界ID
	DWORD		dwOutZoneID;				// 迁出场景ID
	int			nOutMapID;					// 迁出地图ID

	int			nInGameWorldID;				// 迁入世界ID
	DWORD		dwInBridgeServerID;			// 迁入服务器ID
	DWORD		dwInZoneID;					// 迁入场景ID
	POINT		ptInLoc;					// 迁入地图位置

	int			nStarTrekType;				// 传送类型
	DWORD		dwStarTrekFlag;				// 传送标志
	
	char		szCanInLuaFunction[ST_LUAFUNCTION_MAXLEN];
	char		szCheckResultLua[ST_LUAFUNCTION_MAXLEN];

	SMsgStarTrekConsultInWorld_SS(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

struct SMsgStarTrekConsultInWorld_reSS
{ 
	STID		stid;						// STID

	DWORD		PDBID;						// PDBID
	DWORD		UDBID;						// UDBID

	int			nFromGameWorldID;			// 角色原游戏世界ID
	int			nInGameWorldID;				// 迁入世界ID
	DWORD		dwInBridgeServerID;			// 迁入服务器ID
	DWORD		dwInZoneID;					// 迁入场景ID
	POINT		ptInLoc;					// 迁入地图位置

	int			nStarTrekType;				// 传送类型
	DWORD		dwStarTrekFlag;				// 传送标志

	char		szCheckResultLua[ST_LUAFUNCTION_MAXLEN];

	int			nConsultResult;				// 征询结果

	SMsgStarTrekConsultInWorld_reSS(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

// 将数据发往迁入游戏世界
#define MSG_STARTREK_POSTDATATOINWORLD							2
// 消息体
struct SMsgStarTrekPostDataToInWorld_SS
{ 
	SStarTrekContext	context;			// 上下文
	int					nExtendContextLen;	// 扩展上下文大小

	SMsgStarTrekPostDataToInWorld_SS(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

// 通知公共区踢人
#define MSG_STARTREK_KICKOUTINPUBLICWORLD					3
// 消息体
struct SMsgStarTrekKickoutInPublicWorld_SS
{
	DWORD				dwUDBID;
	int					nFromGameWorldID;

	SMsgStarTrekKickoutInPublicWorld_SS(void)
	{
		memset(this, 0, sizeof(*this));
	}
};
	

// tolua_end
#pragma pack()