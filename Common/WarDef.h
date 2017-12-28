/*******************************************************************
** 文件名:	WarDef.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	3/23/2015
** 版  本:	1.0
** 描  述:	战场公用定义

			
********************************************************************/


#ifndef __COMMON_WAR_DEF_H__
#define __COMMON_WAR_DEF_H__

//输出DEBUG信息
#ifdef VERSEION_DEBUG
#   define WAR_DEBUG_OUTPUT
#endif
#ifdef WAR_DEBUG_OUTPUT
# define WAR_TRACELN(x)			    TraceLn(x)
# define WAR_WARNINGLN(x)			WarningLn(x)
# define WAR_ERRORLN(x)			    ErrorLn(x)
# define WAR_EMPHASISLN(x)			EmphasisLn(x)
#else
# define WAR_TRACELN(x)
# define WAR_WARNINGLN(x)
# define WAR_ERRORLN(x)
# define WAR_EMPHASISLN(x)
#endif

#pragma pack(1)

/////////////////////////实体通用配置/////////////////////////////////////////////////
// 战场相关配置表
#define WAR_CONFIG_SCHEME_FILENAME				"Scp\\WarPersonConfig.xml"

#define  WARDATA_LEN_MAX			168

#define  MAX_WAR_MINIMAP_DEFINE		3

#define  MAX_WAR_NEAREST_COUNT		512

#define  WAR_SOLIDER_VIGOUR_MAX 300  // 最大气势系数

#define MaxVigourRatioCount 4        // 最大气势系数个数

#define MAX_TAB_REQUEST_TICK_INTERVAL   1500

#define SPELL_FINAL_SKILL_SLOT   2	// 技能大招槽位

#define WAR_ECM_EXP_ADVANTAGE   1	// 经济经验综合优势
#define WAR_ECM_EXP_STALEMATE   0	// 经济经验综合僵持
#define WAR_ECM_EXP_INFERIORITY -1  // 经济经验综合劣势

#define BOTTING_COUNT_MAX 3			// 挂机最大次数,大于等于该次数后直接收到挂机惩罚

#define BOTTING_COUNT_KICK_OUT_MAX 2		// 挂机被踢下线最大次数,大于等于该次数后直接踢下线

#define BENEFIT_BUFF_DURATION 5000	// 增益buff持续时间

#define KILL_BASE_BUFFID 3706		// 杀死水晶buff id

#define WARQUIPSCHEME_EQUIP_MAXSIZE 12	// 装备方案总个数

// 战场模块定时器ID定义 防止覆盖定时器
enum
{
    ETimerEventID_WarProcess = 0,	        // 战场流程处理
    ETimerEventID_DeadKillCount,	        // 战斗中连续击杀处理
    ETimerEventID_WarHaveNoPerson,	        // 战斗中连续击杀处理
    ETimerEventID_WarActorLog,              // 记录一场战斗的记录信息
    ETimerEventID_XpCdTime,                 // xp技能冷却时间
    ETimerEventID_EcmExpSituation,          // 优劣势情况记录
    ETimerEventID_EcmControlRatio,			// 计算经济控制系数
    ETimerEventID_EcmControlLine,			// 计算经济控制线
    ETimerEventID_Max,                      // 本模块最大定时器ID
};

// 战场 阶段
enum EWarDieType
{
	EWDT_BOSS = 0,		// BOSS死亡
	EWDT_PERSON,		// 人物死亡
	EWDT_TOWER,			// 塔
	EWDT_CRYSTAL,		// 水晶
};

// 战场 阶段
enum EWarPhase
{
	EWP_Ready = 0,		// 准备，读图，需要在开始前进行校验
	EWP_Start,			// 开始
	EWP_End,			// 结束
	EWP_All,			// 表示，在全局调用
};

// 战场条件
enum EWarCondition
{
	WC_Level = 0,		// 战场等级
	WC_MaxPerson,

};


// 掉落增益或衰减
enum EWarDropType
{
	EWDT_None = 0,
	EWDT_Gain,				// 增益
	EWDT_Damping,			// 衰减
};


// 掉落增益或衰减
enum EWarRevocationType
{
	EWRT_None = 0,
	EWRT_Buy,				// 购买
	EWDT_Sell,				// 出售
    EWDT_ExChange,			// 交换
};

// 时间范围内连杀类型
enum ETimeKillType
{
    ETKT_TYPE_3 = 3,        // 3连杀
    ETKT_TYPE_4,            // 4连杀
    ETKT_TYPE_5,            // 5连杀

    ETKT_TYPE_MAX
};

// 不死情况下杀人数对应的称号
enum EDeadKillNumTitile
{
    EKNT_TYPE_3 = 3,        // 大杀特杀
    EKNT_TYPE_4,            // 接近暴走
    EKNT_TYPE_5,            // 无人可档
    EKNT_TYPE_6,            // 主宰比赛
    EKNT_TYPE_7,            // 接近神了
    EKNT_TYPE_8,            // 已经超神了

    EKNT_TYPE_MAX
};

struct SEDTPersonData
{
    PDBID   dwActorID;
    int     nBaseRecord;

    SEDTPersonData()
    {
        memset(this, 0 , sizeof(*this));
    }
};

struct SESVTPersonData
{
	PDBID   dwActorID;
	int     nBaseRecord;
	SESVTPersonData()
	{
		memset(this, 0 , sizeof(*this));
	}
};

// 玩家战绩的基本数据(其实是称号相关)
enum EDataType
{
    EDT_OutPut = 0,             // 总输出（对敌方英雄的）
    EDT_KillCount,              // 杀人数
    EDT_AssistCount,            // 助攻数
    EDT_Damage,                 // 承受伤害(对敌方英雄)
    EDT_DestoryTower,           // 推塔数
    EDT_Money,                  // 总的金币
    EDT_KillMonster,            // 击杀怪物数
	EDT_Cure,					// 团队治疗
	EDT_ThrillSave,				// 惊险救治
	EDT_DeadlyControl,			// 夺命控制
	EDT_TeamBattleControl,		// 团控
    EDT_Score,                  // 综合分数
    EDT_KillDragon,             // 杀龙数
	EDT_JoinBattleRate,			// 参战率
	EDT_MvpOfLoser,				// 失败方mvp

    EDT_MAX
};

// 经验掉落的类型
enum EExpDropType
{
    EEXPDROPTYPE_MONSTER = 0,       // 野怪掉落经验
    EEXPDROPTYPE_SOLIDER,           // 小兵掉落经验
    EEXPDROPTYPE_PLAYER,            // 玩家掉落经验
    EEXPDROPTYPE_TOWER,             // 塔怪掉落经验
    EEXPDROPTYPE_BOSS,              // BOSS掉落经验
    EEXPDROPTYPE_NORMAL,            // 自然掉落经验
    EEXPDROPTYPE_INIT,				// 初始掉落经验
    EEXPDROPTYPE_MAX
};


// 金钱掉落类型
#define  TalentDropTypeFilter (ETALENTDROPTYPE_MONSTER|ETALENTDROPTYPE_SOLIDER|ETALENTDROPTYPE_PLAYER)
enum ETalentDropType
{
    ETALENTDROPTYPE_MONSTER = 0,    // 野怪增加金钱
    ETALENTDROPTYPE_SOLIDER,        // 小兵增加金钱
    ETALENTDROPTYPE_PLAYER,         // 玩家增加金钱
    ETALENTDROPTYPE_TOWER,          // 塔怪增加金钱
    ETALENTDROPTYPE_NORMAL,         // 自然增加金钱
    ETALENTDROPTYPE_EFFECT,         // 效果增加金钱
    ETALENTDROPTYPE_SELLEQUIP,      // 卖装备增加金钱
    ETALENTDROPTYPE_BOSS,           // BOSS掉落金钱
    ETALENTDROPTYPE_INIT,           // 初始金钱

    ETALENTDROPTYPE_MAX
};

// 气势士兵属性添加类型
enum Vigour_Property_Type
{
	VPT_ADD_HP,         // 气势加血
	VPT_ADD_PA,         // 气势加攻击

	VPT_ADD_MAX,        // 气势属性最大
};

// 快捷键技能特殊选择的怪物作用类型
enum ESpecial_Select_Entity_Function_Type
{
	ESSEFT_INVALID,               // 无效类型
	ESSEFT_ALTARHANDIN,           // 用于祭坛上交

	ESSEFT_MAX,				      // 最大类型
};

enum ESpecial_Entity_Range_Search_Function_Type
{
	ESRSFT_INVALID,               // 无效类型
	ESRSFT_WATCHOUTTOWER,         // 用于瞭望台搜寻玩家

	ESRSFT_MAX,				      // 最大类型
};

// 玩家正在运行中的进度条类型
enum EPlayerProgressType
{
	EPPT_INVALID,                 // 无效类型
	EPPT_ALTARPROGRESS,           // 祭坛读条
	EPPT_WATCHTOWERPROGRESS,      // 瞭望台读条
	ELSFT_MAX,				      // 最大类型
};



// 特殊贡献类型
enum ESpecialContributionType
{
	ESCT_THRILLSAVE,				// 惊现救治
	ESCT_DEADLYCONTROL,				// 夺命控制
	ESCT_TEAMBATTLECONTROL,			// 团控

	ESCT_MAX,
};

// 玩家点赞评选类型
enum ESupportVoteType
{
	ESVT_MVP = 0,				// MVP
	ESVT_PENTA,					// 5杀荣誉获得者
	ESVT_ENTERBATTLERATE,       // 参团积极分子 
	ESVT_OUTPUTRATE,            // 输出爆炸 
	ESVT_CURETEARATE,           // 最佳治疗者
	ESVT_BEARDEMAGERATE,        // 伤害承受者
	ESVT_KILLLDRAGEN,           // 驾驭恶魔的勇士！
	ESVT_KILLSDRAGEN,			// 屠龙强者		
	ESVT_SLEVELTITLE,			// SSR,全程S评分最多的玩家
	ESVT_KILLPERSON,			// 疯狂杀戮
	ESVT_ALIVEKILLMAX,			// 超神不败
	ESVT_ASSISPERSON,           // 助攻最多      
	ESVT_ALIVEASSISTMAX,        // 连续助攻最多
	ESVT_DESTROYTOWER,			// 推塔
	ESVT_OUTPUTDEMAGE,			// 伤害输出
	ESVT_BEARDEMAGE,			// 承受伤害
	ESVT_CURETEAM,				// 治疗
	ESVT_DIEMAX,				// 死里逃生
	ESVT_DIEMOREKILL,			// 绝地反杀
	//ESVT_KILLMONSTER,			// 杀怪物
	ESVT_MONERYMOST,			// 最有钱
	ESVT_FLYSUPPOT,				// 飞行支援
	ESVT_CAMPCOMSCORE,			// 本阵营最佳玩家

	ESVT_MAX,
};

// 优劣势类型
enum EWarEcmExpSituationType
{
	EWEEST_ADVANTAGE,				// 优势
	EWEEST_STALEMATE,				// 僵持
	EWEEST_INFERIORITY,				// 劣势

	EWEEST_MAX,
};


// Boss类型
enum EWBossType
{
	EWBossType_NONE,			// 未知
	EWBossType_SMALLDRAGON,		// 小龙
	EWBossType_BIGDRAGON,		// 大龙
	EWBossType_OTHER,			// 其他
};


// 战场结果数据,提供给任务-lua层
enum EWarResultToTask
{
	WRTT_HeroType,
	WRTT_MatchType,
	WRTT_IsHaveClanMember,
	WRTT_IsHaveMentorship,
	WRTT_IsWin,
	WRTT_IsSuperGod,
	WRTT_IsMVP,
	WRTT_IsTopOutput,
	WRTT_IsTopKillTower,
	WRTT_IsTopAssist,
	WRTT_IsTopGold,
	WRTT_TimeContKillMax,
	WRTT_DeadContKillMax,
	WRTT_KillNum,
	WRTT_AssistNum,
	WRTT_Score,
	WRTT_Gold,
	WRTT_DeadlyControlNum,
	WRTT_ThrillSave,
	WRTT_KillTowerNum,
	WRTT_KillSmallBoss,
	WRTT_KillBigBoss,
	WRTT_DeadNum,

	WRTT_Max,
};

// 战场的轮询时间 1000毫秒
#define WarDefinRTime 1000

// 战场连杀轮询 2000毫秒
#define WarDefinRTimeKill 2000

// 战场自动结束 30秒
#define WarAutoEndTime	30000

// 优劣势情况记录 30秒
#define WarEcmExpSituationTime	30000

// 计算经济控制系数 10秒
#define WarEcmContorlRatioTime	10000

// 计算经济控制线 60秒
#define WarEcmControlLineTime	60000

// 创建War上下文
struct SWarEffectAddBuff
{
	
	DWORD   uidSrcEntity;      // 添加实体UID
	int		nID;				// spellID
	BYTE	bySelfCamp;
	BYTE	byGetCampType;
	BYTE	bySerchType; 
	BYTE	byAddType;
	int		nBuffID;
	int		nBuffLevel;

	SWarEffectAddBuff()
	{
		memset(this, 0, sizeof(*this));
	}
};

//////////////////////////////////////////////////////////////////////////
// 创建War上下文
struct SWarCreateContext
{
	BYTE	byRoomType;			// 房间类型 EMHallRoomType
    DWORD   dwMatchTypeID;      // 撮合匹配类型 EMMatchType
	DWORD	dwRoomID;			// 房间ID
	DWORD	dwMapID;			// 地图ID
	BYTE	byMode;				// 战场类型
	int		nConfigWarID;		// 
	int		nWarID;				// 战场ID
	DWORD	dwAvgRankScore;		// 平均匹配分
	BYTE    byRoleCount;		// 总人数
    BYTE    byCampCount;		// 阵营数

	SWarCreateContext()
	{
		memset(this, 0 , sizeof(*this));
	}
};

struct sTabRecordInfo
{
    bool bOpen;
    DWORD dwOpenTicks;

    sTabRecordInfo()
	{
		memset(this, 0 , sizeof(*this));
	}
};

struct WarRoomInfo
{
	int nMapID;
	int nRoomIndex;
	int nWarID;
	WarRoomInfo(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

struct PersonWarRoomInfo
{
	DWORD	clientID;
	DWORD	PersonDBID;
	UID		PersonUID;
	int		nCamp;
	int		HeroID;
	int		nIndex;
	PersonWarRoomInfo(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

// 战场阵营信息
struct WarRoomCampInfo
{
	// 阵营信息
	int nCamp;
	// 复活坐标点
	float fX;
	float fY;
	float fZ;
	WarRoomCampInfo(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

// 战场相关系数配置信息
struct SchemeWarPersonConfigInfo
{
	//战场ID
	int nWarID;

	// 第一次自动加天赋点间隔(单位秒)
	int nFirstAutoAddTalentInterval;

	//战场初始化天赋点
	int nInitialTalentPoint;

	//战场自动添加天赋点
	int nAutoAddTalentPoint;

	//战场自动添加天赋点时间间隔(单位秒)
	int nAutoAddTalentInterval;

    //战场初始化经验
    int nInitialExp;

    //战场自动添加经验
    int nAutoAddExp;

    // 第一次自动加经验间隔(单位秒)
    int nFirstAutoAddExpInterval;

    //战场自动添加经验时间间隔(单位秒)
    int nAutoAddExpInterval;

	// 人物死亡经验掉落系数
	float fDieExpRate;
	// 人物死亡掉落天赋点
	int nDieTalentPoint;
	// 人物助攻天赋点分配系数
	float fAssistTalentRate;

	//复活系数
	int nReliveRatio;

	//复活系数基数
	int nReliveBaseTime;

	// 水晶被攻击后小地图信息时间间隔
	int nCrystalDamageIntervalTime;

	// 人物战斗小地图信息时间间隔
	int nPersonFightIntervalTime;

	// 掉落经验衰减系数
	float fDropDumpingEXPRate;

	// 掉落天赋衰减系数
	float fDropDumpingTalentRate;

	// 掉落经验增益系数
	float fDropGainEXPRate;

	// 掉落天赋增益系数
	float fDropGainTalentRate;

	// 未造成伤害掉落经验天赋添加范围
	int nAddAroundEXPDistance;

	// 击杀人数经验掉落加层系数
	float fKillPersonDropEXPRate;

    // 首杀获取的金币
    int nFirstKillGetMoney;

    // 野怪增益系数
    int nIncreaseMonster;

    // 野怪增益系数上限
    int nIncreaseMonsterMax;

    // 野怪衰减系数
    int nDecreaseMonster;

    // 野怪衰减系数
    int nDecreaseMonsterMin;

    // 小兵增益系数
    int nIncreaseSolider;

    // 小兵增益系数上限
    int nIncreaseSoliderMax;

    // 等级控制
    int nControlLv;

    // 英雄增益系数
    int nIncreaseHero;

    // 英雄增益系数上限
    int nIncreaseHeroMax;

    // 英雄衰减系数
    int nDecreaseHero;

	// 英雄衰减系数下限
	int nDecreaseHeroMin;

	// 经济差控制系数
	float fEconomyControlRate;

	// 最高连杀
	int nMaxMultiKill;
	
	// 连杀间隔
	int nTimeCountKillInterval;

	// 士兵气势比例 对应四个等级
	int VigourLevelRatio[VPT_ADD_MAX][MaxVigourRatioCount];

	// 英雄衰减系数下限
	int nAddGemstoneExp;

	// 小兵金币分享基数
	float fGoldShareBase;

	// 塔兵削减系数
	float fTowerKillDecrease;

	SchemeWarPersonConfigInfo(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

struct SWarPersonConfig
{
    int nMonsDistance;          // 怪物的监视距离
    int nTowerDistance;         // 塔的监视距离
	int nMonsterDeadEffect;         // 怪物光效
    int nMoneyEffect;         // 怪物光效
    SWarPersonConfig()
    {
        memset(this, 0, sizeof(*this) );
    }
};

struct SCampProgressInfo
{
	EPlayerProgressType ePlayerProgressType;
	SCampProgressInfo()
	{
		memset( this, 0, sizeof(*this));
	}
};

#define WarAltarTimeCampCount 3
struct SWarAltarInfo
{
	UID uidAltar;			                          // 祭台uid
	DWORD dwCampLastAltarTime[WarAltarTimeCampCount]; // 祭坛阵营上次上交宝物足够 时刻
	bool bHandIning;							   	  // 祭坛是否在接受玩家上交宝物  以后做同阵营一起交宝物用
	int  nHandIningCamp;		                      // 正在上交宝物的阵营 以后做同阵营一起交宝物用

	SWarAltarInfo()
	{
		memset( this, 0, sizeof(*this));
	}
};

struct SWarWatchTowerInfo
{
	UID uidWatchTower;		         	              // 瞭望台uid
	bool bCapture;									  // 是否已经被占领
	int  nCaptureCamp;								  // 被占领阵营
	DWORD dwCaptureTick;        					  // 被占领时刻
	
	SWarWatchTowerInfo()
	{
		memset( this, 0, sizeof(*this));
	}
};

struct SActorPreaperEnterWar
{
    PDBID   uPDBID;
    char    szName[ACTORNAME_MAXSIZE];
    int     nHeroID; 
    BYTE    byHeroStarLv;                             // 英雄星级
    BYTE    byHeroStaticLv;                            // 英雄等级
	int     nHeroSkinID;
    int     nCamp; 
    int     nIndex;
    int     nSrcWorldID;
    int     nSrcSceneID; 
    bool    isComputer;
    int     nAIID;
    Vector3	vPosition;
    Vector3 vAngle; 
	int     nSummonserSkill[SUMMONER_SKILL_COUNT];  // 召唤师技能
    int     nHeroXPSkillID;
    BYTE    byActorSex;                                 // 玩家性别
    int     nHideRank;                              // 隐藏分
    

    SActorPreaperEnterWar()
    {
        memset(this, 0 , sizeof(*this));
    }
};

struct SActorEnterWarBaseInfo
{
    DWORD       dwUserID;
    UID         uidActor;
    DWORD       ulClient;
    PDBID       pdbid;
    int         nHeroID;
    int         nCamp;
	int			nSummerID;
	int			nHeroSkinID;
    bool        bIsOB;
    SActorEnterWarBaseInfo()
    {
        memset(this, 0, sizeof(*this) );
    }
};

// 装备权重信息
struct SEquipWeightInfo
{
	int nEquipID;
	int nWeight;

	SEquipWeightInfo()
	{
		memset(this, 0, sizeof(*this));
	}

	bool operator == (const SEquipWeightInfo& info)
	{
		return nEquipID == info.nEquipID;
	}

	// 为了权重比较
	static bool compareForWeight (const SEquipWeightInfo& info1, const SEquipWeightInfo& info2)
	{
		if (info1.nWeight != info2.nWeight)
		{
			return info1.nWeight > info2.nWeight;
		}
		else
		{
			return info1.nEquipID > info2.nEquipID;
		}
	}

	// 为了去重比较
	static bool compareForUnique(const SEquipWeightInfo& info1, const SEquipWeightInfo& info2)
	{
		return info1.nEquipID < info2.nEquipID;
	}
};

// 装备方案表
struct SWarEquipScheme
{
	//DWORD dwPDBID;											// 玩家ID
	//WORD wHeroID;												// 英雄ID
	BYTE bySchemeID;											// 装备方案ID
	char szSchemeName[GAME_NAME_MAXSIZE];						// 装备方案名字
	char szAuthorName[ACTORNAME_MAXSIZE];						// 装备方案作者名字
	char szSchemeTitle[GAME_NAME_MAXSIZE];						// 装备方案称号
	SEquipWeightInfo equipWeightInfo[WARQUIPSCHEME_EQUIP_MAXSIZE];	// 装备权重信息

	SWarEquipScheme(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

// 英雄增加BUFF列表
struct SActor_Add_Buff_head
{
    UID    uid; 
    int    nCount;  // SActor_Add_Buff_Node记录数量

    SActor_Add_Buff_head(void)
    {
        memset(this, 0, sizeof(*this));
    }
};
// 英雄被其他英雄增加BUFF信息
struct SActor_Add_Buff_Node
{
    UID     uidAdd;             // 给自己增加BUFF人
    DWORD   dwUpdateTick;       // 更新时间

    SActor_Add_Buff_Node(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

#pragma pack()
#endif	// __COMMON_WAR_DEF_H__