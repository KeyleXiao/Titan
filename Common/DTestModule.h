/*******************************************************************
** 文件名:	e:\Rocket\Common\DTestModule.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	宋文武 (sww8@163.com)
** 日  期:	2009/4/13
** 版  本:	1.0
** 描  述:	测试模块定义	
** 应  用:  	
	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once


#ifndef __D_TESTMODULE_H__
#define __D_TESTMODULE_H__


#include "GameDef.h"


// 是否打开测试功能
#define OPEN_TEST

// 打印调试信息,并设为测试模式
//#define TEST_PRINT_DEBUG_INF

#define TESTUSER_SCHEME_FILENAME 			"TestUser.csv"					// 测试帐号配置名字
#define TESTACTION_SCHEME_FILENAME 			"TestAction.csv"				// 测试动作配置名字
#define TESTBASEINFO_SCHEME_FILENAME		"TestBaseInfo.csv"				// 连网关配置名字

#pragma pack(1)

/******************************************************************/
/////////////////////测试的公共结构体定义 ////////////////////////
/******************************************************************/
///////////////////////////////////////////////////////////////////

// tolua_begin

// 公用定义 ( 打头: TEST )

#define TEST_SENDLIST_MAXNUM		80000				// 所有在线玩家列表数组最大数
#define TEST_USER_MAX_ID			100000				// 最大测试ID
#define TEST_USER_AI_TIMERS			2000				// AI定时器
#define TEST_PURSUE_MAXDISTANCE		32					// 最大追击距离
#define TEST_MOVE_SEARCH_MAX		32					// 移动搜路最大距离
#define TEST_USERDATA_MAX			5					// 参数个数
//////////////////////////////////////////////////////////////////////////

// 所有测试行为定义
enum EMTestActionID
{
	EmTestAction_Login = 0,				// 登陆	
	EmTestAction_Select,				// 选择人物
	EmTestAction_PreAction,				// 预做动作
	EmTestAction_Move,					// 移动
	EmTestAction_Skill,					// 技能
	EmTestAction_Chat,					// 聊天
	EmTestAction_QuitGame,				// 小退
	EmTestAction_ChangeMap,				// 换地图
	EmTestAction_UseItem,				// 用物品
	EmTestAction_ChangeWeapon,			// 脱换装备
	EmTestAction_ChangeLingHun,			// 切换三魂
	EmTestAction_ChangePKMode,			// 切换PK模式
	EmTestAction_OpenFuBeng,			// 开副本
	EmTestAction_RemoveAllItem,			// 清除所有物品
	EmTestAction_Team,					// 进退队伍
	EmTestAction_UseTank,				// 使用载具
	EmTestAction_UseStock,				// 使用股票交易
	EmTestAction_AddBuff,				// 添加BUFF
	EmTestAction_MatchTeam,				// 使用组队撮合
	EmTestAction_CreateActor,			// 角色名创建

	EmTestAction_MaxID,					// 最大数
};

// tolua_end

// TestUser.csv
struct STestUserSchemeInfo
{
	DWORD			dwUserID;							// 帐号ID
	char			szUserName[32];						// 帐号名称
	char			szUserPwd[32];						// 帐号密码
	DWORD			dwActorID1;							// 角色ID1
	char			szActorName1[32];					// 角色名称1
	DWORD			dwActorID2;							// 角色ID2
	char			szActorName2[32];					// 角色名称2

	char			szEnterWorldName[32];				// 进入六国的游戏世界名称（入口点）

	STestUserSchemeInfo(void)
	{
		memset(this, 0, sizeof(*this));
	};
	void clear(void)
	{
		memset(this, 0, sizeof(*this));
	};
};

struct STestBaseSchemeInfo
{
	int				nBaseInfoID;						// 基本配置ID
	char			szServerIP[32];						// 服务器IP
	int				nServerPort;						// 端口
	int				nMinUserIndex;
	int				nMaxUserIndex;
	int				nLoginSpeed;

	STestBaseSchemeInfo(void)
	{
		memset(this, 0, sizeof(*this));
	};
};

// 动作信息
struct STestActionInfo
{
	DWORD dwIndex;					// 序号
	DWORD dwFlag;					// 是否起用 0:不用,1：起用
	DWORD dwRand;					// 触发概率 1/n ,0为不触发
	DWORD dwTimer;					// 定时器时间
	DWORD dwMaxTimer;				// 最大间隔时间
	char szName[32];				// 名称
	DWORD dwUserData1;				// 参数1
	DWORD dwUserData2;				// 参数2
	DWORD dwUserData3;				// 参数3
	DWORD dwUserData4;				// 参数4
	DWORD dwUserData5;				// 参数5
};

/// AI类型标志
enum EMTestAITypeFlag
{
	TestAI_Flag_None			= 0x0,			// 0	:无
	TestAI_Flag_Fightback		= 0x1,			// 1	:还击
	TestAI_Flag_Attack			= 0x2,			// 2	:主动找目标攻击
	TestAI_Flag_Pursue			= 0x4,			// 4	:追击目标
	TestAI_Flag_Enable			= 0x8,			// 8	:起用AI
	TestAI_Flag_NoUse3			= 0x10,			// 16	:未用3
};

/// AI处理状态
enum EMTestAIStateFlag
{
	TestAI_State_None = 0,				// 空闲
	TestAI_State_Move,					// 行走
	TestAI_State_Attack,				// 攻击追杀

	TestAI_State_MaxID,					// 最大数
};
/// AI移动类型
enum EMTestAIMoveFlag
{
	TestAI_Move_None = 0,				// 不行走
	TestAI_Move_Rand,					// 随机巡逻
	TestAI_Move_MapPoint,				// 行走地图传送点
	TestAI_Move_Pursue,					// 来回巡逻

	TestAI_Move_MaxID,					// 最大数
};


struct STestPartData
{
	DWORD	dwAIType;		// AI处理标识 EMTestAITypeFlag
	DWORD	dwAISate;		// 状态,EMTestAIStateFlag
	LONGLONG targetUid;		// 追杀的目标UID
	DWORD	dwKillTimes;	// 攻击目标次数
	DWORD	dwZoneID;		// 要去地图场景ID
	DWORD	dwMoveType;		// 移动方式 EMTestAIMoveFlag
	DWORD	dwReachRang;	// 到达时可离目标半径
	POINT	ptTarget;		// 要去的地方
	bool	bMoveFlag;		// 移动方向,与来去位置一起处理巡逻
	int		nGoodPlace;		// 换装原来的位置 -1为没有换
	DWORD	dwLostHP;		// 少到多少百分比血时跑 n/100

	STestPartData(void)
	{
		memset(this, 0, sizeof(*this));
	};
};



//////////////////////////////////////////////////////////////////////////
// 消息码与数据结构

/***************************************************************/
///////////////////// 测试系统的消息码/////////////////////
// 上层的消息码必定是 MSG_MODULEID_TEST
/***************************************************************/

///////////////////////////////////////////////////////////////////
// 消息码枚举定义: ENUM + 消息码名
enum EMMsgTestModuleList
{
	ENUM_Msg_Test_InvalID = 0,
	ENUM_Msg_Test_RunAction,					// 运行指定行为

	ENUM_Msg_Test_MaxID,						// 测试系统最大消息码
};
// 最大消息码
#define Msg_Test_MaxID						ENUM_Msg_Test_MaxID


//////////////////////////////////////////////////////////////////////////
// 测试客户端 发送给测试服务器的运行指定行为消息
#define Msg_Test_RunAction				ENUM_Msg_Test_RunAction

// 测试客户端 发送给测试服务器的运行指定行为消息数据结构
struct SMsgTestRunAction_CS	
{
	DWORD dwActionID;			// 行为代码 EMTestActionID
	DWORD dwUserData1;			// 用户数据1
	DWORD dwUserData2;			// 用户数据2
	DWORD dwUserData3;			// 用户数据3
	DWORD dwUserData4;			// 用户数据4
	DWORD dwUserData5;			// 用户数据5

	SMsgTestRunAction_CS(void)
	{
		memset(this, 0, sizeof(*this));
	};
};
// 测试服务器 发送给测试服务器的运行指定行为消息数据结构
struct SMsgTestRunAction_SO	
{
	DWORD dwPlayerID;			// 玩家角色ID
	DWORD dwActionID;			// 行为代码 EMTestActionID
	DWORD dwUserData1;			// 用户数据1
	DWORD dwUserData2;			// 用户数据2
	DWORD dwUserData3;			// 用户数据3
	DWORD dwUserData4;			// 用户数据4
	DWORD dwUserData5;			// 用户数据5

	SMsgTestRunAction_SO(void)
	{
		memset(this, 0, sizeof(*this));
	};
};

//////////////////////////////////////////////////////////////////////////

// 取得用户类型名称
static char * gFunGetTestActionTypeString(DWORD dwActionID)
{
	//用户类型名称
	static char *_gszGameTestActionTypeName[EmTestAction_MaxID+1] =
	{
		"未知行为",
		"登陆",
		"选择人物",
		"预做动作",
		"巡逻移动",
		"使用技能",
		"聊天",
		"小退",
		"换地图",
		"使用物品",
		"脱换装备",
		"切换三魂",
		"切换PK模式",
		"开副本",
		"清除所有物品",
		"进退队伍",
		"使用载具",
		"使用股票交易",
		"添加BUFF",
		"使用组队撮合",
		"角色名创建",
	};

	if (dwActionID<EmTestAction_MaxID)
	{
		return _gszGameTestActionTypeName[dwActionID+1];
	}
	return _gszGameTestActionTypeName[0];
};




#pragma pack()


#endif	// __D_TESTMODULE_H__