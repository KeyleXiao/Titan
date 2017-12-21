/*******************************************************************
** 文件名: SchemeDef.h
** 版  权:    (C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人: 陈俊涛
** 日  期:    3/6/2015
** 版  本:    1.0
** 描  述:    配置文件解析，结构定义处
********************************************************************/

// 地图区域文件路径
#define MAPAREA_SCHEME_FILENAME             "Scp\\MapArea.xml"
#define MAPAREA_SCHEME_FILEPATH             "Scp\\MapArea\\"
// MapArea.xml
struct SMapAreaInfo
{
    int     nMapID;                 // 地图ID
    int     nAreaType;              // 区域类型
    char    szAreaName[32];         // 区域名字

    SMapAreaInfo(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

// 地图着陆配置名字
#define MAPLAND_SCHEME_FILENAME             "Scp\\mapland.csv"
// MapLand.csv
struct SPosWithAngle
{
    Vector3         ptPos;   
    float           fAngleY; // 绕Y朝向角度
};
struct SMapLandSchemeInfo
{
    BYTE            nModelType;                                 // 模式
    int             nCampID;                                    // 阵营ID
    vector<SPosWithAngle>   vecBornPos;                         // 出生点
    vector<SPosWithAngle>   vecRebornPos;                       // 复活点
    vector<SPosWithAngle>   vecTransportBackPos;                // 回城点
    long            lReliveCountDown;                           // 复活倒计时长
    long            lReliveGoods;                               // 复活消耗的物品

	Vector3			ptCenterZone;								// 区域中心
	float			fRadiusZone;								// 区域半径
	float			fCampDistance;								// 阵营间隔


    SMapLandSchemeInfo(void)
    {
        nModelType=nCampID=lReliveCountDown=lReliveGoods=0;

		fRadiusZone = 0.f;
		fCampDistance = 0.f;
    }
};


// 地图信息配置名字
#define MAPINFO_SCHEME_FILENAME             "Scp\\MapInfo.xml"

// MapInfo.xml
struct SMapSchemeInfo
{
    long            lMapID;                                             // 地图ID
    char            szName[GAME_NAME_MAXSIZE];                          // 地图名字
    
    long            lZoneServerIDForJustLaunch;                         // 刚开服时，开在哪个场景服上
    long            lStartLoadForJustLaunch;                            // 刚开服时，启动时是否载入 

    long            lZoneServerIDForLongLaunch;                         // 开服一个星期后，开在哪个场景服上
    long            lStartLoadForLongLaunch;                            // 开服一个星期后，启动时是否载入

    long            lZoneServerIDForCivilization;                       // 老区精简服模式的场景服  
    long            lStartLoadfForCivilization;                         // 老区精简服模式启动时是否加载 1：载入。2：不载入

    long            lDynamicFlag;                                       // 是否动态载入， 1：动态。2：不动态
    char            szMapFile[GAME_NAME_MAXSIZE];                       // 地图文件集合
    char            szCageFile[GAME_NAME_MAXSIZE];                      // 地图笼子脚本
    char            szMonsterBuildFile[GAME_NAME_MAXSIZE];              // 怪物生成配置文件
    char            szChunnelBuildFile[GAME_NAME_MAXSIZE];              // 传送门生成配置文件
    char            szBoxBuildFile[GAME_NAME_MAXSIZE];                  // 草药，矿等等的生成配置文件
    long            lBackMusicID;                                       // 背景音乐id

    long            lMapPixelWidth;                                     // 地图像素宽
    long            lMapPixelHeight;                                    // 地图像素高    

    // 090429 新增光复用地图信息
    long            lRecover;                                           // 是否需要光复，1：需要，其它不需要    
    char            szRecoverMonsterBuildFile[GAME_NAME_MAXSIZE];       // 光复前 怪物生成配置文件 
    char            szRecoverBoxBuildFile[GAME_NAME_MAXSIZE];           // 光复前 草药，矿等等的生成配置文件

    
    UINT            uPathFindServiceID;                                 // 0无障碍，两点直达  
    char            szHeightFieldFile[GAME_NAME_MAXSIZE];               // 高度场文件

    Vector3         vOffset;                                            // 地图偏移量 服务器忽略(服务器直接读高度场获取)
    int             nXAxisLen;                                          // x轴长度    服务器忽略(服务器直接读高度场获取)
    int             nZAxisLen;                                          // z轴长度    服务器忽略(服务器直接读高度场获取)


    SMapSchemeInfo(void)
    {
        lMapID=0;
        lZoneServerIDForJustLaunch=lStartLoadForJustLaunch=lZoneServerIDForLongLaunch=lStartLoadForLongLaunch=lZoneServerIDForCivilization=lStartLoadfForCivilization=0;
        lDynamicFlag=lBackMusicID=lMapPixelWidth=lMapPixelHeight=lRecover=uPathFindServiceID=nXAxisLen=nZAxisLen=0;

        memset(szName, 0, sizeof(szName));
        memset(szMapFile, 0, sizeof(szMapFile));
        memset(szCageFile, 0, sizeof(szCageFile));
        memset(szMonsterBuildFile, 0, sizeof(szMonsterBuildFile));
        memset(szChunnelBuildFile, 0, sizeof(szChunnelBuildFile));
        memset(szBoxBuildFile, 0, sizeof(szBoxBuildFile));
        memset(szRecoverMonsterBuildFile, 0, sizeof(szRecoverMonsterBuildFile));
        memset(szRecoverBoxBuildFile, 0, sizeof(szRecoverBoxBuildFile));
        memset(szHeightFieldFile, 0, sizeof(szHeightFieldFile));
    }
};

// 场景保姆标识
enum SCENE_MATRON_FLAG
{
    SCENE_FLAG_SPELL_LIGHT_ONLY_SELF_SEE    = 0x1,      // 场景中技能光效仅自己可见
};

// 场景保姆
#define SCENE_MATRON_SCHEME_FILENAME         "Scp\\SceneMatron.csv"
struct SSchemeSceneMatron
{
    int             nMapID;                                             // 地图ID
    bool            bForbitWing;                                        // 禁止使用翅膀
    bool            bForbitPK;                                          // 禁止决斗
    bool            bForbitSwitchHero;                                  // 禁止切换英雄
    bool            bEnableRightMenu;                                   // 实体右键菜单（0不开启，1开启）
    int             nHotkeyIndex;                                       // 快捷键方案调用序号（场景脚本配置）
    bool            bAccelerateRun;                                     // 能加速跑
    int             nFlag;                                              // 标识位 SCENE_MATRON_FLAG

    SSchemeSceneMatron()
    {
        memset(this, 0, sizeof(*this) );
    }
};

// SummonerSkill.csv
#define SUMMONER_SKILL_INFO         "Scp\\SummonerSkill.csv"
#define SUMMONER_SKILL_NAME     32
#define SUMMONER_SKILL_DESC     256
struct SSummonerSkillData
{
    char            stName[SUMMONER_SKILL_NAME];                        // 召唤师技能名字
    int             nId;                                                // 召唤师技能id
    int             nLv;                                                // 需求玩家等级
    int             nIconId;                                            // 图片id
    char            stDesc[SUMMONER_SKILL_DESC];                        // 召唤师技能描述
    SSummonerSkillData()
    {
        memset(this, 0, sizeof(*this) );
    }
};

// SlotMapedShortcut.csv
#define SLOT_SCHEME_FILENAME            "Scp\\SlotMapedShortcut.csv"
struct SSlotShortcutScheme
{
    int             nSlotIndex;                                         //槽位索引
    int             nSlotType;                                          //槽位类型
    char            szKeyName[GAME_SHORTCUT_NAME_MAXSIZE];              //槽位对应的快捷键名称
    int             nAirCons;                                           //空中限制
    int             nSkillType;                                         //技能类型

    SSlotShortcutScheme()
    {
        memset(this, 0, sizeof(*this));
    }
};


#ifdef FUNCTION_TALENTPAGE_OPEN
#define TALENT_PAGE_CSV             "Scp\\TalentPage.csv"
#define TALENT_PAGE_NAME     32
#define TALENT_PAGE_EFFECT_ID   32

struct STalentPageData
{
    int             nId;                                            // 天赋id
    char            szName[TALENT_PAGE_NAME];                       // 天赋名字
    int             nIconId;                                        // 天赋图片id
    int             nTalentType;                                    // 天赋类型
    int             nLevel;                                         // 天赋等级
    int             nAllowNum;                                      // 前置天赋点
    int             nMaxNum;                                        // 最大加点数
    char            szEffectIdArray[TALENT_PAGE_EFFECT_ID];         // 效果id组 
    STalentPageData()
    {
        memset(this, 0, sizeof(*this) );
    }
};
#endif

#ifdef FUNCTION_TALENTPAGE_OPEN
#define TALENT_PAGE_EFFECT_CSV             "Scp\\TalentPageEffect.csv"
#define TALENT_PAGE_EFFECT   32
struct STalentPageEffect
{
    int             nEffectId;                                // 效果组id
    char            stEffectArray[TALENT_PAGE_EFFECT];        // 效果组
    STalentPageEffect()
    {
        memset(this, 0, sizeof(*this) );
    }
};
#endif

//////////////////////////////// 动态障碍寻路 /////////////////////////////////// 
// 寻路服务配置文件名字
#define PATHFIND_SCHEME_FILENAME                "MapPath\\Config.xml"
// 寻路服务配置
struct PathFindServiceScheme
{
    unsigned int    uPathFindServiceID;     // 寻路服务ID
    char            szName[64];             // 寻路文件名字
    char            szChfName[64];          // 场景高度场文件名称

    PathFindServiceScheme()
    {
        memset( this, 0, sizeof(*this) );
    }
};

//// 动态障碍有效性Flag
//enum {
//    OBS_EFFECT_INIT = 0,
//
//    OBS_EFFECT_OWN = 1,        // 仅对本方阵营有效
//    OBS_EFFECT_OPS = 1<<1,     // 仅对对方阵营有效
//    OBS_EFFECT_ALL = 1<<2,     // 对双方阵营有效
//
//    OBS_EFFECT_MAX,
//};

// 动态障碍配置
struct DynamicObstacleScheme
{
    int             nDynamicMonsterID;      // 动态障碍怪ID
    int             nShape;                 // 动态障碍形状类型:1:方体2 : 圆柱体
    float           fLength;                // 动态障碍 方体:长度/柱体:半径
    float           fWidth;                 // 动态障碍 方体:宽度/柱体:高度
    float           fAngle;                 // 障碍物旋转角度
    int             nEntityCampFlag;        // 动态障碍实体阵营标志位，见PathFindDef.h 定义
    int             nSpellCampFlag;         // 动态障碍技能阵营标志位，见PathFindDef.h 定义
    int             nSpellFlag;             // 动态障碍技能是否可穿越标志位
    int             nPreloadFlag;           // 动态障碍是否预加载（允许在客户端创建前生成）
};

//////////////////////////////// 符文 Rune //////////////////////////////////////
#ifdef FUNCTION_RUNE_OPEN
#define MAX_RUNE_NAME_LEN 64
#define MAX_RUNE_DES_LEN 256
#define MAX_RUNE_PROPERTY_NUM 3           // 每个符文最多拥有属性个数
#define MAX_RUNE_PROPERTY_KIND_NUM 24     // 符文属性种类数

// 符文配置文件名字
#define RUNE_SCHMEME_FILENAME               "Scp\\RuneInfo.csv"

// 玩家符文配置信息
#define RUNE_CONFIG_FILENAME                "Scp\\RuneConfig.csv"

// 玩家符文配置信息
#define RUNE_PAGE_FILENAME                  "Scp\\RunePage.csv"

enum EMRuneInfoSchemeColunm
{
    RUNE_INFO_COL_INIT = 0,                 

    RUNE_INFO_COL_ID = RUNE_INFO_COL_INIT,  // 符文ID
    RUNE_INFO_COL_NAME,                     // 符文名称
    RUNE_INFO_COL_ICON,                     // 符文图标
    RUNE_INFO_COL_SPC,                      // 符文专属
    RUNE_INFO_COL_INLAYTYPE,                // 符文镶嵌类型
    RUNE_INFO_COL_PROPERTYTYPE,             // 符文属性类型
    RUNE_INFO_COL_LEVEL,                    // 符文等级
    RUNE_INFO_COL_SCR,                      // 符文描述
    RUNE_INFO_COL_EffectArray,              // 符文效果ID组
    RUNE_INFO_COL_EffectValueArray,         // 符文效果改变值组
    RUNE_INFO_COL_PropertyNameAppendType,   // 符文名称附加类型
    RUNE_INFO_COL_LimitCount,               // 符文id限制个数
    RUNE_INFO_COL_MAX,                      // 符文最大列数
};

// 符文属性
struct RuneProperty
{
    int nRunePropertyIdx;                 // 符文对应属性indix
    float fRunePropertyVal;               // 符文对应属性值

    RuneProperty()
    {
        memset(this, 0, sizeof(*this));
    }
};
#define RUNE_PAGE_EFFECT   32
// 符文信息配置
struct RuneInfoScheme
{
    int nID;                                                  // 符文ID
    char sName[MAX_RUNE_NAME_LEN];                            // 符文名称
    int nIcon;                                                // 符文图标
    int nSpecial;                                             // 符文专属
    int nType;                                                // 符文类型 EMRuneType
    int nLevel;                                               // 符文等级
    int nLimitCount;                                          // 符文限制个数
    char            stEffectArray[RUNE_PAGE_EFFECT];          // 符文效果组
    char            stEffectValueArray[RUNE_PAGE_EFFECT];     // 符文改变值组(与效果组id位置一一对应)

    RuneInfoScheme()
    {
        memset(this, 0, sizeof(*this));
    }
};
#endif // #ifdef FUNCTION_RUNE_OPEN

// 玩家英雄新天赋配置信息
#define HEROTALENT_CONFIG_FILENAME              "Scp\\HeroTalentConfig.csv"


#define GLORY_CONDITION_NUM           2
#define GLORY_CONDITION_TYPE          0       // 条件类型
#define GLORY_CONDITION_VALUE         1       // 条件内容

#define GLORY_SCHEME_FILENAME       "Scp\\PlayerGloryInfo.csv"
struct SGlorySchemeInfo
{
    int                 nId;              // 称号id
    std::map<int,long>  mapCondition;     // 条件信息
    string              stDesc;           // 称号描述
    int                 nDifficulty;      // 难度
    SGlorySchemeInfo()
    {
        mapCondition.clear();
        nId = 0;
        stDesc = "";
        nDifficulty = 0;
    }
};

#define STATICHEROUPGRADE_SCHEME_FILENAME     "Scp\\StaticHeroLvUpgrade.csv"
struct SStaticHeroUpgradeSchemeInfo
{
    int             nLv;                // 当前等级
    long             nNeedExp;           // 当前等级升级需要的经验
    SStaticHeroUpgradeSchemeInfo()
    {
        memset(this, 0, sizeof(*this));
    }
};
typedef std::map<int, SStaticHeroUpgradeSchemeInfo> StaticHeroUpgradeMap;


#define  HEROSTAR_SCHEME_FILENAME               "Scp\\HeroStarData.csv"
#define  HEROPROPERTY_SCHEME_FILENAME           "Scp\\HeroProperty.csv"
#define  HEROEXCLUSIVETALENT_SCHEME_FILENAME    "Scp\\HeroExclusiveTalentInfo.csv"
enum EN_GROW_PROPERTY_TYPE
{
    EGPT_BLOOD = 0,                     // 星级血量
    EGPT_BLOOD_GROW,                    // 星级血量成长
    EGPT_BLOOD_GROW_RATE,               // 星级血量成长率
    EGPT_BLOOD_RATE_GROW,               // 星级血量成长率的成长

    EGPT_MAGIC,                         // 星级魔法
    EGPT_MAGIC_GROW,                    // 星级魔法成长
    EGPT_MAGIC_GROW_RATE,               // 星级血量成长率
    EGPT_MAGIC_RATE_GROW,               // 星级血量成长率的成长

    EGPT_PHYSIC_ATTACK,                 // 星级物理攻击
    EGPT_PHYSIC_ATTACK_GROW,            // 星级物理攻击成长
    EGPT_PHYSIC_ATTACK_GROW_RATE,       // 星级物理攻击成长率
    EGPT_PHYSIC_ATTACK_RATE_GROW,       // 星级物理攻击成长率成长

    EGPT_MAGIC_ATTACK,                  // 星级魔法攻击
    EGPT_MAGIC_ATTACK_GROW,             // 星级魔法攻击成长
    EGPT_MAGIC_ATTACK_GROW_RATE,        // 星级魔法攻击成长率
    EGPT_MAGIC_ATTACK_RATE_GROW,        // 星级魔法攻击成长率成长
    
    EGPT_PHYSIC_DEFENSE,                 // 星级物理防御
    EGPT_PHYSIC_DEFENSE_GROW,            // 星级物理防御成长
    EGPT_PHYSIC_DEFENSE_GROW_RATE,       // 星级物理防御成长率
    EGPT_PHYSIC_DEFENSE_RATE_GROW,       // 星级物理防御成长率成长

    EGPT_MAGIC_DEFENSE,                 // 星级魔法防御
    EGPT_MAGIC_DEFENSE_GROW,            // 星级魔法防御成长
    EGPT_MAGIC_DEFENSE_GROW_RATE,       // 星级魔法防御成长率
    EGPT_MAGIC_DEFENSE_RATE_GROW,       // 星级魔法防御成长率成长

    EGPT_MAX
};

struct SPersonStarProp 
{
    int     nPropID;        // 属性ID
    int     nStarBase;      // 星级基础值
    float   nStarGrow;      // 星级基础值成长
    float   nStarGrowRate;  // 星级基础值成长率
    float   nStarRateGrow;  // 星级基础值成长率成长
    SPersonStarProp()
    {
        memset(this, 0, sizeof(*this));
    }
};
// 人物星级属性数量
#define PERSON_STAR_PROP_COUNT      6
struct SHeroGrowProInfo
{
    int         nGrowID;
    SPersonStarProp sStarProp[PERSON_STAR_PROP_COUNT];
    SHeroGrowProInfo()
    {
        memset(this, 0, sizeof(*this));
    }
};
typedef std::map<int, SHeroGrowProInfo> GROW_PROPERTY_SCHEMEINFOMAP;

struct SHeroExclusiveTalentSchemeInfo
{
    int                 nID;                // 英雄专属天赋ID
    string              stName;             // 英雄专属天赋名字
    string              stDesc;             // 英雄专属天赋描述
    int                 nPic;               // 图标
    std::vector<int>    vecEffect;          // 效果组
    std::map<int,int>   mapCondition;       // 激活专属天赋的条件

    SHeroExclusiveTalentSchemeInfo()
    {
        nID = 0;
        stName = "";
        stDesc = "";
        vecEffect.clear();
        mapCondition.clear();
    }
};
typedef std::map<int, SHeroExclusiveTalentSchemeInfo> EXCLUSIVE_TALENT_SCHEMEINFOMAP;

#define  INITIALHERO_SCHEME_FILENAME            "Scp\\InitialHero.csv"
struct SInitialHeroSchemeInfo
{
    int     nHeroID;                // 英雄id;
    int     nIntialLv;              // 英雄初始登记
    int     nSkinID;                // 英雄皮肤ID
    int     nHeroStar;              // 英雄星级
    DWORD   dwDeadLine;             // 使用时长
    int     nFlag;                  // 标记
    std::vector<int> vecExclusiveID;// 特权id
    int     nIsActivate;            // 是否激活  0代表全部不激活， 1代表全部激活
    SInitialHeroSchemeInfo()
    {
        nHeroID = 0;
        nIntialLv = 0;
        nSkinID = 0;
        nHeroStar = 0;
        dwDeadLine = 0;
        nFlag = 0;
        nIsActivate = 0;
        vecExclusiveID.clear();
    }
};
typedef std::map<int, SInitialHeroSchemeInfo> INITIAL_HERO_SCHEMEINFOMAP;

#define  HEROTYPE_SCHEME_FILENAME           "Scp\\HeroType.csv"

#define  HEROCARDCREATPRO_SCHEME_FILENAME           "Scp\\HeroCardCreatPro.csv"
#define  HEROCARDCREATCOUNT_SCHEME_FILENAME         "Scp\\HeroCardCreatCount.csv"
#define  PROBABILITY_ENLARGE        1000000
struct SHeroCardCreatPro
{
    int     nPlayerLv;                      // 玩家等级
    int     nMainHeroPro;                   // 主英雄概率
    int     nTeamHeroPro;                   // 队友英雄概率
    int     nPrivateHeroPro;                // 私有英雄概率
    int     nWeekFreePro;                   // 周免英雄概率
    SHeroCardCreatPro()
    {
        memset(this, 0, sizeof(*this));
    }
};

#define MATCHCOEFFICIENT_SCHEME_FILENAME        "Scp\\MatchCoefficient.csv"
struct SMatchCoefficient
{
    int     nWarID;                     // 战场id
    int     nAG;
    int     nBG;
    int     nCG;
    int     nAE;
    int     nBE;
    int     nCE;
    int     nW11;
    float   fW12;
    int     nW13;
    int     nW14;
    int     nW15;
    int     nW21;
    float   fW22;
    float   fW23;
    int     nW24;
    int     nW25;

    SMatchCoefficient()
    {
        memset(this, 0, sizeof(*this));
    }
};

struct SCardCountPro
{
    int nCardCount;         // 胜利一场比赛获得的卡牌数量
    int nPro;               // 使用这种卡牌数量的概率
    SCardCountPro()
    {
        memset(this, 0, sizeof(*this));
    }
};

#define HEROCARD_NUM_TYPE   3   // 获得卡牌数量的类型
struct SHeroCardCreatCount
{
    int             nPlayerLv;                          // 玩家等级
    int             nWarType;                           // 战场类型
    int             nDayNum;                            // 一天可获得的卡牌次数
    int             nWeekNum;                           // 一周可获得的卡牌次数
    SCardCountPro   sCardCount[HEROCARD_NUM_TYPE];      // 卡牌数量
    SHeroCardCreatCount()
    {
        memset(this, 0, sizeof(*this));
    }
};

#define  EXPORGOLDCREATE_SCHEME_FILENAME        "Scp\\ExpOrGoldCreate.csv"

enum ENECPORGOLDCOEFF
{
    ENECPORGOLDCOEFF_GOLD_TIME = 0,             //金币_时间系数
    ENECPORGOLDCOEFF_EXP_BASELV,            // 经验_初始等级
    ENECPORGOLDCOEFF_EXP_BASESLOPE,         // 经验_初始斜率系数
    ENECPORGOLDCOEFF_EXP_SLOPEGROW,         // 经验_斜率成长
    ENECPORGOLDCOEFF_EXP_TIMECRITICAL,      // 经验_时间临界点
    ENECPORGOLDCOEFF_EXP_CRITICALEXP,       // 经验_临界点经验值
    ENECPORGOLDCOEFF_MAX,
};

struct SExpOrGoldCreateConfig
{
    int     nWarTypeID;                             // 战场id
    int     nExpType;                               // 经验类型
    float   fGoldAddition;                          // 金币模式加成
    float   fWinCoeff[ENECPORGOLDCOEFF_MAX];        // 胜利的所有系数
    float   fFailCoeff[ENECPORGOLDCOEFF_MAX];       // 失败的所有系数
    int     nPersonLimit;                           // 玩家下限
    SExpOrGoldCreateConfig()
    {
        memset(this, 0, sizeof(*this));
    }
};

#define  FIGHTCONFIG_SCHEME_FILENAME        "Scp\\FightingCapacityConfig.csv"
struct SFightingCapacityConfig
{
    int     nMatchType;             // 比赛类型
    float   fFightCoeff;            // 系数
    SFightingCapacityConfig()
    {
        memset(this, 0, sizeof(*this));
    }
};

#define  HEROACHIEVEMENTSCORE_SCHEME_FILENAME     "Scp\\HeroAchievementScore.csv"
#define  HEROACHIEVEMENTLV_SCHEME_FILENAME        "Scp\\HeroAchievementLv.csv"
struct SHeroAchievementScoreConfig
{
    int nID;                // 战场类型id
    int nRankLV;            // 段位等级
    int nExpectValue;       // 期望成就
    int nWinRateValue;      // 胜率基数
    int nScoreValue;        // 得分基数
    int nScoreMinLimit;     // 评分下限
    int nScoringRatio;      // 评分倍率系数
    int nScoringRewards;    // 评分奖励系数
    SHeroAchievementScoreConfig()
    {
        memset(this, 0, sizeof(*this));
    }
};

#define ICONGROUP_COUNT_MAX 10
struct SHeroAchievementLvConfig
{
    int nID;                                        // 成就ID;
    char szAchievementDesc[GAME_NAME_MAXSIZE];      // 成就名
    char szIconID[ICONGROUP_COUNT_MAX];             // 图标ID组
    int nAchievementScore;                          // 成就得分
    SHeroAchievementLvConfig()
    {
        memset(this, 0, sizeof(*this));
    }
};

#define OUTPUTRESTRICTION_SCHEME_FILENAME       "Scp\\OutputRestriction.csv"
struct SOutputRestrictionConfig
{
    int nMatchType;                 // 比赛类型
    std::map<int,int> mapPersonNum; // 阵营最低人数
    int nMatchTime;                 // 最低比赛时长 单位 min
    int nPersonMin;                 // 最小总人数
    SOutputRestrictionConfig()
    {
        nMatchType = 0;
        nMatchTime = 0;
        nPersonMin = 0;
        mapPersonNum.clear();
    }
};

#define ACTORPRIZE_SCHEME_FILENAME              "Scp\\ActorPrize.csv"
#define ACTORPRIZEGROUP_SCHEME_FILENAME         "Scp\\ActorPrizeGroup.csv"
#define ACTORPRIZEEXCHANGE_SCHEME_FILENAME      "Scp\\ActorPrizeExchange.csv"
#define ACTORPRIZE_PARAMMAX                     5
#define ACTORPRIZEGROUP_SINGLEGROUPMAX          30
#define ACTORPRIZEGROUP_RANDOMPRIZEMAX          10

enum EMPrizeBroadcastFlagType
{
    PRIZE_BROADCAST_WORLD = 1 << 0,
    PRIZE_BROADCAST_CLAN = 1 << 1,
    PRIZE_BROADCAST_KIN = 1 << 2,
    PRIZE_BROADCAST_NOTICE = 1 << 3,
};

struct SActorPrizeConfig_S
{
    int nPrizeID;                           // 奖励ID
    int nGrade;                             // 品质
    int nBroadcastFlag;                     // 广播标识 EMPrizeBroadcastFlagType
    int nPrizeType;                         // 奖励类型 EMActorPrizeType
    int nPrizeParam[ACTORPRIZE_PARAMMAX];   // 奖励参数
    char szPrizeName[GAME_NAME_MAXSIZE];    // 奖励名称

    SActorPrizeConfig_S()
    {
        memset(this, 0, sizeof(*this));
    }
};

struct SActorPrizeGroupConfig
{
    int nGroupID;                                                                       // 奖励组ID
    int nBroadcastFlag;                                                                 // 广播标志
    int nBroadcastGrade;                                                                // 广播品质要求
    int nConstantPrize[ACTORPRIZEGROUP_SINGLEGROUPMAX];                                 // 固定奖励列表
    int nRandomPrizeCount;                                                              // 可执行随机次数
    int nRandomPrizeID[ACTORPRIZEGROUP_RANDOMPRIZEMAX][ACTORPRIZEGROUP_SINGLEGROUPMAX]; // 随机奖励ID列表
    int nRandomPrizeRate[ACTORPRIZEGROUP_RANDOMPRIZEMAX];                               // 随机概率

    SActorPrizeGroupConfig()
    {
        memset(this, 0, sizeof(*this));
    }
};

struct SActorPrizeExchangeConfig
{
    int nPrizeID;
    int nExchangePrizeID;

    SActorPrizeExchangeConfig()
    {
        memset(this, 0, sizeof(*this));
    }
};

#define PRIZECHEST_SCHEME_FILENAME              "Scp\\PrizeChestInfo.csv"

struct SPrizeChestConfig_S
{
    int nChestID;
    int nChestFlag;
    char szChestName[64];
    int nShowPrizeID;
    int nPrizeGroupID;

    SPrizeChestConfig_S()
    {
        memset(this, 0, sizeof(*this));
    }
};

enum EPrizeChestFlagType
{
    EPCFT_NORMAL = 1 << 0,
    EPCFT_GEMSTONE = 1 << 1,
};

// 商城相关
#define POINTSHOPGOODS_SCHEME_FILENAME          "Scp\\PointShopGoods.csv"
#define POINTSHOPNUMLIMITPRICE_SCHEME_FILENAME  "Scp\\ShopNumLimitPrice.csv"
#define POINTSHOPCARDLISTRULE_SCHEME_FILENAME   "Scp\\PointShopCardListRule.csv"
#define POINTSHOPCARDIDTOSELLID_SCHEME_FILENAME "Scp\\PointShopCardIDToSellID.csv"
#define POINTSHOPHOLIDAYCONFIG_SCHEME_FILENAME  "Scp\\HolidayConfig.csv"
// 商城物品类型定义
enum EMPointShopGoodsType
{
    POINTSHOP_GOODSTYPE_INVALID = 0,// 无效
    POINTSHOP_GOODSTYPE_HERO,       // 英雄
    POINTSHOP_GOODSTYPE_SKIN,       // 皮肤
    POINTSHOP_GOODSTYPE_WING,       // 翅膀
    POINTSHOP_GOODSTYPE_CARD,       // 卡牌
    POINTSHOP_GOODSTYPE_RUNE,       // 符文
    POINTSHOP_GOODSTYPE_CHEST,      // 宝箱
    POINTSHOP_GOODSTYPE_SPECIAL,    // 特殊商品
    POINTSHOP_GOODSTYPE_BENEFITCARD,// 增益卡
    POINTSHOP_GOODSTYPE_XP,         // 终结技
    POINTSHOP_GOODSTYPE_GEMSTONE,   // 水晶
    POINTSHOP_GOODSTYPE_OTHER,      // 杂物
    
    POINTSHOP_GOODSTYPE_MAX
};

// 商城消费途径
enum EPointShopConsumpApproach
{
	CONSUMP_APPROACH_INVALID,
	CONSUMP_APPROACH_ACTOR_RENAME,				// 玩家改名
	CONSUMP_APPROACH_GEMSTONE_APPRAISAL,		// 宝石抛光
	CONSUMP_APPROACH_MYSTERYSHOP_REFLUSH,		// 神秘商店刷新
	CONSUMP_APPROACH_TASKOPT_REFLUSH,			// 任务操作刷新

	CONSUMP_APPROACH_MAX
};

struct SPointShopGoodsConfig_S
{
    int nSellID;            // 出售ID
    int nShopGoodsType;     // 商城物品类型   EMPointShopGoodsType
    int nPrizeID;           // 奖励ID
    int nNeedVipLevel;      // 需要VIP等级
    int nLevelLimitMin;     // 最低等级限制
    int nLevelLimitMax;     // 最高等级限制
    int nBeginSellTime;     // 开始时间
    int nEndSellTime;       // 结束时间
    int nNumPriceID;        // 动态变更价格ID
    int nPointPrice;        // 点券价格
    int nMoneyPrice;        // 金币价格
    int nMoneyUseType;      // 金币使用类型
    int nDiscountRate;      // 折扣率
    int nDiscountBeginTime; // 折扣开始时间
    int nDiscountEndTime;   // 折扣结束时间
	int nConsumpApproach;	// 消费途径（1改名消费、2水晶充能消耗、3神秘商店刷新消费）
    char szSellName[GAME_NAME_MAXSIZE];     // 商品名称
    SPointShopGoodsConfig_S()
    {
        memset(this, 0, sizeof(*this));
    }
};

struct SPointShopNumLimitPriceSub
{
    int nLimitNum;          // 到下一段为止所需数量
    int nLimitPointPrice;   // 点券价格
    int nLimitMoneyPrice;   // 金币价格

    SPointShopNumLimitPriceSub()
    {
        memset(this, 0, sizeof(*this));
    }
};

struct SPointShopNumLimitPrice
{
    int nLimitPriceID;
    vector<SPointShopNumLimitPriceSub> vecLimitStep;

    SPointShopNumLimitPrice()
    {
        memset(this, 0, sizeof(*this));
    }
};

#define POINTSHOP_CARDLISTRULE_MAXCOMBINECOUNT 2
struct SPointShopCardListRule
{
    int nNeedHeroNum;                                           // 需要英雄数量
    int nCombineFlag[POINTSHOP_CARDLISTRULE_MAXCOMBINECOUNT];   // 组合类型(1已拥有英雄；2周免英雄；4不可使用英雄)
    int nHeroNum[POINTSHOP_CARDLISTRULE_MAXCOMBINECOUNT];       // 随机数量
    int nHeroNumEx[POINTSHOP_CARDLISTRULE_MAXCOMBINECOUNT];     // 节日随机数量
    int nPassUsedHero[POINTSHOP_CARDLISTRULE_MAXCOMBINECOUNT];  // 是否排除已随机过的
    int nResetUsedHeroDay;                                      // 已随机英雄记录重置时间

    SPointShopCardListRule()
    {
        memset(this, 0, sizeof(*this));
    }
};

// 解锁相关定义
#define PRIZEUNLOCK_CONFIG_FILENAME             "Scp\\PrizeUnlock.csv"
#define PRIZEUNLOCK_PRIZEID_MAXCOUNT 9
// 角色解锁定义
struct SActorPrizeUnlockItem
{
    int nPrizeId[PRIZEUNLOCK_PRIZEID_MAXCOUNT];
    int nUnlockLevel;
    bool bUnlock;
    int nMailId;

    SActorPrizeUnlockItem()
    {
        memset(this, 0, sizeof(*this));
    }
};

struct SActorPrizeUnlockVec
{
    std::vector<SActorPrizeUnlockItem> actorPrizeUnlockVec;
    SActorPrizeUnlockVec()
    {
        actorPrizeUnlockVec.clear();
    }
};
// 英雄等级解锁定义
struct SHeroLevelPrizeUnlockItem
{
    int nPrizeId[PRIZEUNLOCK_PRIZEID_MAXCOUNT];
    int nUnlockLevel;
    bool bUnlock;
    int nMailId;

    SHeroLevelPrizeUnlockItem()
    {
        memset(this, 0, sizeof(*this));
    }
};

struct SHeroLevelPrizeUnlockVec
{
    std::vector<SHeroLevelPrizeUnlockItem> heroLevelPrizeUnlockVec;
    SHeroLevelPrizeUnlockVec()
    {
        heroLevelPrizeUnlockVec.clear();
    }
};

// 英雄星级解锁定义
struct SHeroStarPrizeUnlockItem
{
    int nPrizeId[PRIZEUNLOCK_PRIZEID_MAXCOUNT];
    int nUnlockLevel;
    bool bUnlock;
    int nMailId;

    SHeroStarPrizeUnlockItem()
    {
        memset(this, 0, sizeof(SHeroStarPrizeUnlockItem));
    }
};

struct  SHeroStarPrizeUnlockVec
{
    std::vector<SHeroStarPrizeUnlockItem> heroStarPrizeUnlockVec;
    SHeroStarPrizeUnlockVec()
    {
        heroStarPrizeUnlockVec.clear();
    }
};


// 周免英雄相关定义
#define HEROFREEWEEKLY_SCHEME_FILENAME              "Scp\\HeroFreeWeekly.csv"
#define HEROFREEWEEKLY_FREEID       32
// 周免英雄定义
struct SHeroFreeWeeklyScheme
{
    time_t tBeginTime;
    time_t tEndTime;
    int arrayHeroIDLow[HEROFREEWEEKLY_FREEID];
    int nPlayerLvLow;
    int arrayHeroIDHigh[HEROFREEWEEKLY_FREEID];
    int nPlayerLvHigh;

    SHeroFreeWeeklyScheme()
    {
        memset(this, 0, sizeof(*this));
    }
};

// 增益卡
#define BENEFITCARD_SCHEME_FILENAME             "Scp\\BenefitCard.csv"
struct SBenefitCardConfig_S
{
    int nBenefitCardID;             // 增益卡ID
    char szBenefitCardDesc[64];     // 增益卡描述
    int nBenefitCardType;           // 1为金币增益卡；2为角色经验增益卡；3为英雄经验增益卡
    int nBenefitCardSubType;        // 1为时间卡，2为次数卡
    int nBenefitValue;              // 如果是时间卡，该参数为天数；如果是次数卡，该参数为次数
    int nMultiple;                  // 增益倍数
    int nPriceID;                   // 商城价格ID

    SBenefitCardConfig_S()
    {
        memset(this, 0, sizeof(*this));
    }
};

// 皇冠配置
#define CROWGEMSTONE_SCHEME_FILENAME                    "Scp\\CrownGemstoneInfo.csv"
#define CROWGEMSTONE_EFFECT_SCHEME_FILENAME             "Scp\\CrownGemstoneEffect.csv"
#define CROWGEMSTONE_COBSTONE_RANDOM_SCHEME_FILENAME    "Scp\\CrownCobblestonRandom.csv"
#define CROWGEMSTONE_RANDOM_SCHEME_FILENAME             "Scp\\CrownGemstoneRandom.csv"
#define CROWGEMSTONE_EXTERIOR_SCHEME_FILENAME           "Scp\\CrownGemstoneExterior.csv"
#define CGI_STRING_ARRAY_LEN                            32
#define CGI_LEVEL_COUNT_MAX                             7    /*等级最大组序号,要和EGGT_MAX一致*/
#define CROWN_GEMSTONE_RANDOMGROUP_MAX                  10   /* 宝石ID组最大组数 */
#define CROWN_GEMSTONE_RANDOMID_MAX                     500
#define CROWN_GEMSTONE_APPRAISALCRIT_MAX                3    /* N倍暴击率最大数量 */

struct SCrownGemstoneAppraisalCrit
{
    BYTE nCritNum;          // 暴击
    int  nCritRate;         // 暴击率
};
struct SCrownGemstoneConfig_S
{
    int nGemstoneID;                                // 宝石类型ID
    char  chGemstoneName[GAME_MSGSTR_MAXSIZE];        // 宝石名称
    int nCobGroupID;                                // 原石id组
    int nGradeLimitUpper;                           // 宝石品级上限
    int nExpLimitUpper;                             // 经验上限
    char  stExpArray[CGI_STRING_ARRAY_LEN];         // 升级品级所需经验组
    char  stPropertyIDArray[CGI_LEVEL_COUNT_MAX][CGI_STRING_ARRAY_LEN];          // 不同品级对应不同的属性ID组
    int   nExteriorIDArray[CGI_LEVEL_COUNT_MAX];    // 宝石等级外观ID组
    char  stSeilMoneyArray[CGI_STRING_ARRAY_LEN];   // 出售价格组
    int nAppraisalProcessMax;                       // 鉴定进度上限
    int nAppraisalCostMoney;                        // 鉴定消耗金币
    int nAppraisalCostTicket;                       // 鉴定消耗点卷
    int nAppraisalProcess;                          // 鉴定增加进度
    SCrownGemstoneAppraisalCrit appraisalCritArray[CROWN_GEMSTONE_APPRAISALCRIT_MAX];   // 鉴定暴击率个数

    SCrownGemstoneConfig_S()
    {
        memset(this, 0, sizeof(*this));
    }

    void Clear()
    {
        memset(this, 0, sizeof(*this));
    }
};

struct SCrownGemstoneEffectConfig_S
{
    int nPeopertyID;                                        // 宝石属性id
    int nPropertyValue;                                     // 属性威力
    char  stEffectIDArray[CGI_STRING_ARRAY_LEN];            // 效果组
    char  stEffectValueArray[CGI_STRING_ARRAY_LEN];         // 效果值组

    SCrownGemstoneEffectConfig_S()
    {
        memset(this, 0, sizeof(*this));
    }

    void Clear()
    {
        memset(this, 0, sizeof(*this));
    }
};

struct SCrownGemstoneRandomItemConfig_S
{
    vector<int>  GemStoneIDArray;                               // 宝石id组列表
    int   nGemstoneCount;                                       // 宝石id对应数量

    SCrownGemstoneRandomItemConfig_S() : nGemstoneCount(0)
    {
    }
    void Clear()
    {
        nGemstoneCount = 0;
        GemStoneIDArray.clear();
    }
    int Get(int nIndex)
    {
        if (nIndex > nGemstoneCount || nIndex >= GemStoneIDArray.size())
            return 0;
        return GemStoneIDArray[nIndex];
    }
    int Size()
    {
        return GemStoneIDArray.size();
    }
};

struct SCrownGemstoneRandomConfig_S
{
    int   nGemstoneGroupID;                                     // 宝石组id
    int   nOpenWinCost;                                         // 开窗花费
    int   nAdditionWinCost;                                     // 开窗叠加费用
    SCrownGemstoneRandomItemConfig_S   gemStoneIDArray[CROWN_GEMSTONE_RANDOMGROUP_MAX];     // 宝石组列表
    
    SCrownGemstoneRandomConfig_S()
        : nGemstoneGroupID(0)
    {
        Clear();
    }
    ~SCrownGemstoneRandomConfig_S()
    {
        Clear();
    }

    void Clear()
    {
        nGemstoneGroupID = nOpenWinCost = nAdditionWinCost = 0;
        for (int i = 0; i < CROWN_GEMSTONE_RANDOMGROUP_MAX; ++i)
        {
            gemStoneIDArray[i].Clear();
        }
    }
};

struct SCrownCobstoneRandomItemConfig_S
{
    int   nCobstoneIDLower;                                     // ID组下限
    int   nCobstoneIDUpper;                                     // ID组上限
    int   nCobstoneIDRandomRate;                                // 概率

    SCrownCobstoneRandomItemConfig_S()
    {
        memset(this, 0, sizeof(*this));
    }

    void Clear()
    {
        memset(this, 0, sizeof(*this));
    }
};

struct SCrownCobstoneRandomConfig_S
{
    int   nCobstoneGroupID;                                     // 原石组id
    SCrownCobstoneRandomItemConfig_S   cobstoneRandomArray[CROWN_GEMSTONE_RANDOMGROUP_MAX]; // 原石列表

    SCrownCobstoneRandomConfig_S()
    {
        memset(this, 0, sizeof(*this));
    }

    void Clear()
    {
        memset(this, 0, sizeof(*this));
    }
};

struct SCrownGemstoneExteriorConfig_S
{
    int nExteriorID;                                                // 外观ID
    int nFacescore;                                                 // 颜值

    SCrownGemstoneExteriorConfig_S()
    {
        memset(this, 0, sizeof(*this));
    }

    void Clear()
    {
        memset(this, 0, sizeof(*this));
    }
};

#define WARENDSUPPORTVOTE_SCHEME_FILENAME               "Scp\\WarEndSupportVote.csv"
#define MAX_SUPPORTVOTE_LABEL_NUM 7
struct SWarEndSupportVoteConfig_S
{
    int nVoteID;                                            // 点赞评选ID
    int nLabelIDList[MAX_SUPPORTVOTE_LABEL_NUM];            // 标签个数
    int nParam1;                                            // 评选参数1

    SWarEndSupportVoteConfig_S()
    {
        memset(this, 0, sizeof(*this));
    }
};

#define HERXPSKILL_SCHEME_FILENAME              "Scp\\HeroXPSkill.csv"
enum EMXPSkillType
{
    EMXP_SKILLTYPE_GENERAL  = 0,    // 通用
    EMXP_SKILLTYPE_EXCLUSIVE,       // 专属

    EMXP_SKILLTYPE_MAX
};

struct SHeroXPSkillConfig_S
{
    int nHeroID;                                            // 英雄ID
    int nXPSkillID;                                         // XPID
    int nXPType;                                            // XP类型(0通用 1 专属)
    bool bAutoActive;                                       // 是否自动激活 
    int nLockXPFragmentNum;                                 // 解锁xp所需碎片数
    bool bDefualtEquip;                                     // 解锁后默认装备
    SHeroXPSkillConfig_S()
    {
        memset(this, 0, sizeof(*this));
    }
};

#define  ECONOMYCONTROL_SCHEME_FILENAME "Scp\\EconomyControl.csv"
// 经济差控制类型
enum EWEcmExpCtrlRatioType
{
    EWEcmExpCtrlRatioType_None = -1,
    EWEcmExpCtrlRatioType_Sodlier = 0, // 击杀小兵加成
    EWEcmExpCtrlRatioType_Monster,     // 击杀野怪加成
    EWEcmExpCtrlRatioType_Salary,      // 工资加成
    EWEcmExpCtrlRatioType_Hero,        // 击杀英雄加成

    EWEcmCtrlRatioType_Max,         
};
struct SEconomyControlConfig_S
{
    int configWarID;
    int id;
    int aRatio;
    int bRatio;
    float nRatioType[EWEcmCtrlRatioType_Max];       // 系数类型，获得金币的方式（工资、杀小兵、杀英雄等）

    SEconomyControlConfig_S()
    {
        memset(this, 0, sizeof(*this));
    }
};

// AI经济控制配置
#define  AIECM_CTR_SCHEME_FILENAME "Scp\\AIEcmCtrl.csv"
struct SAIEcmCtrlConfig_S
{
	int configWarID;
	int matchScoreMin;
	int matchScoreMax;  
	int id;
	int aRatio;
	int bRatio;
	float nRatioType[EWEcmCtrlRatioType_Max];       // 系数类型，获得金币的方式（工资、杀小兵、杀英雄等）

	SAIEcmCtrlConfig_S()
	{
		memset(this, 0, sizeof(*this));
	}
};

// AI经济控制配置
#define  AIEXP_CTR_SCHEME_FILENAME "Scp\\AIExpCtrl.csv"
struct SAIExpCtrlConfig_S
{
	int configWarID;
	int matchScoreMin;
	int matchScoreMax;  
	int id;
	int aRatio;
	int bRatio;
	float nRatioType[EWEcmCtrlRatioType_Max];       // 系数类型，获得金币的方式（工资、杀小兵、杀英雄等）

	SAIExpCtrlConfig_S()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 英雄复活时间
#define HERORELIVETIME_SCHEME_FILENAME          "Scp\\HeroReliveTime.csv"
struct SHeroReliveTime
{
	int nConfigWarID;		// 比赛类型
	int nLevel;				// 等级
	int nReliveTime;		// 复活时间
};

#define ACTIVITY_SCHEME_FILENAME                "Scp\\Activity.csv"
enum EMActivityType
{
	EMACTIVITYTYPE_NONE,
	EMACTIVITYTYPE_WAR,
	EMACTIVITYTYPE_LOGIN,
};
struct SActivityConfig
{
	int activityID;                            // 活跃ID
	int subActivityID;                         // 子活跃类型ID
	int value;                                 // 活跃值
};


#define WEBURL_SCHEME_FILENAME                  "Scp\\WebUrl.csv"
struct SSchemeWebUrl
{
    unsigned int nID;                           // 活跃ID
    char szName[32];                            // 子活跃类型ID
    bool isClientNav;                           // 客户端导航显示
    BYTE nClientFunType;                        // 功能类型(客户端函数channel；0上发到逻辑层)
    bool isAutoLogin;                           // 是否自动登录方式
    char szPrevUrl[64];                         // 前置URL
    char szFollowUrl[64];                       // 结尾URL

    SSchemeWebUrl()
    {
        memset(this, 0, sizeof(*this));
    }
};


#pragma once