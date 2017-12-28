/*******************************************************************
** 文件名:	EntityDef.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	12/12/2014
** 版  本:	1.0
** 描  述:	实体相关公共定义


********************************************************************/

#ifndef __COMMON_ENTITYDEF_H__
#define __COMMON_ENTITYDEF_H__

#include "Entity_ManagedDef.h"
#include "DamageDef_ManagedDef.h"
#include "Slot_ManagedDef.h"
#include "IGameDatabaseDef.h"
#include "ServiceCommonDef.h"
#include "War_ManagedDef.h"
#include "WarDef.h"

// 无效的(移动同步)包序号
#define INVLAID_PKG_SEQ 0

/************************************************************************/
/* 调试信息输出宏定义                                                   */
/************************************************************************/
// 输出DEBUG信息
#ifdef VERSEION_DEBUG
#   define ENTITY_DEBUG_OUTPUT
// 校验皮肤合法性
#   define ENTITY_CHECK_SKIN
// 校验坐标合法性
#   define ENTITY_CHECK_LOCATION
#endif
#ifdef ENTITY_DEBUG_OUTPUT
#	define ENTITY_TRACELN(x)			TraceLn(x)
#	define ENTITY_EMPHASISLN(x)			EmphasisLn(x)
#	define ENTITY_ERRORLN(x)			ErrorLn(x)
#	define ENTITY_WARNINGLN(x)			WarningLn(x)
#else
#	define ENTITY_TRACELN(x)
#	define ENTITY_EMPHASISLN(x)
#	define ENTITY_ERRORLN(x)
#	define ENTITY_WARNINGLN(x)
#endif
#define TRACE_ENTITY(v)                 ENTITY_TRACELN(v)
#define TRACE_AOI(v)                    ENTITY_TRACELN(v)
// 属性信息输出
//#	define ENTITY_PROP_OUTPUT
// 位置检测最大阀值
#define CHECK_LOCATION_THRESHOLD        5000


// 天赋名称最大长度
#define  MAX_TALENTNAME_LEN                 DBDEF_TALENTNAME_MAXSIZE
// 天赋最大个数
#define  MAX_TALENTNUM_LEN                  40
// 策划测试数值log开关
//#define PRINTF_PLANLOG                      true
// 战场天赋id起始
#define  MAX_HERO_TALENTID_START            10000
// 天赋信息列表
typedef vector<TALENT_INFO>                 TALENT_INFO_VECTOR;
struct EntityHeroRuneStore;
struct Entity_Rune_Page_Info;
typedef std::map<int, EntityHeroRuneStore>                 RUNESTORE_MAP;
typedef std::map<int, Entity_Rune_Page_Info>       		   RUNE_PAGE_MAP;
//////////////////////////////////////////////////////////////////////////

#pragma pack(1)

// UID结构
struct UID_DATA							
{
	unsigned int type  : 4;				// 实体类型，最多支持16种实体类型
	unsigned int scene : 12;             // 场景ID：最多支持4096个场景
	unsigned int serial: 16;				// 对象索引，最多支持65535个对象(同场景)
};
// NOTE!!! 分配ID时不能超过以下值
#define  MAX_ENTITY_TYPE_COUNT  (1<<4)
#define  MAX_NPC_COUNT          (1<<16)
#define  MAX_TANK_COUNT         (1<<16)
#define  MAX_SCENE_COUNT        ((1<<12) - 2)


// NPC的UID结构
typedef UID_DATA	NPC_UID_DATA;
//struct NPC_UID_DATA							
//{
//	int type  : 4;				// 实体类型，最多支持16种实体类型
//	int scene : 12;             // 场景ID：最多支持4096个场景
//	int serial: 16;				// 对象索引，最多支持65535个对象(同场景)
//};

// TANK的UID结构
typedef UID_DATA	TANK_UID_DATA;
//struct TANK_UID_DATA							
//{
//	int type  : 4;				// 实体类型，最多支持16种实体类型
//	int scene : 12;             // 场景ID：最多支持4096个场景
//	int serial: 16;				// 对象索引，最多支持65535个对象(同场景)
//};

#define  INVALID_UID			0xFFFFFFFF
#define  UID_2_TYPE(uid)		((UID_DATA*)&uid)->type
#define  UID_2_SERIAL(uid)		((UID_DATA*)&uid)->serial
#define  UID_2_SCENE(uid)		((UID_DATA*)&uid)->scene

// 是否电脑ID
#define MIN_COMPUTER_PDBID  80000
#define MAX_COMPUTER_PDBID  100000
#define IsComputerPDBID(nActorID)              (MIN_COMPUTER_PDBID < (nActorID) && (nActorID) < MAX_COMPUTER_PDBID)


// 实体ID
typedef int ENTITY_ID;
// 实体UID
typedef unsigned long  UID;

// 设置默认搜索仇恨
#define DEFAULT_PLAYER_ENMITY		1000
#define DEFAULT_MONSTER_ENMITY		1099

//#define  CROWN_PAGE_MAX				10			// 皇冠页最大页数
//#define  CROWN_GEMSTONE_MAX_COUNT	50			// 宝石拥有最大数量
//#define  CROWN_PROPERTY_COUNT_MAX   20			// 属性个数
//#define  GEMSTONEID_COUNT_MAX   	10

enum ENTITY_MASK
{
	MASK_CREATURE	= 1<<1,
	MASK_MONSTER	= 1<<2,
	MASK_NPC		= 1<<3,
	MASK_ACTOR		= 1<<4,
	MASK_TANK		= 1<<5,
};

// 金币使用方式
enum MONEY_USE_FLAG
{
    MONEY_USE_BIND,         // 只使用绑定的
    MONEY_USE_NOBIND,       // 只使用非绑的
    MONEY_USE_ALL,          // 先使用绑定的，绑定全部用完后再使用非绑的
};
// 货币类型
enum GAME_MONEY_TYPE
{
	MONEY_TICKET=1,         // 点券
	MONEY_GOLD,             // 金币
    MONEY_ALL,              // 点券和金币
};


// 玩家活跃度事件标识
enum PLAYER_ACTIVITY_EVENT_FLAG
{
    ACITIVITY_EVENT_FLAG_DAY,       // 日活跃度
    ACITIVITY_EVENT_FLAG_WEEK,      // 周活跃度
    ACITIVITY_EVENT_FLAG_DAYWEEK,   // 日周活跃度
};


// 符文不能购买错误类型
enum RUNE_BUY_ERROR_TYPE
{
	RBYEY_INVALID,         // 无效错误类型 后面再详细区分
	RBYEY_COUNTOVER,       // 该id符文个数超标
	RBYEY_MAX,             // 错误类型最大值
};

// 宝石不能添加错误类型
enum EGEMSTONE_ADD_ERROR_TYPE
{
	EGAET_INVALID,         // 无效错误类型 后面再详细区分
	EGAET_COUNTOVER,       // 宝石个数超标
	EGAET_MAX,             // 错误类型最大值
};

// 宝石不能减少错误类型
enum EGEMSTONE_DECREASE_ERROR_TYPE
{
	EGDET_INVALID,         // 无效错误类型 后面再详细区分
	EGDET_COUNTOVER,       // 宝石个数不能再减
	EGDET_MAX,             // 错误类型最大值
};

// 鉴定宝石类型
enum EAPPRAISAL_GEMSTONE_TYPE
{
	EAGT_MAINGENSTONE,         // 主宝石
	EAGT_ASSISTANTGENSTONE,    // 辅助宝石
	EAGT_MAX,                  // 最大值
};

// 宝石品级
enum EGEMSTONE_GRADE_TYPE
{
	EGGT_OBSOLETEGRADE,		// 陈旧宝石
	EGGT_FIRSTGRADE,        // 一级宝石
	EGGT_SECONDGRADE,		// 二级宝石
	EGGT_THIRDGRADE,		// 三级宝石
	EGGT_FOURTHGRADE,		// 四级宝石
	EGGT_FIVEGRADE,			// 五级宝石
	EGGT_SIXTHTGRADE,		// 六级宝石
	EGGT_MAX,               // 最大值
};


//////////// 实体创建上下文 //////////////////////////////////////////////////////////////
// 附属怪物通知事件标识
enum MONSTER_BELONG_EVENT_NOTIFY_FLAG
{
    MBEN_FLAG_LIFE,         // 生命周期
};
// 怪物创建上下文标识
enum MONSTER_CREATE_CONTEXT_FLAG
{
    MONSTER_CREATE_NOUSE  = 0x0,          // 默认为未用
    MONSTER_CREATE_CLONE  = 0x00000001,   // 分身怪物 
};

// 给英雄设置核晶效果的时刻
enum ETIME_TO_ADD_EFFECT
{
    ENTITY_CREATE,
    ENTITY_UPGRADE,
};

// 附属物创建/销毁内容
struct EntityBelongContext
{
    bool    isCreated;  // 是否创建
    UID     uid;        // 实体UID
    UID     uidMaster;  // 主人UID
    int     nFlag;      // 标识

    EntityBelongContext(void)
    {
        memset( this, 0, sizeof(*this) );
    }
};

// 用来填充创建怪物时所用的上下文
struct CreateNpcContext
{
	// 战场ID(静态地图的时候战场ID为-1)
	int nWarID;
	// 等级
	int nLevel;
	// 阵营
	int nCamp;
	// AI ID
	int nAIID;
	// 兵线路径ID
	int nPathID;
	// 组ID
	int nGroupID;
	// 是否共享图标
	BYTE bShareIcon;
	// 图标ID
	int nIconID;
	// 朝向
	Vector3 Angle;

	UID uidCreator;

	int VigourRatio[VPT_ADD_MAX];
    // 怪物创建上下文标识 MONSTER_CREATE_CONTEXT_FLAG
    int nFlag;

	// PK类型
	int nPKType;

	// PK状态
	int nPKState;

	CreateNpcContext()
	{
		memset(this, 0, sizeof(*this));
		nLevel = 1;
	}
};

struct sEntityDieInfo 
{
	DWORD   uDiedPropertyID;    // 怪物ID或者人物PDBID
	UID     uDiedUID;           // 死亡的UID
	int     nDiedCamp;          // 死亡玩家的阵营
	int     nDiedVocation;
	char    szDiedName[GAME_NAME_MAXSIZE];
    int     nLevel;             // 等级

	DWORD   uMurderPropertyID;  // 凶手的怪物ID或者人物PDBID
	UID     uMurderUID;         // 凶手的UID
	int     nMurderCamp;        // 死亡玩家的阵营
	int     nMurderVocation;
	char    szMurderName[GAME_NAME_MAXSIZE];
	// 死亡位置
	float		x;				// 死亡X坐标
	float		y;				// 死亡Y坐标
	float		z;				// 死亡Z坐标
	sEntityDieInfo()
	{
		memset(this, 0, sizeof(*this));
	} 

};

// 创建角色时的上下文参数
struct SCreatePlayerContext
{
	DWORD   client;                     // 连接器id
	char    name[GAME_NAME_MAXSIZE];    // 角色名
	short   heroid;                     // 角色创建时选择的英雄id
	BYTE    type;                       // 0:静态角色, 1:MOBA英雄
	BYTE    camp;                       // 阵营ID
	int     sceneid;                    // 场景ID(动态的)
	int     mapid;                      // 地图ID(静态的)

	SCreatePlayerContext()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 人物信息
struct SPlayerInfo
{
	DWORD	dwUserID;                                       // 用户ID
	DWORD	dwWorldID;                                      // 当前角色所属的游戏世界ID
	DWORD	dwActorID;                                      // 角色ID
	char	szActorName[DBDEF_ACTORNAME_MAXSIZE];           // 角色名
    char    szPwd[GAME_PWD_MAXSIZE];                        // 密码
	WORD	nHeroID;                                        // 静态英雄ID: 这里的nHeroID和SHeroInfo.nHeroID没有任何关系
	WORD	nSkinID;                                        // 静态皮肤ID   ****(策划提出可能和英雄皮肤不一样是单独的一个皮肤， 之后还可能是表情高低可以改变的情况 策划说不会这么搞)*****
	WORD    nLevel;                                         // 静态角色等级
	bool    bSex;                                           // 性别
	DWORD   dwMapId;                                        // 地图ID
	int     nLocX;                                          // x坐标，由于坐标会有小数将扩10000倍
	int     nLocY;                                          // y坐标，由于坐标会有小数将扩10000倍
	int     nLocZ;                                          // z坐标，由于坐标会有小数将扩10000倍
    int     nAngle;                                         // 角度，由于会有小数将扩10000倍

	DWORD   dwExp;                                          // 经验
	DWORD   dwBindTicket;                                   // 金币
	DWORD   dwTicket;                                       // 点券
	DWORD   dwHeroPoolLimit;                                // 英雄池上限
	WORD    wHeadID;                                        // 头像Id
	DWORD   dwClanID;                                       // 工会id
	DWORD   dwKinID;                                        // 家族id
	DWORD   dwTeamID;                                       // 战队id
	BYTE    bySummonerSkill[DBDEF_SUMMONER_SKILL_MAXCOUNT]; // 召唤师技能
	BYTE    byGlory[DBDEF_GLORY_BUFFSIZE];                  // 个人荣耀BUFF
	DWORD   dwFightCapacity;                                // 玩家战力
	BYTE    byCampHeroID[DBDEF_CAMPUSE_HEROID_MAXCOUNT];    // 上一次队友使用英雄id
	DWORD	dwPKWinNum;										// PK胜利场次
	DWORD	dwPKTotalNum;									// PK总场次
	BYTE    byRankbuffdata[DBDEF_RANK_BUFF_SIZE];           // 排位数据相关
    DWORD   dwRankMatchNum;                                 // 排位比赛次数
    WORD    wRankWinNum;                                    // 排位胜利场次 
	DWORD	dwSupportNum;									// 点赞数
    int     nPrivilege;                                     // 权限

	SPlayerInfo()
	{
		memset(this, 0, sizeof(SPlayerInfo));
	}
};

struct SPlayerDatabasesInfo
{
	DWORD       dwUserID;                               // 用户ID
	DWORD       dwIdCard;
	bool        bIsAdult;
    DWORD       dwPassportFlags;	                    // 通行证相关标识 PassportFlags
    DWORD       dwServiceFlags;                         // 服务器标识 ServiceFlags
	SPlayerInfo player;                                 // 角色信息
	int         herocnt;								// 英雄数量
	SPlayerDatabasesInfo()
	{
		memset(this, 0, sizeof(SPlayerDatabasesInfo));
	}
};

////////////配置脚本接口//////////////////////////////////////////////////////////////


// 人物模型表
#define PERSONMODEL_SCHEME_FILENAME				"Scp\\PersonModel.csv"
// 定义移到托管定义文件中


// 英雄信息
struct HeroSimpleInfo
{
	uint16 uVocation;
	char   chName[GAME_NAME_MAXSIZE];
	int     nOnOff;
	HeroSimpleInfo()
	{
		memset(this,0,sizeof(*this));
	}
};

struct SVocationName
{
	uint16 wVocation;								// 英雄职业
	string strName;

	SVocationName()
	{
		wVocation = VOCATION_INVALID;
	}
};


// 英雄卡牌表
#define HEROCARD_SCHEME_FILENAME			    "Scp\\HeroCard.csv"
// 卡牌名称
#define CARD_NAME_MAXSIZE                       128
// 卡牌描述
#define CARD_REMARK_MAXSIZE                     256
// 使用卡牌目标英雄最大数量
#define CARD_HERO_MAXCOUNT                      3
// 英雄卡牌配置
struct SHeroCardScheme
{
	int nCardID;                            // 卡牌ID
	int nCardGrade;                         // 卡牌等级
	char szCardName[CARD_NAME_MAXSIZE];     // 卡牌名称
	int nCardIcon;                          // 卡牌图标
	int nCardFrameIcon;                     // 卡牌框图标
	WORD arrayHeroID[CARD_HERO_MAXCOUNT];   // 使用卡牌目标英雄ID集，策划配置0为所有英雄，内存结构用最大英雄ID
	char szRemark[CARD_REMARK_MAXSIZE];     // 卡牌描述
	int nFlag;                              // 控制标识

	SHeroCardScheme(void)
	{
		memset(this, 0, sizeof(*this) );
	}
};

// 英雄卡牌等级颜色表
#define HEROCARDGRADECOLOR_SCHEME_FILENAME			    "Scp\\HeroCardGradeColorConfig.csv"
// 卡牌颜色
#define CARD_COLOR_MAXSIZE                       42
// 英雄卡牌等级颜色配置
struct SHeroCardGradeColorScheme
{
	int nCardGrade;                          // 卡牌等级
	char szCardColor[CARD_COLOR_MAXSIZE];    // 卡牌颜色

	SHeroCardGradeColorScheme(void)
	{
		memset(this, 0, sizeof(*this) );
	}
};

// 槽位对应的快捷键
#define SLOT_MAPED_SHORTCUT_SCHEME_FILENAME			"Scp\\SlotMapedShortcut.csv"
// 槽位对应快捷键配置
struct SSlotMapedShortcutScheme
{
	int		nSlotIndex;								//槽位索引
	int     nSlotType;								//槽位类型
	char	szKeyName[GAME_SHORTCUT_NAME_MAXSIZE];	//槽位对应的快捷键名称
	int		nAirCons;								//空中限制
	int     nSkillType;                             //技能类型

	SSlotMapedShortcutScheme()
	{
		memset(this, 0, sizeof(*this));
	}
};

#define MONSTER_SCHEME_FILENAME				"Scp\\Monster.csv"
struct SMonsterSkillData
{
	long lSkillID;
	long lSkillPer;
	long lSkillCD;

	SMonsterSkillData()
	{
		memset(this, 0, sizeof(*this));
	}
};
// 怪物配置信息
struct SMonsterScheme
{
	int		nMonsterID;								// 怪物ID	
	int		nMonsType;								// 怪物类型 MONSTER_TYPE
	int     nSubType;                               // 子类型 MONSTER_SUB_TYPE
	int		nMaxHP;									// 生命值
	float	fAddHP;									// 生命增长值
	int		nMaxMP;									// 法力值
	float	fAddMP;									// 法力增长值
	int		nPA;									// 物理攻击力
	float	fAddPA;									// 物理攻击增长值
	int		nMA;									// 法术攻击力
	float	fAddMA;									// 法术攻击增长值
	int		nPD;									// 物理防御力
	float	fAddPD;									// 物理防御增长值
	int		nMD;									// 法术防御力
	float	fAddMD;									// 法术防御增长值
	int		nASD;									// 攻击速度
	float	fAddAS;									// 攻击速度增长值
	int		nCRC;									// 暴击率
	float	fAddCRC;								// 暴击增长值
	int		nMoveSpeed;								// 移动速度
	float	fEyeshot;								// 视野距离
	float   fSearchRadii;                           // 搜索范围
	BYTE	byCamp;									// 阵营
	char	szName[GAME_NAME_MAXSIZE];				// 怪物名
	//char szPrefabPath[GAME_NAME_MAXSIZE];			// 预设体路径
	long	lStrategy;								// 搜索策略
	long	lSeachEnmity;							// 搜索仇恨值
	long	lStrategyVal;							// 搜索频率
	SMonsterSkillData stMonstoerSkill[4];			// 可使用的技能
	int		nExpVal;								// 经验值
	int		nExpRange;								// 经验范围
	BYTE	byExpType;								// 经验类型
	int		nRetrieveDis;							// 回归半径
	int		nRetrieveMask;							// 回归标志
	int		nDefaultSkin;							// 默认皮肤
	int		nIconID;								// 怪物图标ID
	int		nMonsterMask;							// 标志位 ENTITY_MASKFLAG
	int		nBossBloodShowDistance;					// boss血条的显示距离,如果标志位中没有配显示boss血条或者值为0，忽略
	int		nBossBloodShowPriority;					// boss血条的显示优先级，越大越高
	int     nMinimapIconID;							// 小地图图标ID
	int     nColor;                                 // 怪物可秒杀时血管颜色
	int     nBloodStyle;                            // 血条样式
	int     nReserve;                               // 保留数值; 注:动态怪.障碍配置(DynamicObstacle.csv)索引号

	SMonsterScheme()
	{
		memset(this, 0, sizeof(*this));
	}
};

#define PATROL_SCHEME_FILENAME				"Scp\\Patrol.csv"

/////////////////////////动态障碍配置相关/////////////////////////////////////////////////
// 动态障碍配置文件名字
#define DYNAMICOBSTACLE_SCHEME_FILENAME				"Scp\\DynamicObstacle.csv"
enum EMDynamicObsShapeScheme
{
	OBS_SHAPE_TRI = 0,          // NULL
	OBS_SHAPE_CUBE = 1,         // 方体
	OBS_SHAPE_CYLINDER = 2,     // 圆柱体
	OBS_SHAPE_HEX = 3,          // 六边形
};

/////////////////////////符文相关/////////////////////////////////////////////////
// 符文功能关闭
//#define FUNCTION_RUNE_OPEN

#ifdef FUNCTION_RUNE_OPEN
// 符文类型
enum EMRuneType
{
	RUNE_TYPE_INIT = 0,

	RUNE_TYPE_RED,          // 红色符文
	RUNE_TYPE_YELLOW,       // 黄色符文
	RUNE_TYPE_BLUE,         // 蓝色符文
	RUNE_TYPE_ELITE,        // 精华符文

	RUNE_TYPE_MAX
};

// 符文的特殊效果定义
enum EMRuneExtraEffect
{
	RUNE_EXTRA_INIT = PROPERTY_MAX,

	RUNE_EXTRA_PA_PER_LEVEL = PROPERTY_MAX + 1,                  // 每级加物理攻击力
	RUNE_EXTRA_MA_PER_LEVEL = PROPERTY_MAX + 11,                 // 每级加法术攻击
	RUNE_EXTRA_HP_PER_LEVEL = PROPERTY_MAX + 21,                 // 每级加固定生命值
	RUNE_EXTRA_MP_PER_LEVEL = PROPERTY_MAX + 31,                 // 每级增加法力值
	RUNE_EXTRA_MD_PER_LEVEL = PROPERTY_MAX + 41,                 // 每级加法术防御
	RUNE_EXTRA_PD_PER_LEVEL = PROPERTY_MAX + 51,                 // 每级加物理防御
	RUNE_EXTRA_MP_REPLY_PER_LEVEL = PROPERTY_MAX + 61,           // 每级加魔法回复
	RUNE_EXTRA_PCT_COOL_REDUCE = PROPERTY_MAX + 71,              // 每级加冷却缩减(百分比)
	RUNE_EXTRA_ENERGY_REPLY_PER_LEVEL = PROPERTY_MAX + 81,       // 每级加能量回复
	RUNE_EXTRA_HP_REPLY_PER_LEVEL = PROPERTY_MAX + 91,           // 每级加生命回复
	RUNE_EXTRA_MONEY_GAIN_PER_TEN_SECONDS = PROPERTY_MAX + 101,   // 每10秒获得金钱

	RUNE_EXTRA_MAX,
};

// 符文
struct Entity_Rune_Info
{
	int     nID;         // 符文ID
	int     nNum;        // 该类符文的个数 
	Entity_Rune_Info()
	{
		memset(this, 0, sizeof(*this));
	};
};

struct EntityHeroRuneStore
{
	int nRuneId;
	BYTE byActiveRuneCount;
	BYTE byUnActiveRuneCount;

	EntityHeroRuneStore()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 符文页
// 注意：已经镶嵌的符文对应固定槽位(nRuneGroove)，规则如下:
// 1. index为01 ~ 09对应红色符文
// 2. index为10 ~ 17对应黄色符文
// 3. index为18 ~ 26对应蓝色符文
// 4. index为26 ~ 28对应精华符文
#define RUNE_GROOVE_MAX 7    // 符文槽位数
struct Entity_Rune_Page_Info
{
	int nPageIdx;                        // 符文页码
	int nRuneGroove[RUNE_GROOVE_MAX];    // 符文ID数组，每一个符文数组成员对应固定槽位,一个玩家有30个槽位,仅当槽位值为非0值时，该槽位有镶嵌的符文
	int nRuneNum;                        // 已镶嵌符文数
	char	szRunePageName[GAME_NAME_MAXSIZE];	       // 该符文页名称
	Entity_Rune_Page_Info()
	{
		memset(this, 0, sizeof(*this));
	};
};

// 符文同步类型
enum EMRuneSyncType
{
	RUNE_SYNC_TYPE_INTI = 0,

	RUNE_SYNC_TYPE_FETCH,               // 请求同步服务器的符文信息
	RUNE_SYNC_TYPE_SET,                 // 客户端更改符文设置，请求服务器设置符文信息

	RUNE_SYNC_TYPE_MAX,
};

// 请求指定玩家的所有符文情况 CS_MSG_ENTITY_RUNE_REQUEST_OTHER_INFO
struct msg_Request_Other_Rune_Points
{
	DWORD	dwFriendID;            // 请求看到的实体uid
	msg_Request_Other_Rune_Points()
	{
		memset(this, 0, sizeof(*this));
	}
};

// CS_MSG_ENTITY_RUNE_SAVE
// 保存指定符文页
struct msg_runePage_save
{
	int nRunePageIdx;   // 符文页页码
	int nRuneNum;       // 符文数量
	char szRunePageName[GAME_NAME_MAXSIZE];	       // 该符文页名称
	// RUNE_INFO*nRuneNum
	msg_runePage_save()
	{
		memset(this, 0, sizeof(*this));
	}
};
// CS_MSG_ENTITY_RUNE_DELETE
// 删除指定符文页
struct msg_runePage_delete
{
	int nPageIdx;               // 符文页页码
	msg_runePage_delete()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 设置单页天赋页效果 CS_MSG_ENTITY_TALENTPAGEPOINTS_INDEX_EFFECT
struct msg_Entity_RunePage_Index_Effect
{
	int    nCurPageIndex;                                // 第几页符文页

	msg_Entity_RunePage_Index_Effect()
	{
		memset(this, 0, sizeof(*this));
	}
};
#endif

// 单个英雄天赋类型的结构 
struct Single_Hero_Talent_Info 
{
	DWORD dwTalentID;       // 天赋id
	bool  bActive;          // 是否激活
	int nActiveType;		// 激活类型
	int nStarActiveLevel;	// 激活等级
	int nLearnLevel;		// 学习等级

	bool  operator == (const Single_Hero_Talent_Info &other) const
	{ 
		if (this->dwTalentID == other.dwTalentID)
		{
			return true;
		}

		return false;
	}

	Single_Hero_Talent_Info()
	{
		memset(this, 0, sizeof(*this));
	}
};
struct Single_Hero_Talent_InfoEx : public Single_Hero_Talent_Info
{
    std::vector<int>    matchTypes;                 // 战场类型列表
};

typedef struct Single_Hero_Talent_Info HERO_TALENT_INFO;

typedef std::vector<HERO_TALENT_INFO> HERO_TALENT_VEC;
struct EntityHeroTalentInfoVec
{
	HERO_TALENT_VEC TalentInfoVec;
	EntityHeroTalentInfoVec()
	{
		TalentInfoVec.clear();
	}
};

// 单个英雄天赋类型的结构 
struct Single_Hero_Skin_Info 
{
	DWORD dwSkinID;                                  // 皮肤id
	DWORD dwDurationTime;							 // 皮肤时间（0为永久皮肤）
	bool  operator == (const Single_Hero_Skin_Info &other) const
	{ 
		if (this->dwSkinID == other.dwSkinID)
		{
			return true;
		}

		return false;
	}

	Single_Hero_Skin_Info()
	{
		memset(this, 0, sizeof(*this));
	}
};

typedef struct Single_Hero_Skin_Info HERO_SKIN_INFO;

typedef std::vector<HERO_SKIN_INFO> HERO_SKIN_VEC;
struct EntityHeroSkinInfoVec
{
	HERO_SKIN_VEC SkinInfoVec;
	EntityHeroSkinInfoVec()
	{
		SkinInfoVec.clear();
	}
};

struct Single_Hero_XP_Skill_Info
{
	int nXPSkillID;		// XP技能id
	bool bGuide;		// xp技能引导
	bool operator == (const Single_Hero_XP_Skill_Info &other) const
	{ 
		return (this->nXPSkillID == other.nXPSkillID);
	}

	Single_Hero_XP_Skill_Info()
	{
		memset(this, 0, sizeof(*this));
	}
};

typedef std::vector<Single_Hero_XP_Skill_Info> heroXPSkillList;

struct Single_Hero_XP_Fragment_Info
{
	int nXPSkillID;									// XP技能id
	int nXPFragmentCount;							// XP技能碎片数量
	bool  operator == (const Single_Hero_XP_Fragment_Info &other) const
	{ 
		if (this->nXPSkillID == other.nXPSkillID)
		{
			return true;
		}

		return false;
	}

	Single_Hero_XP_Fragment_Info()
	{
		memset(this, 0, sizeof(*this));
	}
};

typedef std::vector<Single_Hero_XP_Fragment_Info> heroXPFragmentList;

struct Msg_Hero_XP_Skill_Unlock
{
	int nXPSkillID;										// XP技能id
	int nHeroID;

	Msg_Hero_XP_Skill_Unlock()
	{
		memset(this, 0, sizeof(*this));
	}
};

struct Msg_Hero_XP_Skill_Select
{
	int nXPSkillID;										// XP技能id
	int nHeroID;

	Msg_Hero_XP_Skill_Select()
	{
		memset(this, 0, sizeof(*this));
	}
};
// CS_MSG_ENTITY_ACTORNAME_CHANGE
// 删除指定符文页
struct msg_ActorName_Change
{
	char	szActorName[GAME_NAME_MAXSIZE];               // 角色名字
	int		nCostType;									  // 金币花费类型 (0点券1金币)	
	msg_ActorName_Change()
	{
		memset(this, 0, sizeof(*this));
	}
};

struct sRandHeroCard
{
	int heroID;				// 英雄ID
	int heroType;			// 英雄概率类型
	int matchNum;			// 英雄比赛场次
	int heroCardPro;		// 英雄卡牌概率
	int heroCardNum;		// 英雄卡牌数量

	sRandHeroCard()
	{
		memset(this, 0, sizeof(*this));
	}
};
// 皇冠系统
// CS_MSG_ENTITY_CROWN_SAVE
// 保存指定皇冠页
struct Msg_CrownPage_Save
{	
	int nPageDBID;											 // 皇冠页页码
	char szCrownPageName[GAME_NAME_MAXSIZE];				 // 该皇冠页名称
	int crownInlaySlotList[DBDEF_CROWN_PAGE_INLAYSlOT_MAX];  // 皇冠镶嵌列表		
	Msg_CrownPage_Save()
	{
		memset(this, 0, sizeof(*this));
	}
};
// CS_MSG_ENTITY_CROWN_DELETE
// 删除指定皇冠页
struct Msg_CrownPage_Delete
{
	int nPageDBID;									   // 皇冠页页码
	Msg_CrownPage_Delete()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 设置单页皇冠效果 CS_MSG_ENTITY_CROWNPAGEPOINTS_INDEX_EFFECT
struct Msg_Entity_CrownPage_Index_Effect
{
	int    nCurPageIndex;                              // 第几页皇冠页

	Msg_Entity_CrownPage_Index_Effect()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 请求指定玩家的所有符文情况 CS_MSG_ENTITY_CROWN_REQUEST_OTHER_INFO
struct Msg_Request_Other_Crown_Points
{
	DWORD	dwFriendID;           					   // 请求看到的实体uid
	Msg_Request_Other_Crown_Points()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 请求鉴定 CS_MSG_ENTITY_CROWN_REQUEST_OTHER_INFO
struct Msg_Appraisal_Crown_Gemstone
{
	int gemstoneDBIDList[EAGT_MAX];
	Msg_Appraisal_Crown_Gemstone()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 请求出售 CS_MSG_ENTITY_CROWN_REQUEST_OTHER_INFO
struct Msg_Seil_Crown_Gemstone
{
	int nGemstoneDBID;
	Msg_Seil_Crown_Gemstone()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 宝石产出选取原宝石 CS_MSG_ENTITY_CROWN_REQUEST_OTHER_INFO
struct Msg_Cobbleston_Selected_Crown_Gemstone
{
	int nGemstoneID;
	Msg_Cobbleston_Selected_Crown_Gemstone()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 宝石产出开窗选宝石 CS_MSG_ENTITY_CROWN_REQUEST_OTHER_INFO
struct Msg_WindowStone_Selected_Crown_Gemstone
{
	int nGemstoneID;
	int nSlotID;
	Msg_WindowStone_Selected_Crown_Gemstone()
	{
		memset(this, 0, sizeof(*this));
	}
};

/////////////////////////人物升级配置相关/////////////////////////////////////////////////

#define LEVELUPGRADE_SCHEME_FILENAME				"Scp\\LevelUpgrade.csv"
enum ENLVUPSCHEMECOL
{
	ENLVUPSCHEMECOL_LV = 0,						// 等级
	ENLVUPSCHEMECOL_EXP,						// 经验
	ENLVUPSCHEMECOL_ATTRIBUTION,                // 经验所属
	ENLVUPSCHEMECOL_EXPTYPE,                        // 经验类型
};

enum ENEXPTYPE
{
	ENEXPTYPE_WARHEROEXP = 0,                       // 占场内英雄经验
	ENEXPTYPE_STATICHEROEXP,                        // 战场外英雄经验
	ENEXPTYPE_STATICPLAYEREXP,                      // 玩家经验

	EXA_MAX,
};

// 升级配置信息(暂时没用先占个空位)
struct SLevelUpgradeScheme
{
	int nLv;                                // 等级
	int	nExp;								// 经验
	int nExpAttribution;                    // 经验所属
	int nExpType;                           // 经验类型
	SLevelUpgradeScheme()
	{
		memset(this, 0, sizeof(*this));
	}
};



/////////////////////////人物基础属性配置/////////////////////////////////////////////////
// 人物基础属性配置表
#define PERSON_BASE_PROP_SCHEME_FILENAME				"Scp\\PersonBaseProp.csv"
// 人物基础属性主城配置表
#define PERSON_BASE_PROP_HKEE_SCHEME_FILENAME			"Scp\\PersonBasePropHkee.csv"
// 人物基础属性数量
#define PERSON_BASE_PROP_COUNT		        12
//人物限制属性数量
#define PERSON_PROP_LIMIT_COUNT		        8
// 人物附加百分比属性数量
#define PERSON_APPEND_PCT_PROP_COUNT		1

enum PROP_TYPE
{
	// 基础属性
	BASE_PROP_TYPE = 0,
	// 主城属性
	BASE_PROP_HKEE_TYPE,
};

struct SPersonBaeProp
{
	int nPropID;		// 属性ID
	int nBase;			// 基础值
	float fGrowRate;	// 成长率
	SPersonBaeProp()
	{
		memset(this, 0, sizeof(*this));
	}
};
struct SPersonBaseLimit
{
	int nPropID;		// 属性ID
	int nMinLimit;		// 最小值下限
	int nMaxLimit;		// 最大值上限

	SPersonBaseLimit()
	{
		memset(this, 0, sizeof(*this));
	}
};
// 人物基础属性配置
struct SPersonBasePropScheme
{
	int				nVocation;							// 职业
	SPersonBaeProp	sProp[PERSON_BASE_PROP_COUNT];		// 基础属性
	int				nMagicType;							// 魔法类型
    int             nEnergyGetMode;                     // 能量获得/衰减方式
	SPersonBaseLimit sLimitProp[PERSON_PROP_LIMIT_COUNT];	// 限制属性
	int				nAimDistance;						// 瞄准距离
	SPersonBaeProp  sAppendPCTProp[PERSON_APPEND_PCT_PROP_COUNT];       // 附加百分比属性
	int				nAttackRange;						// 暂时给个距离

	SPersonBasePropScheme()
	{
		memset(this, 0, sizeof(*this));
	}
};


/////////////////////////实体通用配置/////////////////////////////////////////////////
// 人物基础属性配置表
#define ENTITY_SCHEME_FILENAME				"Scp\\Entity.xml"

// 实体复活信息
struct SEntityReliveInfo
{
	int			nSceneID;		// 复活到目标场景ID
	float		x;				// 复活X坐标
	float		y;				// 复活Y坐标
	float		z;				// 复活Z坐标
	float		fAngleY;		// 复活绕向Y朝向角度
	int			nActiveInterval;// 死亡激活复活操作时间长 ms
	DWORD		dwOperateTick;	// 当前操作的时间
	int			nDiePKType;		// 死亡时候的PK类型
	bool		bNoRelive;		// 不可以复活

	SEntityReliveInfo()
	{
		memset(this,0,sizeof(*this)); 
	}
};


/////////////////////////体力BUFF配置/////////////////////////////////////////////////
// 体力BUFF配置表
#define STAMINA_BUFF_SCHEME_FILENAME				"Scp\\StaminaBuff.csv"

struct SStaminaBuff
{
	int nVocation;		// 职业ID
	int nBuffID;		// buffID
	int nBuffLevel;		// buff等级

	SStaminaBuff()
	{
		memset(this, 0, sizeof(*this));
	}
};

/////////////////////////魔法类型配置/////////////////////////////////////////////////
// 魔法类型配置表
#define MAGIC_TYPE_SCHEME_FILENAME				"Scp\\MagicType.csv"

const char MagicTypeName[MAGIC_TYPE_MAX][MAGIC_NAME_LEN] = {"", "法术", "能量"};

struct SMagicType
{
	int		nType;				// 能量值增加衰减方式
	char	szName[MAGIC_NAME_LEN];			// 名字
	int		nProgressBarType;	// 进度条类型
	int		nLimitVale;			// 上限值
	int		nInitialValue;		// 初始值
	int		nHitValue;			// 击中增加值
	int		nKillMonsterValue;	// 击杀怪物增加值
	int		nKillRoleValue;		// 击杀英雄增加值
	int		nAssistValue;		// 助攻增加值
	int		nAutoGrowTime;		// 自动增长时间
	int		nAutoGrowValue;		// 自动增加值
	int		nAutoDecayTime;		// 自动衰减时间
	int		nAutoDecayValue;	// 自动衰减值
	int		nSecondaryAutoDecayTime;	// 二次自动衰减时间
	int		nSecondaryAutoDecayValue;	// 二次自动衰减值

	SMagicType()
	{
		memset(this, 0, sizeof(*this));
	}
};

/////////////////////////死亡光效配置/////////////////////////////////////////////////
// 死亡光效配置表
#define DIE_LIGHTING_SCHEME_FILENAME				"Scp\\DieLighting.csv"

struct SDieLightingKey
{
	int		nVocation;						// 职业ID，怪物ID
	int		nType;							// 实体类型

	bool operator<(const SDieLightingKey &other) const
	{
		if (nVocation != other.nVocation)
		{
			return nVocation < other.nVocation;
		}

		return nType < other.nType;
	}

	SDieLightingKey()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 死亡光效长度
#define DIE_LIGHTING_NAME_LEN   16
// 死亡光效长度
#define DIE_ANIMATION_NAME_LEN   16
struct SDieLighting
{
    char    szCoinLighting[DIE_LIGHTING_NAME_LEN];                  // 死亡贡献金币光效
	char	szName[DIE_LIGHTING_NAME_LEN];						    // 必播死亡光效
	int		nHideBody[DeadEffect_Max];								// 隐藏尸体
	char	szDielName[DeadEffect_Max][DIE_LIGHTING_NAME_LEN];		// 死亡光效
	int		nSoundID;												// 必播死亡光效
	int		nDieSoundID[DeadEffect_Max];							// 死亡音效ID
	char	szAnimationName[DIE_ANIMATION_NAME_LEN];							// 必播死亡动作
	char	szDielAnimationName[DeadEffect_Max][DIE_ANIMATION_NAME_LEN];		// 死亡动作

	SDieLighting()
	{
		memset(this, 0, sizeof(*this));
	}
};


// 技能升级脚本
#define SPELL_UPGRADE_SCHEME_FILENAME				"Scp\\SpellUpgrade.csv"

struct SSpellUpgrade
{
	int		nVocation;						// 职业ID
	int		nSlotIndex;						// 槽索引
	int		nSlotLevel;						// 槽等级
	int		nSpellID;						// 技能ID
	int		nLevel;							// 英雄等级

	SSpellUpgrade()
	{
		memset(this, 0, sizeof(*this));
	}
};

#define SUMMONER_SKILL_INFO                         "Scp\\SummonerSkill.csv"

enum SUMMONER_SCHEME_COLUMN
{
	SUMMONER_NAME = 0,                  // 召唤师技能名字
	SUMMONER_ID,                        // 召唤师天赋技能id
	SUMMONER_LV,                        // 激活技能需要的等级
	SUMMONER_ICONID,                    // 召唤师天赋技能图片id
	SUMMONER_DESC,                      // 召唤师技能描述
	SUMMONER_WARMODE,					// 适用战场模式
};

struct SSummonerSkillInfo
{
	char	        stName[SUMMONER_SKILL_NAME];          // 技能名字
	int             nId;                                  // 召唤师技能id
	int             nLv;                                  // 需求玩家等级
	int             nIconId;                              // 图片id
	char            stDesc[SUMMONER_SKILL_DESC];          // 技能描述
	SSummonerSkillInfo()
	{
		memset(this, 0, sizeof(*this) );
	}
};

// 转镜头场景数量
#define CAMERA_BUFF_COUNT  3
#define MAX_SPEED_UP_BUFF	4
#define MAX_FREE_EMOJI_COUNT 3
// 实体配置信息-服务器
struct SEntityXml_Server
{
	float			fMinFocoEnergiaPCT;                   // 最低蓄力百分比
	float			fMaxFocoEnergiaPCT;                   // 最高蓄力百分比
	float           fHeroBackwardSpeedFactor;             // 角色后退速度系数
	float           fHeroSidewaySpeedFactor;              // 角色左右移动速度系数
	int             nDamageInfo;                          // 显示伤害信息
	int             nChangeRankInfo;                      // 显示比赛分数改变信息
	int             nRecordInfo;                          // 是否在比赛过程中存储战绩表信息 0代表否， 1代表是
	int             nTime;                                // 间隔时间 单位ms
	int             nWarShowAddEXPAndMoney;               // 显示战场加钱加经验记录
	int             nFirstWinPlayerExp;                   // 首勝玩家經驗
	int             nFirstWinPlayerGold;                  // 首勝玩家金幣
	int             nFirstWinHeroExp;                     // 首勝英雄經驗
	int             nFirstWinCardCount;                   // 首勝卡牌數量
	int             nFirstWinPrizeID;                     // 首勝奖励ID

	int             nXPCdTime;                            // xp技能冷却时间
	int             nDeadIsDisappear;                     // 死亡是否消失 1代表不消失 0 代表消失
	int             nXPSkillBuffID;                       // xp技能BuffID
	int             nXPSkillID;							  // xp技能ID
	
	int             nSpellAlertOut;                       // 是否输出技能执行超时(0不输出技能执行超时,不为0输出)
	int             nSpellAlertOutTime;                   // 技能执行超时时间
	int             nSpellOutTime;                        // 技能超时时间丢弃技能
	int             nAddExpOrTalentLogTimeInterval;       // 增加经验和金钱log的时间间隔
	int             nIsShowPlanLog;				          // 是否服务器显示金币打印log(for策划)
	int             nAIMilitaryExploitsFlag;              // 机器人是否战绩炫耀标志位
    int             nTurnCameraBuffID[CAMERA_BUFF_COUNT]; // 新手引导转镜头加1级BUFF
	int             nMaxDayActivity;                      // 人物每天活跃度上限
    int             nMinPenetCelt;                        // 最低穿透系数
    int             nMatchTypeBaseScore;                  // 比赛类型基础分数

    int             nSpecificActorHideBuffID;               // 特殊角色隐身BUFFID
    int             nSpecificActorHideBuffLevel;            // 特殊角色隐身BUFF等级	

	int				pArrWarSpeedBuffIDs[MAX_SPEED_UP_BUFF];	// 加速BuffID
	int				nWarSpeedBuffLevel;						// 加速Buff等级

	int				nKillBaseBuffID;						// 杀死基地BuffID
	int				nKillFakeDeadPlayerBuffID;				// 杀死假死玩家BuffID
	int				nCureFakeDeadPlayerBuffID;				// 救治假死玩家BuffID

	int				nChangeVocSpellID;						// 改变职业技能ID
    int             nOpenMovePkgOptimize;                   // 开启移动包优化
	int				nArrFreeEmojiID[MAX_FREE_EMOJI_COUNT];	// 免费表情包列表	

	SEntityXml_Server()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 实体配置信息-客户端
struct SEntityXml_Client
{
	float			fSpellHeight;                         // 技能高度
	float           fHeroBackwardSpeedFactor;             // 角色后退速度系数
	float           fHeroSidewaySpeedFactor;              // 角色左右移动速度系数
	int             nDamageLightingID;                    // 伤害光效ID
	float           fContinueDamageLightingFactor;        // 持续伤害系数
	int             nContinueDamageLightingID;            // 持续伤害光效ID
	int             nMonsterDistance;                     // 怪物相关距离
	int             nTalentMaxUseCount;                   // 天赋最大可使用数
	int             nXPSkillTime;                         // xp技能充能时间
	int             nAIMilitaryExploitsFlag;              // 机器人是否战绩炫耀标志位 目前是1机器可以炫耀 0机器人不可以炫耀
	float			fWarEndWndDelayTime;				  // 战场结束后结束界面延迟显示时间(秒)
	int             nContactTaskSwitch;					  // 交互任务模块开关，后续功能稳定后会删除
    int             nMatchTypeBaseScore;                  // 比赛类型基础分数
    int             nOpenMovePkgOptimize;                 // 开启移动包优化
	SEntityXml_Client()
	{
		memset(this, 0, sizeof(*this));
	}
};


/////////////////////////翅膀配置/////////////////////////////////////////////////
// 翅膀配置表
#define WING_SCHEME_FILENAME				"Scp\\Wing.csv"

// 飞行需要类型
enum FlyNeedType
{
    FLY_NEED_STAMINA,            // 体力
    FLY_NEED_MP,                 // 魔法
    FLY_NEED_ENERGY,             // 能量
};

#define WING_BUFF_COUNT         3
struct SWing
{
	int		nID;				    // ID
	char	szName[32];			    // 名字
	char	szDes[128];			    // 描述
	char	szPath[GAME_TITLE_MAXSIZE];			// 预制体路径
	int     nIcon;                  // 图标
	float   fSpeed;                 // 速度
	float   fAcceleration;          // 加速度
	float   fMaxSpeed;              // 最高速度
	float   fUpSpeed;               // 上升速度
	float   fMinFlyHeight;          // 最小飞行高度
	float   fMaxFlyHeight;          // 最高飞行高度
	float   fMinCameraAngle;        // 镜头最小仰角
	float   fMaxCameraAngle;        // 镜头最大仰角
    int     nFlyNeedType;           // 飞行需要类型(FlyNeedType)
	int     nFlyNeedStamina;        // 起飞需要体力
	int     nFlyCostStamina;        // 飞行消耗体力
	int     nAccelerationNeedStamina;// 加速飞行需要体力
	int     nAccelerationCostStamina;// 加速飞行消耗体力
	int     nExitAccelerationStamina;// 退出加速飞行体力
	int     nFloorStamina;          // 降落体力
	int     nStartFlySpellID;       // 起飞技能ID
	int     nAllowAcceleration;     // 允许加速
	int     nAccelerationID;        // 加速技能ID
	int     nFlyType;               // 飞行类型
	int     nFlyTime;               // 飞行时间
	bool    bShowProgressBar;       // 是否显示进度条
    int     nReqFloorAddBuffID;     // 请求降落时给自己加buff
    int     nReqFloorAddBuffLevel;  // 请求降落时给自己加buff等级
    int     nEncounterDistance;     // 遭遇时自动降落,遭遇距离
	bool	bFloorOnBeHit;			// 被攻击时掉落
    int     nLoadWingBuffs[WING_BUFF_COUNT];       // 戴上翅膀附加BUFF
    int     nUnloadWingBuffs[WING_BUFF_COUNT];      // 卸装翅膀附加BUFF

	SWing()
	{
		memset(this, 0, sizeof(*this));
	}
};


/////////////////////////速度衰减配置/////////////////////////////////////////////////
// 速度衰减
#define SPEED_DECAY_SCHEME_FILENAME				"Scp\\SpeedDecay.csv"

struct SSpeedDecay
{
	int		nSpeedLower;				    // 速度下限
	int     nSpeedUpper;                    // 速度上限
	float   fDecayFactor;                   // 速度衰减系数
	int     nValue;                         // 速度值

	SSpeedDecay()
	{
		memset(this, 0, sizeof(*this));
	}
};

/////////////////////////PK配置/////////////////////////////////////////////////
// PK
#define PK_SCHEME_FILENAME				"Scp\\PlayerKill.csv"

struct SPlayerKill
{
	int		nCoolTime;                      // 发起切磋冷却时间(毫秒)
	int		nDistance;                      // 发起切磋范围要求
	int     nRequestValidTime;              // 请求切磋有效时间(毫秒)
	int     nRequestValidCount;             // 请求切磋有效数量
	int     nFightDistance;                 // 切磋范围
	int     nPrepareTime;                   // 准备时间(毫秒)
	int     nFightTime;                     // 切磋时间(毫秒)
	int     nMonsterID;                     // PK怪物(旗杆)
	int     nStartPKBuffID;                 // 开始PK增加BuffID
	int     nStartPKBuffLevel;              // 开始PK增加BuffLevel
	int     nEndPKBuffID;                   // 结束PK增加BuffID
	int     nEndPKBuffLevel;                // 结束PK增加BuffLevel

	SPlayerKill()
	{
		memset(this, 0, sizeof(*this));
	}
};

/////////////////////////皮肤技能配置/////////////////////////////////////////////////
#define SKIN_SPELL_SCHEME_FILENAME				"Scp\\SkinSpell.csv"

enum SKIN_SPELL_TYPE
{
    SKIN_SPELL_LIGHTING = 1,                // 光效类型
    SKIN_SPELL_SOUND,                       // 声音类型
    SKIN_SPELL_DEAD_TYPE,                   // 死亡类型
};

struct SSkinSpellKey
{
    int		nSkinID;                                    // 皮肤ID
    int     nType;                                      // 类型ID
    int     nValue;                                     // 数值

    SSkinSpellKey()
    {
        memset(this, 0, sizeof(*this));
    }

    // 重载小于号
    bool operator<(const SSkinSpellKey &other) const
    {
        if (nSkinID != other.nSkinID)
        {
            return nSkinID < other.nSkinID;
        }

        if (nType != other.nType)
        {
            return nType < other.nType;
        }

        return nValue < other.nValue;
    }
};

struct SSkinSpell
{
    int		nSkinID;                                    // 皮肤ID
    int     nType;                                      // 类型ID
    int     nOldValue;                                  // 旧数据
    int     nNewValue;                                  // 新数据

    SSkinSpell()
    {
        memset(this, 0, sizeof(*this));
    }
};

////////////消息处理//////////////////////////////////////////////////////////////
// 实体模块消息码定义
enum ENTITY_MESSAGE
{
	// 服务端->客户端
	SC_MSG_ENTITY_CREATE=1,		                    // 创建实体
	SC_MSG_ENTITY_DESTROY,						    // 实体销毁
	SC_MSG_ENTITY_APPEAR,                           // 实体出现
	SC_MSG_ENTITY_DISAPPEAR,                        // 实体消失
	SC_MSG_ENTITY_APPEAR_INSIGHT,        		    // 实现进入视野
	SC_MSG_ENTITY_MOVE,							    // 实体移动
	SC_MSG_ENTITY_CTRL,							    // 实体操作
	SC_MSG_ENTITY_UPDATEPROPS,					    // 更新某些属性
	SC_MSG_ENTITY_RELIVE,						    // 实体复活
	SC_MSG_ENTITY_DIE,							    // 实体死亡
	SC_MSG_ENTITY_TRANSPORT,					    // 实体传送
	SC_MSG_ENTITY_SWITCH_STATE,					    // 状态切换
	SC_MSG_ENTITY_STEALTH_CHANGE,                   // 实体隐形状态改变 
    SC_MSG_REPORT_ENTITY_LOC_LATER,                 // 通知客户端过段时间上报实体位置(动作决定位移)
	SC_MSG_ENTITY_MATCHTYP_RANK,                    // 人物所有撮合段位信息
	SC_MSG_ENTITY_MATCHTYP_SINGLE_RANK,             // 人物单独撮合合段位信息
	SC_MSG_ENTITY_MATCH_HEROSCORE,                  // 人物所有英雄熟练度信息
	SC_MSG_ENTITY_MATCH_SINGLE_HEROSCORE,           // 人物单独英雄数据更新
	SC_MSG_ENTITY_SUMMONER_SKILL,                   // 将更改的召唤师技能广播出去
	SC_MSG_ENTITY_TALENTPAGEPOINTS_ALL,             // 人物的所有天赋页加点更新

	SC_MSG_ENTITY_RUNE_SYNC,                        // 同步符文信息
	SC_MSG_ENTITY_OTHERRUNE_SYNC,                   // 同步其他人符文信息
	SC_MSG_ENTITY_RUNESTORE_UPTATE,                 // 更新符文商店
	SC_MSG_ENTITY_HEROTALENT_INFO,                  // 同步英雄天赋信息
	SC_MSG_ENTITY_HEROTALENT_UPTATE,                // 更新激活系统英雄天赋
	SC_MSG_ENTITY_HEROSKIN_INFO,                    // 同步英雄皮肤信息
	SC_MSG_ENTITY_HEROSKIN_UPTATE,                  // 更新英雄皮肤信息
	SC_MSG_ENTITY_RETURN_PLAYER_INFO,               // 返回玩家召唤师信息
	SC_MSG_ENTITY_RETURN_HERO_INFO,                 // 返回玩家已经拥有的英雄信息
	SC_MSG_ENTITY_UPDATE_HERO_INFO,                 // 更新英雄数据到客户端
	SC_MSG_ENTITY_UPDATE_PLAYER_INFO,               // 更新玩家数据到客户端
	SC_MSG_ENTITY_UPDATE_HERO_CARD,                 // 更新玩家英雄卡牌信息
	SC_MSG_ENTITY_UPGRADE_STAR_SUCESS,              // 升星成功返回给客户端
	SC_MSG_ENTITY_WARENDAWARK_RESULT,               // 战场结束将奖励信息发送到客户端
	SC_MSG_ENTITY_STATICHERO_UPGRADE,               // 静态场景英雄升级事件
	SC_MSG_ENTITY_LOGIN_SENDDATA,                   // 登录时bank部件向客户端传递一些数据
	SC_MSG_ENTITY_TIMELIMIT_GOODUPDATE,             // 同步快过期物品(皮肤)
	SC_MSG_ENTITY_UPDATE_CHEST_INFO,                // 更新宝箱数据
	SC_MSG_ENTITY_RETURN_CHEST_RANDCARD_INFO,		// 返回宝箱随机卡牌数据
	SC_MSG_ENTITY_OPENCHEST_RESULT,	                // 打开宝箱结果
	SC_MSG_ENTITY_POINTSHOP_UPDATE_BOUGHTNUM,		// 更新特定商品已购买数量
	SC_MSG_ENTITY_POINTSHOP_BUYRESULT,				// 商城购买结果
	SC_MSG_ENTITY_POINTSHOP_UPDATE_CARDLIST,		// 卡牌商城更新列表
	SC_MSG_ENTITY_POINTSHOP_REFLUSH_BOUGHTNUM,		// 刷新特定商品已购买数量
	SC_MSG_ENTITY_PROPERTY_RETURN_DRAGONBALL,		// 龙珠指定实体信息返回
	SC_MSG_ENTITY_RENMAE_RESULT,                    // 同步改名字结果
	SC_MSG_ENTITY_BENEFITCARD_INFO,					// 增益卡信息
	SC_MSG_ENTITY_CROWNALLGEMSTONE_UPDATE,			// 初始数据全部宝石更新
	SC_MSG_ENTITY_CROWNALLCROWNPAGE_UPDATE,			// 初始数据全部皇冠页更新
	SC_MSG_ENTITY_CROWNGEMSTONE_UPDATE,				// 宝石变化同步
	SC_MSG_ENTITY_CROWNGEMSTONE_PRODUCE,			// 宝石产出同步
	SC_MSG_ENTITY_HERO_XP_SKILL_UPDATE,				// 玩家英雄xp技能同步
	SC_MSG_ENTITY_HERO_XP_FRAGMENT_UPDATE,		    // 玩家英雄xp技能碎片同步
	SC_MSG_ENTITY_HEROUPSTAR_UNLOCK,			    // 英雄升星奖励解锁

	SC_MSG_ENTITY_NPC_OPEN_SHOP,					// 打开NPC商店
	SC_MSG_ENTITY_NPC_RESPONSE,						// 触发NPC后返回
	SC_MSG_ENTITY_NPC_CLOSE_DIALOGBOX,				// 关闭NPC对话框
	SC_MSG_ENTITY_SYNC_DATA,						// 同步一些数据到客户端
	SC_MSG_ENTITY_CROWNGEMSTONE_APPRAISAL_RET,		// 宝石鉴定结果
	SC_MSG_ENTITY_NOVICEGUIDE_DATA,                 // 新手引导数据
	SC_MSG_ENTITY_LABEL_LIST,						// 标签数据
    SC_MSG_ENTITY_SEND_NAVGROUP,					// 发送导航到客户端
    SC_MSG_ENTITY_SEND_WEBURL,						// 发送WebUrl到客户端


	// 客户端->服务器
	CS_MSG_CREATURE_SYNC,						    // 实体同步
	CS_MSG_CREATURE_CTRL,						    // 操控实体
	CS_MSG_CREATURE_LAND,						    // 实体着陆
	CS_MSG_ENTITY_RELIVE,						    // 实体复活
	CS_MSG_ENTITY_TEST,							    
	CS_MSG_ENTITY_TEST_RELIVE,                      // 七龙珠的实体复活
	CS_MSG_ENTITY_MATCHTYP_RANK,                    // 人物撮合段位信息
	CS_MSG_ENTITY_MATCH_HEROSCORE,                  // 人物英雄熟练度信息
	CS_MSG_ENTITY_SUMMONER_SKILL,                   // 召唤师技能信息
	CS_MSG_ENTITY_TALENTPAGEPOINTS_SET_SINGLE,      // 保存单页天赋页的加点数据
	CS_MSG_ENTITY_TALENTPAGEPOINTS_INDEX_EFFECT,    // 告诉服务器最后调用哪页的效果
	CS_MSG_ENTITY_TALENTPAGEPOINTS_INDEX_DELETE,    // 删除某页

	CS_MSG_ENTITY_RUNE_INDEX_EFFECT,                // 告诉服务器最后调用哪页的效果
	CS_MSG_ENTITY_RUNE_SAVE,                        // 客户端请求保存符文页
	CS_MSG_ENTITY_RUNE_DELETE,                      // 客户端请求删除符文页
	CS_MSG_ENTITY_RUNE_REQUEST_OTHER_INFO,		    // 客户端查询其他玩家符文信息

	CS_MSG_ENTITY_REQ_PLAYER_INFO,                  // 客户端请求召唤师信息
	CS_MSG_ENTITY_REQ_HERO_INFO,					// 客户端请求召唤师英雄信息
	CS_MSG_ENTITY_REQ_UPDATE_HERO_STAR,             // 客户端请求升星
	CS_MSG_ENTITY_AOI_SYNC_INFO,                    // 客户端同步AOI信息到服务器
	CS_MSG_REPORT_AI_LOC,                           // 上报实体(假人)位置
	CS_MSG_ENTITY_CHANGE_HERO,                      // 切换英雄
	CS_MSG_ENTITY_OPEN_CHEST,                       // 打开宝箱
	CS_MSG_ENTITY_BROADCAST_CHEST_RESULT,			// 广播开宝箱结果
	CS_MSG_ENTITY_POINTSHOP_BUYGOODS,				// 请求购买商城物品
	CS_MSG_ENTITY_PROPERTY_REQ_DRAGONBALL,			// 龙珠指定实体信息请求
	CS_MSG_ENTITY_PROPERTY_REQ_DRAGONBALL_CLOSE,	// 龙珠指定实体信息关闭
	CS_MSG_ENTITY_ACTORNAME_FIX,					// 角色名字修改
	CS_MSG_ENTITY_CROWNAGEMSTONEPPRAISAL,			// 宝石鉴定
	CS_MSG_ENTITY_CROWNSAVEPAGE,					// 保存皇冠页
	CS_MSG_ENTITY_CROWNDELETEPAGE,					// 删除皇冠页
	CS_MSG_ENTITY_CROWNDROOMSELECTPAGE,				// 大厅选择皇冠页
	CS_MSG_ENTITY_CROWNDCOBBLESTON_SELECTED,		// 选择原石
	CS_MSG_ENTITY_CROWNDWINDOWSTONE,				// 花钱开窗
	CS_MSG_ENTITY_CROWNDWINDOWSTONE_SELECTED,		// 选择开窗原石
	CS_MSG_ENTITY_CROWNDGEMSTONE_SEIL,				// 宝石出售
	CS_MSG_ENTITY_UNLOCK_XP_SKILL_BYFRAGMENT,		// 客户端碎片集齐解锁XP
	CS_MSG_ENTITY_SELECT_XP_SKILL,					// 大厅动态点选英雄xp技能
	CS_MSG_ENTITY_NPC_TRIGGER,						// 玩家触发NPC
	CS_MSG_ENTITY_NPC_FUNC,							// 玩家点击NPC功能
	CS_MSG_ENTITY_SET_NOVICEGUIDE,					// 玩家设置新手引导类型
	CS_MSG_ENTITY_SET_FORMTYPE,					    // 玩家设置立足点类型
	CS_MSG_ENTITY_TURN_CAMERA,					    // 玩家转镜头
    CS_MSG_ENTITY_REQUEST_NAVGROUP,                 // 请求发送导航
    CS_MSG_ENTITY_REQUEST_WEBURL,                   // 请求发送WebURL

    CS_MSG_FOLLOW_ME,                               // 请求别人跟随自己
    CS_MSG_BREAK_FOLLOW_ME,                         // 取消别人跟随自己

	// 服务器->服务器
	SS_MSG_ENTITY_IN_SIGHT,						// 通知进入视野
	SS_MSG_ENTITY_OUT_OF_SIGHT,                 // 通知离开视野
	SS_MSG_ENTITY_CREATE_NOTIFY,	    		// 向某个玩家或者某个阵营通知自己创建的消息
	SS_MSG_ENTITY_DESTROY_NOTIFY,               // 向某个玩家或者某个阵营通知自己消失的消息
	SS_MSG_ENTITY_STOP_MOVE,					// 通知停止移动
	SS_MSG_ENTITY_FOREST_CHANGE,                // 通知进出草丛 
    SS_MSG_ENTITY_PROPERTY_CHANGE,              // 通知属性改变
    SS_MSG_ENTITY_UPDATE_EXTRACT_TICKET,        // 通知提取点券
	SS_MSG_ACTOR_LOGOUT,                        // 角色登出

	// AI消息
	SS_MSG_AI_CLOSE_TO,                         // 靠近目标
	SS_MSG_AI_MOVE,								// 移动
    SS_MSG_AI_JUMP_TO,                          // 跳向目标
	SS_MSG_AI_STOP_MOVE,                        // 停止移动
	SS_MSG_AI_CAST_SPELL,                       // 使用技能
	SS_MSG_ENITYT_LOOK_AT_TARGET,               // 朝向目标
	SS_MSG_AI_ADD_BUFF,                         // ai加buff
    SS_MSG_AI_PICKED_UP,                        // 请求捡起自己
    SS_MSG_AI_RESET_GOODS,                      // 重置AI物品

    SS_MSG_FORBID_MOVE_INITIATIVELY,            // 禁止主动移动(被嘲讽或者被拖拽)

    SC_MSG_FORBIT_MOVE_INITIATIVELY,            // 通知客户端禁止主动移动(被嘲讽或者被拖拽)

	CS_MSG_ENTITY_REFLUSH_CARD,					// 刷新勋章
    CS_MSG_RIGHTBUTTON_DOWN,                    // 鼠标右键按下

	// 本命英雄相关消息
	CS_MSG_ENTITY_LIFEHERO_ADD,                 // 添加本命英雄
	CS_MSG_ENTITY_LIFEHERO_DEL,                 // 删除本命英雄
	CS_MSG_ENTITY_LIFEHERO_REPLACE,             // 替换本命英雄
	CS_MSG_ENTITY_LIFEHERO_EXCHANGE,            // 交换本命英雄培养仓
	CS_MSG_ENTITY_LIFEHERO_QUERY,               // 获取本命英雄列表
	SC_MSG_ENTITY_LIFEHERO_QUERY_RESULT,        // 发送本命英雄列表到客户端
	SC_MSG_ENTITY_LIFEHERO_TASKDATA,            // 获取本命英雄任务数据
	SC_MSG_ENTITY_LIFEHERO_UPDATE,              // 更新单个本命英雄

	SC_MSG_ENTITY_ACTOR_UNLOCK_RESULT,          // 角色升级奖励

	CS_MSG_ENTITY_GAMESTATE_LEAVE,				// 玩家游戏状态是否离开
	SC_MSG_ENTITY_LIFEHERO_PRIZE_TASKDATA,      // 获取本命英雄任务数据
	CS_MSG_ENTITY_LIFEHERO_OBTAIN_PRIZE,        // 本命英雄领取奖励

    SC_MSG_ENTITY_NPC_BUY_SUCCESS,              // NPC购买装备成功

	CS_MSG_ENTITY_WAR_CHANGE_VOCATION,			// 战场切换职业

	CS_MSG_ENTITY_REQ_SEASON_DETAIL,			// 请求排位赛季基础数据
	SC_MSG_ENTITY_RET_SEASON_DETAIL,			// 返回排位赛季基础数据

	CS_MSG_ENTITY_REQ_SEASON_RECORD,			// 请求排位赛季历史数据
	SC_MSG_ENTITY_RET_SEASON_RECORD,			// 返回排位赛季历史数据

	SC_MSG_ENTITY_UPDATE_EMOJI_INFO,            // 更新表情包数据

    CS_MSG_ENTITY_REQ_RECOMMEND_DATA,           // 请求推广员奖励任务数据和总局数
    SC_MSG_ENTITY_RET_RECOMMEND_DATA,           // 返回推广员奖励任务数据和总局数
    CS_MSG_ENTITY_REQ_GAIN_RECOMMPRIZE,         // 请求领取推广奖励
    SC_MSG_ENTITY_GAIN_RECOMMPRIZE_SUCCESS,     // 请求领取推广奖励成功
};

// 实体类统一用子头来用于转向部件
struct SMsgEntityHead
{
	UID				uidMaster;					// 实体UID
	uint8			byPartID;					// 部件ID，ENTITY_PART_MAX时将由实体来处理

#ifdef ENTITY_MESSAGE_ROUTE_TIME_HACK
	/* 用于探测实体在各个端之间的网络消息时间 */ 
	unsigned long	dwServerTickCS;				// 应用服时间C->S
	unsigned long	dwGateConnectTickCS;		// 网关连接器时间C->S
	unsigned long	dwGateWayTickCS;			// 网关服时间C->S
	unsigned long	dwServerTickSC;				// 应用服时间S->C
	unsigned long	dwGateConnectTickSC;		// 网关连接器时间S->C
	unsigned long	dwGateWayTickSC;			// 网关服时间S->C
#endif

	SMsgEntityHead(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

// SC_MSG_CREATE_ENTITY
// 实体要导出的内容
struct SEntiyExportShadowContext
{
	BYTE		byType;			// 实体类型
	BYTE		byFlag;			// 实体类型 SERIALIZE_FLAG
	BYTE		byCurState;		// 实体状态 ENTITY_STATE
	BYTE        byReserve;      // 保留数据，用于扩展用途
	Vector3		loc;			// 实体坐标
	float		fAngleY;		// 角色出生时的绕Y朝向角度    

	SEntiyExportShadowContext(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

// SS_MSG_ENTITY_STOP_MOVE
// 停止移动
struct msg_creatrue_move_stop
{
	Vector3 ptNewLoc;				// 被击退者停止点
	bool    bSync;                  // 同步

	msg_creatrue_move_stop()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 移动包优化头
struct msg_loc_sync_pkg_optimize_head
{
    WORD wPkgSeq;           // 包序号
    WORD wPrecondPkgSeq;    // 前置包序号

    msg_loc_sync_pkg_optimize_head()
    {
        memset(this, 0, sizeof(*this));
    }
};

// CS_MSG_CREATURE_SYNC(当客户端移动状态改变时发，客户端自己寻路，服务器只检测)
// SC_MSG_ENTITY_MOVE
struct msg_creature_move_sync
{
	UID     uid;            
    DWORD   dwTick;
    int16   nPosX;  // 扩大了10倍
    int16   nPosY;  // 扩大了10倍
    int16   nPosZ;  // 扩大了10倍

    int16   nRotX;  // 扩大了10倍
    int16   nRotY;  // 扩大了10倍
    int16   nRotZ;  // 扩大了10倍

    int16   nVelocityX; // 扩大了10倍
    int16   nVelocityY; // 扩大了10倍
    int16   nVelocityZ; // 扩大了10倍


    BYTE    direction; // WALK_DIRECTION:1前进，2后退

    BYTE    nCreaturState;   // 客户端当前状态机

    BYTE    nBehaviour;

	// Behaviour对象数据

	msg_creature_move_sync()
	{
		memset(this, 0, sizeof(*this));
	}
};
enum WALK_DIRECTION
{
	WALK_NONE = 0,
	WALK_FORWARD,
	WALK_BACK,
};

//SC_MSG_ENTITY_TRANSPORT
struct msg_creature_transport_sync 
{
	int         nSceneSrc;  // 起始场景id
	Vector3		ptSrc;		// 起始位置
	int			nSceneDst;	// 目标场景id
	int         nMapDst;    // 目标地图id
	Move3		ptDes;		// 目标位置
	int         nReason;    // TRANSPORT_REASON
};

struct msg_creature_state_sync
{
	int32   nPostureID;		// 当前的状态	
	int     bIsMoving;		// 是否运动中
	float   fForward;		// 向前状态控制
	float   fLeft;			// 方向
	int     bJump;			// 是否正在跳跃
	int     bFall;			// 是否正在下落
};

// SC_MSG_ENTITY_CTRL
struct msg_creature_act_sync
{
	Move3 move;
	msg_creature_state_sync     state;

	int32   nCtrID;			// 操作码
	int32   nCtrlParam;     // 操作参数
	int32   nCtrlParam2;    // 操作参数
	Vector3 fCtrParam;		// 操作参数,比如技能方向

	msg_creature_act_sync()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 实体着陆 CS_MSG_CREATURE_LAND
struct msg_creature_act_land
{
	float   fValue;			// 高度值

	msg_creature_act_land()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 玩家设置立足点类型 CS_MSG_ENTITY_SET_FORMTYPE
struct msg_creature_act_set_formtype
{
    int   nValue;

    msg_creature_act_set_formtype()
    {
        memset(this, 0, sizeof(*this));
    }
};

// 更新属性				SC_MSG_ENTITY_UPDATEPROP
struct msg_entity_updateprop
{
	short	nPropertyID;	// 更新的属性ID
	short	nDataLen;		// 属性数值长度
	// void*	data;		// 属性具体数值

	msg_entity_updateprop()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 更新某些属性			SC_MSG_ENTITY_UPDATEPROPS
struct msg_entity_updateprops
{
	BYTE	propnum;		// 更新的属性个数
	/*
	BYTE	id;				// 属性ID
	BYTE	changedTag;		// 属性更新标识
	short	nDataLen;		// 属性数值长度
	BYTE	data[1];		// 属性具体数值
	*/
	msg_entity_updateprops()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 实体复活	SC_MSG_ENTITY_RELIVE
// 实体死亡	SC_MSG_ENTITY_DIE						
struct msg_entity_state_info
{
	BYTE	byState;		// 状态
	Vector3	v;				// 坐标
	int		nData;			// 附加数据
	UID		uidOperator;    // 击杀者id
    int     nDeadType;
	bool	bNoRelive;	// 是否可以复活

	msg_entity_state_info()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 七龙珠请求				
struct msg_entity_smart_test
{
	int		nStrLen;		// 长度
	// 内容 char*nStrLen

	msg_entity_smart_test()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 获取实体属性			CS_MSG_ENTITY_GET_PROP
struct msg_entity_get_prop
{
	UID		uid;
	BYTE	propnum;		// 需要获取的属性个数
	// BYTE*				// 属性ID列表

	msg_entity_get_prop()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 请求显示指定实体			CS_MSG_ENTITY_REQ_DISPLAY
struct msg_entity_req_display
{
	UID		uid;            // 请求看到的实体uid
	msg_entity_req_display()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 请求指定玩家的Rank数据 CS_MSG_ENTITY_MATCHTYP_RANK
struct msg_entity_matchtype_rank
{
	bool    bRequestSelf;
	DWORD	dwFriendID;            // 请求看到的实体uid

	msg_entity_matchtype_rank()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 请求指定玩家的英雄熟练度 CS_MSG_ENTITY_MATCH_HEROSCORE
struct msg_entity_match_heroscore
{
	bool    bRequestSelf;
	DWORD	dwFriendID;            // 请求看到的实体uid

	msg_entity_match_heroscore()
	{
		memset(this, 0, sizeof(*this));
	}
};

////////////////////////////////////天赋页/////////////////////////////////////
// 天赋页开关
//#define FUNCTION_TALENTPAGE_OPEN

#ifdef FUNCTION_TALENTPAGE_OPEN
// 请求指定玩家的所有天赋页加点情况 CS_MSG_ENTITY_TALENTPAGEPOINTS_REQUEST_ALL
struct msg_Entity_TalentPage_Points_Request_All
{
	bool    bRequestSelf;
	DWORD	dwFriendID;            // 请求看到的实体uid

	msg_Entity_TalentPage_Points_Request_All()
	{
		memset(this, 0, sizeof(*this));
	}
};
// 设置单页天赋页加点情况 CS_MSG_ENTITY_TALENTPAGEPOINTS_SET_SINGLE
struct msg_Entity_TalentPage_Points_Set_Single
{
	char	szTalentPageName[MAX_TALENTNAME_LEN];	       // 该天赋页名称
	int     nCurPageIndex;                                 // 第几页天赋页
	int     nPageTalentNum;                                // 当前天赋页的天赋个数
	msg_Entity_TalentPage_Points_Set_Single()
	{
		memset(this, 0, sizeof(*this));
	}
};

struct Entity_TalentPage_Points
{
	msg_Entity_TalentPage_Points_Set_Single talentBaseInfo;
	TALENT_INFO_VECTOR  vTalentInfoVec;             // 当前页的天赋加点信息
	Entity_TalentPage_Points()
	{
		vTalentInfoVec.clear();
	}
};
// 设置单页天赋页效果 CS_MSG_ENTITY_TALENTPAGEPOINTS_INDEX_EFFECT
struct msg_Entity_TalentPage_Index_Effect
{
	int    nCurPageIndex;                                // 第几页天赋页

	msg_Entity_TalentPage_Index_Effect()
	{
		memset(this, 0, sizeof(*this));
	}
};
// 设置删除单页天赋页情况 CS_MSG_ENTITY_TALENTPAGEPOINTS_INDEX_DELETE
struct msg_Entity_TalentPage_Index_Delete
{
	int    nCurPageIndex;                                // 第几页天赋页

	msg_Entity_TalentPage_Index_Delete()
	{
		memset(this, 0, sizeof(*this));
	}
};
////////////////////////////////////////////////////////////////////////////
#endif

// 请求指定玩家的Rank数据 SC_MSG_ENTITY_MATCHTYP_SINGLE_RANK
struct msg_entity_match_single_rank
{
	bool    bRiseGrade;
	bool    bFallGrade;

    int     nPreGrade;
    int     nPreScore;
	MatchTypeRankNode RankNodeInfo;

	msg_entity_match_single_rank()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 	CS_MSG_ENTITY_REQ_PLAYER_INFO 客户端请求召唤师信息
struct msg_req_player_info
{
	DWORD	dwActorID;            // 请求看到的实体uid
    bool    bFake;              // 是否要造假数据
	msg_req_player_info()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 	CS_MSG_ENTITY_REQ_HERO_INFO 客户端请求召唤师英雄信息
struct msg_req_hero_info
{
	DWORD	dwActorID;            // 请求看到的实体uid
    bool    bFake;                // 是否假人
	msg_req_hero_info()
	{
		memset(this, 0, sizeof(*this));
	}
};

// SC_MSG_ENTITY_RETURN_PLAYER_INFO  返回召唤师信息
struct msg_entity_player_info_return
{
    bool    bFake;                              // 是否假人
	PDBID   pDbid;                              // 玩家pDbid
	int     nLv;                                // 英雄等级
	WORD    nHeadID;                            // 头像Id
	WORD	nHeroID;                            // 静态英雄ID: 这里的nHeroID和SHeroInfo.nHeroID没有任何关系
	WORD	nSkinID;                            // 静态皮肤ID   ****(策划提出可能和英雄皮肤不一样是单独的一个皮肤， 之后还可能是表情高低可以改变的情况 策划说不会这么搞)*****
	char    chClanName[GAME_NAME_MAXSIZE];      // 工会名字
	char    chClanPos[GAME_NAME_MAXSIZE];       // 工会爵位
	char    chKinName[GAME_NAME_MAXSIZE];       // 战队名字
	char	chActorName[GAME_NAME_MAXSIZE];		// 召唤师名称
	DWORD   dwGlory[ENACHIEVE_MAX];             // 个人荣耀
	LONGLONG llWarID[ENACHIEVE_MAX];			// 个人荣耀对应战场ID
	DWORD   dwFighting;                         // 战斗力
	DWORD   dwGloryScore;                       // 荣耀积分
	DWORD   dwCurrentExp;                       // 当前经验
	DWORD	nRankScore;							// 排位得分
    DWORD   nRankGrade;                         // 排位等级
	DWORD	dwPKWinNum;							// PK胜利场次
	int     nCount;                             // 英雄数量
	int		nSupportNum;						// 点赞数量
	DWORD	dwPKTotalNum;						// PK总场次
    DWORD   dwRankMatchNum;                    // 排位比赛次数
    WORD    wRankWinNum;                       // 排位胜利场次
	bool	bSex;								// 性别

	// nCount * msg_entity_hero_info
	msg_entity_player_info_return()
	{
		memset(this, 0, sizeof(*this) );
	}
};

// 一个英雄的基本信息
struct msg_entity_hero_info
{
	int     nHeroID;                    // 英雄id
	int     nLv;                        // 英雄等级
	DWORD   dwExp;                      // 英雄经验
	BYTE    byStarLv;                   // 星级
	WORD    nWinPro;                    // 胜率
	DWORD   dwGameCount;                // 比赛的场次
	DWORD   dwScore;                    // 英雄评分
	int     nRank;                      // 英雄排名
	DWORD   dwUseTime;                  // 最后一次使用时间
	bool	bIsBuy;						// 是否已购买
	int		nUseXPSkillID;				// xp技能id
	DWORD	dwUnlockTime;				// 解锁时间
	msg_entity_hero_info()
	{
		memset(this, 0, sizeof(*this) );
	}
};

//------------- 本命英雄相关结构 ----------------
// 本命英雄相关数据
struct SLifeHero
{
	int     nHeroID;                                        // 英雄ID
	int     nCultureID;                                     // 培养仓ID
	SLifeHero()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 本命英雄信息
struct msg_entity_lifehero_info
{
	int     nHeroID;                                        // 英雄ID
	int     nCultureID;                                     // 培养仓ID
	msg_entity_lifehero_info()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 删除指定英雄ID的本命英雄
struct msg_entity_lifehero_del
{
	int heroID;
	msg_entity_lifehero_del()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 替换新的英雄
struct msg_entity_lifehero_replace
{
	int heroID;
	msg_entity_lifehero_info hero;
	msg_entity_lifehero_replace()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 两个英雄互换培养仓
struct msg_entity_lifehero_exchange
{
	int heroID1;
	int heroID2;
	msg_entity_lifehero_exchange()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 查找所有本命英雄
struct msg_entity_lifehero_query
{
	DWORD actorID;
	msg_entity_lifehero_query()
	{
		memset(this, 0, sizeof(*this));
	}
};

//-------------- END -------------------------

struct msg_entity_other_actor_info_all
{
	int queryType;
	int loadState;
	msg_entity_player_info_return playerInfo;
	vector<SHeroInfo> heroInfoList;				// 英雄战绩列表

	msg_entity_other_actor_info_all()
	{
		queryType = 0;
		loadState = 0;
		heroInfoList.clear();
	}
};

// 实体看向锁定目标 SC_MSG_ENTITY_LOOKAT
struct msg_entity_look_at
{
	UID uidTarget;
	msg_entity_look_at()
	{
		memset(this, 0, sizeof(*this) );
	}
};

// SC_MSG_ENTITY_WARENDAWARK_RESULT 战场结束将奖励信息发送到客户端
struct msg_entity_warend_awark
{
	LONGLONG lDBWarID;          // 战场唯一id
	int     nHeroID;            // 英雄id
	bool    bIsWin;             // 是否胜利
	int     nActorGetExp;       // 获得角色经验
	int     nActorCurExp;       // 当前角色经验
	int     nActorOldExp;       // 以前角色经验
	int     nHeroExp;           // 英雄获得经验
	int     nGold;              // 获得绑定金币
	int     nSuccessScore;      // 成就得分
	DWORD   dwFirstWinTime;     // 首胜时间
	int     nHeroOldExp;        // 英雄以前的经验
	int     nHeroOldLv;         // 英雄以前的英雄等级
	int     nCurrentExp;        // 英雄当前经验
	int     nCurrentLv;         // 英雄当前等级
	int     nPlayerOldLv;       // 玩家旧等级
	int     nPlayerNewLv;       // 玩家新等级
	bool    bIsFirstWinAwark;   // 是否是首胜奖励
	int		nAntiReduceRate;	// 防沉迷衰减速率

	int     nCount;         // 英雄类型的数量
	// nCount * msg_entity_hero_card
	msg_entity_warend_awark()
	{
		memset(this, 0, sizeof(*this));
	}
};

struct msg_entity_hero_card
{
	int nHeroID;
	int nCount;
	msg_entity_hero_card()
	{
		memset(this, 0, sizeof(*this));
	}
};

//SC_MSG_ENTITY_STATICHERO_UPGRADE  静态场景升级事件
struct msg_entity_static_hero_upgrade
{
	int nHeroID;
	int nCurrentStarLv;
	int nFrontLv;
	int nCurrntLv;
	int nCurrntExp;
	msg_entity_static_hero_upgrade()
	{
		memset( this, 0, sizeof(*this));
	}
};

// 玩家数据包
#define FIRSTWINTIME_INTERVAL       22*60*60
struct SActorImagePack
{
	DWORD dwFirstWinTime;       // 0 代表有首胜奖励，  其他代表没有首胜

	SActorImagePack()
	{
		memset(this, 0, sizeof(*this));
	}
};


// SC_MSG_REPORT_ENTITY_LOC_LATER
struct msg_request_report_loc_later
{
	unsigned int uSerial;

	msg_request_report_loc_later()
	{
		memset(this, 0, sizeof(*this));
	}
};

// CS_MSG_REPORT_ENTITY_LOC
struct msg_report_ai_loc
{
	unsigned int uSerial;
	Move3 moveState;

	msg_report_ai_loc() : uSerial(0)
	{
	}
};

// SC_MSG_ENTITY_BUYHERO_RESULT
struct msg_buyhero_result
{
	int nHeroID;
	int nHeroStaticLv;      // 英雄静态等级
	BYTE byHeroStarLv;      // 英雄星级
	int nExpendGold;        // 消耗的金币
	bool bResult;           //结果类型
	msg_buyhero_result()
	{
		memset( this, 0, sizeof(*this));
	}
};

// SC_MSG_ENTITY_UPGRADE_STAR_SUCESS
struct msg_up_star_sucess
{
	int nHeroID;
	bool bIsSucess;
	int nCurrentStar;
	//int nCurrentBindGold;
	int nCurrentGold;
	int nCurrentCardCount;
	msg_up_star_sucess()
	{
		memset( this, 0, sizeof(*this));
	}
};

// CS_MSG_ENTITY_AOI_SYNC_INFO,          客户端同步AOI信息到服务器
struct msg_entity_aoi_sync_info
{
	bool isGodMode;             // 是否为上帝模式
	Vector3 vLookLoc;           // 观察点

	msg_entity_aoi_sync_info()
	{
		memset(this, 0, sizeof(*this));
	}
};

//CS_MSG_ENTITY_CHANGE_HERO,            切换英雄信息（英雄、皮肤）
struct msg_entity_change_hero
{
	int nOptType;         // 当前操作类型:0.切英雄（带皮肤ID），1.切皮肤(带英雄ID)
	int nChangeID;        // 英雄/皮肤ID
    int nReserveData;     // 备用数据

	msg_entity_change_hero()
	{
		memset(this, 0, sizeof(*this));
	}
};

// CS_MSG_ENTITY_OPEN_CHEST,            打开宝箱
struct msg_entity_open_chest
{
	int nChestType;			// 宝箱类型

	msg_entity_open_chest()
	{
		memset(this, 0, sizeof(*this));
	}
};

// SC_MSG_ENTITY_OPENCHEST_RESULT		打开宝箱结果
struct msg_entity_open_chest_result
{
	int nChestType;			// 宝箱类型
	int nPrizeID[PRIZEGROUP_RESULT_MAX];		// 奖励ID

	msg_entity_open_chest_result()
	{
		memset(this, 0, sizeof(*this));
	}
};

// SC_MSG_ENTITY_POINTSHOP_UPDATE_BOUGHTNUM
struct msg_entity_pointshop_update_boughtnum
{
	int nCount;
	// nCount * sizeof(msg_entity_pointshop_update_boughtnum_sub)

	msg_entity_pointshop_update_boughtnum()
	{
		memset(this, 0, sizeof(*this));
	}
};

struct msg_entity_pointshop_update_boughtnum_sub
{
	int nSellID;			// 出售ID
	int nBoughtNum;			// 已购买数量

	msg_entity_pointshop_update_boughtnum_sub()
	{
		memset(this, 0, sizeof(*this));
	}
};

// CS_MSG_ENTITY_POINTSHOP_BUYGOODS
struct msg_entity_pointshop_buygoods
{
	int nSellID;			// 出售ID
	int nResType;			// 资源类型 ENPOINTSHOPRESTYPE
	int nClientPrice;		// 客户端价格
	int nNum;				// 购买数量

	msg_entity_pointshop_buygoods()
	{
		memset(this, 0, sizeof(*this));
	}
};

// SC_MSG_ENTITY_POINTSHOP_BUYRESULT
struct msg_entity_pointshop_buyresult
{
	int nSucceed;

	msg_entity_pointshop_buyresult()
	{
		memset(this, 0, sizeof(*this));
	}
};

// SC_MSG_ENTITY_POINTSHOP_UPDATE_CARDLIST
struct msg_entity_pointshop_update_cardlist
{
	DWORD nReflushLeftTime;
	int nReflushLeftNum;
	int nSellID[POINTSHOP_CARDLIST_MAXCOUNT];

	msg_entity_pointshop_update_cardlist()
	{
		memset(this, 0, sizeof(*this));
	}
};

// SS_MSG_ENTITY_UPDATE_EXTRACT_TICKET
struct msg_entity_update_extract_ticket
{
	DWORD   dwClientID;         // 角色ClientID
	DWORD   dwPDBID;            // 角色ID
	DWORD	dwUserID;			// 帐号ID
	int		nTicket;			// 提取点券
	int		nTotalTicket;		// 当前游戏库总点券

	msg_entity_update_extract_ticket()
	{
		memset(this, 0, sizeof(*this));
	}
};


// CS_MSG_FOLLOW_ME
struct msg_fllow_me
{
	UID uidTarget;

	msg_fllow_me() : uidTarget(INVALID_UID)
	{

	}
};

// CS_MSG_BREAK_FOLLOW_ME
struct msg_break_fllow_me
{
	UID uidTarget;

	msg_break_fllow_me() : uidTarget(INVALID_UID)
	{

	}
};

// CS_MSG_ENTITY_PROPERTY_REQ_DRAGONBALL,		// 龙珠指定实体信息请求
struct msg_entity_property_req_dragonball
{
	UID		uidReq;           

	msg_entity_property_req_dragonball(void)
	{
		memset(this, 0, sizeof(*this));
	}
};


// SC_MSG_ENTITY_PROPERTY_RETURN_DRAGONBALL,	// 龙珠指定实体信息返回
struct msg_entity_property_return_dragonball
{
	UID		uidReq;           
	int		nPa;           
	int     nHp;
	int		nPd;
	int		nMd;

	msg_entity_property_return_dragonball(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

// CS_MSG_ENTITY_NPC_TRIGGER		// 玩家触发NPC
struct msg_entity_npc_trigger
{
	UID		uidNPC;

	msg_entity_npc_trigger()
	{
		memset(this, 0, sizeof(*this));
	}
};

//   CS_MSG_ENTITY_NPC_FUNC		玩家点击NPC功能
struct msg_entity_npc_func
{
	UID		uidNPC;									// NPC UID
	DWORD	dwParamLen;								// 参数长度

	msg_entity_npc_func()
	{
		memset(this, 0, sizeof(*this));
	}
};

// SC_MSG_ENTITY_NPC_OPEN_SHOP
struct msg_entity_npc_open_shop
{
	UID		uidNPC;

	msg_entity_npc_open_shop()
	{
		memset(this, 0, sizeof(*this));
	}
};
// SC_MSG_ENTITY_NPC_RESPONSE
struct msg_entity_npc_response
{
	UID		uidNPC;
	int		nLen;

	msg_entity_npc_response()
	{
		memset(this, 0, sizeof(*this));
	}
};

// CS_MSG_ENTITY_SET_NOVICEGUIDE  请求设置新手引导类型
struct msg_req_set_noviceguide
{
	DWORD dwNoviceGuideType;

	msg_req_set_noviceguide()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 通知MASK
enum MSG_NOTIFY_MASK
{
    NOTIFY_NONE = 0,			// 数据类型 NOTIFY_NONE
    NOTIFY_GAMESTATE = 1 << 0,  // 数据类型 ESDT_GAMESTATE
    NOTIFY_DATA      = 1 << 1,  // 通知数据类型 NOTIFY_DATA_TYPE
};

// SC_MSG_ENTITY_SYNC_DATA
struct msg_entity_sync_data
{
	int nDataType;				// 数据类型, MSG_NOTIFY_MASK
	int nData1;					// 数值1
    int nData2;                 // 数值2
    int nStrLen;                // 字符串长度
    // char*

	msg_entity_sync_data()
	{
		memset(this, 0, sizeof(*this));
	}
};

// SC_MSG_ENTITY_CROWNGEMSTONE_APPRAISAL_RET
struct msg_entity_gemstone_appraisal_ret
{
	int nGemstoneDBID;			// 宝石DBID
	int nCritNum;				// 暴击倍数
	bool bAppraisalRet;			// 抛光结果

	msg_entity_gemstone_appraisal_ret()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 英雄升星奖励解锁 SC_MSG_ENTITY_HEROUPSTAR_UNLOCK
struct msg_entity_heroupstar_unlock
{
	int nHeroID;
	int nStarLevel;

	msg_entity_heroupstar_unlock()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 标签列表 SC_MSG_ENTITY_LABEL_LIST
struct msg_entity_label_list
{
	int nCount;

	// LabelInfo * nCount

	msg_entity_label_list()
	{
		memset(this, 0, sizeof(*this));
	}
};

// SC_MSG_ENTITY_GAMESTATE_LEAVE
struct msg_entity_gamestate_leave
{
	bool isLeave;		// 0:未离开，1：离开

	msg_entity_gamestate_leave()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 施放进度条技能 CS_MSG_ENTITY_WAR_CHANGE_VOCATION
struct msg_entity_war_change_vocation
{
	int nGenicSlotID;		// 基因槽位ID

	msg_entity_war_change_vocation()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 发送导航到客户端         SC_MSG_ENTITY_SEND_NAVGROUP
struct msg_entity_send_navgroup
{
    BYTE nNavCount;
    // nNavCount * SGameNavItem
};

// 发送WebUrl到客户端       SC_MSG_ENTITY_SEND_WEBURL
struct msg_entity_send_weburl_sc
{
    int nWebUrlID;
    int  nStrLen;
    //char[]

    msg_entity_send_weburl_sc()
    {
        memset(this, 0, sizeof(*this));
    }
};

// 请求发送WebURL           CS_MSG_ENTITY_REQUEST_WEBURL
struct msg_entity_request_weburl_cs
{
    DWORD nWebUrlID;

    msg_entity_request_weburl_cs()
    {
        memset(this, 0, sizeof(*this));
    }
};

//////////////////////////////////////////////////////////////////////////
// 天赋点改变的类型
enum ENUM_TALENT_CHANGE_TYPE
{
	ENUM_TALENT_EFFECT,         // 效果改变
	ENUM_TALENT_BUFF,           // buff改变
	ENUM_TALENT_SELL,           // 卖物品改变
	ENUM_TALENT_BUY,            // 买物品改变
	ENUM_TALENT_KILLMONSTER,    // 击杀怪物
	ENUM_TALENT_KILLSOLIDER,    // 击杀士兵
	ENUM_TALENT_KILLPLAYER,     // 击杀玩家
	ENUM_TALENT_KILLTOWER,      // 击杀塔
	ENUM_TALENT_KILLBOSS,       // 击杀Boss
	ENUM_TALENT_REVOKE,         // 撤销物品改变
	ENUM_TALENT_INIT,           // 初始化改变
	ENUM_TALENT_DRAGON,         // 七龙珠改变
	ENUM_TALENT_SELL_REVOKE,    // 卖出物品撤销
	ENUM_TALENT_ACTIVE_TALENT,  // 激活天赋
	ENUM_TALENT_WARTIMER,       // 战场定时添加

	ENUM_TALENT_MAX,       
};

enum ENUMTALENTADDTYPE
{
	ENUMTALENTADDTYPE_NORMAL = 1,       // 正常情况下加金币 即不下发到客户端
	ENUMTALENTADDTYPE_SPECIAL,           // 要下发到客户端的类型

	ENUMTALENTADDTYPE_SPECIAL_MAX,
};

// 翅膀飞行类型
enum WING_FLY_TYPE
{
	WING_FLY_STAMINA       = 0x1,      // 体力飞行
	WING_FLY_TIME          = 0x2,      // 时间飞行
};

// 角色增值服务类型标识 （1变性别 2改角色名 ） 
enum EMActorServiceType
{
	ActorServiceType_None = 0,				// 无效
	ActorServiceType_ChangeSex,				// 改变性别
	ActorServiceType_Rename,				// 改角色名称
	ActorServiceType_MaxID,					// 最大ID
};


enum ENHEROINFOTYPE
{
	ENHEROINFOTYPE_SKIN = 0,         // 皮肤类型
	ENHEROINFOTYPE_LV,               // 英雄等级类型
	ENHEROINFOTYPE_DEADLINE,         // 使用期限
	ENHEROINFOTYPE_FLAGS,            // 英雄标识类型
	ENHEROINFOTYPE_EXP,              // 英雄经验类型
	ENHEROINFOTYPE_WINNUM,           // 比赛胜利场次
	ENHEROINFOTYPE_MATCHNUM,         // 比赛场次
	ENHEROINFOTYPE_STAR,             // 英雄星级类型
	ENHEROINFOTYPE_USETIME,          // 最后一次使用时间
	ENHEROINFOTYPE_SCORE,            // 英雄成就得分

	EHIT_MAX,
};

enum ENCHANGEDATAMODE
{
	ENCHANGEDATAMODE_SET = 0,              // 直接设置
	ENCHANGEDATAMODE_ADD,                  // 相加上去

	ECMD_MAX,
};

enum ENPOINTSHOPRESTYPE
{
	ENPOINTSHOPRESTYPE_POINT = 0,		// 点券购买
	ENPOINTSHOPRESTYPE_MONEY,			// 金钱购买

	ENPOINTSHOPRESTYPE_MAX,
};

enum ENRENAMERESUNLTTYPE
{
	ENRENAMERESUNLTTYPE_FAIL = 0,		// 改名成功
	ENRENAMERESUNLTTYPE_SUCCESS,		// 改名失败
	ENRENAMERESUNLTTYPE_NOSURE,			// 改名待定

	ENRENAMERESUNLTTYPE_MAX,
};

////////////////////////////////////增益卡相关 开始////////////////////////////////////
struct BenefitCardInfo
{
	int   nBenefitCardType;      // 增益卡类型（1：金币增益卡 2：角色经验增益卡 3：英雄经验增益卡）
	int   nBenefitCardSubType;   // 增益卡使用范围类型（1：时间天数 2：次数，一局消耗一次）
	DWORD nBenefitValue;         // 当nBenefitCardSubType的值为1时，该值是一个Unix时间戳，表示过期时间；为2时，表示剩余使用次数
	int   nMultiple;			 // 增益卡的增益倍数
};
typedef std::vector<BenefitCardInfo> BENEFITCARD_VECTOR;


#define BENEFITCARD_TYPE_COUNT 3        // 增益卡种类个数

// 增益卡列表
struct msg_entity_benefitcard_info
{
	int nListType;     // 1:表示玩家剩余的卡列表，2：表示玩家本局触发效果的卡列表
	int nCount;
	msg_entity_benefitcard_info(void)
	{
		memset(this, 0, sizeof(msg_entity_benefitcard_info));
	}
};

/////////////////////////////////////增益卡相关 结束///////////////////////////////////

/////////////////////////任务配置/////////////////////////////////////////////////
// 任务配置表
#define TASK_SCHEME_FILENAME				"Scp\\Task.csv"

struct STask
{
    int         nType;              // 类型
    string      strValue;           // 数值

    STask()
    {
        nType = 0;
        strValue.clear();
    }
};

////////////////////////////////NPC配置///////////////////////////////////////////////
#define NPC_SCHEME_FILENAME				"Scp\\NPC.csv"
struct SNPC
{
	int         nType;              // 类型
	string      strValue;           // 数值

	SNPC()
	{
		nType = 0;
		strValue.clear();
	}
};

#define LIFEHERO_PRIZE_SCHEME_FILENAME				"Scp\\LifeHeroPrize.csv"
struct SLifeHeroPrizeScheme
{
	int targetID;                  // 目标ID
	int heroCount;                 // 英雄数量
	int starLv;                    // 达到评级
	vector<int> prizeIDVec;        // 奖励ID

	SLifeHeroPrizeScheme()
	{
		targetID = 0;
		heroCount = 0;
		starLv = 0;
		prizeIDVec.clear();
	}
};

#define LIFEHERO_CONFIG_SCHEME_FILENAME				"Scp\\LifeHero.csv"
struct SLifeHeroScheme
{
	string szWarTypeID;                             //满足本命英雄的比赛类型ID值数组
	int rule;                                       // 入选本英雄的规则
	int starLv;                                     // 初次获得本命英雄提升指定星级
	int prizeID;                                    // 初次获得本命英雄的奖励ID
	int goldCount;                                  // 替换需要消耗的金币数量
	int cardCount;                                  // 替换需要消耗的卡牌数量

	SLifeHeroScheme()
	{
		rule = 0;
		starLv = 0;
		prizeID = 0;
		goldCount = 0;
		cardCount = 0;
		szWarTypeID.clear();
	}
};

// 价值物品类型
enum ENVALUEGOODSTYPE
{
	ENVALUEGOODSTYPE_NONE,		// 无效
	ENVALUEGOODSTYPE_CHEST,		// 宝箱
	ENVALUEGOODSTYPE_EMOJI,		// 表情包 

	ENVALUEGOODSTYPE_MAX,
};

//////////////////////////////////// 赛季管理 ////////////////////////////////////
// 当前赛季英雄详细信息
struct SRankSeasonDetailHero
{
	WORD	wHeroID;
	WORD	wMatchCount;
	WORD	wWinCount;

	SRankSeasonDetailHero()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 当前赛季详细信息
typedef DBREQ_TABLE_RANK_SEASON_DETAIL SRankSeasonDetail;

// 赛季历史信息
typedef DBREQ_TABLE_RANK_SEASON_RECORD SRankSeasonRecord;

// 战场结束赛季数据更新
struct SRankSeasonWarEndInfo
{
	DWORD	dwWarEndTime;			// 战场结束时间
	WORD	wMvpCount;				// MVP数量
	WORD	wSuperGodCount;			// 超神数量
	WORD	wKill5Count;			// 5杀数量
	WORD	wKill4Count;			// 4杀数量
	WORD	wKill3Count;			// 3杀数量
	WORD	wHeroID;				// 使用英雄ID
	bool	bWin;					// 是否胜利
		
	SRankSeasonWarEndInfo()
	{
		memset(this, 0, sizeof(*this));
	}
};
			
// 返回排位赛季基础数据 SC_MSG_ENTITY_RET_SEASON_DETAIL	
struct msg_entity_season_detail
{
	DWORD	dwEndTime;				// 赛季截止时间

	WORD	wTopRankGrade;			// 最高段位等级
	WORD	wMatchCount;			// 比赛场次
	WORD	wWinCount;				// 胜利场次
	WORD	wContinueWinCount;		// 最高连胜场次
	WORD	wMvpCount;				// MVP数量
	WORD	wSuperGodCount;			// 超神数量
	WORD	wKill5Count;			// 5杀数量
	WORD	wKill4Count;			// 4杀数量
	WORD	wKill3Count;			// 3杀数量

	WORD	wPrizeConfigID;			// 赛季奖励ID

	msg_entity_season_detail()
	{
		memset(this, 0, sizeof(msg_entity_season_detail));
	}
};

// 返回排位赛季历史数据 SC_MSG_ENTITY_RET_SEASON_RECORD
struct msg_entity_season_record_node
{
	WORD	wSeasonID;		// 赛季ID
	WORD	wRankGrade;		// 段位等级
	WORD	wMatchCount;	// 总场次
	WORD	wWinCount;		// 胜场
	WORD	wHeroRecord[DBDEF_SEASON_HERO_RECORD_COUNT];	// 英雄数据
	char	szSeasonTitle[GAME_NAME_MAXSIZE];				// 赛季名称
	DWORD	dwSeasonBeginTime;								// 赛季开始时间
	DWORD	dwSeasonEndTime;								// 赛季结束时间

	msg_entity_season_record_node()
	{
		memset(this, 0, sizeof(msg_entity_season_record_node));
	}
};

#pragma pack()



#endif	// __COMMON_ENTITYDEF_H__