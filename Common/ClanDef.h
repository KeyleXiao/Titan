/*******************************************************************
** 文件名:	ClanDef.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	4/1/2016	10:39
** 版  本:	1.0
** 描  述:	帮会结构体定义
********************************************************************/

#ifndef __CLANDEF_H__
#define __CLANDEF_H__

#pragma pack(1)

#include "Clan_ManagedDef.h"

//输出DEBUG信息
#define CLAN_DEBUG_OUTPUT
#ifdef CLAN_DEBUG_OUTPUT
#	define	CLAN_TRACELN(x)			TraceLn(x)
#	define	CLAN_WARNINGLN(x)			WarningLn(x)
#	define	CLAN_ERRORLN(x)			ErrorLn(x)
#	define	CLAN_EMPHASISLN(x)			EmphasisLn(x)
#else
#	define	CLAN_TRACELN(x)
#	define	CLAN_WARNINGLN(x)
#	define	CLAN_ERRORLN(x)
#	define	CLAN_EMPHASISLN(x)
#endif

// 一个世界最多可以容纳的帮会个数
#define CLAN_MAX_COUNT		1024
// 帮会事件的最大日志保存量
#define CLAN_EVENT_LOG_NUM	20
// 帮会的最大等级
#define CLAN_MAX_LEVEL		6
// 族内等级的最大等级
#define CLAN_MAX_MEMBERLEVEL 6
// 最大的字符串属性长度
#define CLAN_MAX_PROPLEN	256

// 帮会升级脚本
#define CLAN_LEVEL_FILENAME			"Scp\\ClanLevel.csv"
// 帮会常量脚本
#define CLAN_CONFIG_FILENAME		"Scp\\ClanConfig.csv"
// 帮会卡牌脚本
#define CLAN_CARD_FILENAME			"Scp\\ClanCard.csv"

// 帮会成员状态
enum
{
	emMemberState_None = 0,			// 无帮会
	emMemberState_Apply,			// 申请中
	emMemberState_Formal,			// 正式成员
	emMemberState_Num,				// 成员状态类型个数
};

// 帮会的权限标志
enum
{
	emClanPopedom_AppointShaikh   = 0x0001,		// 任命族长
	emClanPopedom_AppointElder    = 0x0002,		// 任命长老
	emClanPopedom_SetTitle        = 0x0004,		// 设置称号

	emClanPopedom_AcceptMember    = 0x0008,		// 招收成员
	emClanPopedom_KickMember      = 0x0010,		// 开除成员(不包括族长和长老)
	emClanPopedom_ModifyNotice	  = 0x0020,		// 修改公告
	emClanPopedom_ModifyManifesto = 0x0040,		// 修改宣言

	emClanPopedom_ShaikhPopedom	  = 0x007F,		// 族长权限
	emClanPopedom_ElderPopedom	  = 0x007C,		// 长老权限
};

// 帮会整型属性
enum TClanProp
{
	emClanProp_ID = 0,							// 帮会ID
	emClanProp_State,							// 状态
	emClanProp_Flag,							// 帮会旗帜
	emClanProp_Level,							// 帮会等级
	emClanProp_EstablishTime,					// 帮会创立时间
	emClanProp_DismissApplyTime,				// 发起解散的时间
	emClanProp_ShaikhID,						// 帮主PDBID
	emClanProp_MemberLastLogoutTime,			// 成员的最后下线时间
	emClanProp_ReadFlag,						// 已经从数据库读出数据的标志
	emClanProp_ConstructDegree,					// 建设度
	emClanProp_Gold,							// 帮会资金
	emClanProp_Activity,						// 活跃度
	emClanProp_WeekActivity,                    // 周活跃度
	emClanProp_Place,							// 排名
	emClanProp_WorldID,							// 游戏世界ID
	emClanProp_KickTime,						// 踢人的时间
	emClanProp_KickNum,							// 踢人次数
	emClanProp_MemberCount,						// 战队个数
	emClanProp_LegendCupCount,					// 可举行杯赛次数
	emClanProp_AutoAcceptFlag,					// 自动接受标识
	emClanProp_Shaikh_State,					// 盟主状态

	emClanProp_NumPropCount,					// 属性个数,以上是整型的属性

	emClanProp_Name = 100,						// 帮会名
	emClanManifesto,							// 帮会的宣言
	emClanNotice,								// 帮会的每日公告
	emClanShaikhName,							// 帮主名字
};

// 帮会数据已读标志
enum
{
	emClanRead_Head     = 0x0001,					// 列表数据
	emClanRead_Member   = 0x0002,					// 成员数据

	emClanRead_AllData  = 0x0003,					// 所有数据
};

// 排序标志
enum TClanSortType
{
	emClanSort_Name = 1,							// 名字排序
	emClanSort_Kin,									// 隶属战队
	emClanSort_Level,								// 等级排序
	emClanSort_Online,								// 最后在线排序
};

// 解散原因
enum TClanDismissType
{
	emClanDismiss_Dismiss = 0,	// 主动解散
	emClanDismiss_ZeroKin,
	emClanDismiss_Activity,		// 活跃度解散
	emClanDismiss_Public,		// 收到普通区解散帮会消息
};

struct SClanListNode
{
	unsigned long dwID;							// 帮会ID
	char szName[CLAN_NAME_SIZE];				// 帮会名
	short nMemberCount;							// 帮会人数
	unsigned char nLevel;						// 帮会等级

    SClanListNode()
    {
        memset(this, 0, sizeof(*this));
    }
};

// 帮会事件结构
struct SClanEvent
{
	int  nTime;									// 发生时间
	char szCoutext[256];						// 时间内容（UBB格式）

	SClanEvent()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 帮会申请信息结构
struct SApplyMember
{
	DWORD dwKinID;								// 战队ID
	char  szName[KIN_NAME_SIZE];				// 战队名字
	char  szShaikhName[ACTORNAME_MAXSIZE];		// 族长名字
	DWORD nMemberCount;							// 战队人数

    SApplyMember()
    {
        memset(this, 0, sizeof(*this));
    }
};

// 客户端帮会成员信息结构
struct SClientClanMember
{
	DWORD dwPDBID;								// 玩家ID
	char  szName[ACTORNAME_MAXSIZE];			// 玩家名字
	DWORD dwKinID;								// 隶属战队
	char  szKinName[KIN_NAME_SIZE];				// 隶属战队名字
	BYTE  nFaceID;								// 玩家头像
	BYTE  nLevel;								// 玩家等级
	BYTE  nIdentity;							// 族内职位
	BYTE  nIsOnline;							// 玩家是否在线 
	BYTE  nGameStatus;							// 游戏状态
	BYTE  nGrade;								// 段位
	WORD  nDonateThisWeek;						// 本周捐赠
	DWORD nDonateTotal;							// 总捐赠
	DWORD dwBattleVal;							// 战力值
	DWORD dwSrcBattleVal;						// 上一次战力值
	WORD  dwBVOrder;							// 战力值排行
	WORD  dwLastOrder;							// 上一次战力值排行
	int   nLogoutTime;							// 玩家离线时间
    BYTE  nSex;                                 // 玩家性别
	DWORD dwWorldID;					        // 世界ID
	DWORD dwSceneID;					        // 场景ID
	DWORD serverID;					            // 服务器ID

	SClientClanMember()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 客户端主角帮会数据
struct SHeroClanInfo
{
	DWORD dwPDBID;								// 角色ID
	DWORD dwClanID;								// 帮会ID
	DWORD dwKinID;								// 战队ID
	int   nClanCtrb;							// 帮会贡献度
	int   nTotalClanCtrb;						// 帮会贡献度
	int   nIdentity;							// 职位

    SHeroClanInfo()
    {
        memset(this, 0, sizeof(*this));
    }
};

// 帮会每级的升级信息
struct SClanLevel
{
	int nMaxMemberCount;						// 最大战队数
	int nMaxKinCount;							// 最大战队数
	int nNeedActivity;							// 本等级需要活跃度
	int nMinusActivity;							// 每天扣除活跃度
	
	SClanLevel()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 场景服帮会成员信息结构
struct SZoneClanMember
{
	DWORD dwClanID;								// 帮会ID
	DWORD dwKinID;								// 战队ID

	SZoneClanMember()
	{
		memset(this, 0, sizeof(*this));
	}
	
	bool operator < (const SZoneClanMember& member) const 
	{
		return dwKinID < member.dwKinID;
	}
};

// 帮会常量信息
struct SClanConfig
{
	int nCreateLevel;				// 创建帮会需要的等级
	int nCreateMoney;				// 创建帮会需要的金钱
	int nDismissingLife;			// 解散帮会的生存周期（单位秒）
    int nDismissWeeks;			    // 周活跃度连续{nDismissWeeks}周低于{nDismissActivity}自动解散
    int nDismissActivity;			// 周活跃度连续{nDismissWeeks}周低于{nDismissActivity}自动解散
	int	nKickNum;					// 每天能踢出的战队数量
	int nOfflineDays;               // 盟主连续{nOfflineDays}天不上线自动进入卸任状态
	int nOutgoingDays;              // 进入卸任状态{nOutgoingDays}天后自动丢失盟主职位
    int nClanCupMiniWeekActivity;   // 最小发布联盟杯赛周活跃度
	SClanConfig()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 帮会卡牌信息
struct SClanCardSchemeInfo
{
    int nLevel;					// 英雄等级
    int nRequestDayCountLimit;	// 每日可请求次数
    int nRequestCooldownTime;	// 请求冷却时间
    int nRequestSingleNumLimit;	// 单次请求获得最大数量
    int nRequestDayNumLimit;	// 每日请求获得最大数量
    int nDonateSingleNumLimit;	// 单次个人捐赠最大数量
    int nDonateDayNumLimit;		// 捐赠日最大数量
    int nDonatePrizeExp;		// 捐赠获得经验
    int nDonatePrizeGold;		// 捐赠获得金币
	int nRequestItemLastTime;	// 捐赠请求持续时间

	SClanCardSchemeInfo()
	{
		memset(this, 0,sizeof(*this));
	}
};

#pragma pack()

#endif // __CLANDEF_H__