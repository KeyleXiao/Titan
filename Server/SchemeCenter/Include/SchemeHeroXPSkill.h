/*******************************************************************
** 文件名:	E:\work\w1\speed\Server\SchemeCenter\Src\SchemeCrownPage.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈言蒙
** 日  期:	12/12/2016	
** 版  本:	1.0
** 描  述:	
            读xp系统配置文件
********************************************************************/

#pragma once

#include "ISchemeEngine.h"
#include "ISchemeCenter.h"
#include "EntityDef.h"
#include "SchemeDef.h"
#include "SchemeCache.h"


class CSchemeHeroXpSkill : public ISchemeHeroXpSkill,
                        public ISchemeUpdateSink,
                        public CSchemeCache<CSchemeHeroXpSkill>
{
    enum
    {
        COL_XP_HERO_ID,					    // 英雄id
        COL_XP_SPELL_ID,                    // 必杀技技能id
        COL_XP_SPELL_TYPE,  				// 技能类型(通用0,专属1)
        COL_XP_AUTO_UNLOCK,					// 是否自动解锁(否0,是1)
        COL_XP_CHIP_NUM,                    // 必杀技解锁所需碎片数
        COL_XP_ICON,						// 必杀技图标
        COL_XP_NAME,                        // 必杀技名字
        COL_XP_LOCK_DESC,                   // 必杀技未解锁时描述
        COL_XP_UNLOCK_DESC,                 // 必杀技解锁时描述
        COL_XP_GUIDE,                       // 解锁时是否引导(否0,是1)
        COL_XP_DRESS,                       // 解锁时是否默认装备(否0,是1)
    };


    typedef list<SHeroXPSkillConfig_S>      XP_LIST;
	typedef std::map<int, XP_LIST>          HEROXP_MAP;

public:
	
	// ISchemeUpdateSink
	virtual bool OnSchemeLoad(SCRIPT_READER reader, const char *szFileName);
	virtual bool OnSchemeUpdate(SCRIPT_READER reader, const char *szFileName);

	// ISchemeHeroXpSkill
	// 获得XP信息
	virtual SHeroXPSkillConfig_S* getHeroXpSkillConfig(int nHeroID,int nXPSkillID);

	// 获得自动激活XP列表
	virtual bool getHeroXpActiveList(SHeroXPSkillConfig_S activeXpSkillList[],int nListCount);

public:
	bool LoadScheme();
	void Close();

private:
	bool updateXpConfigToCache( SHeroXPSkillConfig_S& sConfigInfo );

private:
	HEROXP_MAP              m_schemeInfo;
};
