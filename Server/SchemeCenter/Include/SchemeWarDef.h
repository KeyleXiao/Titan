/*******************************************************************
** 文件名: E:\speed\Server\SchemeCenter\Include\SchemeWarDef.h
** 版  权: (C) 深圳冰川网络技术有限公司(www.weimingtech.com)
** 创建人: 陈俊涛
** 日  期: 2015/3/13
** 版  本: 1.0
** 描  述: 战场解析文件数据定义
** 应  用: 
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#pragma once
#include "WarDef.h"
#include "CampDef.h"
#include "Vector3.h"
#include <string>

// SchemeWarManager
#define WARMANAGER_SCHEME_FILENAME "Scp\\WarManager.csv"

#define  MAX_WAR_BROADCAST_PROCESS 	7

struct SBroadcastProcess
{
	int BroadCastTime;
	int BroadCastID;
};

struct SWarScheme
{
	// 战场ID
	int nStaticID;
	// 战场名称
	char szName[GAME_NAME_MAXSIZE];			// 预设体路径

	// 游戏类型
	int nGameType;
	// 玩法类型
	int nPlayType;

	// 流程广播数量
	int nBroadcastProcessCount;

	// 战场流程广播时间列表
	SBroadcastProcess BroadCastTimeList[MAX_WAR_BROADCAST_PROCESS];

	// 怪物最大数量
	int MaxMonsterCount;

	SWarScheme()
	{
		memset(this,0,sizeof(*this));
	};
};


// SchemeWarInfo
#define WARINFO_SCHEME_FILENAME "Scp\\WarInfo.csv"

// 建筑死亡复活事件
enum EWarInfoEvent
{
	EWIE_None = 0,
	EWIE_ProtectTower,		// 保护塔事件, 保护的塔显露出来
	EWIE_MonsterRefresh,	// 设置开启刷新
	EWIE_ProtectBase,		// 保护基地
	EWIE_MutexProtectBase,	// 保护基地互斥事件
	EWIE_CreateCampMonster,// 创建阵营怪物
	EWIE_UltimateSoldiers,	// 终极兵刷新
    EWIE_Max,	            // 最大
};

#define  MAX_WAR_EVENT_DEFINE 7		// 影响事件最大定义

#define  MAX_WAR_SYN_DEFINE   3		// 同步刷新时间最多行数量 会影响刷怪效率（谨慎添加）

#define  MAX_WAR_BROADCAST_TYPE_COUNT 3	// 广播类型最大数量

#define  MAX_WAR_POINT_POS_RADOM  4	// 随机坐标最大值

// 塔类型
enum EWarTowerType
{
	WETT_None = 0,      // 不存在的处理方式
	WETT_Tower,     // 正常塔类型
	WETT_Crystal,       // 水晶类型

	WETT_TYPEMAX,
};

// 经验增加类型类型
enum EWarAddEXPType
{
	EWEXP_None = 0,		// 不存在的处理方式
	EWEXP_Alone,		// 单独
	EWEXP_Around,		// 周围
	EWEXP_CampAll,		// 己方全部

	EWEXP_MAX,
};

// 掉落天赋类型
enum EWarTalentType
{
	EWTT_None = 0,		// 不存在的处理方式
	EWTT_Alone,			// 单独
	EWTT_Around,		// 周围
	EWTT_CampAll,		// 己方全部
	EWTT_MurderAdd,		// 补兵添加
	EWTT_MurderAddEX,	// 补刀掉落扩展

	WETT_MAX
};

// 掉落天赋类型
enum EWarBuffType
{
	EWBT_None = 0,		// 不存在的处理方式
	EWBT_Alone,			// 单独
	EWBT_Around,		// 周围
	EWBT_CampAll,		// 己方全部

	EWBT_MAX,
};

// 掉落天赋类型
enum EWarBroadCastType
{
	EWBCT_None		= 0x0,		// 不存在的处理方式
	EWBCT_DieIcon	= 0x01,		// 死亡图标广播
	EWBCT_Relive	= 0x02,		// 重生广播
	EWBCT_DieText	= 0x04,		// 死亡文字广播
	EWBCT_OnTime	= 0x08,		// 计算定时广播
	EWBCT_Both		= 0x10,		// 出生广播
	EWBCT_KillSelf	= 0x20,		// 自杀广播

	EWBCT_All		= 0x0F,		// 以上全部
};

//击杀英雄获得经验方式
enum EWarGetPersonDieEXPType
{
	EWGP_Kill,
	EWGP_Assist,
	EWGP_Around,
};


// 小地图信息事件
enum EMiniMapInfoType
{
	EMMIT_None = 0,
	EMMIT_Protect,			// 保护
	EMMIT_Attack,			// 攻击
};

//#define  MAX_WAR_EVENT_DEFINE 5

struct SWarEventDefine
{
	EWarInfoEvent eWarEvent;
	int nContIndex;
	int nParm;

	SWarEventDefine()
	{
		memset(this, 0, sizeof(*this));
	};
};

struct SWarBroadCastTypeInfo
{
	EWarBroadCastType eBroadCastType;
	int nIndex;

	SWarBroadCastTypeInfo()
	{
		memset(this, 0, sizeof(*this));
	};
};

struct SWarBroadCastIDInfo
{
	int nBroadCastID;
	int nIndex;

	SWarBroadCastIDInfo()
	{
		memset(this, 0, sizeof(*this));
	};
};

struct SWarBroadCastParameterInfo
{
	int nBroadCastParameter;
	int nIndex;

	SWarBroadCastParameterInfo()
	{
		memset(this, 0, sizeof(*this));
	};
};

#define MAX_REFRESH_INDEX_COUNT 6
#define MAX_REFRESH_INTERVAL_COUNT 3

// 解析文件数据
struct SWarInfoScheme
{
	// 序列号
	int nIndex;
	// 名称
	char szName[GAME_NAME_MAXSIZE];	
	// 是否直接刷出来
	bool bIsUse;
	// 阵营
	int nCamp;
	// 事件类型
	EWarMonsterExec eMonsterType;
	// 塔类型
	EWarTowerType eTowerType;
	// 是否共享图标
	bool bIsShare;

	// 图标类型
	int nIconIndex;

	// 战场ID
	int nWarID;

    // 怪物刷新列表数量 记录配置的数量
    EWarMonsterRefreshType eMonsterRefreshType;

	// 怪物刷新列表
    int nMonsterIndexCount;
	int nMonsterIndexList[MAX_REFRESH_INDEX_COUNT];

    int nIntervalCount;
    int nInterval[MAX_REFRESH_INTERVAL_COUNT];                          //时段

    int nRatioCount;
    int nRatioList[MAX_REFRESH_INDEX_COUNT * (MAX_REFRESH_INTERVAL_COUNT + 1)]; 

	// 起始时间
	int nStartTime;
	// 刷新时间
	int nRefreshTime;

	// 同步到哪一行刷新时间
	int SynRefreshTimeIndexList[MAX_WAR_SYN_DEFINE];

	// 附加字段（不用配置的列） 用于加快效率判定
	bool bIsNeedRefreshTime;

	//基础等级	
	int nBaseLevel;

	//升级时间
	int nUpgradeTime;

	//最大等级
	int nMaxLevel;

	// 怪物刷新显示的序列
	int nTimeIndex;

	// 怪物对应战绩表位置
	int nTableIndex;

	// 兵线路径ID
	int nPathID;

	// 出身影响事件定义
	bool bHaveBornEvent;
	SWarEventDefine WarBornEventDefine[MAX_WAR_EVENT_DEFINE];

	// 死亡影响事件定义
	bool bHaveDeadEvent;
	SWarEventDefine WarDeadEventDefine[MAX_WAR_EVENT_DEFINE];


	// 兵线路径类型
	EWarSoliderPathType ePathType;

	SWarInfoScheme()
	{
		memset(this, 0, sizeof(*this));
	};
};




// SchemeWarPoint
#define WARPOINT_SCHEME_FILENAME "Scp\\WarPoint.csv"
struct SWarPointScheme
{
	// 序列号
	int nIndex;
	// 怪物ID
	int nMonsterID;

	// 朝向
	float fAX;
	float fAY;
	float fAZ;

	// 坐标
    int nPosCount;
	float fX[MAX_WAR_POINT_POS_RADOM];
	float fY[MAX_WAR_POINT_POS_RADOM];
	float fZ[MAX_WAR_POINT_POS_RADOM];

	// 附加字段（不用配置的列） 用于加快效率判定
	bool bNeedBroadCast;

	// 广播类型列表 EWarBroadCastType
	SWarBroadCastTypeInfo BroadCastTypeList[MAX_WAR_BROADCAST_TYPE_COUNT];
	// 广播id列表
	SWarBroadCastIDInfo BroadCastIDList[MAX_WAR_BROADCAST_TYPE_COUNT];
	// 广播参数列表
	SWarBroadCastParameterInfo BroadCastParameterList[MAX_WAR_BROADCAST_TYPE_COUNT];

	// 位置索引
	int nLocalIndex;

	// 死亡增加经验方式 EWarAddEXPType
	EWarAddEXPType nAddEXPType;
	// 死亡的经验
	int nExp;

	// 掉落天赋类型 EWarTalentType
	EWarTalentType nTalentType;
	// 天赋值
	int nTalentValue;

	//补刀掉落类型衰减系数
	float fDecayFactor;

	// 掉落天赋类型 EWarBuffType
	EWarBuffType nBuffType;
	// buff值
	int nBuffValue;
	// buff等级
	int nBuffLevel;

	// 经验范围
	int nDistanceValue;
	// 范围
	int nDistanceTalent;

	// 怪物累积次数多少次数刷新
	int nRefreshCumulant;

	// 同步到哪个索引累积刷新次数
	int nSynRefreshCumulantIndex;

	// 开启小地图信息防守进攻标志
	int nMiniMapAttackProtectFlag;

	// 隐形广播怪标志位
	bool bIsInvisibleMonster;

	// 气势掉落路径类型
	EWarSoliderPathType eVigourPathType;

	// 气势掉落增加值
	int nVigourAddValue;

	SWarPointScheme()
	{
		memset(this, 0, sizeof(*this));
	};
};

// 怪物个数
typedef vector<SWarPointScheme> WarPointVect;

#define WAR_SCHEME_EVALUATH     "Scp\\WarEvaluate.csv"
struct SWarEvaluathInfo
{
	// 为了实现变量名字简洁 现如下定义 T = Threshold 阀值| R = Ratio 系数| WR = WeightRatio 加权系数
	int         nMatchTypeID;            // 比赛类型ID

	int			nComprehensiveScoreEnlargeR; // 总分放大系数
	float       fKillDestroyWR;			 // 击杀破塔得分加权系数
	float       fTeamContributionWR;     // 团队贡献加权系数
	float       fSpecialContributionWR;  // 特殊贡献加权系数

	float       fKDAWR;                  // KDA加权系数

	float       fTimeMultiKillR;         // 多杀系数
	int			nTimeMultiKillT;		 // 多杀起始阀值
	float		fTimeMultiKillTScore;	 // 多杀起始阀值得分
	float		fTimeMultiKillWR;		 // 多杀系数加权

	float       fAliveMultiKillR;        // 不死连杀系数
	int			nAliveMultiKillT;		 // 不死连杀起始阀值
	float		fAliveMultiKillTScore;	 // 不死连杀起始阀值得分
	float		fAliveMultiKillWR ;	     // 不死连杀系数加权

	float       fDestoryTowerR;          // 破塔系数
	float       fDestoryTowerWR;         // 破塔加权系数

	float		fKillDragenR;			 // 击杀龙系数
	float		fKillDragenWR;			 // 击杀龙加权系数

	float		fKillScoreFunctionParam1;// 击杀破塔分数函数参数1 缩放
	float		fKillScoreFunctionParam2;// 击杀破塔分数函数参数1 平移

	float		fOutputDamageWithMoneyBaseWR;	//输出金钱比加权基本系数
	float		fOutputDamageWithMoneyWinLoseWR;//输出金钱比加权胜负系数
	float		fKDAR;							// KDA系数

	float		fThrillSaveHPT;		    //惊险救治生命阀值
	float		fThrillSaveTimeT;		//惊险救治时间阀值
	float		fThrillSaveTimeCool;	//惊险救治冷却时间
	float		fThrillSaveRange;		//惊险救治范围
	float		fThrillSaveWR;			//惊险救治生命阀值

	float		fDeadlyControlTimeT;	//致命控制时间阀值
	float		fDeadlyControlTimeCool; //致命控制时间冷却
	float		fDeadlyControlWR;		//致命控制加权系数

	int			nTeamBattleControlT;		//团战控制人数阀值
	float		fTeamBattleControlR;		//团战控制系数
	float		fTeamBattleControlTScore;	//团战控制阀值起始值
	float		fTeamBattleControlWR;		//团战控制加权系数

	float		fBearDamageWR;				// 承受伤害加权系数	
	float		fCureWR;				    // 治疗量加权系数

	// 荣誉称号
	float		fCureHonorT;				// 治疗量荣誉阀值
	int			nThrillSaveHonorT;          // 惊险救治荣誉阀值
	int			nDeadlyControlHonorT;	    // 夺命控制荣誉阀值
	float		fTeamBattleControlHonorT;   // 团战控制荣誉阀值
	int			nKillDragonHonorT;			// 杀龙荣誉阀值

	// 上下限
	float		fKillDestroyMin;			// 杀人破塔下限值
	float		fKillDestroyMax;			// 杀人破塔上限值
	float       fTeamContributionMin;		// 团队贡献下限值
	float       fTeamContributionMax;		// 团队贡献上限值
	float		fSpecialContributionMin;	// 特殊贡献下限值
	float		fSpecialContributionMax;	// 特殊贡献上限值

	SWarEvaluathInfo()
	{

		memset(this, 0, sizeof(*this) );
	}
};


#define WAR_SCHEME_FORCE_END	"Scp\\WarForceEnd.csv"
struct SWarForceEndInfo
{
	int nMatchType;			// 比赛类型（房间匹配类型）
	bool bEnable;			// 是否开启强制结束类型
	DWORD dwTriggerTime;	// 触发时间
	int nCondition;			// 触发条件
	int nKillDiffWeight;	// 人头差权重
	int nEcmDiffWeight;		// 经济差权重
	int nTowerDiffWeight;	// 推塔差权重

	SWarForceEndInfo()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 子功能类型
enum WarSubFuncType
{
	WarSubFuncType_XPSkill = 0,			// XP技能
	WarSubFuncType_Support,				// 点赞
	WarSubFuncType_SaveOftenUseEquip,	// 保存战场常用装备
	WarSubFuncType_AIEcmCtrl,			// AI经济控制
	WarSubFuncType_AIExpCtrl,			// AI经验控制

	WarSubFuncType_Max,					// 最大值
};
#define WAR_SCHEME_SUB_FUNC "Scp\\WarSubFunc.csv"
struct SWarSubFunc
{
	int nMatchType;							// 比赛类型（房间匹配类型）
	bool bEnableList[WarSubFuncType_Max];	// 子功能开关列表

	SWarSubFunc()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 战场常用装备
#define WAR_SCHEME_COMMON_EQUIP "SCP\\WarOftenUseEquip.csv"
struct SWarOftenUseEquip
{
	int nBaseWeight;		// 基础权重
	int nDampRatio;			// 衰减比例

	SWarOftenUseEquip()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 大神推荐装备
#define WAR_SCHEME_GOD_RECOMMD_EQUIP "SCP\\WarGodRecommdEquip.csv"
struct SWarGodRecommdEquip
{
	char szLeagueName[GAME_NAME_MAXSIZE];		// 联赛名称
	int LeagueType;								// 联赛类型
	char szTitle[GAME_NAME_MAXSIZE];			// 称号
	int nHeroID;								// 英雄ID
	char szSchemeName[GAME_NAME_MAXSIZE];		// 方案名字
	char szAuthorName[ACTORNAME_MAXSIZE];		// 作者名字
	int nEquipList[WARQUIPSCHEME_EQUIP_MAXSIZE];		// 装备ID组

	SWarGodRecommdEquip()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 战场添加buf
#define WAR_SCHEME_ADD_BUFF "SCP\\WarAddBuff.csv"
struct SWarAddBuff
{
	int nMatchType;		// 比赛类型
	int nBuffID;		// buff ID
	int nBuffLv;		// buf等级
	int nLimitLv;		// 限制等级
	int nTime;			// 时间段
	bool bAddToAI;		// 是否给机器人添加
	bool bAddToAIEx;	// 是否只给有真人玩家阵营的机器人添加

	SWarAddBuff()
	{
		memset(this, 0, sizeof(*this));
	}

};

// 大逃杀战场安全区
#define WAR_SCHEME_SAFETYZONE "SCP\\WarSafetyZone.csv"
struct SWarSafetyZone
{
	vector<int>	nSpreadTipsTime;					// 扩散提示播放时间
	Vector3		centerOfNoxiousZone;				// 毒区中心

	int			nRound;								// 轮次
	float		fRadiusOfNoxiousZone;				// 毒区半径
	float		fRadiusOfSafetyZone;				// 安全区半径
	int			nSpreadCountdown;					// 扩散倒计时
	int			nSpreadTime;						// 扩散时间
	char		szCDBroadcast[GAME_MSGSTR_MAXSIZE] ;	// 安全区广播文本
	char		szSpreadBeginBcast[GAME_MSGSTR_MAXSIZE];	// 开始扩散广播文本
	int			nBuffID;							// BuffID
	int			nBuffIDLevel;						// BuffLevel


	SWarSafetyZone()
	{
		memset(&nRound, 0, sizeof(*this) - ((char*)&nRound - (char*)this));
		centerOfNoxiousZone.x = 0.f;
		centerOfNoxiousZone.y = 0.f;
		centerOfNoxiousZone.z = 0.f;
		fRadiusOfNoxiousZone = 0.f;
		fRadiusOfSafetyZone = 0.f;
	}
};

// 战场随机刷怪
#define WAR_SCHEME_RANDREFRESHMONS  "SCP\\WarRandRefreshMonster.csv"
struct SWarRandRefreshMons
{
	Vector3 centerPos;		// 刷新圆心坐标

	int nConfigWarID;		// 战场ID
	int nRefreshTime;		// 刷新时间
	int nMonsterIndexID;	// 怪物表序列号
	int nMonsterCamp;		// 怪物阵营
	//int nMonsterNum;		// 怪物数量
	int nAliveTime;			// 存活时间
	bool isInSafetyZone;	// 是否在安全区
	float fMonsDistance;	// 相同怪物距离
	float radiusRange;		// 刷新半径

	SWarRandRefreshMons()
	{
		memset(&nRefreshTime, 0, sizeof(*this) - ((char*)&nRefreshTime - (char*)this));
		fMonsDistance = 0.f;
		radiusRange = 0.f;
	}
};

// 战场魔王玩法
#define WAR_SCHEME_DEVILKING "SCP\\WarDevilKing.csv"
struct SWarDevilKing
{
	int nKillNumCond;		// 达成魔王击杀条件
	int nDevilKingBuff;		// 魔王Buff
	int nCloseTime;		// 魔王消失时间
	int nBcastTime;		// 广播时间

	SWarDevilKing()
	{
		memset(this, 0, sizeof(*this));
	}

};

// 战场魔王玩法
#define WAR_SCHEME_DEVILKINGNPC "SCP\\WarDevilKingNPC.csv"
struct SWarDevilKingNPC
{
	int nHeroID;
	int nMonsterID;
	int nMonsterBuff;

	SWarDevilKingNPC()
	{
		memset(this, 0, sizeof(*this));
	}

};