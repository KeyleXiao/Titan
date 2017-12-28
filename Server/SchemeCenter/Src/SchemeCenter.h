
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

#include "ISchemeCenter.h"


class SchemeCenter : public ISchemeCenter
{
public:
	virtual bool load();
	virtual void release();

	virtual MAP_SCHEME * getMapScheme( int mapID )
	{
		static MAP_SCHEME s_mapinfo;
		sprintf(s_mapinfo.szName,"%s","金陵城" );
		s_mapinfo.nMapID   = 1;
		s_mapinfo.nWidth   = 1024;
		s_mapinfo.nHeight  = 1024;
		s_mapinfo.nOffsetX = 0;           // 地图坐标偏移
		s_mapinfo.nOffsetY = 0;           // 地图坐标偏移
		s_mapinfo.nOffsetZ = 0;           // 地图坐标偏移
		s_mapinfo.uPathFindServiceID = 0; // 无障碍，两点直达

		return &s_mapinfo;
	}

	virtual IPathFindServiceInfo *	getPathFindServiceScheme();

	virtual ISchemeMatchRoom *		getSchemeMatchRoom();



	/** 取得皮肤配置
	@return  
	*/
	virtual ISchemeSkinInfo*						getSchemeSkinInfo();

	/** 取得人物模型对象
	@return  
	*/
	virtual ISchemePersonModel*						getSchemePersonModel();
		
	/** 取得怪物配置对象
	@return  
	*/
	virtual ISchemeMonster*							getSchemeMonster();

	/** 取得巡逻配置对象
	@return  
	*/
	virtual ISchemePatrol*							getSchemePatrol();

	/** 取得地图区域
	@return  
	*/
	virtual ISchemeAllMapsArea *                    getSchemeMapArea();

	/** 取得地图大陆
	@return  
	*/
	virtual ISchemeMapLand *                        getSchemeMapLand();

    /** 取得场景保姆接口
    @return  
    */
    virtual ISchemeSceneMatron *                    getSchemeSceneMatron();

	/** 取得地图信息
	@return  
	*/
	virtual ISchemeMapInfo *                        getSchemeMapInfo();

	/** 取得载具信息
	@return  
	*/
	virtual ISchemeTankInfo *                       getTankShemeInfo();
	/** 取得升级配置信息
	@return  
	*/
	virtual ISchemeLevelUpgrade* getLevelUpgradeSheme();

	/** 取得配置人物基础属性对象
	@return  
	*/
	virtual ISchemePersonBaseProp* getShemePersonBaseProp();


	/** 取得战场管理
	@return  
	*/
	virtual ISchemeWarManager* getSchemeWarManager();

	/** 取得怪配置
	@return  
	*/
	virtual ISchemeWarPoint* getSchemeWarPoint();

	/** 取得实体着陆BUFF配置
	@return  
	*/
	virtual ISchemeLandBuff* getSchemeLandBuff();

	/** 取得实体通用配置
	@return  
	*/
	virtual ISchemeEntity* getSchemeEntity();
    /** 取得天赋配置
	@return  
	*/
    virtual ISchemeTalent* getSchemeTalent();

	/** 取得战场人物配置
	@return  
	*/
	virtual ISchemeWarPersonConfigInfo* getSchemeWarPersonConfigInfo();

	/** 取得体力buff配置
	@return  
	*/
	virtual ISchemeStaminaBuff* getSchemeStaminaBuff();

	/** 取得魔法类型配置
	@return  
	*/
	virtual ISchemeMagicType* getSchemeMagicType();

	/** 取得技能升级配置
	@return  
	*/
	virtual ISchemeSpellUpgrade* getSchemeSpellUpgrade();

	/** 取得物品配置
	@return  
	*/
	virtual ISchemeMobaGoods* getSchemeMobaGoods();

	/** 取得技能槽配置信息
	@return  
	*/
	virtual ISchemeSpellSlot* getSchemeSpellSlot();

    // 召唤师技能信息配置表
    virtual ISchemeSummonerSkill* getSchemeSummonerSkill();

    // 技能槽位信息配置表
    virtual ISchemeSlotMapedShortcut* getSchemeSlotInfo();

    // 战场分数系数配置表
    virtual ISchemeWarEvaluate* getSchemeWarEvaluate();

    // 战场撮合段位配置表
    virtual ISchemeMatchRankConfigInfo* getSchemeMatchRankConfig();

    /** 取得翅膀配置
	@return  
	*/
	virtual ISchemeWing* getSchemeWing();

    /** 取得动态障碍配置
	@return  
	*/
	virtual IDynamicObstacleInfo* getSchemeDynamicObstacle();

	// 取得飘字提示配置信息
	virtual ISchemeSystemMessage* getSchemeSystemMessage();

#ifdef FUNCTION_TALENTPAGE_OPEN
    // 取得天赋页点配置信息
    virtual ISchemeTalentPage* getSchemeTalentPage() ;

    // 取得天赋页效果配置信息
    virtual ISchemeTalentPageEffect* getSchemeTalentPageEffect() ;
#endif
    
#ifdef FUNCTION_RUNE_OPEN
    // 获取符文信息
    virtual ISchemeRuneInfo* getSchemeRuneInfo();

    // 获取玩家的符文配置信息
    virtual ISchemeRuneConfig* getSchemeRuneConfig();

    // 获取玩家的符文页配置信息
    virtual ISchemeRunePage* getSchemeRunePage();
#endif

	// 获取玩家的英雄配置信息
	virtual ISchemeHeroTalentConfig* getSchemeHeroTalentConfig() ;

    // 取得称号配置信息
    virtual ISchemePlayerGloryInfo* getSchemeGloryInfo();

    // 取得英雄星级信息
    virtual ISchemeHeroStarData* getSchemeHeroStarData();

    // 取得初始英雄信息
    virtual ISchemeInitialHero* getSchemeInitialHeroInfo();

 	virtual ISchemeKinDataCenter* getSchemeKinDataCenter();

    // 取得帮会配置相关
    virtual ISchemeClanDataCenter* getSchemeClanDataCenter();

    // 取得AI配置列表
    virtual ISchemeAIIDTable* getAISchemeIDTable();

    // 英雄卡牌配置接口
    virtual ISchemeHeroCard* getSchemeHeroCard();

    // 获得英雄卡牌创建配置列表
    virtual ISchemeHeroCardCreat* getSchemeHeroCardCreat();

    // 获得战场系数配置信息
    virtual ISchemeMatchCoefficient* getSchemeMatchCoeff();

    // 获得金钱获得经验产出配置信息
    virtual ISchemeExpOrGoldCreate* getSchemeExpOrGoldCreate();

    // 获得战力值配置信息
    virtual ISchemeFightingCapacityConfig* getSchemeFightingCapacityConfig();

    // 获得英雄成就配置信息
    virtual ISchemeHeroAchievementConfig* getSchemeHeroAchievementConfig();

    // 获得产出限制配置信息
    virtual ISchemeOutputRestriction* getSchemeOutputRestriction();

    // 取得速度衰减配置信息
	virtual ISchemeSpeedDecay* getSchemeSpeedDecay();

	// 获得奖励配置信息
	virtual ISchemeActorPrize* getSchemeActorPrize();

	// 获得宝箱配置信息
	virtual ISchemePrizeChest* getSchemePrizeChest();

	// 获得商城配置信息
	virtual ISchemePointShop* getSchemePointShop();

    // 取得PK配置信息
    virtual ISchemePlayerKill* getSchemePlayerKill();

    // 取得解锁角色英雄奖励配置
    virtual ISchemePrizeUnlock* getSchemePrizeUnlock();

	// 取得周免英雄配置
	virtual ISchemeHeroFreeWeekly* getSchemeHeroFreeWeekly();

    // 取得AI购买物品配置
    virtual ISchemeAIBuyItem* getSchemeAIBuyItem();

	// 获得宝箱配置信息
	virtual ISchemeBenefitCard* getSchemeBenefitCard();

	// 获得皇冠主配置
	virtual ISchemeCrownGemstoneInfo* getSchemeCrownGemstoneInfo() ;

	virtual ISchemeLegendCup* getSchemeLegendCup();
	
	// 获得点赞配置
	virtual ISchemeWarEndSupportPlayer* getSchemeWarEndSupportPlayer() ;

	// 取得师徒配置相关
	virtual ISchemeMentorshipDataCenter* getSchemeMentorshipDataCenter();

	// 取得XP置相关
	virtual ISchemeHeroXpSkill* getSchemeHeroXpSkill();

	// 聊听冷却时间相关
	virtual ISchemeChatCD* getSchemeChatCD();

    /** 取得任务配置
	*/
	virtual ISchemeTask* getSchemeTask();

	// 取得战场经济控制配置
	virtual ISchemeEconomyControl* getSchemeEconomyControl();

    // 取得英雄推荐线路配置
    virtual ISchemeHeroRecommendPath* getSchemeHeroRecommendPath();

	// 取得战场强制结算配置
	virtual ISchemeWarForceEnd* getSchemeWarForceEnd();

    // 取得新手引导步骤配置配置
    virtual ISchemeGuideStep * getSchemeGuideStep();

	// 取得战场子功能配置
	virtual ISchemeWarSubFunc* getSchemeWarSubFunc();

	// 取得战场常用装备配置
	virtual ISchemeWarOftenUseEquip* getSchemeWarOftenUseEquip();

	// 取得大神推荐装配置
	virtual ISchemeWarGodRecommdEquip* getSchemeWarGodRecommdEquip();

    // 取得撮合限制
    virtual ISchemeMatchRankLimitInfo* getSchemeMatchRankLimit();

	// 取得英雄复活时间
	virtual ISchemeHeroReliveTime* getSchemeHeroReliveTime();

	// 取得活跃度配置相关
	virtual ISchemeActivityCenter* getSchemeActivityCenter();

	// 取得战场添加Buff配置
	virtual ISchemeWarAddBuff* getSchemeWarAddBuff();

	// 获取AI动态经济调整配置
	virtual ISchemeAIEcmAdjust* getAIEcmAdjust();

	// 获取本命英雄相关配置
	virtual ISchemeLifeHero* getSchemeLifeHero();

	// 获取AI经济经验控制配置
	virtual ISchemeAIEcmExpCtrl* getSchemeAIEcmExpCtrl();

	// 获取大逃杀安全区相关配置
	virtual ISchemeWarSafetyZone* getSchemeWarSafetyZone();

	// 获取随机刷怪配置
	virtual ISchemeWarRandRefreshMons* getSchemeWarRandRefreshMons();

	// 战场魔王玩法配置
	virtual ISchemeWarDevilKing* getSchemeWarDevilKing();

	// 战场魔王召唤NPC配置
	virtual ISchemeWarDevilKingNPC* getSchemeWarDevilKingNPC();

    // WebUrl配置
    virtual ISchemeWebUrl* getSchemeWebUrl();

	// 获取排位赛季时间配置
	virtual ISchemeMatchSeasonTime* getSchemeMatchSeasonTime();

	// 获取排位赛季奖励配置
	virtual ISchemeMatchSeasonPrize* getSchemeMatchSeasonPrize();

    // 推广奖励配置
    virtual ISchemeRecommPrize* getSchemeRecommPrize();
};