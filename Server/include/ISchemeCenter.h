/*******************************************************************
** 文件名:	ISchemeCenter.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	9/1/2014
** 版  本:	1.0
** 描  述:	
游戏脚本配置中心
********************************************************************/

#pragma once

#include "Vector3.h"
#include "War_ManagedDef.h"
#include "KinSchemeDef.h"

struct PersonSkinModelItem;
struct SMonsterScheme;

class	ISchemeMatchRoom;
struct SMapAreaInfo;
struct SMapLandSchemeInfo;
class  CSchemeMapInfo;
struct SMapSchemeInfo;
struct STankSchemeInfo;
struct SPersonBasePropScheme;
struct SBuffNode;
struct SchemeWarPersonConfigInfo;
struct SStaminaBuff;
struct SMagicType;
struct SSpellUpgrade;
struct SSchemeMobaGoods;
struct SSchemeRecommendGood;
struct SSchemeGoodEffectGroupType;
struct SWarEvaluathInfo;
struct SMatchRankConfigSchemeInfo;
struct SSchemePunish;
struct SWing;
struct STalentPageData;
struct STalentPageEffect;
struct SSchemeSystemMessage;
struct SWarPersonConfig;
struct PathFindServiceScheme;
// 符文
struct DynamicObstacleScheme;
struct RuneProperty;
struct RuneInfoScheme;
struct Entity_Rune_Page_Info;
struct EntityHeroRuneStore;
// 刷新类型集合
using namespace War;
//enum EWarMonsterExec;
struct SWarInfoScheme;
typedef vector<EWarMonsterExec> EWarMonsTypeVect;
struct SWarScheme;
typedef vector<SWarInfoScheme> SWarInfoSchemeVect;
struct SWarPointScheme;
typedef vector<SWarPointScheme> WarPointVect;

struct SGlorySchemeInfo;
struct EntityHeroTalentInfoVec;
struct SSpeedDecay;
struct Single_Hero_Talent_InfoEx;

struct STask;
typedef vector<STask>               TASK_VECTOR;
typedef vector<TASK_VECTOR>         SCHEME_TASK;

struct SNPC;
typedef vector<SNPC>				NPC_VECTOR;
typedef vector<NPC_VECTOR>			SCHEME_NPC;

struct SMatchRankLimitTimeSchemeInfo;
struct SMatchRankLimitHeroSchemeInfo;

struct SWarAddBuff;
class ISchemeWarDevilKing;
class ISchemeWarDevilKingNPC;

class ISchemeMatchSeasonTime;
class ISchemeMatchSeasonPrize;
struct SSchemeChatCD;

// 命名空间下声明
namespace SPELL
{
    struct SPELL_DATA;
    struct SPELL_SLOT;
}
// 使用命名空间
using namespace SPELL;
// 命名空间下声明
namespace ConditionData
{
    struct ConditionData_HP;
    struct ConditionData_SpellFlag;
}
// 使用命名空间
using namespace ConditionData;

// 地图配置信息
struct MAP_SCHEME
{
    char    szName[64];			// 地图名字
    int     nMapID;				// 地图ID
    int     nWidth;				// 地图宽度
    int     nHeight;			// 地图高度
    int     nOffsetX;           // 地图坐标偏移
    int     nOffsetY;           // 地图坐标偏移
    int     nOffsetZ;           // 地图坐标偏移
    unsigned int uPathFindServiceID;	// 使用的寻路服务ID

};

// 皮肤配置信息
struct SkinInfoScheme_S;
struct ISchemeSkinInfo
{
    /** 取得皮肤配置信息
    @param nSkinID:	皮肤ID
    @return  
    */
    virtual SkinInfoScheme_S* getSkinShemeInfo(int nSkinID) = 0;
};

struct ISchemePersonModel
{
    /** 取得职业名称
    @param wVocation: 职业ID  
    @return  
    */
    virtual const char *							getHeroName(int nHeroID) = 0;

     /** 取得职业名称
    @param wVocation: 职业ID  
    @return  
    */
    virtual BYTE							       GetHeroType(int nHeroID) = 0;

	/** 取得人物英雄指定的皮肤模型项
	@param wVocation:	英雄ID
	@param nSkinID:		皮肤ID
	@return  
	*/
	virtual const PersonSkinModelItem*				getPersonSkinModelItem(int nHeroID,int nSkinID=0) = 0;

    /** 判断配置表中是否有这个英雄，并且是否开启
    @param nHeroID:	职业ID  
    @return  
    */
    virtual bool                                    isHaveThisHero( int nHeroID ) = 0;

	// 获取配置英雄数量
	virtual int                                     getHeroCount() = 0;
	// 获取所有开放英雄列表
	virtual bool                                    getOpenHeroList(int *pOutArray, int &nCount) = 0;
};

// 怪物接口
struct ISchemeMonster
{
    /** 取得怪物配置信息
    @param nMonsterId:	怪物ID
    @return  
    */
    virtual SMonsterScheme* getMonsterShemeInfo(int nMonsterId) = 0;
};

// 巡逻配置接口
struct ISchemePatrol
{
    /** 取得巡逻
    @param nPatrolID:	巡逻配置ID
    @return  
    */
    virtual bool getPatrolShemeInfo(int nPatrolID, vector<Vector3> &vctPatrolPath) = 0;
};

// 升级经验
struct ISchemeLevelUpgrade
{
    /** 取得某等级经验配置信息
    @param nlevel:	等级
    @return  @exp 经验
    */
    virtual int GetNextLevelExp(int nlevel, int nAttributton = 0, int nExpTypeID = 0) = 0;


    /** 取得升级到某等级所需最低总经验值
    @param nlevel:	等级
    */
    virtual int GetLevelAllExp(int nlevel, int nAttributton = 0, int nExpTypeID = 0) = 0;

    /** 取得配置中最大等级配置信息
    @param 
    @return  最大等级
    */
    virtual int GetSchemeLevelUpgradeMaxLevel(int nAttributton = 0)=0;
    /** 取得配置中最小等级配置信息
    @param 
    @return  最小等级
    */
    virtual int GetSchemeLevelUpgradeMinLevel(int nAttributton = 0)=0;
};

// 区域类型定义
enum AreaType
{
    eAreaTypeNone,   
    eAreaTypeGrass,  // 草丛区域
    eAreaTypeFly,    // 可以触发飞檐走壁区域
};

// 地图区域接口
struct ISingleMapArea
{
    virtual int getAreaID( Vector3 loc ) = 0;

    // nIndex: 顶点索引 0-->n
    virtual bool getConvex( int nAreaID, int nIndex, Vector3& convex ) = 0;

	virtual void getAreas( Vector3* pResult, int& nCnt) = 0;
};

class ISchemeAllMapsArea
{
public:
    /** 返回指定地图的区域配置
    @param   
    @param   
    @return  
    */
    virtual ISingleMapArea * getMapArea( int nMapID, AreaType eType ) = 0;

    // 直接获取区域ID
    virtual int getAreaID( int nMapID, AreaType eType, Vector3 loc ) = 0;
};

// 地图大陆
class ISchemeMapLand
{
public:
    /** 取得地图着陆配置信息
    @param   lMapID ：何个地图
    @param   nModeType:EGameMode 游戏类型（这个参数应当不起过滤作用）
    @param   nCampID ：阵营ID
    @return  
    */
    virtual SMapLandSchemeInfo * GetMapLandSchemeInfo(long lMapID, BYTE nModeType, int nCampID) = 0;

	/** 取得地图着陆配置信息
	@param   lMapID ：何个地图
	@param   nModeType:EGameMode 游戏类型（这个参数应当不起过滤作用）
	@param   nCampID ：阵营ID
	@return
	*/
	virtual SMapLandSchemeInfo * GetRandMapLandSchemeInfo(long lMapID, BYTE nModeType) = 0;
};

// 地图信息
class ISchemeMapInfo
{
public:
    /** 取得地图配置信息
    @param   
    @param   
    @return  
    */
    virtual SMapSchemeInfo * GetMapSchemeInfo(long lMapID) = 0;

    /** 取得寻路id=0地图配置信息
    @param   
    @param   
    @return  
    */
    virtual void GetMapSchemeInfoNoPathID(vector<SMapSchemeInfo*>& vecRet) = 0;

    /** 格式化地图特殊配置信息
    @param   
    @param   
    @return  
    */
    virtual bool FormatMapSchemeInfo(void) = 0;

    // 获取所有地图ID
    virtual void getMapIDList(int* pOutArray, int& nLen) = 0;
};

// 地图信息
class ISchemeWarManager
{
public:
    // 通过静态ID和类型，获得当前怪物的的列表
    virtual SWarInfoSchemeVect* GetWarInfoSchemeMap(int nStaticID, EWarMonsterExec eType) = 0;
    // 通过静态ID获得当前战场信息
    virtual SWarScheme GetWarScheme(int nStaticID) = 0;
    // 获得刷新类型
    virtual EWarMonsTypeVect* GetWarEMonstTypeVect(int nStaticID) = 0;
};

// 地图信息
class ISchemeWarPoint
{
public:
    // 通过序列号，获得怪物信息
    virtual WarPointVect GetWarPointByIndex(int nIndex) = 0;
};

// 寻路服务信息
class IPathFindServiceInfo
{
public:
    /** 取得配置信息
    @param   
    @param   
    @return  
    */
    virtual PathFindServiceScheme * GetSchemeInfo(unsigned int uPathFindID) = 0;

    virtual void	GetAllID(vector<unsigned int>& vecRet) = 0;
    virtual void	GetAllPath2ID(map<string, unsigned int>& ret) = 0;

};

// 动态障碍配置信息
class IDynamicObstacleInfo
{
public:
    /** 取得配置信息
    @param   
    @param   
    @return  
    */
    virtual DynamicObstacleScheme * GetSchemeInfo(int nDynamicMonsterID) = 0;
};

// 符文信息
class ISchemeRuneInfo
{
public:
    /** 取得配置信息
    @param   
    @param   
    @return
    */
    virtual RuneInfoScheme * GetRuneSchemeInfo(int nRuneID) = 0;
};
// 玩家符文配置信息
class ISchemeRuneConfig
{
public:
    /** 取得配置信息
    @param
    @param
    @return
    */
    virtual void GetRuneInfoByID(DWORD dPlayerID, 
        // <RUNE_INFO, 该类型符文数量>
        std::map<int, EntityHeroRuneStore> &mPlayerRuneConfig) = 0;
};
// 玩家符文页配置信息
class ISchemeRunePage
{
public:
    /** 取得配置信息
    @param
    @param
    @return
    */
    virtual void GetRunePageByID(DWORD dPlayerID, 
        // <RUNE_INFO, 该类型符文数量>
        std::map<int, Entity_Rune_Page_Info> &pRunePage) = 0;
};
// 玩家新英雄天赋配置信息
class ISchemeHeroTalentConfig
{
public:
    /** 取得配置信息
    @param
    @param
    @return
    */
    virtual std::map<int, std::vector<Single_Hero_Talent_InfoEx> >* GetHeroTalentInfo() = 0;

	// 返回本英雄[0,nStarLv]星级的所有激活天赋列表
	// @param nStarLv: 升星等级
	virtual vector<int> getAllHeroStarActiveTalent(int nHeroID, int nStarLv) = 0;

	// 获取英雄天赋信息
	virtual Single_Hero_Talent_InfoEx* getHeroTalent(int nHeroID, int nTalentID) = 0;
};
// Tank接口
struct ISchemeTankInfo
{
    /** 取得怪物配置信息
    @param nTankId:	怪物ID
    @return  
    */
    virtual STankSchemeInfo* getTankShemeInfo(int nTankId) = 0;
};


// 角色基础属性
struct ISchemePersonBaseProp
{
    /** 取得人物基础属性配置信息
    @param nVocation:	职业ID
    @return 人物基础属性配置信息
    */
    virtual SPersonBasePropScheme* getPersonBaseProp(int nVocation, int nPropType) = 0;
};

// 天赋页数据
struct ISchemeTalentPage
{
    /** 取得天赋页属性配置信息
    @param nTalentId:	天赋ID
    @return 天赋页属性配置信息
    */
    virtual STalentPageData* getTalentPageSchemeInfo(int nTalentId)=0;

};

// 天赋页效果
struct ISchemeTalentPageEffect
{
    /** 取得天赋页效果配置信息
    @param nTalentArrayId:	天赋组ID
    @return 天赋页效果配置信息
    */
    virtual STalentPageEffect* getTalentEffectSchemeInfo(int nTalentArrayId)=0;
};

// 实体着陆BUFF配置
struct ISchemeLandBuff
{
    /** 取得着陆对应的BUFF配置信息
    @param nLandSpeed:	着陆速度
    @return  
    */
    virtual SBuffNode*  GetBuffNode(int nLandSpeed) = 0;
};

struct SEntityXml_Server;
// 实体通用配置
struct ISchemeEntity
{
    virtual SEntityXml_Server *getEntityXml(void) = 0;
};
struct STalent;
struct STalentEffectBase; 

// 天赋配置
struct ISchemeTalent
{
    // 获取某个职 某个等级激活的天赋列表
    virtual void GetVocationTalentListByLevel(int nMatchType, int nMapID, int nVocation, int nLevel, int* pResultArray, int& nLen) = 0;

    // 获取天赋
    virtual const STalent* GetTalent(int nTalentID) = 0;

    // 获取天赋效果
    virtual const STalentEffectBase* GetTalentEffect(int nTalentEffectID) = 0;

    // 获取某个天赋的天赋效果列表
    virtual void GetEffectsOfTalent(int nTalentID,  const STalentEffectBase** pArrayResult, int& nLen) = 0;
};

// 实体通用配置
struct ISchemeWarPersonConfigInfo
{
    /** 取得战场相关配置
    */
    virtual const SchemeWarPersonConfigInfo* GetSchemeWarPersonConfigInfo(int WarID) = 0;

    // 获得战场其他附带配置
    virtual SWarPersonConfig* GetWarPersonConfig() = 0;
};

// 实体通用配置
struct ISchemeMatchRankConfigInfo
{
    /** 取得撮合匹配配置
    */
    virtual const SMatchRankConfigSchemeInfo* getMatchRankConfigShemeInfo(int eMatchTypeID, int nGradeId) = 0;


    /** 根据得分取得撮合配置信息
    */
    virtual SMatchRankConfigSchemeInfo* getShemeInfoByTypeAndScore(int eMatchTypeID, int nScore) = 0;

    /** 根据得分取得撮合配置信息
    */
    virtual SMatchRankConfigSchemeInfo* getShemeHideInfoByTypeAndScore(int eMatchTypeID, int nScore) = 0;

    /** 取得王者分段最低等级分
    */
    virtual int getMatchTypeKingRankBaseScore(int eMatchTypeID) = 0;

	/** 取得王者分段最低等级
	*/
	virtual int getMatchTypeKingRankBaseGrade(int eMatchTypeID) = 0;
};


struct ISchemeMatchRankLimitInfo
{
    /** 取得撮合类型Rank开放时间限定
    */
    virtual bool matchTypeRankIsOpen(int eMatchTypeID, int nDay, int nHour, int nMinute) = 0;

    /** 获取开放失败描述
    */
    virtual int getTimeOpenFailedDesID(int eMatchTypeID) = 0;


    /** 取得撮合类型英雄限定配置
    */
    virtual SMatchRankLimitHeroSchemeInfo* getRankLimitHeroShemeInfo(int eMatchTypeID) = 0;

    /** 取得撮合英雄限定相关
    */
    virtual int getMatchHeroFailedDesID(int eMatchTypeID, bool isSelf) = 0;

    /** 取得英雄等级失败信息
    */
    virtual int getMatchPlayerLevelFailedDesID(int eMatchTypeID, bool isSelf) = 0;

	/** 取得惩罚类型配置信息
	*/
	virtual SSchemePunish* getPunishInfo(BYTE byPunishID) = 0;
};


// 体力buff配置
struct ISchemeStaminaBuff
{
    /** 取得体力buff配置
    @param nVocation:	职业ID
    @return 体力buff配置
    */
    virtual SStaminaBuff* getStaminaBuff(int nVocation) = 0;
};

// 魔法类型配置
struct ISchemeMagicType
{
    /** 取得魔法类型配置信息
    @param nType:	类型
    @return 魔法类型配置信息
    */
    virtual SMagicType* getMagicType(int nType) = 0;
};

// 技能升级配置信息
struct ISchemeSpellUpgrade
{
    /** 取得技能升级配置信息
    @param nVocation:	职业
    @param vSpellUpgrade:	技能升级容器数据
    @return 技能升级配置信息
    */
    virtual void getSpellUpgrade(int nVocation, vector<SSpellUpgrade> &vSpellUpgrade) = 0;
};

// 战场物品接口
struct ISchemeMobaGoods
{
    /** 取得物品配置信息
    @param 物品ID
    @return  
    */
    virtual SSchemeMobaGoods* getSchemeMobaGoodsInfo(int nGoodID) = 0;

    /** 取得属性配置信息
    @param 物品ID
    @return  
    */
    virtual SSchemeGoodEffectGroupType* getSchemeMobaGoodsPropInfo(int nEffectID) = 0;

    /** 取得物品合成信息
    @param 物品ID
    @return  
    */
    virtual void getSchemeNeedComposeList(int nGoodID, int *NeedComposeList, int &nCount) = 0;	

    /** 取得物品技能能却信息
    @param 物品ID
    @return  
    */
    virtual int getGoodsSkillFreeze(int nGoodsSkillID) = 0;

    /** 取得推荐物品信息
    @param 物品ID
    @return  
    */
    virtual SSchemeRecommendGood* getRecommandGoods(int nHeroID, int nMatchType) = 0;
};

// 技能槽配置信息
struct ISchemeSpellSlot
{
    /** 取得技能槽配置信息
    @param nVocation:	职业
    @return 技能槽配置信息
    */
    virtual SPELL_SLOT *getSpellSlot(int nVocation) = 0;

    /** 取得技能槽位索引
    @param nVocation:	职业
    @param nSpellID:	技能ID
    @return 技能槽位索引
    */
    virtual int getSpellSlotIndex(int nVocation, int nSpellID) = 0;
};


// 召唤师技能配置信息
struct ISchemeSummonerSkill
{

    // 根据技能id获得iconId
    virtual int getIconId( int nSkillId ) = 0;
};

// 快捷键映射
struct ISchemeSlotMapedShortcut
{
    // 获得技能类型对应的槽位
    virtual int getSlotAccordSkillType( int nSkillType ) = 0;
};

// 战场评估 系数配置表
struct ISchemeWarEvaluate
{
    // nId 位战场id 默认为1战场
    virtual SWarEvaluathInfo* getEvaluathInfo( int nMatchTypeID = 1) = 0;
};

// 翅膀配置
struct ISchemeWing
{
    /** 取得翅膀配置信息
    @param nID:	翅膀ID
    @return 翅膀配置信息
    */
    virtual SWing* getWing(int nID) = 0;
};

struct ISchemeSystemMessage
{
    // 取得配置信息
    virtual SSchemeSystemMessage* getSystemMessageInfo(int nId) = 0;
};

struct ISchemeChatCD
{
	// 获取等级限制
	virtual int getLimitLevel(int nChannel) = 0;
	// 获取CD时间
	virtual int getCDTime(int nChannel) = 0;
	// 获取频道配置
	virtual SSchemeChatCD* getSchemeChatCD(int nChannel) = 0;
};

struct ISchemePlayerGloryInfo
{
    // 取得称号配置信息
    virtual std::map<int, SGlorySchemeInfo>* getGlorySchemeInfo() = 0;
};

struct SHeroGrowProInfo;
struct SHeroStarSchemeInfo;
struct SHeroExclusiveTalentSchemeInfo;
struct ISchemeHeroStarData
{
    // 获得英雄升级所需要的经验
    virtual int                             getUpgradeNeedExpTypeID( int nHeroId, int nStarLv) = 0;

    // 获得英雄属性增加信息
    virtual SHeroGrowProInfo*                getGrowProInfo( int nHeroID, int nStarLv) = 0;

    // 获取星级信息
    virtual SHeroStarSchemeInfo             getHeroStarData( int nHeroID, int nStarLv) = 0;

    // 获取特权信息
    virtual SHeroExclusiveTalentSchemeInfo  getHeroExclusive( int nExclusiveID) = 0;

    // 获得英雄的星级限制等级
    virtual int                             getHeroStarLvLimit(int nHeroID ) = 0;
};

struct SInitialHeroSchemeInfo;
struct ISchemeInitialHero
{
    // 获得玩家初始英雄信息
    virtual std::map<int, SInitialHeroSchemeInfo>      getInitialHero() = 0;
};

struct SClanLevel;
struct SClanConfig;
struct SClanCardSchemeInfo;
struct ISchemeClanDataCenter
{
    /// purpose: 取得帮会每级信息
    virtual SClanLevel* getLevelInfo(int nClanLevel) = 0;

    /// purpose: 取得帮会常量信息
    virtual SClanConfig* getConfig() = 0;

    /// purpose: 帮会卡牌信息
    virtual SClanCardSchemeInfo* getClanCardInfo(int nLevel) = 0;
};

struct SKinLevel;
struct SKinConfig;
struct ISchemeKinDataCenter
{
    /// purpose: 取得帮会每级信息
    virtual SKinLevel* getLevelInfo(int nClanLevel) = 0;

    /// purpose: 取得帮会常量信息
    virtual SKinConfig* getConfig() = 0;
};

struct SMentorshipConfig;
struct ISchemeMentorshipDataCenter
{
	/// purpose: 取得师徒常量信息
	virtual SMentorshipConfig* getConfig() = 0;
};

// AI配置ID表
struct ISchemeAIIDTable
{

    // 校验AI配置
    virtual bool isHeroAIValid(int nBattleFieldID, int nVocation, int nAIID) =0;

    // 获取NPC AI配置ID
    virtual int getNpcAISchemeID(int nBattleFieldID, int nNpcID) = 0;

    // 获取英雄 AI配置ID
    virtual int getHeroAISchemeID(int nBattleFieldID, int nVocation) = 0;

    // 获取英雄掉线回城AI nTime:持续时间
    virtual int getHeroDisconnectBackAI(int nBattleFieldID, int nVocation, int& nTime) = 0;
};


// 英雄卡牌配置
struct SHeroCardScheme;
// 英雄卡牌配置接口
struct ISchemeHeroCard
{
    // 取得指定卡牌的配置信息
    virtual SHeroCardScheme* getByCardID( int nCardID ) = 0;

    // 取得某个英雄配置的所有卡牌配置信息
    virtual int getCardsByHeroID(int nHeroID, const SHeroCardScheme** arrayCard, const int nArrayCount ) = 0;

    // 判断配置表中是否有此卡牌id
    virtual bool isHaveThisCard( int nCardID ) = 0;
};

struct SHeroCardCreatPro;
struct SHeroCardCreatCount;
struct ISchemeHeroCardCreat
{
    // 获得英雄卡牌获得的概率
    virtual SHeroCardCreatPro* getHeroCardCreatPro(int nPlayerLV) = 0;

    // 获得一场比赛可获得卡牌的数量配置
    virtual SHeroCardCreatCount* getHeroCardCreateCount(int nPlayerLV, int nWarType) = 0;
};

struct SMatchCoefficient;
struct ISchemeMatchCoefficient
{
    // 获得战场配置信息
    virtual SMatchCoefficient* getMatchCoeff( int nWarID ) = 0;
};

struct SExpOrGoldCreateConfig;
struct ISchemeExpOrGoldCreate
{
    // 获得经验金币产出配置信息
    virtual SExpOrGoldCreateConfig* getExpOrGoldCreateConfig( int nWarTypeID, int ExpType ) = 0;
};

struct SFightingCapacityConfig;
struct ISchemeFightingCapacityConfig
{
    // 获得战力值配置信息
    virtual const SFightingCapacityConfig* getFightConfig( int nWarTypeID ) = 0;
};

// 英雄成就得分配置信息
struct SHeroAchievementScoreConfig;
struct ISchemeHeroAchievementConfig
{
    // 获得成就配置信息
    virtual const SHeroAchievementScoreConfig* getHeroAchievementConfig( int nMatchTypeID, int nRankLv) = 0;
};

// 所有产出限制配置信息
struct SOutputRestrictionConfig;
struct ISchemeOutputRestriction
{
    virtual const SOutputRestrictionConfig* getOutputRestrictionConfig( int nMatchTypeID ) = 0;
};

// 速度衰减配置
struct ISchemeSpeedDecay
{
    /** 取得速度衰减配置信息
    @param nSpeed:	速度值
    @return 速度衰减配置
    */
    virtual SSpeedDecay* getSpeedDecay(int nSpeed) = 0;
};

// 角色奖励配置信息
struct SActorPrizeConfig_S;
struct SActorPrizeGroupConfig;
struct SActorPrizeExchangeConfig;
struct ISchemeActorPrize
{
    // 获得奖励配置信息
    virtual SActorPrizeConfig_S* getActorPrizeConfig(int nPrizeID) = 0;
    // 获得奖励组配置信息
    virtual SActorPrizeGroupConfig* getActorPrizeGroupConfig(int nPrizeGroupID) = 0;
    // 获得奖励转换配置信息
    virtual SActorPrizeExchangeConfig* getPrizeExchangeConfig(int nPrizeID) = 0;
    // 校验是否存在此奖励ID
    virtual bool isExists(int nPrizeID) = 0;
};

// 奖励宝箱配置信息
struct SPrizeChestConfig_S;
struct ISchemePrizeChest
{
    // 获得宝箱配置信息
    virtual SPrizeChestConfig_S* getPrizeChestConfig(int nChestID) = 0;
};

// PK配置
struct SPlayerKill;
struct ISchemePlayerKill
{
    /** 取得PK配置信息
    @return PK配置
    */
    virtual SPlayerKill* getPlayerKill(void) = 0;
};

// 商城配置信息
struct SPointShopGoodsConfig_S;
struct SPointShopCardListRule;
enum EMPointShopGoodsType;
struct ISchemePointShop
{
    // 获得商城物品配置信息
    virtual SPointShopGoodsConfig_S* getPointShopGoodsConfig(int nSellID) = 0;
    // 获取系统配置最大数量
    virtual int getLimitNumTotal(int nLimitID) = 0;
    // 获得物品随数量变动价格配置信息
	virtual int getPointShopNumLimitPrice(int nLimitID, int nResType, int nBoughtNum, int nNeedBuyNum) = 0;
    // 获得商城卡牌随机规则配置信息
    virtual SPointShopCardListRule* getPointShopCardListRuleConfig(int nHaveHeroNum) = 0;
	// 获得商城卡牌ID对应的出售ID
	virtual int getPointShopCardSellID(int nCardID) = 0;
	// 判断出售ID是否是商城卡牌
	virtual bool checkIfPointShopCardSellID(int nSellID) = 0;
	// 检查指定时间是否是节假日
	virtual bool checkIfHoliday(time_t lTime) = 0;
	// 获得某个类型的商品列表
	virtual bool getPointShopSellConfigList(EMPointShopGoodsType eType,vector<SPointShopGoodsConfig_S*> &listSellID) = 0;
};

// 解锁角色英雄奖励配置
struct SActorPrizeUnlockVec;
struct SHeroLevelPrizeUnlockVec;
struct SHeroStarPrizeUnlockVec;
typedef std::map<int, SHeroLevelPrizeUnlockVec> SHeroLevelPrizeUnlockMap;
typedef std::map<int, SHeroStarPrizeUnlockVec> SHeroStarPrizeUnlockMap;
struct ISchemePrizeUnlock
{
    /** 解锁全部英雄等级奖励配置
    @return success or fail
    */
    virtual SHeroLevelPrizeUnlockMap*  getHeroLevelPrizeUnlockInfo()  = 0;
    /** 解锁角色奖励配置
    @return success or fail
    */
    virtual SActorPrizeUnlockVec* getActorPrizeUnlockInfo()  = 0;
	/** 解锁全部英雄升星奖励配置
    @return success or fail
    */
    virtual SHeroStarPrizeUnlockMap*  getHeroStarPrizeUnlockInfo()  = 0;
};

// 周免英雄配置
struct SHeroFreeWeeklyScheme;
// 周免英雄配置接口
struct ISchemeHeroFreeWeekly
{
	// 取得周免英雄列表
	virtual int GetFreeHeroListByLv( int arrayFreeHero[], int arrayLen, int nLv ) = 0;

	// 检测本次周免列表是否改变
	virtual bool IsFreeHeroListChanged(int nLv) = 0;
};

// 场景保姆配置
struct SSchemeSceneMatron;
// 场景保姆配置接口
struct ISchemeSceneMatron
{
	// 目标地图是否禁止使用翅膀
    virtual bool IsForbitWing(int nMapID) = 0;

    // 目标地图是否禁止决斗
    virtual bool IsForbitPK(int nMapID) = 0;

    // 目标地图是否禁止切换英雄
    virtual bool IsForbitSwitchHero(int nMapID) = 0;

    /** 实体右键菜单（0不开启，1开启）
    @param   nMapID：地图ID
    @return
    */
    virtual bool IsEnableEntityRightMenu(int nMapID) = 0;

    /** 获取快捷键方案调用序号（场景脚本配置）
    @param   nMapID：地图ID
    @return
    */
    virtual int GetHotkeyIndex(int nMapID) = 0;

    /** 是否开启加速跑
    @param   nMapID：地图ID
    @return
    */
    virtual bool CanAccelerate(int nMapID) = 0;

    // 取得地图对应的场景保姆配置信息
    virtual SSchemeSceneMatron* getByMapID( int nMapID ) = 0;
};

/////////////////////////////////////////////////////////////////
// AI购买物品配置
#define AI_BUY_PROGRESS_MAX 32
#define AI_SELL_ONCE_MAX_COUNT 4
#define AI_MAX_BUY_CONSUMABLES 4
struct SBuyScheme
{
    struct SBuyOp  // 一次买入
    {
        short arraySellItem[AI_SELL_ONCE_MAX_COUNT]; // 卖出物品列表
        short nBuyItem;                              // 买入物品
    };

    // 出装进度表
    SBuyOp arrayBuyProgress[AI_BUY_PROGRESS_MAX];

    // 消耗品
    short arrayConsumables[AI_MAX_BUY_CONSUMABLES];

    SBuyScheme()
    {
        memset(this, 0, sizeof(*this));
    }
};

struct ISchemeAIBuyItem
{
    // 获的AI的购买物品配置
    virtual SBuyScheme* getBuyScheme(int nAISchemeID) = 0; 
};

struct PathRecommendHeroNum
{
    int nPathID;
    int nRecommendHeroNum;

    PathRecommendHeroNum()
    {
        memset(this, 0, sizeof(*this));
    }
};

struct ISchemeHeroRecommendPath
{
    // 获取英雄的推荐线路
    virtual int getRecommendPath(int nHeroID, int nBattleField) = 0;

    // 获得某个战场的PathRecommendHeroNum列表
    virtual bool getPathList(int nBattleField, PathRecommendHeroNum* pResultBuf, int& nRefCount) = 0;
};


// 增益卡配置信息
struct SBenefitCardConfig_S;
struct ISchemeBenefitCard
{
	// 获得宝箱配置信息
	virtual SBenefitCardConfig_S* getBenefitCardConfig(int nChestID) = 0;
};

// 玩家皇冠宝石配置信息
struct SCrownGemstoneConfig_S;
struct SCrownGemstoneEffectConfig_S;
struct SCrownGemstoneRandomConfig_S;
struct SCrownCobstoneRandomConfig_S;
struct SCrownGemstoneExteriorConfig_S;
class ISchemeCrownGemstoneInfo
{
public:
	// 获得皇冠宝石配置信息
	virtual SCrownGemstoneConfig_S* getCrownGemstoneConfig(int nGemstoneID) = 0;

	// 获得皇冠宝石效果配置信息
	virtual SCrownGemstoneEffectConfig_S* getCrownGemstoneEffectConfig(int nPeopertyID) = 0;

	// 获得皇冠宝石随机配置信息
	virtual SCrownGemstoneRandomConfig_S* getCrownGemstoneRandomConfig(int nGemstoneGroupID)=0;

	// 获得皇冠原石随机配置信息
	virtual SCrownCobstoneRandomConfig_S* getCrownCobstoneRandomConfig(int nCobstoneGroupID)=0;

	// 获得皇冠外观配置信息
	virtual SCrownGemstoneExteriorConfig_S* getCrownCobstoneExteriorConfig(int nExteriorID)=0;
};

struct SSchemeLegendCup;
struct SSchemeLegendCupDetail;
struct SSchemeLegendCupLimit;
struct ISchemeLegendCup
{
public:

    /// purpose: 取得战队常量信息
    virtual SSchemeLegendCup* getConfig(int nConfigID) = NULL;

    virtual SSchemeLegendCupDetail* getDetailInfo(int nConfigID, int nRoundID) = NULL;

    virtual int getRoundIDByCount(int nConfigID,  int dwKinCount) = NULL;

    virtual SSchemeLegendCupLimit* getLimitConfig(int nConfigID) = NULL;
};

// 玩家点赞系统配置
struct SWarEndSupportVoteConfig_S;
class ISchemeWarEndSupportPlayer
{
public:
	// 获得点赞配置信息
	virtual bool getEndSupportPlayerVoteInfoList(SWarEndSupportVoteConfig_S supportVoteList[]) = 0;

	// 是否含有nVoteID对应的标签ID
	virtual bool isHasLabelID(int nVoteID, int nLabelID) = 0;

	// 随机一个标签
	virtual int getRandomLabel(int nVoteID) = 0;
};

// 玩家XP配置
struct SHeroXPSkillConfig_S;
class ISchemeHeroXpSkill
{
public:
	// 获得XP信息
	virtual SHeroXPSkillConfig_S* getHeroXpSkillConfig(int nHeroID,int nXPSkillID) = 0;
	// 获得自动激活XP列表
	virtual bool getHeroXpActiveList(SHeroXPSkillConfig_S activeXpSkillList[],int nListCount)=0 ;
};

// 任务配置
struct ISchemeTask
{
    /** 取得任务配置信息
    @return 任务配置信息
    */
    virtual SCHEME_TASK *getTaskScheme() = 0;
};

// 战场经济控制配置
struct SEconomyControlConfig_S;
class ISchemeEconomyControl
{
public:
	virtual SEconomyControlConfig_S* getEconomyCtrl(int nConfigWarID, int nLineID) = 0;
};

// 战场AI经济经验控制配置
struct SAIExpCtrlConfig_S;
struct SAIEcmCtrlConfig_S;
class ISchemeAIEcmExpCtrl
{
public:
	// 获取AI经济控制配置
	virtual SAIEcmCtrlConfig_S* getAIEcmCtrl(int nConfigWarID, int nMatchScore, int nLineID) = 0;
	// 获取AI经验控制配置
	virtual SAIExpCtrlConfig_S* getAIExpCtrl(int nConfigWarID, int nMatchScore, int nLineID) = 0;
};

// 战场强制结算
struct SWarForceEndInfo;
class ISchemeWarForceEnd
{
public:
	// 获取强制结算信息
	virtual SWarForceEndInfo* getWarForceEndInfo(int nMatchType) = 0;

	// 是否开启了强制结算
	virtual bool isEnableForceEnd(int nMatchType) = 0;
};

// 新手引导步骤配置接口
struct ISchemeGuideStep
{
    // 取得指定ID对应的步骤数字
    virtual int getStepByID( int nPID ) = 0;
};

//战场子功能设置
class ISchemeWarSubFunc
{
public:
	// 是否开启了某项功能
	virtual bool isEnableSubFunc(int nMatchType, int nSubFuncType) = 0;
};

// 常用装备配置
class ISchemeWarOftenUseEquip
{
public:
	// 获取基本权重
	virtual int getBaseWeight() = 0;
	// 获取衰减比例
	virtual float getDampRatio() = 0;
};

// 大神推荐装备
struct SWarGodRecommdEquip;
class ISchemeWarGodRecommdEquip
{
public:
	// 获取英雄榜第一名的推荐装备
	virtual vector<SWarGodRecommdEquip>* getGodRecommdEquip() = 0;
};

// 英雄复活时间
class ISchemeHeroReliveTime
{
public:
	virtual int getHeroReliveTime(int nConfigWarID, int nLevel) = 0;
};

// 不同类型活跃值配置中心
enum EMActivityType;
struct ISchemeActivityCenter
{
	/** 取得不同类型的活跃值
	@param activityID:	活跃值类型ID
	@param subActivityID:	活跃值子类型ID
	@return 活跃值
	*/
	virtual int getActivity(EMActivityType activityID, int subActivityID) = 0;
};

// 战场添加buf
class ISchemeWarAddBuff
{
public:
	// 获取添加Buff信息
	virtual vector<SWarAddBuff>* getAddBuffInfo(int nMatchID) = 0;
};

// AI经济动态调整
struct ISchemeAIEcmAdjust
{
	virtual float getAdjustFactor(int nMatchCount, int nEcmDiff) = 0;
};

struct SLifeHeroScheme;
struct SLifeHeroPrizeScheme;
struct ISchemeLifeHero
{
	// 获取本命英雄奖励配置
	virtual vector<SLifeHeroPrizeScheme>* getLifeHeroPrizeScehme() = 0;

	// 获取本命英雄配置
	virtual SLifeHeroScheme* getLifeHeroScehme(int i) = 0;
};

// 战场安全区配置
struct SWarSafetyZone;
class ISchemeWarSafetyZone
{
public:
	// 获取安全区信息
	virtual SWarSafetyZone* getWarSafetyZone(int nRound) = 0;
};


// 战场随机刷怪
struct SWarRandRefreshMons;
class ISchemeWarRandRefreshMons
{
public:
	// 获取该战场类型的所有随机刷怪信息
	virtual vector<SWarRandRefreshMons> getRandRefreshMonsList(int nConfigWarID) = 0;

	// 获取随机刷怪信息
	virtual SWarRandRefreshMons* getRandRefreshMons(int nMonsterIndex) = 0;
};

// 战场魔王玩法
struct SWarDevilKing;
class ISchemeWarDevilKing
{
public:
	virtual SWarDevilKing* getWarDevilKing() = 0;
};

// 战场魔王怪物NPC
struct SWarDevilKingNPC;
class ISchemeWarDevilKingNPC
{
public:
	virtual SWarDevilKingNPC* getWarDevilKingNPC(int nHeroID) = 0;
};


// WebUrl配置
struct SSchemeWebUrl;
struct  ISchemeWebUrl
{
    // 获取指定配置
    virtual SSchemeWebUrl* getByID(DWORD nID) = 0;

    // 获取所有的配置
    virtual std::map<DWORD, SSchemeWebUrl*>* getScheme() = 0;
};

// 推广员奖励配置
struct SSchemeRecommPrize;
// 推广员奖励配置接口
struct ISchemeRecommPrize
{
    // 取得推广奖励配置信息
    virtual SSchemeRecommPrize* getByTargetID(int nTargetID) = 0;
};

/////////////////////////////ISchemeCenter/////////////////////////////////////////////
/**
1.实现配置脚本的快速加载
2.实现配置脚本的热部属（即在不重启服务器的情况修改游戏配置）
3.根据发布环境，支持多种脚本加载方式（例如服务器的脚本从数据库中读取，本地不留源文件)
*/
struct ISchemeCenter
{
    virtual bool load() = 0;

    virtual void release() = 0;

    virtual MAP_SCHEME * getMapScheme( int mapID ) = 0;

    virtual IPathFindServiceInfo * getPathFindServiceScheme() = 0;

    virtual ISchemeMatchRoom *		getSchemeMatchRoom() = 0;

    /** 取得皮肤配置
    @return  
    */
    virtual ISchemeSkinInfo* getSchemeSkinInfo() = 0;

    /** 取得人物模型对象
    @return  
    */
    virtual ISchemePersonModel* getSchemePersonModel() = 0;

    /** 取得怪物配置对象
    @return  
    */
    virtual ISchemeMonster* getSchemeMonster() = 0;

    /** 取得巡逻配置对象
    @return  
    */
    virtual ISchemePatrol* getSchemePatrol() = 0;

    /** 取得地图区域
    @return  
    */
    virtual ISchemeAllMapsArea* getSchemeMapArea() = 0;

    /** 取得地图大陆
    @return  
    */
    virtual ISchemeMapLand* getSchemeMapLand() = 0;

    /** 取得地图信息
    @return  
    */
    virtual ISchemeMapInfo* getSchemeMapInfo() = 0;

    /** 取得场景保姆接口
    @return  
    */
    virtual ISchemeSceneMatron* getSchemeSceneMatron() = 0;

    /** 取得载具信息
    @return  
    */
    virtual ISchemeTankInfo* getTankShemeInfo() = 0;

    /** 取得升级配置对象
    @return  
    */
    virtual ISchemeLevelUpgrade* getLevelUpgradeSheme() = 0;

    /** 取得配置人物基础属性对象
    @return  
    */
    virtual ISchemePersonBaseProp* getShemePersonBaseProp() = 0;

    /** 取得战场管理
    @return  
    */
    virtual ISchemeWarManager* getSchemeWarManager() = 0;

    /** 取得怪配置
    @return  
    */
    virtual ISchemeWarPoint* getSchemeWarPoint() = 0;

    /** 取得实体着陆BUFF配置
    @return  
    */
    virtual ISchemeLandBuff* getSchemeLandBuff() = 0;

    /** 取得实体通用配置
    @return  
    */
    virtual ISchemeEntity* getSchemeEntity() = 0;
    /** 取得天赋配置
    @return  
    */
    virtual ISchemeTalent* getSchemeTalent() = 0;

    /** 取得战场人物配置
    @return  
    */
    virtual ISchemeWarPersonConfigInfo* getSchemeWarPersonConfigInfo() = 0;

    /** 取得体力buff配置
    @return  
    */
    virtual ISchemeStaminaBuff* getSchemeStaminaBuff() = 0;

    /** 取得魔法类型配置
    @return  
    */
    virtual ISchemeMagicType* getSchemeMagicType() = 0;

    /** 取得技能升级配置
    @return  
    */
    virtual ISchemeSpellUpgrade* getSchemeSpellUpgrade() = 0;

    /** 取得Moba物品配置信息
    @return  
    */
    virtual ISchemeMobaGoods* getSchemeMobaGoods() = 0;

    /** 取得技能槽配置信息
    @return  
    */
    virtual ISchemeSpellSlot* getSchemeSpellSlot() = 0;

    virtual ISchemeSummonerSkill* getSchemeSummonerSkill() = 0;

    virtual ISchemeSlotMapedShortcut* getSchemeSlotInfo() = 0;

    // 战场分数系数配置表
    virtual ISchemeWarEvaluate* getSchemeWarEvaluate() = 0;

    // 战场撮合段位配置表
    virtual ISchemeMatchRankConfigInfo* getSchemeMatchRankConfig() = 0;

    // 取得翅膀配置信息
    virtual ISchemeWing* getSchemeWing() = 0;

    // 取得动态障碍配置信息
    virtual IDynamicObstacleInfo* getSchemeDynamicObstacle() = 0;

    // 取得飘字提示配置信息
    virtual ISchemeSystemMessage* getSchemeSystemMessage() = 0;

#ifdef FUNCTION_TALENTPAGE_OPEN
    // 取得天赋页点配置信息
    virtual ISchemeTalentPage* getSchemeTalentPage() = 0;

    // 取得天赋页效果配置信息
    virtual ISchemeTalentPageEffect* getSchemeTalentPageEffect() = 0;
#endif

#ifdef FUNCTION_RUNE_OPEN
    // 取得符文配置信息
    virtual ISchemeRuneInfo* getSchemeRuneInfo() = 0;

    virtual ISchemeRuneConfig* getSchemeRuneConfig() = 0;

    virtual ISchemeRunePage* getSchemeRunePage() = 0;
#endif

    virtual ISchemeHeroTalentConfig* getSchemeHeroTalentConfig() = 0;
    // 取得称号配置信息
    virtual ISchemePlayerGloryInfo* getSchemeGloryInfo() = 0;

    // 取得星级相关配置信息
    virtual ISchemeHeroStarData* getSchemeHeroStarData() = 0;

    // 取得玩家初始英雄信息
    virtual ISchemeInitialHero* getSchemeInitialHeroInfo() = 0;

    // 取得战队配置相关
    virtual ISchemeKinDataCenter* getSchemeKinDataCenter() = 0;

    // 取得帮会配置相关
    virtual ISchemeClanDataCenter* getSchemeClanDataCenter() = 0;

    // 取得AI ID配置表
    virtual ISchemeAIIDTable* getAISchemeIDTable() = 0;

    // 英雄卡牌配置接口
    virtual ISchemeHeroCard* getSchemeHeroCard() = 0;

    // 获得heroCardCreateCount信息
    virtual ISchemeHeroCardCreat* getSchemeHeroCardCreat() = 0;

    // 获得战场配置信息
    virtual ISchemeMatchCoefficient* getSchemeMatchCoeff() = 0;

    // 获得金钱获得经验产出配置信息
    virtual ISchemeExpOrGoldCreate* getSchemeExpOrGoldCreate() = 0;

    // 获得战力值配置信息
    virtual ISchemeFightingCapacityConfig* getSchemeFightingCapacityConfig() = 0;

    // 获得英雄成就配置信息
    virtual ISchemeHeroAchievementConfig* getSchemeHeroAchievementConfig() = 0;

    // 获得产出限制配置信息
    virtual ISchemeOutputRestriction* getSchemeOutputRestriction() = 0;

    // 取得速度衰减配置信息
    virtual ISchemeSpeedDecay* getSchemeSpeedDecay() = 0;

    // 获得奖励配置信息
    virtual ISchemeActorPrize* getSchemeActorPrize() = 0;

    // 获得宝箱配置信息
    virtual ISchemePrizeChest* getSchemePrizeChest() = 0;

    // 取得PK配置信息
    virtual ISchemePlayerKill* getSchemePlayerKill() = 0;

    // 获得商城配置信息
    virtual ISchemePointShop* getSchemePointShop() = 0;

    // 取得解锁角色英雄奖励配置
    virtual ISchemePrizeUnlock* getSchemePrizeUnlock() = 0;

	// 取得周免英雄配置
	virtual ISchemeHeroFreeWeekly* getSchemeHeroFreeWeekly() = 0;

    // 取得AI购买物品配置
    virtual ISchemeAIBuyItem* getSchemeAIBuyItem() = 0;

	// 获得增益卡配置
	virtual ISchemeBenefitCard* getSchemeBenefitCard() = 0;

	// 获得皇冠主配置
	virtual ISchemeCrownGemstoneInfo* getSchemeCrownGemstoneInfo() = 0;

	virtual ISchemeLegendCup *	getSchemeLegendCup() = 0;
	
	// 获得点赞配置
	virtual ISchemeWarEndSupportPlayer* getSchemeWarEndSupportPlayer() = 0;

	// 取得师徒配置相关
	virtual ISchemeMentorshipDataCenter* getSchemeMentorshipDataCenter() = 0;

	// 取得XP置相关
	virtual ISchemeHeroXpSkill* getSchemeHeroXpSkill() = 0;

	virtual ISchemeChatCD* getSchemeChatCD() = 0;

    // 取得任务配置信息
    virtual ISchemeTask* getSchemeTask() = 0;

	// 取得战场经济控制配置
	virtual ISchemeEconomyControl* getSchemeEconomyControl() = 0;

    // 取得英雄推荐线路配置
    virtual ISchemeHeroRecommendPath* getSchemeHeroRecommendPath() = 0;

	// 取得战场强制结算配置
	virtual ISchemeWarForceEnd* getSchemeWarForceEnd() = 0;

    // 取得新手引导步骤配置配置
    virtual ISchemeGuideStep * getSchemeGuideStep() = 0;

	// 取得战场子功能配置
	virtual ISchemeWarSubFunc* getSchemeWarSubFunc() = 0;

	// 取得战场常用装备配置
	virtual ISchemeWarOftenUseEquip* getSchemeWarOftenUseEquip() = 0;

	// 取得大神推荐装配置
	virtual ISchemeWarGodRecommdEquip* getSchemeWarGodRecommdEquip() = 0;

    // 取得撮合限定配置
    virtual ISchemeMatchRankLimitInfo* getSchemeMatchRankLimit() = 0;

	// 取得英雄复活时间配置
	virtual ISchemeHeroReliveTime* getSchemeHeroReliveTime() = 0;

	// 取得活跃度配置相关
	virtual ISchemeActivityCenter* getSchemeActivityCenter() = 0;

	// 取得战场添加Buff配置
	virtual ISchemeWarAddBuff* getSchemeWarAddBuff() = 0;

	// 获取AI动态经济调整配置
	virtual ISchemeAIEcmAdjust* getAIEcmAdjust() = 0;

	// 获取本命英雄相关配置
	virtual ISchemeLifeHero* getSchemeLifeHero() = 0;

	// 获取AI经济经验控制配置
	virtual ISchemeAIEcmExpCtrl* getSchemeAIEcmExpCtrl() = 0;

	// 获取大逃杀安全区相关配置
	virtual ISchemeWarSafetyZone* getSchemeWarSafetyZone() = 0;

	// 获取随机刷怪配置
	virtual ISchemeWarRandRefreshMons* getSchemeWarRandRefreshMons() = 0;

	// 战场魔王玩法配置
	virtual ISchemeWarDevilKing* getSchemeWarDevilKing() = 0;

	// 战场魔王召唤NPC配置
	virtual ISchemeWarDevilKingNPC* getSchemeWarDevilKingNPC() = 0;

    // WebUrl配置
    virtual ISchemeWebUrl* getSchemeWebUrl() = 0;

	// 获取排位赛季时间配置
	virtual ISchemeMatchSeasonTime* getSchemeMatchSeasonTime() = 0;

	// 获取排位赛季奖励配置
	virtual ISchemeMatchSeasonPrize* getSchemeMatchSeasonPrize() = 0;

    // 推广奖励配置
    virtual ISchemeRecommPrize* getSchemeRecommPrize() = 0;
};


///////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(_LIB) || defined(SCHEMECENTER_STATIC_LIB)		/// 静态库版本
#	pragma comment(lib, MAKE_LIB_NAME(SchemeCenter))
extern "C" ISchemeCenter * createSchemeCenter();
#	define	createSchemeCenterProc	createSchemeCenter
#else													/// 动态库版本
typedef ISchemeCenter * (RKT_CDECL * procCreateSchemeCenter)();
#	define	createSchemeCenterProc	DllApi<procCreateSchemeCenter>::load(MAKE_DLL_NAME(SchemeCenter), "createSchemeCenter")
#endif