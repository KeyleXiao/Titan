/*******************************************************************
** 文件名:	E:\speed\Common\KinDef.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	10/15/2015	15：50
** 版  本:	1.0
** 描  述:	
            排行榜相关定义
********************************************************************/

#pragma once
#include "PackagePtr.h"
#include "NameLenDef.h"
#include "NetMessageDef.h"
#include "IGameDatabaseDef.h"

//输出DEBUG信息
#define KIN_DEBUG_OUTPUT
#ifdef KIN_DEBUG_OUTPUT
#	define	KIN_TRACELN(x)			TraceLn(x)
#	define	KIN_WARNINGLN(x)			WarningLn(x)
#	define	KIN_ERRORLN(x)			ErrorLn(x)
#	define	KIN_EMPHASISLN(x)			EmphasisLn(x)
#else
#	define	KIN_TRACELN(x)
#	define	KIN_WARNINGLN(x)
#	define	KIN_ERRORLN(x)
#	define	KIN_EMPHASISLN(x)
#endif

#pragma pack(1)
// 家族常量脚本
#define KIN_CONFIG_FILENAME			"Scp\\KinConfig.csv"

// 家族升级脚本
#define KIN_LEVEL_FILENAME			"Scp\\KinLevel.csv"

//============================= 基本系统 ====================================
// 一个国家最多可以容纳的家族个数
#define KIN_MAX_COUNT		2048

#define COUNTRY_MAXID       6
// 家族名字长度
//#define KIN_NAME_SIZE		17	// Delete by zhanggx 将定义转至NameLenDef.h

// 家族副族长的个数
#define KIN_ELDER_LEN		4

// 家族宣言的长度
#define KIN_MANIFESTO_LEN	256

// 家族公告的长度
#define KIN_NOTICE_LEN		256

// 家族口号长度
#define KIN_SLOGAN_LEN		256

// 家族的最大等级
#define KIN_MAX_LEVEL		6

// 家族列表一页的大小
#define KIN_PAGE_SIZE		60

// 最大的字符串属性长度
#define KIN_MAX_PROPLEN		256

// 族内等级的最大等级
#define KIN_MAX_MEMBERLEVEL 5

// 族内等级名字最大长度
#define KIN_GRADENAME_LEN	17

// 家族称号ID
#define KINNAME_TITLE_ID	220

// 家族加入帮会的时间Key
#define KIN_JOINCLAN_TIME	82

// 6级家族排名
#define KIN_LEVEL_PLACE_6	3

// 5级家族排名
#define KIN_LEVEL_PLACE_5	15

// 4级家族排名
#define KIN_LEVEL_PLACE_4	30

// 4级家族排名
#define KIN_CALCULATE_FIGHT_INVTERVAL_TIME	7*24*3600

//============================= 家族技能 ====================================
// 家族技能名字长度
//#define KINSKILL_NAME_LEN	17	// Delete by zhanggx 将定义转至NameLenDef.h

// 家族技能描述长度
#define KINSKILL_DESC_LEN	256

// 最大家族技能个数
#define KINSKILL_MAX_NUM	16

// 最大家族技能个数
#define KINSKILL_RSDATA_LEN	64

// 五行决Buff最大个数
#define KINSKILL_MAX_GS_COUNT	5

// 五行咒Buff最大个数
#define KINSKILL_MAX_DJ_COUNT	5

// 经验共享技能ID
#define KINSKILL_SHAREEXP_SKILLID	1

//============================= 家族野战 ====================================
// 家族战最多限制的日期数
#define KINWAR_MAX_DATECOUNT	5

// 家族战开始前准备阶段
#define KINWAR_DAWN_TIME		(5 * 60)

//============================= 家族事件 ====================================
// 家族事件最大个数
#define KIN_EVENT_COUNT			20

// 家族军饷事件最大个数
#define KIN_SOLDIERPAY_EVENT_COUNT			20

//============================= 家族Boss ====================================
// 一个Boss食物的最大种类数
#define KIN_BOSS_MAXFOOD		3

// 家族Boss数据长度
#define KIN_BOSSDATA_LEN		16

//============================= 家族战车 ====================================
// 可打造战车最大数量
#define KIN_TANK_MAXCOUNT		9

// 战车的种类
#define KIN_TANK_TYPECOUNT		3

// 最多材料种类
#define KIN_TANK_MAXSTUFF		3

// 家族战车范围共享的范围
#define KIN_TANK_BUFFRADIUS		16

// 家族成员每日默认可使用资金 最小0 最大 1000000
#define KIN_MEMBER_CANUSEMONEY		10000000


// 家族荣耀的名字
#define KIN_CREDIT_NAME		"家族资金"

// tolua_begin

// 家族资金由最小单位"文"改成"银"，即放大100倍
#define KIN_PROP_CREDIT_NUM		100

// int的最大整数
#define MAX_INT_VALUE						2147483647

// 家族状态枚举
enum TKinState
{
    emKinState_Invaild  = -1,					// 非法状态
    emKinState_Informal,						// 非正式的
    emKinState_Formal,							// 正式的
    emKinState_Dismissing,						// 解散中
    emKinState_Num,								// 状态数量
};


// 排序标志
enum TKinSortType
{
    emKinSort_Name = 1,							// 名字排序
    emKinSort_Level,							// 等级排序
    emKinSort_Vocation,							// 职业排序
    emKinSort_Online,							// 最后在线排序
    emKinSort_BattleValue,						// 战力值排序
    emKinSort_BattleOrder,						// 战力排名排序

};


// tolua_end
// 家族数据已读标志
enum TKinRead
{
    emKinRead_Head			= 0x0001,				// 列表数据
    emKinRead_BaseData		= 0x0002,				// 基本数据
    emKinRead_Member		= 0x0004,				// 成员数据
	emKinRead_LegendAward	= 0x0008,				// 杯赛奖励数据
	emKinRead_LegendWar		= 0x0010,				// 杯赛战绩数据(单独使用)

    emKinRead_PlusData		= 0x001E,				// 所有附加数据（成员数据， 联盟数据， 称号数据, 杯赛奖励数据）
    emKinRead_AllData		= 0x001F,				// 所有数据
};

// 解散原因
enum TKinDismissType
{
    emKinDismiss_Informal = 0,					// 临时家族解散
    emKinDismiss_Dismiss,						// 主动解散时间到
    emKinDismiss_OffLine,						// 长期没有玩家在线（已废弃）
    emKinDismiss_Activity,						// 活跃度解散
    emKinDismiss_Public,						// 公共区被动解散（不存数据库）
    emKinDismiss_Test,							// 七龙珠
    emKinDismiss_Merge,							// 家族合并
};


// 领取奖励状态
enum
{
    emKinSoldierPayState_Init = 0,		// 初始状态
    emKinSoldierPayState_NoAlloted,		// 未分配
    emKinSoldierPayState_Alloted,		// 已分配

    emKinSoldierPayState_Max,		// 状态最大值
};

// 家族杯赛战绩荣耀
enum ELegendGlory
{
	emLegendGlory_GoldCount,								// 金奖次数
	emLegendGlory_SilverCount,								// 银奖次数
	emLegendGlory_WinCount,									// 杯赛胜场
	emLegendGlory_LoseCount,								// 杯赛负场
	emLegendGlory_GodCount,									// 团队超神次数
	emLegendGlory_5KillCount,								// 团队5杀次数
	emLegendGlory_4KillCount,								// 团队4杀次数
	emLegendGlory_3KillCount,								// 团队3杀次数
	emLegendGlory_DeathCount,								// 团队死亡次数
	emLegendGlory_KillCount,								// 团队击杀次数
	emLegendGlory_AssistCount,								// 团队助攻次数

	emLegendGlory_Max,
};

// 是否接收成员数据
struct SKinInfo
{
    char szName[KIN_NAME_SIZE];                 // 家族名称
    int nKinProp_ID;							// 家族ID
    int nKinProp_State;							// 状态
    int nKinProp_Level;							// 家族等级
    int nKinProp_EstablishTime;					// 家族创立时间
    int nKinProp_DismissApplyTime;				// 发起解散的时间
    int nKinProp_ShaikhID;						// 家族族长PDBID
    int nKinProp_MemberLastLogoutTime;			// 成员的最后下线时间
    int nKinProp_ReadFlag;						// 已经从数据库读出数据的标志
    int nKinProp_Money;                         // 资金
    int nKinProp_Activity;						// 活跃度
    int nKinProp_Place;							// 排名
    int nKinProp_WorldID;						// 游戏世界ID

    struct SKinInfo()
    {
        memset(this, 0, sizeof(SKinInfo));
    }
};

// 是否接收成员数据
struct SAutoAccept
{
    bool  bAutoAccept;							// 是否自动接收成员
    short nAcceptLevel;							// 自动收人等级(2字节)
    DWORD dwAcceptFlag;							// 自动收人设置(4字节) TKinAutoFlag

    SAutoAccept()
    {
        memset(this, 0, sizeof(SAutoAccept));
    }
};

struct SKinListNode
{
    unsigned long dwID;							// 家族ID
    char szName[KIN_NAME_SIZE];					// 家族名
    short nMemberCount;							// 家族人数
    unsigned char nLevel;						// 家族等级

    SKinListNode()
    {
        memset(this, 0, sizeof(SKinListNode));
    }
};

// 客户端家族成员信息结构
struct SKinClientApplyMember
{
    DWORD dwPDBID;								// 玩家ID
    char  szName[ACTORNAME_MAXSIZE];			// 玩家名字
    BYTE  nLevel;								// 玩家等级
    int   nVocation;							// 玩家职业

    SKinClientApplyMember()
    {
        memset(this, 0, sizeof(SKinClientApplyMember));
    }
};

struct SClientKinMember
{
    DWORD   dwUDBID;                            // 账号ID
    DWORD	dwPDBID;							// 玩家ID
    char	szName[ACTORNAME_MAXSIZE];			// 玩家名字
    BYTE	nLevel;								// 玩家等级
    BYTE	nIdentity;							// 族内职位
    bool	bIsOnline;							// 玩家是否在线
    int		nLogoutTime;						// 玩家最后下线时间
    BYTE	nVocation;							// 玩家职业
    BYTE    nFaceID;                            // 头像ID
    bool	bIsPublicOnline;					// 玩家是否在公共区在线
    DWORD   dwBattleVal;						// 战力值
    DWORD   dwSrcBattleVal;						// 上一次战力值
    BYTE	dwBVOrder;							// 战力值排行
    BYTE	dwLastOrder;						// 上一次战力值排行
    DWORD   dwState;                            // 游戏状态 
    BYTE    bySex;                              // 性别

    // 以下是最大段位信息（nGrade）
    BYTE    nMatchType;
    DWORD   nRankScore;
    DWORD   nGrade;

	DWORD		dwWorldID;					    // 世界ID
	DWORD		dwSceneID;					    // 场景ID
	DWORD		serverID;					    // 服务器ID

    // 字节数 97
    SClientKinMember()
    {
        memset(this, 0, sizeof(SClientKinMember));
    }
};

// 家族成员扩展数据(不能超过32位)
struct SKinMemberData
{
    int nReserveData;							// 保留数据

    SKinMemberData()
    {
        memset(this, 0, sizeof(SKinMemberData));
    }
};

// 家族成员信息结构
struct SKinMember
{
    BYTE  bySex;								// 性别 0男 1女 2不区分
    DWORD dwUDBID;                              // 账号ID
    DWORD dwPDBID;								// 玩家角色ID
    char  szName[ACTORNAME_MAXSIZE];			// 角色名字
    DWORD dwKinID;								// 家族ID
    int	  nIdentity;							// 家族职位

	int   nDayActivity;                         // 当天活跃度
    int   nWeekActivity;                        // 周活跃度
    int   nClanCtrb;							// 帮会贡献度,本周捐献数
    int   nTotalClanCtrb;						// 累计帮会贡献度，累计捐献数
    DWORD dwLastClanCtrbTime;                   // 上次捐赠时间

    DWORD dwPopedomFlag;						// 权限标志（一个位表示一种权限，一个整数表示玩家所拥有的权限）
    bool  bIsOnline;							// 是否在线
    int	  nLogoutTime;							// 最近下线时间

    int   nVocation;                            // 职业
    int   nLevel;								// 等级
    short nFaceID;								// 玩家头像

    int   nLoginTime;							// 成员上线时间（刚加入的成员，加入时候为他的上线时间）
    DWORD dwBattleVal;							// 战力值

    DWORD dwSrcBattleVal;						// 旧战力值
    DWORD dwBVOrder;							// 战力值排名
    DWORD dwLastOrder;							// 上一次战力值排行
    DWORD dwState;                              // 玩家状态

                                                // 以下是最大段位信息（nGrade）
    BYTE    nMatchType;
    DWORD   nRankScore;
    DWORD   nGrade;

	DWORD		dwWorldID;					    // 世界ID
	DWORD		dwSceneID;					    // 场景ID
	DWORD		serverID;					    // 服务器ID

    SKinMemberData  ExtendData;					// 扩展数据

    SKinMember()
    {
        memset(this, 0, sizeof(SKinMember));
    }
};

// 家族每级的升级信息
struct SKinLevel
{
    int nMaxMemberCount;						// 最大人数

    SKinLevel()
    {
        memset(this, 0, sizeof(SKinLevel));
    }
};

// 场景服家族成员信息结构
struct SSceneKinMember
{
    DWORD   dwUDBID;                            // 玩家账号ID
    DWORD	dwPDBID;							// 玩家角色ID
    int     nLevel;                             // 玩家等级
    char	szName[17];							// 玩家角色名字
    DWORD	dwKinID;							// 家族ID
    int		nIdentity;							// 家族职位
    int		nClanCtrb;							// 帮会贡献度
    int		nTotalClanCtrb;						// 累计帮会贡献度
    DWORD	dwPopedomFlag;						// 权限标志（一个位表示一种权限，一个整数表示玩家所拥有的权限）
    SKinMemberData ExtendData;					// 扩展数据
    DWORD   dwBattleVal;						// 战力值
    DWORD   dwSrcBattleVal;						// 旧战力值
    DWORD   dwBVOrder;							// 战力值排行
    DWORD	dwLastOrder;						// 上一次战力值排行


    SSceneKinMember()
    {
        memset(this, 0, sizeof(SSceneKinMember));
    }

    bool operator < (const SSceneKinMember& member) const 
    {
        return dwPDBID < member.dwPDBID;
    }
};

// 家族事件结构
struct SKinEventHead
{
    int nCount;
};

struct SKinEvent
{
    tm tTime;									// 发生时间
    char szCoutext[256];						// 时间内容（UBB格式）
    SKinEvent()
    {
        memset(this, 0, sizeof(SKinEvent));
    }
};


// 家族成员族内等级信息
struct SKinMemberGrade
{
    char szName[KIN_GRADENAME_LEN];				// 该级别对应的称谓
    int  nGradeContribute;						// 家族内等级对应的累计贡献度
    int  nWarScore;								// 家族内等级的成员家族战被杀给予积分

    SKinMemberGrade()
    {
        memset(this, 0, sizeof(SKinMemberGrade));
    }
};

// 家族常量信息
struct SKinConfig
{
    int  nCreateLevel;							// 创建家族需要的等级
    int  nCreateMoney;							// 创建家族需要的金钱
    int  nJoinNeedLevel;						// 加入家族需要的等级
    int  nJoinKinFreeze;						// 加入家族冷却
	int nDismissWeeks;			                // 周活跃度连续{nDismissWeeks}周低于{nDismissActivity}自动解散
	int nDismissActivity;			            // 周活跃度连续{nDismissWeeks}周低于{nDismissActivity}自动解散
	int nOfflineDays;                           // 队长连续{nOfflineDays}天不上线自动进入卸任状态
	int nOutgoingDays;                          // 进入卸任状态{nOutgoingDays}天后自动丢失队长职位
    int  nRenameMoney;                          // 重命名金钱
    SKinConfig()
    {
        memset(this, 0, sizeof(SKinConfig));
    }
};

// 家族活动
struct SKinActivity
{
    short nActivityID;			// 活动ID
    // 开始时间
    // 剩余时间
};

// 玩家家族相关的数据
struct SKinUserInfo
{
    int nQuitKinTime;			// 退出家族时间
};

// 观察者结构
struct SKinObserve
{
    DWORD dwPDBID;	// 观察角色
    int nCount;		// 计数

    SKinObserve()
    {
        memset(this, 0, sizeof(SKinObserve));
    }
};

// 观察者结构
struct SKinListObserve
{
    DWORD dwPDBID;	// 观察角色
    int nCount;		// 计数

    SKinListObserve()
    {
        memset(this, 0, sizeof(SKinListObserve));
    }
};

struct SKinCupAwardNode
{
	DWORD		dwKinID;								// 家族ID
	LONGLONG	llLegengCupID;							// 杯赛ID
	BYTE		byCupRank;								// 杯赛名次
	char		szCupName[DBDEF_LEGENDCUP_NAME_LEN];	// 杯赛名称
	int			nCupConfigID;							// 杯赛配置
	DWORD		dwCupEndTime;							// 杯赛结束时间

	SKinCupAwardNode()
	{
		memset(this, 0, sizeof(SKinCupAwardNode));
	}
};

struct SKinWarRecordNode
{
	LONGLONG	llWarID;						// 战场ID
	DWORD		dwKin1ID;						// 家族1ID
	DWORD		dwKin2ID;						// 家族2ID
	DWORD		dwBeginTime;					// 战场开始时间
	WORD		dwWinKin;						// 胜利家族ID
	char		szKin1Name[DBDEF_KINNAME_MAXSIZE];		// 家族1名称
	char		szKin2Name[DBDEF_KINNAME_MAXSIZE];		// 家族2名称

	int			nKin1Glory[emLegendGlory_Max];	// 家族1杯赛荣誉buff
	int			nKin2Glory[emLegendGlory_Max];	// 家族2杯赛荣誉buff

	SKinWarRecordNode()
	{
		memset(this, 0, sizeof(SKinWarRecordNode));
	}
};

struct SKinSingleWarNode
{
	LONGLONG nWarID;							// 战场ID
	DWORD	dwBeginTime;                        // 战场开始时间
	DWORD	dwEndTime;                          // 战场结束时间
	WORD	wWarType;                           // 战场类型
	WORD	wEndBecause;                        // 结束原因 1、代表阵营1胜利 2、代表阵营2胜利 3、代表阵营1投降 4、代表阵营2投降
	WORD	wActorCount;						// 角色数量

	vector<DBREQ_PARAM_WAR_ACTOR_INFO> actorinfo;	// 参与角色信息

	SKinSingleWarNode()
	{
		nWarID = 0;
		dwBeginTime = 0;
		dwEndTime = 0;
		wWarType = 0;
		wEndBecause = 0;
		wActorCount = 0;
	}
};

// 公共结构体定义


template<typename TMsg>
bool getKinMsg(TMsg*& pMsg, void*& pData, size_t& stLen)
{
	if (nullptr == pData)
	{
		WarningLn("getMsg nullptr == pData");
		return false;
	}
	if ( stLen < sizeof(TMsg) )
	{
		WarningLn("getMsg stLen < sizeof(TMsg) stLen=" << stLen << " sizeof(TMsg)=" << sizeof(TMsg));
		return false;
	}

	pMsg	= (TMsg *)pData;
	pData	= (void*)(pMsg+1);
	stLen	-= sizeof(TMsg);

	return true;
}


static SNetMsgHead*	getKinMsgHead( PACKAGE_PTR msg, void*& pData, size_t& stLen )
{
	pData	= (void*)msg->c_str();
	stLen	= msg->size();

	SNetMsgHead* pMsg;
	if ( !getKinMsg( pMsg, pData, stLen ) )
	{
		WarningLn("getMsgHead failed! !getMsg");
		return nullptr;
	}
	return pMsg;
}

#include "KinMsgDef.h"

#pragma pack()