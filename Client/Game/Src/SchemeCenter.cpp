/*******************************************************************
** 文件名:	SchemeCenter.cpp
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


#include "stdafx.h"
#include "SchemeCenter.h"



bool SchemeCenter::load()
{
	// 加载脚本

	// 加载人物皮肤模型数据
	if (!m_SchemePersonModel.LoadScheme())
	{
		return false;
	}

	// 加载怪物配置数据
	if (!m_SchemeMonster.LoadScheme())
	{
		return false;
	}

	// 加载怪物配置数据
	if (!m_SchemeTankInfo.LoadScheme())
	{
		return false;
	}

	// 加载地图信息
	if (!m_SchemeMapArea.LoadScheme())
	{
		return false;
	}
	if (!m_SchemeMapInfo.LoadScheme())
	{
		return false;
	}
    // 加载场景保姆信息
    if (!m_SchemeSceneMatron.LoadScheme())
    {
        return false;
    }

	if ( !m_SchemeMatchRoom.LoadScheme() )
	{
		return false;
	}
	if ( !m_SchemeTextInfo.LoadScheme() )
	{
		return false;
	}

	if ( !m_SchemeEntity.LoadScheme() )
	{
		return false;
	}
	//皮肤配置
	if ( !m_ScshemeSkinInfo.LoadScheme() )
	{
		return false;
	}

    CTalentScheme::getInstance().Load();

    CMinimapScheme::getInstance().Load();

    if ( !m_SchemeAOI.LoadScheme() )
    {
        return false;
    }

	// 加载升级经验配置数据
	if(!m_SchemeLevelUpgrade.LoadScheme())
	{
		return false;
	}

	if ( !m_MagicType.LoadScheme() )
	{
		return false;
	}

    if ( !m_SchemePathFindService.LoadScheme() )
    {
        return false;
    }

	if ( !m_DieLighting.LoadScheme() )
	{
		return false;
	}

	if ( !m_SpellUpgrade.LoadScheme() )
	{
		return false;
	}

	if ( !m_SpellSlot.LoadScheme() )
	{
		return false;
	}
	
	if ( !m_SlotMapedShortcut.LoadScheme() )
	{
		return false;
	}

	if ( !m_WarBroadCastInfo.LoadScheme() )
	{
		return false;
	}

    if ( !m_SummnnerSkill.LoadScheme())
    {
        return false;
    }
	
    if ( !m_MatchRankConfig.LoadScheme())
    {
        return false;
    }

    if ( !m_SystemMessage.LoadScheme())
    {
        return false;
    }

    if ( !m_Preload.LoadScheme() )
    {
        return false;
    }

    if ( !m_WarPersonConfig.LoadScheme() )
    {
        return false;
    }

    if ( !m_SchemeGlory.LoadScheme() )
    {
        return false;
    }

    if ( !m_SchemeStarData.LoadScheme() )
    {
        return false;
    }

    if ( !m_SchemeBasePropData.LoadScheme() )
    {
        return false;
    }

    
    if ( !m_SchemeKinDataCenter.LoadScheme() )
    {
        return false;
    }

    if ( !m_SchemeClanDataCenter.LoadScheme() )
    {
        return false;
    }

	if (!m_SchemeHeroCardGradeColor.LoadScheme())
	{
		return false;
	}

    if ( !m_SchemeHeroCard.LoadScheme() )
    {
        return false;
    }

    if ( !m_SchemeHeroAchievementLv.LoadScheme() )
    {
        return false;
    }

    if ( !m_SchemeOutputRestriction.LoadScheme() )
    {
        return false;
    }

    if ( !m_SchemePlayerKill.LoadScheme() )
    {
        return false;
    }

	if ( !m_SchemeHeroFreeWeekly.LoadScheme() )
	{
		return false;
	}

    if (!m_ComputerRankConfig.LoadScheme())
    {
        return false;
    }

	if ( !m_SchemeMentorshipDataCenter.LoadScheme() )
	{
		return false;
	}

    if ( !m_SchemeSkinSpell.LoadScheme() )
    {
        return false;
    }
    if (!m_SchemeAIIDTable.LoadScheme())
    {
        return false;
    }
    
    if (!m_SchmeWing.LoadScheme())
    {
        return false;
    }

	return true;
}

void SchemeCenter::release()
{
	// 删除人物皮肤模型数据
	m_SchemePersonModel.Close();

	// 删除怪物配置数据
	m_SchemeMonster.Close();

	// 删除怪物配置数据
	m_SchemeTankInfo.Close();

	// 地图信息
	m_SchemeMapArea.Close();
	m_SchemeMapInfo.Close();
    // 场景保姆信息
    m_SchemeSceneMatron.Close();

	m_SchemeMatchRoom.Close();
	m_SchemeTextInfo.Close();

	m_SchemeEntity.Close();

	m_ScshemeSkinInfo.Close();

	m_SchemeAOI.Close();

	// 删除升级经验配置数据
	m_SchemeLevelUpgrade.Close();

	m_MagicType.Close();

	m_DieLighting.Close();

	m_SpellUpgrade.Close();

	m_SpellSlot.Close();

	m_SlotMapedShortcut.Close();

	m_WarBroadCastInfo.Close();

    m_MatchRankConfig.Close();

    m_SystemMessage.Close();

    m_Preload.Close();

    m_WarPersonConfig.Close();

    m_SchemeGlory.Close();

    m_SchemeStarData.Close();

    m_SchemeBasePropData.Close();

    m_SchemeKinDataCenter.Close();

    m_SchemeClanDataCenter.Close();

    m_SchemeHeroCard.Close();

    m_SchemeHeroAchievementLv.Close();

	m_SchemeHeroCardGradeColor.Close();

    m_SchemeOutputRestriction.Close();

    m_SchemePlayerKill.Close();

	m_SchemeHeroFreeWeekly.Close();

    m_ComputerRankConfig.Close();

	m_SchemeMentorshipDataCenter.Close();

    m_SchemeSkinSpell.Close();

    m_SchemeAIIDTable.Close();

    m_SchmeWing.Close();

	delete this;
}

/** 取得人物模型对象
@return  
*/
ISchemePersonModel*	SchemeCenter::getSchemePersonModel()
{
	return &m_SchemePersonModel;
}

/** 取得怪物配置对象
@return  
*/
ISchemeMonster*	SchemeCenter::getSchemeMonster()
{
	return &m_SchemeMonster;
}

/** 取得怪物配置对象
@return  
*/
ISchemeTankInfo*	SchemeCenter::getSchemeTankInfo()
{
	return &m_SchemeTankInfo;
}


/** 取得地图区域
@return  
*/
ISchemeMapArea* SchemeCenter::getSchemeMapArea()
{
	return &m_SchemeMapArea;
}

/** 取得地图信息
@return  
*/
ISchemeMapInfo* SchemeCenter::getSchemeMapInfo()
{
	return &m_SchemeMapInfo;
}

/** 取得场景保姆接口
@return  
*/
ISchemeSceneMatron * SchemeCenter::getSchemeSceneMatron()
{
    return &m_SchemeSceneMatron;
}

ISchemeMatchRoom* SchemeCenter::getSchemeMatchRoom()
{
	return &m_SchemeMatchRoom;
}

ISchemeTextInfo* SchemeCenter::getSchemeTextInfo()
{
	return &m_SchemeTextInfo;
}

/** 取得实体通用配置
*/
ISchemeEntity *SchemeCenter::getSchemeEntity()
{
	return &m_SchemeEntity;
}

/** 取得皮肤配置
*/
ISchemeSkinInfo *SchemeCenter::getSchemeSkinInfo()
{
	return &m_ScshemeSkinInfo;
}

// 取得天赋配置
ISchemeTalent* SchemeCenter::getSchemeTalent()
{
    return &CTalentScheme::getInstance();
}

// 获取小地图配置
IMinimapScheme* SchemeCenter::getSchemeMinimap()
{
    return &CMinimapScheme::getInstance(); 
}

// 升级经验
ISchemeLevelUpgrade* SchemeCenter::getSchemeLevelUpgrade()
{
	return &m_SchemeLevelUpgrade;
}

ISchemeAOI* SchemeCenter::getSchemeAOI()
{
    return &m_SchemeAOI;
}

/** 魔法类型接口
@return  
*/
ISchemeMagicType* SchemeCenter::getSchemeMagicType()
{
	return &m_MagicType;
}

/** 寻路服务配置接口
@return  
*/
IPathFindServiceInfo* SchemeCenter::getPathFindServiceScheme()
{
    return &m_SchemePathFindService;
}

/** 死亡光效接口
@return  
*/
ISchemeDieLighting* SchemeCenter::getSchemeDieLighting()
{
	return &m_DieLighting;
}


/** 技能升级配置接口
@return  
*/
ISchemeSpellUpgrade* SchemeCenter::getSchemeSpellUpgrade()
{
	return &m_SpellUpgrade;
}

/** 取得技能槽配置信息
@return  
*/
ISchemeSpellSlot* SchemeCenter::getSchemeSpellSlot()
{
	return &m_SpellSlot;
}


ISchemeSlotMapedShortcut* SchemeCenter::getSchemeSlotMapedShortcut()
{
	return &m_SlotMapedShortcut;
}

ISchemeWarBroadCastInfo* SchemeCenter::getSchemeWarBroadCastInfo()
{
	return &m_WarBroadCastInfo;
}

ISchemeSummonerSkill* SchemeCenter::getSchemeSummonerSkill()
{
    return &m_SummnnerSkill;
}


ISchemeMatchRankConfigInfo* SchemeCenter::getSchemeMatchRankConfig()
{
    return &m_MatchRankConfig;
}

ISchemeSystemMessage* SchemeCenter::getSchemeSystemMessage()
{
    return &m_SystemMessage;
}

ISchemePreloadMonsterRes* SchemeCenter::getSchemePreloadRes()
{
    return &m_Preload;
}

ISchemeWarPersonConfig* SchemeCenter::getSchemeWarPersonConfig()
{
    return &m_WarPersonConfig;
}

ISchemePlayerGloryInfo* SchemeCenter::getSchemeGloryInfo()
{
    return &m_SchemeGlory;
}

ISchemeHeroStarData* SchemeCenter::getSchemeHeroStarInfo()
{
    return & m_SchemeStarData;
}

ISchemePersonBaseProp* SchemeCenter::getSchemePersonBaseProp()
{
    return &m_SchemeBasePropData;
}

ISchemeClanDataCenter* SchemeCenter::getSchemeClanDataCenter()
{
    return &m_SchemeClanDataCenter;
}

ISchemeKinDataCenter* SchemeCenter::getSchemeKinDataCenter()
{
    return &m_SchemeKinDataCenter;
}

ISchemeHeroAchievementLvConfig* SchemeCenter::getSchemeHeroAchievementLv()
{
    return &m_SchemeHeroAchievementLv;
}

/** 取得英雄卡牌配置相关
@return  
*/
ISchemeHeroCard* SchemeCenter::getSchemeHeroCard()
{
    return &m_SchemeHeroCard;
}

/** 取得英雄卡牌等级颜色配置相关
@return  
*/
ISchemeHeroCardGradeColor* SchemeCenter::getSchemeHeroCardGradeColor()
{
	return &m_SchemeHeroCardGradeColor;
}

ISchemeOutputRestriction* SchemeCenter::getSchemeOutPutRestriction()
{
    return &m_SchemeOutputRestriction;
}

/** 取得PK配置
@return PK配置
*/
ISchemePlayerKill* SchemeCenter::getSchemePlayerKill()
{
    return &m_SchemePlayerKill;
}

/** 取得周免英雄配置
@return  
*/
ISchemeHeroFreeWeekly* SchemeCenter::getSchemeHeroFreeWeekly()
{
	return &m_SchemeHeroFreeWeekly;
}


ISchemeComputerRankConfigInfo* SchemeCenter::getSchemeComputerRankConfig()
{
    return &m_ComputerRankConfig;
}

/** 取得师徒配置相关
	@return  
	*/
ISchemeMentorshipDataCenter* SchemeCenter::getSchemeMentorshipDataCenter()
{
	return &m_SchemeMentorshipDataCenter;
}


/** 取得皮肤技能配置
@return
*/
ISchemeSkinSpell* SchemeCenter::getSchemeSkinSpell()
{
    return &m_SchemeSkinSpell;
}

ISchemeAIIDTable* SchemeCenter::getSchemeAIIDTable()
{
    return &m_SchemeAIIDTable;
}

 /** 取得翅膀配置
	@return 
*/
ISchemeWing* SchemeCenter::getSchemeWing()
{
    return &m_SchmeWing;
}


