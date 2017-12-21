/*******************************************************************
** 文件名:	ISchemeCenter.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	1/7/2015
** 版  本:	1.0
** 描  述:	游戏脚本配置中心

			
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#pragma once
#include <set>

struct PersonSkinModelItem;
struct SMonsterScheme;
struct STankSchemeInfo;

class ISchemeMatchRoom;
class ISchemeTextInfo;

struct SMapAreaInfo;
struct SMapSchemeInfo;
struct SMagicType;
struct SDieLighting;
struct SSpellUpgrade;
struct SSlotMapedShortcutScheme;
struct SWarBroadCastInfoScheme;
struct SSummonerSkillInfo;
struct SMatchRankConfigSchemeInfo;
struct SSchemeSystemMessage;
struct SWarPersonConfig;
struct PathFindServiceScheme;
struct DynamicObstacleScheme;
struct SGlorySchemeInfo;
struct SStaticHeroUpgradeSchemeInfo;
struct SComputerRankConfigSchemeInfo;

namespace SPELL
{
	struct SPELL_DATA;
	struct SPELL_SLOT;
}
using namespace SPELL;

struct SVocationName;
struct ISchemePersonModel
{
	/** 取得英雄名称
	@param nHeroID: 英雄ID  
	@return  
	*/
	virtual const char *							getHeroName(int nHeroID) = 0;

	/** 取得人物英雄指定的皮肤模型项
	@param wVocation:	英雄ID
	@param nSkinID:		皮肤ID
	@return  
	*/
	virtual const PersonSkinModelItem*				getPersonSkinModelItem(int nHeroID,int nSkinID=0) = 0;

	/** 取得所有合法职业的人物皮
    @param pArray:
    @param nArrayLen:
	@return  
	*/
    virtual int                                     getAllVocation(PersonSkinModelItem * pArray, const int nArrayCount) = 0;

	/** 取得角色类型
	@param wVocation: 职业ID  
	@return  
	*/
	virtual  int							        getVocationType(int nHeroID) = 0;

	/** 取得英雄解锁等级
	@param wVocation: 职业ID  
	@return  
	*/
	virtual  int							        getUnlockLevel(int nHeroID) = 0;

	/** 取得英雄开关状态
	@param wVocation: 职业ID  
	@return  
	*/
	virtual	int								        getHeroSwitchState(int nHeroID) = 0;

	/** 取得是否fps英雄
	@param wVocation: 职业ID  
	@return  
	*/
	virtual  bool							        checkIsFpsHero(int nHeroID) = 0;
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

// Tank接口
struct ISchemeTankInfo
{
	/** 取得怪物配置信息
	@param nTankId:	TankID
	@return  
	*/
	virtual STankSchemeInfo* getTankShemeInfo(int nTankId) = 0;
};

// 技能配置
struct ISchemeSpell
{
	/** 取得技能配置信息
	@param  nSpellID		技能ID
	return  技能配置信息
	*/
	virtual SPELL_DATA *getSpellShemeInfo(int nSpellID) = 0;
};

// 地图区域
class ISchemeMapArea
{

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

	/** 格式化地图特殊配置信息
	@param   
	@param   
	@return  
	*/
	virtual bool FormatMapSchemeInfo(void) = 0;
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

struct SEntityXml_Client;
// 实体通用配置
struct ISchemeEntity
{
    virtual SEntityXml_Client *getEntityXml(void) = 0;
};

// 战场相关配置
struct ISchemeWarPersonConfig
{
	// 取得战场相关配置
	virtual const SchemeWarPersonConfigInfo* GetSchemeWarPersonConfigInfo(int WarID) = 0;

    virtual SWarPersonConfig* GetSchemeWarPersonConfigInfo() = 0;
};

// 皮肤配置信息
struct SkinInfoScheme;
struct ISchemeSkinInfo
{
	/** 取得皮肤配置信息
	@param nSkinID:	皮肤ID
	@return  
	*/
	virtual SkinInfoScheme* getSkinShemeInfo(int nSkinID) = 0;
};

struct STalent;
struct STalentEffectBase; 
// 天赋配置
struct ISchemeTalent
{
    // 获取某个职业的天赋列表
    virtual const std::set<int>& GetVocationTalentList(int nVocation) = 0;

    // 获取某个职 某个等级激活的天赋列表
    virtual const std::set<int>& GetVocationTalentListByLevel(int nVocation, int nLevel) = 0;

    // 获取天赋
    virtual const STalent* GetTalent(int nTalentID) = 0;

    // 获取天赋效果
    virtual const STalentEffectBase* GetTalentEffect(int nTalentEffectID) = 0;

    // 获取某个天赋的天赋效果列表
    virtual void GetEffectsOfTalent(int nTalentID, std::vector<const STalentEffectBase*>& refOut) = 0;
};

// 小地图配置
struct IMinimapScheme
{
    // 获取小地图信息
    virtual void* GetMinimapInfo(int nMapID) = 0;

    // 获取小地图图标信息
    virtual void* GetMinimapIconInfo(int nIconID) = 0;

    // 根据地图ID、静态打点序号 获得小地图静态打点信息
    virtual void* GetMinimapStaticIcon(int nMapID, int nIndex) = 0; 

    // 根据序号，获取下一个静态打点信息
    virtual void* GetNextMinimapStaticIcon(int nMapID, int nFromIndex) = 0;
};

struct ISchemeAOI
{
    // 获取AOI警戒距离
    virtual float getGuardDistance() = 0;

    // 获取警戒缓冲时间
    virtual int getGuardBufTime() = 0;
};


// 升级经验
struct ISchemeLevelUpgrade
{
	/** 取得某等级经验配置信息
	@param nlevel:	等级
	@return  @exp 经验
	*/
	virtual int					GetNextLevelExp(int nlevel, int nAttributton = 0, int nExpTypeID = 0) = 0;
	
	/** 取得升级到某等级所需最低总经验值
	@param nlevel:	等级
	*/
	virtual int					GetLevelAllExp(int nlevel, int nAttributton = 0, int nExpTypeID = 0) = 0;

	/** 取得配置中最大等级配置信息
	@param 
	@return  最大等级
	*/
	virtual int					GetSchemeLevelUpgradeMaxLevel(int nAttributton = 0)=0;
	/** 取得配置中最小等级配置信息
	@param 
	@return  最小等级
	*/
	virtual int					GetSchemeLevelUpgradeMinLevel(int nAttributton = 0)=0;	
};


// 魔法类型接口
struct ISchemeMagicType
{
	/** 取得魔法类型配置信息
	@param nType:	类型
	@return 魔法类型配置信息
	*/
	virtual SMagicType* getMagicType(int nType) = 0;
};


// 死亡光效接口
struct ISchemeDieLighting
{
	/** 取得死亡光效配置信息
	@param nVocation:	职业ID,怪物ID
	@param nType:		实体类型
	@return 死亡光效配置信息
	*/
	virtual SDieLighting* getDieLighting(int nVocation, int nType) = 0;
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


// 技能槽配置信息
struct ISchemeSpellSlot
{
	/** 取得技能槽配置信息
	@param nVocation:	职业
	@return 技能槽配置信息
	*/
	virtual SPELL_SLOT *getSpellSlot(int nVocation) = 0;
};
struct ISchemeSlotMapedShortcut
{
	/** 取得皮肤配置信息
	@param nSkinID:	皮肤ID
	@return  
	*/
	virtual SSlotMapedShortcutScheme* getSlotMapedShortcutShemeInfo(int nSlotIndex, int nSlotType) = 0;

    // 获得技能类型对应的槽位
    virtual int getSlotAccordSkillType( int nSkillType ) = 0;

    // 获得所有技能快捷键
    virtual vector<SSlotMapedShortcutScheme> getAllSlotInfo() = 0;

    // 获得某一类型的所有快捷键
    virtual vector<SSlotMapedShortcutScheme> getOneTypeSlotInfo(int nSlotType)=0;
};

struct ISchemeWarBroadCastInfo
{
	// 通过序列号，获得广播信息
	virtual SWarBroadCastInfoScheme* getWarBroadCastInfoByIndex(int nIndex) = 0;
};

struct ISchemeSummonerSkill
{
    // 获得所有天赋信息
    virtual std::map<int, SSummonerSkillInfo>* getAllSummonerSkillInfo() = 0;

	virtual std::map<int, SSummonerSkillInfo>* getSummonerSkillByWarMode(int nWarMode) = 0;

    // 根据技能id获得iconId
    virtual int getIconId( int nSkillId ) = 0;

   /** 取得召唤师技能配置信息
	@param nSkillId:	技能ID
	@return  
	*/
    virtual SSummonerSkillInfo* getSummonerSkillInfo(int nSkillId ) = 0;
};

struct ISchemeSystemMessage
{
	// 取得配置信息
    virtual SSchemeSystemMessage* getSystemMessageInfo(int nId) = 0;
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

	/** 比较两个天梯分段位的大小(0为不变，小于0为降段位，大于0为升段位)
	*/
	virtual int cmpRankGrade(int eMatchTypeID, int nPreScore, int nCurScore) = 0;

    /** 取得王者分段最低分
    */
    virtual int getMatchTypeKingRankBaseScore(int eMatchTypeID) = 0;
};

// 电脑段位胜率配置
struct ISchemeComputerRankConfigInfo
{
	/** 取得撮合匹配配置
	*/
	virtual const SComputerRankConfigSchemeInfo* getComputerRankConfigShemeInfo() = 0;

};

// AI配置ID表
struct ISchemeAIIDTable
{
    // 获取默认英雄AI配置
    virtual int getHeroAIDefault(int nBattleFieldID, int nVocation) =0;

    // 校验AI配置
    virtual bool isHeroAIValid(int nBattleFieldID, int nVocation, int nAIID) =0;

    // 获取NPC AI配置ID
    virtual int getNpcAISchemeID(int nBattleFieldID, int nNpcID) = 0;

    // 是否有AI配置
    virtual bool isHaveAIConfig(int nBattleFieldID) = 0;

    // 获取NPC AI配置ID
    virtual const char* getHeroAIDes(int nBattleFieldID, int nVocation, int nAIID) = 0;
};


// 预加载怪物id配置接口
struct ISchemePreloadMonsterRes
{
    virtual void getMapAllMonster(int nMapId, std::vector<int>& allMonster) = 0;
};

// 称号配置表
struct ISchemePlayerGloryInfo
{
    // 取得称号配置信息
    virtual std::map<int, SGlorySchemeInfo>* getGlorySchemeInfo() = 0;
};

// 星级相关配置表
struct SHeroGrowProInfo;
struct SHeroStarSchemeInfo;
struct ISchemeHeroStarData
{
    // 获得英雄升级所需要的经验
    virtual int                 getUpgradeNeedExpTypeID( int nHeroId, int nStarLv) = 0;
    // 获得英雄属性增加信息
    virtual SHeroGrowProInfo*    getGrowProInfo( int nHeroID, int nStarLv) = 0;
    // 获得英雄星级信息
    virtual SHeroStarSchemeInfo* getHeroStarData( int nHeroID, int nStarLv = 1) = 0;
    // 获得英雄的星级限制等级
    virtual int                 getHeroStarLvLimit(int nHeroID ) = 0;
};

struct SPersonBasePropScheme;
// 英雄基础属性相关配置
typedef std::map<int, SPersonBasePropScheme> SCHEME_MAP;
struct ISchemePersonBaseProp
{
    // 获得所有英雄的信息
    virtual SCHEME_MAP*                 getAllHeroInfo() = 0;

    // 根据英雄id获得英雄的详细信息
    virtual SPersonBasePropScheme       getOneHeroDetailInfo(int nHeroID) = 0;
    
};

struct SClanLevel;
struct SClanConfig;
struct ISchemeClanDataCenter
{
    /// purpose: 取得帮会每级信息
    virtual SClanLevel* getLevelInfo(int nClanLevel) = 0;

    /// purpose: 取得帮会常量信息
    virtual SClanConfig* getConfig() = 0;
};

/** 取得战队配置相关
	@return  
	*/

struct SKinLevel;
struct SKinConfig;

// 比赛大厅房间配置接口
struct ISchemeKinDataCenter
{
public:

    /// purpose: 取得战队常量信息
    virtual SKinConfig* getConfig() = NULL;

    virtual SKinLevel* getLevelInfo(int nKinLevel) = NULL;
};

// 英雄卡牌配置
struct SHeroCardScheme;
// 英雄卡牌配置接口
struct ISchemeHeroCard
{
    // 取得指定卡牌的配置信息
    virtual SHeroCardScheme* getByCardID( int nCardID ) = 0;

	virtual string getUBBNameByCardID( int nCardID ) = 0;

	virtual string getUBBNameBySHeroCardScheme( SHeroCardScheme* pSHeroCardScheme ) = 0;

    // 取得某个英雄配置的所有卡牌配置信息
    virtual int getCardsByHeroID(int nHeroID, const SHeroCardScheme** arrayCard, const int nArrayCount ) = 0;
};

// 英雄卡牌等级颜色配置
struct SHeroCardGradeColorScheme;
// 英雄卡牌配置接口
struct ISchemeHeroCardGradeColor
{
	// 取得指定卡牌的配置信息
	virtual SHeroCardGradeColorScheme* getByCardGrade( int nCardGrade ) = 0;
};

// 英雄成就等级配置
struct ISchemeHeroAchievementLvConfig
{
    virtual int getIconIDAccordScore( int nScore, int* pArrIcon, int nCount) = 0;
};

// 战后奖励输出限制
struct SOutputRestrictionConfig;
struct ISchemeOutputRestriction
{
    // 获得某个地图的战后奖励限制信息
    virtual const SOutputRestrictionConfig* getOutputRestrictionConfig( int nMatchTypeID ) = 0;

    // 获得某个阵营的人数限制信息
    virtual int getCampPersonNumRestriction(int nMatchTypeID, int nCamp) = 0;

    // 获得总人数限制
    virtual int getMatchAllPersonRestriction(int nMatchTypeID) = 0;
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

// 周免英雄配置
struct SHeroFreeWeeklyScheme;
// 周免英雄配置接口
struct ISchemeHeroFreeWeekly
{
	// 取得周免英雄列表
	virtual int GetFreeHeroListByLv( int arrayFreeHero[], int arrayLen, int nLv ) = 0;
	// 是否为周免英雄
	virtual bool CheckIsWeekFree( const int nHeroID, const int nLv ) = 0;
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
    virtual int  GetHotkeyIndex(int nMapID) = 0;

    /** 是否开启加速跑
    @param   nMapID：地图ID
    @return
    */
    virtual bool CanAccelerate(int nMapID) = 0;

    // 取得地图对应的场景保姆配置信息
    virtual SSchemeSceneMatron* getByMapID( int nMapID ) = 0;
};

/** 取得师徒配置相关
	@return  
	*/
struct SMentorshipConfig;
struct ISchemeMentorshipDataCenter
{
public:

	/// purpose: 取得师徒常量信息
	virtual SMentorshipConfig* getConfig() = NULL;
};

// 皮肤技能
struct SSkinSpell;
struct ISchemeSkinSpell
{
	/** 取得皮肤技能配置信息
	@return 皮肤技能配置
	*/
	virtual SSkinSpell* getSkinSpell(int nSkinID, int nType, int nValue) = 0;

    /** 取得皮肤技能配置信息
    @return 皮肤技能配置
    */
    virtual int getSkinSpell(int nSkinID, int nType, SSkinSpell* arraySkinSpell, int nArrayCount) = 0;
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

		/** 取得比赛大厅房间配置
	@return  
	*/
	virtual ISchemeTextInfo*	getSchemeTextInfo() = 0;

	/** 取得比赛大厅房间配置
	@return  
	*/
	virtual ISchemeMatchRoom*	getSchemeMatchRoom() = 0;

	/** 取得人物模型对象
	@return  
	*/
	virtual ISchemePersonModel* getSchemePersonModel() = 0;

	/** 取得怪物配置对象
	@return  
	*/
	virtual ISchemeMonster* getSchemeMonster() = 0;

	/** 取得地图区域
	@return  
	*/
	virtual ISchemeMapArea* getSchemeMapArea() = 0;

	/** 取得地图信息
	@return  
	*/
	virtual ISchemeMapInfo* getSchemeMapInfo() = 0;

    /** 取得场景保姆接口
    @return  
    */
    virtual ISchemeSceneMatron * getSchemeSceneMatron() = 0;

	/** 取得载具信息
	@return  
	*/
	virtual ISchemeTankInfo* getSchemeTankInfo() = 0;

	/** 取得实体通用配置
	@return  
	*/
	virtual ISchemeEntity* getSchemeEntity() = 0;

	/** 取得皮肤配置
	@return  
	*/
	virtual ISchemeSkinInfo* getSchemeSkinInfo() = 0;

    virtual ISchemeTalent* getSchemeTalent() = 0; 

    // 获取小地图配置
    virtual IMinimapScheme* getSchemeMinimap() = 0;

    virtual ISchemeAOI* getSchemeAOI() = 0;

    // 升级经验
    virtual ISchemeLevelUpgrade* getSchemeLevelUpgrade() = 0;

	/** 魔法类型接口
	@return  
	*/
	virtual ISchemeMagicType* getSchemeMagicType() = 0;

    /** 寻路服务配置接口
	@return  
	*/
    virtual IPathFindServiceInfo* getPathFindServiceScheme() = 0;

	/** 死亡光效接口
	@return  
	*/
	virtual ISchemeDieLighting* getSchemeDieLighting() = 0;

	/** 取得技能升级配置
	@return  
	*/
	virtual ISchemeSpellUpgrade* getSchemeSpellUpgrade() = 0;

	/** 取得技能槽配置信息
	@return  
	*/
	virtual ISchemeSpellSlot* getSchemeSpellSlot() = 0;

	/** 取得槽位对应快捷键配置
	@return  
	*/
	virtual ISchemeSlotMapedShortcut* getSchemeSlotMapedShortcut() = 0;

	/** 取得广播信息配置
	@return  
	*/ 
	virtual ISchemeWarBroadCastInfo* getSchemeWarBroadCastInfo() = 0;

    virtual ISchemeSummonerSkill* getSchemeSummonerSkill() = 0;

    // 战场撮合段位配置表
    virtual ISchemeMatchRankConfigInfo* getSchemeMatchRankConfig() = 0;

    /** 飘字配置接口
	@return  
	*/
    virtual ISchemeSystemMessage* getSchemeSystemMessage() = 0;

    /** 预加载资源配置接口
	@return  
	*/
    virtual ISchemePreloadMonsterRes* getSchemePreloadRes() = 0;

    /** 获取战场相关配置接口
	@return  
	*/
    virtual ISchemeWarPersonConfig* getSchemeWarPersonConfig() = 0;

    /** 获取称号相关配置接口
	@return  
	*/
    virtual ISchemePlayerGloryInfo* getSchemeGloryInfo() = 0;

    /** 获取英雄星级相关配置接口
	@return  
	*/
    virtual ISchemeHeroStarData* getSchemeHeroStarInfo() = 0;

    /** 获取英雄属性相关配置接口
	@return  
	*/
    virtual ISchemePersonBaseProp* getSchemePersonBaseProp() = 0;

    /** 取得帮会配置相关
	@return  
	*/
    virtual ISchemeClanDataCenter* getSchemeClanDataCenter() = 0;


    /** 取得战队配置相关
	@return  
	*/
    virtual ISchemeKinDataCenter* getSchemeKinDataCenter() = 0;


    /** 取得英雄成就得分
	@return  
	*/
    virtual ISchemeHeroAchievementLvConfig* getSchemeHeroAchievementLv() = 0;

    /** 取得英雄卡牌配置相关
	@return  
	*/
    virtual ISchemeHeroCard* getSchemeHeroCard() = 0;

	/** 取得英雄卡牌等级颜色配置相关
	@return  
	*/
    virtual ISchemeHeroCardGradeColor* getSchemeHeroCardGradeColor() = 0;

    /** 取得战后奖励输出限制
	@return  
	*/
    virtual ISchemeOutputRestriction* getSchemeOutPutRestriction() = 0;

    /** 取得PK配置
	@return  
	*/
    virtual ISchemePlayerKill* getSchemePlayerKill() = 0;

	/** 取得周免英雄配置
	@return  
	*/
	virtual ISchemeHeroFreeWeekly* getSchemeHeroFreeWeekly() = 0;

    /** 取得电脑虚拟rank配置
	@return  
	*/
	virtual ISchemeComputerRankConfigInfo* getSchemeComputerRankConfig() = 0;

	/** 取得师徒配置相关
	@return  
	*/
    virtual ISchemeMentorshipDataCenter* getSchemeMentorshipDataCenter() = 0;

    /** 取得皮肤技能配置
	@return  
	*/
    virtual ISchemeSkinSpell* getSchemeSkinSpell() = 0;

    /** 取得房间AI配置
	@return 
	*/
	virtual ISchemeAIIDTable* getSchemeAIIDTable() = 0;


     /** 取得翅膀配置
	@return 
	*/
    virtual ISchemeWing* getSchemeWing() = 0;
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