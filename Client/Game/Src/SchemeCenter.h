/*******************************************************************
** 文件名:	SchemeCenter.h
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

#include "ISchemeCenter.h"
#include "SchemePersonModel.h"
#include "SchemeMonster.h"
#include "SchemeTankInfo.h"

#include "SchemeMapArea.h"
#include "SchemeMapInfo.h"
#include "SchemeMatchRoom.h"
#include "SchemeTextInfo.h"
#include "SchemeEntity.h"
#include "SchemeSkinInfo.h"
#include "SchemeTalent.h"
#include "SchemeMinimap.h"
#include "SchemeAOI.h"
#include "SchemeLevelUpgrade.h"
#include "SchemeMagicType.h"
#include "SchemePathFindService.h"
#include "SchemeDieLighting.h"
#include "SchemeSpellUpgrade.h"
#include "SchemeSpellSlot.h"
#include "SchemeSlotMapedShortcut.h"
#include "SchemeWarBroadCastInfo.h"
#include "SchemeSummoner.h"
#include "SchemeMatchRankConfig.h"
#include "SchemeSystemMessage.h"
#include "SchemePreloadMonsterRes.h"
#include "SchemeWarPersonConfig.h"
#include "SchemePlayerGloryInfo.h"
#include "SchemeHeroStarData.h"
#include "SchemePersonBaseProp.h"
#include "SchemeKinDataCenter.h"
#include "SchemeClanDataCenter.h"
#include "SchemeHeroCard.h"
#include "SchemeHeroCardGradeColor.h"
#include "SchemeHeroAchievementLv.h"
#include "SchemeOutputRestriction.h"
#include "SchemePlayerKill.h"
#include "SchemeHeroFreeWeekly.h"
#include "SchemeComputerRankConfig.h"
#include "SchemeSceneMatron.h"
#include "SchemeMentorshipDataCenter.h"
#include "SchemeSkinSpell.h"
#include "SchemeAIIDTable.h"
#include "SchemeWing.h"

class SchemeCenter : public ISchemeCenter
{
public:
	virtual bool load();

	virtual void release();

	virtual ISchemeTextInfo*	getSchemeTextInfo();

	/** 取得比赛大厅房间配置
	@return  
	*/
	virtual ISchemeMatchRoom*						getSchemeMatchRoom();

	/** 取得人物模型对象
	@return  
	*/
	virtual ISchemePersonModel*						getSchemePersonModel();
	
	/** 取得怪物配置对象
	@return  
	*/
	virtual ISchemeMonster*							getSchemeMonster();

	/** 取得载具配置对象
	@return  
	*/
	virtual ISchemeTankInfo*						getSchemeTankInfo();

	/** 取得地图区域
	@return  
	*/
	virtual ISchemeMapArea* getSchemeMapArea();

	/** 取得地图信息
	@return  
	*/
	virtual ISchemeMapInfo* getSchemeMapInfo();

    /** 取得场景保姆接口
    @return  
    */
    virtual ISchemeSceneMatron * getSchemeSceneMatron();

	/** 取得实体通用配置
	@return  
	*/
	virtual ISchemeEntity* getSchemeEntity();

	/** 取得皮肤配置
	@return  
	*/
	virtual ISchemeSkinInfo* getSchemeSkinInfo();

    // 取得天赋配置
    virtual ISchemeTalent* getSchemeTalent();

    // 获取小地图配置
    virtual IMinimapScheme* getSchemeMinimap();

    virtual ISchemeAOI* getSchemeAOI();

    // 升级经验
    virtual ISchemeLevelUpgrade* getSchemeLevelUpgrade();

	/** 魔法类型接口
	@return  
	*/
	virtual ISchemeMagicType* getSchemeMagicType();

    /** 寻路服务配置接口
	@return  
	*/
    virtual IPathFindServiceInfo* getPathFindServiceScheme();

	/** 死亡光效接口
	@return  
	*/
	virtual ISchemeDieLighting* getSchemeDieLighting();

	/** 取得技能升级配置
	@return  
	*/
	virtual ISchemeSpellUpgrade* getSchemeSpellUpgrade();

	/** 取得技能槽配置信息
	@return  
	*/
	virtual ISchemeSpellSlot* getSchemeSpellSlot();

	/** 取得槽位对应快捷键配置
	@return  
	*/
	virtual ISchemeSlotMapedShortcut* getSchemeSlotMapedShortcut();

	/** 取得广播信息配置
	@return  
	*/ 
	virtual ISchemeWarBroadCastInfo* getSchemeWarBroadCastInfo();

    //取得召唤师技能信息配置
    virtual ISchemeSummonerSkill* getSchemeSummonerSkill();

    // 战场撮合段位配置表
    virtual ISchemeMatchRankConfigInfo* getSchemeMatchRankConfig();

    /** 飘字配置接口
	@return  
	*/
    virtual ISchemeSystemMessage* getSchemeSystemMessage();

    virtual ISchemePreloadMonsterRes* getSchemePreloadRes();

    /** 获取战场相关配置接口
	@return  
	*/
    virtual ISchemeWarPersonConfig* getSchemeWarPersonConfig();

    /** 获取称号相关配置接口
	@return  
	*/
    virtual ISchemePlayerGloryInfo* getSchemeGloryInfo();

    /** 获取英雄星级相关配置接口
	@return  
	*/
    virtual ISchemeHeroStarData* getSchemeHeroStarInfo();

    /** 获取英雄属性相关配置接口
	@return  
	*/
    virtual ISchemePersonBaseProp* getSchemePersonBaseProp();

    /** 取得战队配置相关
	@return  
	*/
    virtual ISchemeKinDataCenter* getSchemeKinDataCenter();

    /** 取得帮会配置相关
	@return  
	*/
    virtual ISchemeClanDataCenter* getSchemeClanDataCenter();

    virtual ISchemeHeroAchievementLvConfig* getSchemeHeroAchievementLv();

    /** 取得英雄卡牌配置相关
	@return  
	*/
    virtual ISchemeHeroCard* getSchemeHeroCard();

	/** 取得英雄卡牌等级颜色配置相关
	@return  
	*/
    virtual ISchemeHeroCardGradeColor* getSchemeHeroCardGradeColor();

    /** 取得战后奖励输出限制
	@return  
	*/
    virtual ISchemeOutputRestriction* getSchemeOutPutRestriction();

    /** 取得PK配置
	@return PK配置
	*/
	virtual ISchemePlayerKill* getSchemePlayerKill();

	/** 取得周免英雄配置
	@return  
	*/
	virtual ISchemeHeroFreeWeekly* getSchemeHeroFreeWeekly();

    /** 取得电脑虚拟rank配置
	@return  
	*/
	virtual ISchemeComputerRankConfigInfo* getSchemeComputerRankConfig();

	/** 取得师徒配置相关
	@return  
	*/
    virtual ISchemeMentorshipDataCenter* getSchemeMentorshipDataCenter();

    /** 取得皮肤技能配置
	@return 皮肤技能配置
	*/
	virtual ISchemeSkinSpell* getSchemeSkinSpell();

    /** 取得房间AI配置
	@return 
	*/
	virtual ISchemeAIIDTable* getSchemeAIIDTable();

     /** 取得翅膀配置
	@return 
	*/
    virtual ISchemeWing* getSchemeWing();
private:

	CSchemePersonModel		    m_SchemePersonModel;
	CSchemeMonster			    m_SchemeMonster;
	CSchemeTankInfo			    m_SchemeTankInfo;

	CSchemeMapArea			    m_SchemeMapArea;
	CSchemeMapInfo			    m_SchemeMapInfo;
    CSchemeSceneMatron          m_SchemeSceneMatron;

	CSchemeMatchRoom		    m_SchemeMatchRoom;
	CSchemeTextInfo			    m_SchemeTextInfo;

	CSchemeEntity			    m_SchemeEntity;
	CSchemeSkinInfo			    m_ScshemeSkinInfo;

    CSchemeAOI                  m_SchemeAOI;
	CSchemeLevelUpgrade		    m_SchemeLevelUpgrade;

	CSchemeMagicType		    m_MagicType;
    CSchemePathFindServiceInfo  m_SchemePathFindService;

	CSchemeDieLighting		    m_DieLighting;
	CSchemeSpellUpgrade		    m_SpellUpgrade;
	CSchemeSpellSlot		    m_SpellSlot;
	CSchemeSlotMapedShortcut     m_SlotMapedShortcut;
	CSchemeWarBroadCastInfo     m_WarBroadCastInfo;

    CSchemeSummonerSkill        m_SummnnerSkill;

    CSchemeMatchRankConfig      m_MatchRankConfig;
    CSchemeSystemMessage        m_SystemMessage;
    CSchemePreloadMonsterRes    m_Preload;

    CSchemeWarPersonConfig      m_WarPersonConfig;    
    CSchemePlayerGloryInfo      m_SchemeGlory;
    CSchemeHeroStarData         m_SchemeStarData;
    CSchemePersonBaseProp       m_SchemeBasePropData;
    CSchemeKinDataCenter        m_SchemeKinDataCenter;
    CSchemeClanDataCenter       m_SchemeClanDataCenter;
    CSchemeHeroCard             m_SchemeHeroCard;
    CSchemeHeroAchievementLv    m_SchemeHeroAchievementLv;
	CSchemeHeroCardGradeColor   m_SchemeHeroCardGradeColor;
    CSchemeOutputRestriction    m_SchemeOutputRestriction;
    CSchemePlayerKill           m_SchemePlayerKill;
	CSchemeHeroFreeWeekly		m_SchemeHeroFreeWeekly;
    CSchemeComputerRankConfig   m_ComputerRankConfig;
	CSchemeMentorshipDataCenter m_SchemeMentorshipDataCenter;
    CSchemeSkinSpell            m_SchemeSkinSpell;
    CSchemeAIIDTable            m_SchemeAIIDTable;
    CSchemeWing                 m_SchmeWing;    
};
