/*******************************************************************
** 文件名:	TaskStorageDef.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2016-5-31
** 版  本:	1.0
** 描  述:	任务数据定义

			
********************************************************************/

#ifndef __COMMON_TASKSTORAGEDEF_H__
#define __COMMON_TASKSTORAGEDEF_H__

#include <vector>

#define TASKDATA_DEFAULT_VALUE		0x7fffffff		// 任务数据默认值

// 输出DEBUG信息
#ifdef VERSEION_DEBUG
#define TASK_DATA_DEBUG
#endif
#ifdef TASK_DATA_DEBUG
#	define TASK_TRACELN(x)			TraceLn(x)
#	define TASK_EMPHASISLN(x)		EmphasisLn(x)
#	define TASK_ERRORLN(x)			ErrorLn(x)
#	define TASK_WARNINGLN(x)		WarningLn(x)
#else
#	define TASK_TRACELN(x)
#	define TASK_EMPHASISLN(x)
#	define TASK_ERRORLN(x)
#	define TASK_WARNINGLN(x)
#endif

/////////////////////// 集体任务数据类型 //////////////////////
enum DATA_SET_TYPE
{
    DATASET_UNKNOWN,                                // 未定义
    DATASET_GLOBAL = 1,							    // 全局
    DATASET_CLAN,									// 帮会
    DATASET_KIN,									// 战队
    DATASET_MAX,									// 最大
};

/////////////////////// 个人任务数据类型 //////////////////////
// 个人的任务数据键值
enum ACTOR_TASK_KEY
{
	// 任务系统使用 1-9999 占位先

    // 登陆时间
	TASKKEY_ACTOR_LAST_LOGIN_TIME = 10000,			// 上次登录时间

	// 帮会捐卡，ID10001-10020
	TASKKEY_ACTOR_CLANREQUESTCARDCOUNT = 10001,		// 当天请求捐卡次数
	TASKKEY_ACTOR_CLANGETCARDCOUNT,					// 当天被捐卡次数
	TASKKEY_ACTOR_CLANDONATECARDCOUNT,				// 当天捐卡次数
	TASKKEY_ACTOR_CLANLASTREQUESTCARDTIME,			// 上次请求捐卡时间
    TASKKEY_ACTOR_CLANLASTGETCARDTIME,				// 当天被捐卡时间
    TASKKEY_ACTOR_CLANLASTDONATECARDTIME,			// 当天捐卡时间

    // 玩家首胜时间， ID10021 - 10040
	TASKKEY_ACTOR_WAR_FIRSTWINTIME          = 10021,    // 首胜时间

	// 商城限量物品, ID10081-10120
	TASKKEY_ACTOR_POINTSHOP_BUYNUM_START    = 10081,	// 商品已购买数量起始ID(每种一个目前最多10种)
	TASKKEY_ACTOR_POINTSHOP_BUYNUM_END      = 10090,	// 商品已购买数量结束ID

	// 商城卡牌列表, ID10121-10140
	TASKKEY_ACTOR_POINTSHOP_CARDLIST_START              = 10121,	// 商品卡牌列表起始ID(每种一个目前最多10种)
	TASKKEY_ACTOR_POINTSHOP_CARDLIST_END                = 10130,	// 商品卡牌列表结束ID
	TASKKEY_ACTOR_POINTSHOP_CARDLIST_LAST_REFLUSH_TIME  = 10131,	// 商品卡牌列表上次刷新时间
	TASKKEY_ACTOR_POINTSHOP_CARDLIST_REFLUSH_NUM        = 10132,	// 商品卡牌列表当天剩余手动刷新次数

	// 商城刷新过的英雄记录, ID10141-10160
	TASKKEY_ACTOR_POINTSHOP_CARDHERORECORD_CLEARTIME    = 10141,	// 商品卡牌已刷英雄清空时间
	TASKKEY_ACTOR_POINTSHOP_CARDHERORECORD_START        = 10142,	// 商品卡牌已刷英雄起始ID(每个32位目前最多8个)
	TASKKEY_ACTOR_POINTSHOP_CARDHERORECORD_END          = 10149,	// 商品卡牌已刷英雄结束ID

	// 新手引导类型
	TASKKEY_ACTOR_NOVICE_GUIDE                          = 10161,    // 新手引导类型

	TASKKEY_ACTOR_WAR_USEWRING                          = 10162,	// 战场翅膀使用标识
	TASKKEY_ACTOR_WAR_USEXP	                            = 10163,	// 战场使用xp标识
	TASKKEY_ACTOR_ACTIVITY_DAY                          = 10164,	// 玩家每天的活跃度日期
    TASKKEY_ACTOR_ACTIVITY_VALUE                        = 10165,	// 玩家每天的活跃度数值
	TASKKEY_ACTOR_LIFEHERO_FINISHED                     = 10166,	// 玩家本命英雄流程是否走完
	TASKKEY_ACTOR_LIFEHERO_PRIZE                        = 10167,    // 玩家本命英雄奖励领取到哪一步(LifeHeroPrize.csv定义的奖励列表，初始值零，领一个加一)
    TASKKEY_ACTOR_ACTIVITY_WEEK                         = 10168,	// 玩家周活跃度日期
    TASKKEY_ACTOR_ACTIVITY_WEEK_VALUE                   = 10169,	// 玩家周活跃度数值

	// 奖励宝箱，ID10201-10300
	TASKKEY_ACTOR_PRIZECHEST_NUM_START					= 10201,	// 拥有宝箱数量起始ID(每种一个目前最多100种)
	TASKKEY_ACTOR_PRIZECHEST_NUM_END					= 10300,	// 拥有宝箱数量结束ID
	// 预留位置到11000,后续请从11001开始枚举

};

// 任务键值类型
struct STaskKeyType
{
public:
	vector<int> nActorSaveDBIntTaskKey;		// 存DB的键值列表
	vector<int> nActorTempIntTaskKey;		// 不存DB的键值列表

	void Init()
	{
        //////////////////////////////////// 需要存DB的键值在这里加上 ////////////////////////////////////
		nActorSaveDBIntTaskKey.push_back(TASKKEY_ACTOR_LAST_LOGIN_TIME);
		nActorSaveDBIntTaskKey.push_back(TASKKEY_ACTOR_CLANREQUESTCARDCOUNT);
		nActorSaveDBIntTaskKey.push_back(TASKKEY_ACTOR_CLANGETCARDCOUNT);
		nActorSaveDBIntTaskKey.push_back(TASKKEY_ACTOR_CLANDONATECARDCOUNT);
		nActorSaveDBIntTaskKey.push_back(TASKKEY_ACTOR_CLANLASTREQUESTCARDTIME);
        nActorSaveDBIntTaskKey.push_back(TASKKEY_ACTOR_CLANLASTGETCARDTIME);
        nActorSaveDBIntTaskKey.push_back(TASKKEY_ACTOR_CLANLASTDONATECARDTIME);
		nActorSaveDBIntTaskKey.push_back(TASKKEY_ACTOR_WAR_FIRSTWINTIME);
		nActorSaveDBIntTaskKey.push_back(TASKKEY_ACTOR_NOVICE_GUIDE);
		nActorSaveDBIntTaskKey.push_back(TASKKEY_ACTOR_WAR_USEWRING);
		nActorSaveDBIntTaskKey.push_back(TASKKEY_ACTOR_WAR_USEXP);
		nActorSaveDBIntTaskKey.push_back(TASKKEY_ACTOR_ACTIVITY_DAY);
		nActorSaveDBIntTaskKey.push_back(TASKKEY_ACTOR_ACTIVITY_VALUE);
		nActorSaveDBIntTaskKey.push_back(TASKKEY_ACTOR_LIFEHERO_FINISHED);
		nActorSaveDBIntTaskKey.push_back(TASKKEY_ACTOR_LIFEHERO_PRIZE);
        nActorSaveDBIntTaskKey.push_back(TASKKEY_ACTOR_ACTIVITY_WEEK);
        nActorSaveDBIntTaskKey.push_back(TASKKEY_ACTOR_ACTIVITY_WEEK_VALUE);

		for (int i = TASKKEY_ACTOR_PRIZECHEST_NUM_START; i <= TASKKEY_ACTOR_PRIZECHEST_NUM_END; ++i)
		{
			nActorSaveDBIntTaskKey.push_back(i);
		}
		for (int i = TASKKEY_ACTOR_POINTSHOP_BUYNUM_START; i <= TASKKEY_ACTOR_POINTSHOP_BUYNUM_END; ++i)
		{
			nActorSaveDBIntTaskKey.push_back(i);
		}
		for (int i = TASKKEY_ACTOR_POINTSHOP_CARDLIST_START; i <= TASKKEY_ACTOR_POINTSHOP_CARDLIST_END; ++i)
		{
			nActorSaveDBIntTaskKey.push_back(i);
		}
		nActorSaveDBIntTaskKey.push_back(TASKKEY_ACTOR_POINTSHOP_CARDLIST_LAST_REFLUSH_TIME);
		nActorSaveDBIntTaskKey.push_back(TASKKEY_ACTOR_POINTSHOP_CARDLIST_REFLUSH_NUM);
		nActorSaveDBIntTaskKey.push_back(TASKKEY_ACTOR_POINTSHOP_CARDHERORECORD_CLEARTIME);
		for (int i = TASKKEY_ACTOR_POINTSHOP_CARDHERORECORD_START; i <= TASKKEY_ACTOR_POINTSHOP_CARDHERORECORD_END; ++i)
		{
			nActorSaveDBIntTaskKey.push_back(i);
		}

        //////////////////////////////////// 不需要存DB的键值在这里加上 ////////////////////////////////////
		//nActorTempIntTaskKey.push_back();
	}
};
//////////////////////////////////////////////////////////////////////////
#pragma pack(1)

/***************************************************************/
///////////////////// 场景服的行为层的消息码/////////////////////
// 上层的消息码必定是MSG_MODULEID_TASKSTORAGE
/***************************************************************/
// 刷新所有数据前的清除
#define MSG_TASKS_REFRESH_CLEAR									1

// 刷新一个对象的的所有数据
#define MSG_TASKS_REFRESH										2
struct SMsgTaskStRefresh_OS
{
    BYTE nType;				// 类型
    DWORD dwHoder;			// 拥有者
    short nSaveByteLen;		// BYTE的长度
    short nSaveShortLen;	// short的长度
    short nSaveIntLen;		// int的长度

    short nByteLen;			// BYTE的长度
    short nShortLen;		// short的长度
    short nIntLen;			// int的长度

    // ...nByteLen * 3
    // ...nShortLen * 4
    // ...nIntLen * 6
    SMsgTaskStRefresh_OS()
    {
        memset(this, 0, sizeof(*this));
    }
};

// 刷新一种类型的所有数据
#define MSG_TASKS_setData										3
struct SMsgTaskStsetData_OS
{
    BYTE  nType;			// 类型
    DWORD dwHolder;			// 拥有者
    BYTE  nKeyType;			// Key类型
    short nKey;				// KEY
    int	  nValue;			// 值

    SMsgTaskStsetData_OS()
    {
        memset(this, 0, sizeof(*this));
    }
};

// 删除一种类型的数据
#define MSG_TASKS_REMOVEDATA									4
struct SMsgTaskStRemoveData_OS
{
    BYTE  nType;			// 类型
    DWORD dwHolder;			// 拥有者
    BYTE  nKeyType;			// Key类型
    short nKey;				// KEY

    SMsgTaskStRemoveData_OS()
    {
        memset(this, 0, sizeof(*this));
    }
};

// 更新用户点券数据
#define MSG_UPDATE_USERTICKET									5
struct SMsgUpdateUserTicket_OS
{
    DWORD   dwClientID;         // 角色ClientID
    DWORD   dwPDBID;            // 角色ID
    DWORD	dwUserID;			// 帐号ID
    int		nTicket;			// 提取点券
    int		nTotalTicket;		// 当前游戏库总点券

    SMsgUpdateUserTicket_OS()
    {
        memset(this, 0, sizeof(*this));
    }
};

#pragma pack()

#endif  // __COMMON_TASKSTORAGEDEF_H__
