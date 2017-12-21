/*******************************************************************
** 文件名:	TestModuleDef.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	李界华
** 日  期:	2015/11/19
** 版  本:	1.0
** 描  述:	测试模块定义
** 应  用:

**************************** 修改记录 ******************************
** 修改人:
** 日  期:
** 描  述:
********************************************************************/

#pragma once


#ifndef __TESTMODULEDEF_H__
#define __TESTMODULEDEF_H__


#include "GameDef.h"


// 是否打开测试功能
#define OPEN_TEST

// 调试模式
//#define TEST_DEBUG

#define TESTUSER_SCHEME_FILENAME            "TestUser.csv"                  // 测试帐号配置名字
#define TESTACTION_SCHEME_FILENAME          "TestAction.csv"                // 测试动作配置名字
#define TESTBASEINFO_SCHEME_FILENAME        "TestBaseInfo.csv"              // 连网关配置名字
#define TESTHEROAI_SCHEME_FILENAME          "TestHeroAI.csv"                // 英雄AI配置名字

#pragma pack(1)

/******************************************************************/
/////////////////////测试的公共结构体定义 ////////////////////////
/******************************************************************/
// 公用定义 ( 打头: TEST )
#define TEST_NAME_LEN               32                  // 名称长度
#define TEST_PWD_LEN                32                  // 密码长度

#define TEST_SENDLIST_MAXNUM        80000               // 所有在线玩家列表数组最大数
#define TEST_USER_MAX_ID            100000              // 最大测试ID
#define TEST_USER_AI_TIMERS         2000                // AI定时器
#define TEST_PURSUE_MAXDISTANCE     32                  // 最大追击距离
#define TEST_MOVE_SEARCH_MAX        32                  // 移动搜路最大距离
#define TEST_USERDATA_MAX           5                   // 参数个数

#define TEST_SPELL_MAX_DISTANCE     5                   // 使用技能距离
#define TEST_HERO_AI_MAXCOUNT       5                   // 英雄对应AI最大数量
//////////////////////////////////////////////////////////////////////////

// 所有测试行为定义
enum EMTestActionID
{
    EmTestAction_Login = 0,             // 登陆
    EmTestAction_Select,                // 选择人物
    EmTestAction_SetAI,                 // 设定AI
    EmTestAction_Match,                 // 撮合

    EmTestAction_SingleWar,             // 单人战场
    EmTestAction_MaxID,                 // 最大数
};

// 进入游戏模式
enum EMEnterGameMode
{
    GameMode_SingleWar = 0,             // 单人战场
    GameMode_MatchWar,                  // 撮合多人战场
    GameMode_LoginOnly,                 // 只是登陆
};

// TestUser.csv
struct STestUserSchemeInfo
{
    DWORD           dwUserID;                               // 帐号ID
    char            szUserName[TEST_NAME_LEN];              // 帐号名称
    char            szUserPwd[TEST_PWD_LEN];                // 帐号密码
    DWORD           dwActorID;                              // 角色ID1
    char            szActorName[TEST_NAME_LEN];             // 角色名称1
    int             nLevel;                                 // 角色等级
    BYTE            nGameMode;                              // 进入游戏模式 EMEnterGameMode
    int             nData;                                  // 撮合对应天梯分

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
    int             nBaseInfoID;						// 基本配置ID
    char            szServerIP[TEST_NAME_LEN];			// 服务器IP
    int             nServerPort;						// 端口
    int             nMinUserIndex;
    int             nMaxUserIndex;
    int             nLoginSpeed;

    STestBaseSchemeInfo(void)
    {
        memset(this, 0, sizeof(*this));
    };
};

// 动作信息
struct STestActionInfo
{
    DWORD           dwIndex;					    // 序号
    DWORD           dwFlag;					        // 是否起用 0:不用,1：起用
    DWORD           dwRand;					        // 触发概率 1/n ,0为不触发
    DWORD           dwTimer;					    // 定时器时间
    DWORD           dwMaxTimer;				        // 最大间隔时间
    char            szName[32];		                // 名称
    DWORD           dwUserData1;				    // 参数1
    DWORD           dwUserData2;				    // 参数2
    DWORD           dwUserData3;				    // 参数3
    DWORD           dwUserData4;				    // 参数4
    DWORD           dwUserData5;				    // 参数5
};


// 英雄AI配置信息
struct STestHeroAISchemInfo
{
    int             nVocation;                      // 职业ID    
    int             arrayAI[TEST_HERO_AI_MAXCOUNT]; // 配置AI列表
    BYTE            nArrayCount;

    STestHeroAISchemInfo(void)
    {
        memset(this, 0, sizeof(*this));
    };
};

/// AI类型标志
enum EMTestAITypeFlag
{
    TestAI_Flag_None        = 0x0,      // 0    :无
    TestAI_Flag_Fightback   = 0x1,      // 1    :还击
    TestAI_Flag_Attack      = 0x2,      // 2    :主动找目标攻击
    TestAI_Flag_Pursue      = 0x4,      // 4    :追击目标
    TestAI_Flag_Enable      = 0x8,      // 8    :起用AI
    TestAI_Flag_NoUse3      = 0x10,     // 16   :未用3
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
    DWORD       dwAIType;               // AI处理标识 EMTestAITypeFlag
    DWORD       dwAISate;               // 状态,EMTestAIStateFlag
    UID         uidTarget;              // 追杀的目标UID
    DWORD       dwKillTimes;            // 攻击目标次数
    DWORD       dwSceneID;              // 要去地图场景ID
    DWORD       dwMoveType;             // 移动方式 EMTestAIMoveFlag
    DWORD       dwReachRang;            // 到达时可离目标半径
    Vector3     ptBorn;                 // 出生坐标
    Vector3     ptTarget;               // 要去的地方
    DWORD       dwSlotIndex;            // 选定技能槽索引
    int         nSpellID;               // 选定技能ID
    bool        bMoveFlag;              // 移动方向,与来去位置一起处理巡逻
    DWORD       dwLostHP;               // 少到多少百分比血时跑 n/100

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
    ENUM_Msg_Test_SetFlag,                      // 设置FLAG

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

// 设置FLAG
#define Msg_Test_SetFlag				ENUM_Msg_Test_SetFlag
struct SMsgTestSetFlag
{
    int     nGameMode;
    DWORD   dwFlag;
    int     nData1;
    int     nData2;
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
    static char *_gszGameTestActionTypeName[EmTestAction_MaxID + 1] =
    {
        "未知行为",
        "登陆",
        "选择人物",
        "巡逻移动",
        "使用技能",
        "单人战场",
    };

    if (dwActionID < EmTestAction_MaxID)
    {
        return _gszGameTestActionTypeName[dwActionID + 1];
    }
    return _gszGameTestActionTypeName[0];
};


// 程序信息
static STestActionInfo gTestActionList[EmTestAction_MaxID] = {
    // 序号,是否起用 0:不用,1：起用,触发概率 1/n ,0为不触发,定时器时间秒,最大间隔时间秒,参数1~5
    { EmTestAction_Login,				1,	1,	30,	60	,"登陆"			,0,	0,	0,	0,	0 },	// 登陆	
    { EmTestAction_Select,				1,	1,	5,	10	,"选择人物"		,0,	0,	0,	0,	0 },	// 选择人物
    { EmTestAction_SetAI,				1,	1,	10,	30	,"设置AI"		,0,	0,	0,	0,	0 },	// 移动
    { EmTestAction_SingleWar,			1,	1,	10,	30	,"单人战场"		,0,	0,	0,	0,	0 },	// 单人战场
};

#pragma pack()


#endif	// __TESTMODULEDEF_H__