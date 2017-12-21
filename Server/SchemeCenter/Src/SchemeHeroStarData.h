/*******************************************************************
** 文件名:	SchemeHeroStarData.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	马传飞
** 日  期:	2016/8/3
** 版  本:	1.0
** 描  述:	

********************************************************************/
#pragma once
#include "SchemeDef.h"
#include "SchemeCache.h"
#include "ISchemeCenter.h"

class CSchemeHeroStarData:public ISchemeHeroStarData,public ISchemeUpdateSink, public CSchemeCache<CSchemeHeroStarData>
{
public:
    /////////////////////////////////ISchemeHeroStarData/////////////////////////////////////////
    virtual int                             getUpgradeNeedExpTypeID( int nHeroId, int nStarLv);

    virtual SHeroGrowProInfo*                getGrowProInfo( int nHeroID, int nStarLv);

    virtual SHeroStarSchemeInfo             getHeroStarData( int nHeroID, int nStarLv);

    virtual SHeroExclusiveTalentSchemeInfo  getHeroExclusive( int nExclusiveID);

    virtual int                             getHeroStarLvLimit(int nHeroID );
    /////////////////////////////////ISchemeUpdateSink/////////////////////////////////////////
    virtual bool            OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);

    virtual bool            OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);
    /////////////////////////////////CSchemePlayerGloryInfo/////////////////////////////////////////
    bool											LoadScheme(void);
    void											Close(void);

private:
    bool    LoadStarLvScheme(SCRIPT_READER reader);
    bool    LoadHeroPropertyScheme(SCRIPT_READER reader);
    bool    LoadExclusiveTalentScheme(SCRIPT_READER reader);

private:
    // 解析激活专属天赋的条件
    void parseConditionInfo(char* pValue, std::map<int, int>& mapCondition);

private:
	typedef std::map<int, std::map<int, SHeroStarSchemeInfo>> HERO_STAR_SCHEMEINFOMAP;
    // 英雄星级信息 key为英雄id， 次级key为星级
    HERO_STAR_SCHEMEINFOMAP             m_mapHeroStarSchemeInfo;

    // 英雄成长属性信息   key为成长id
    GROW_PROPERTY_SCHEMEINFOMAP         m_mapGrowSchemeInfo;

    // 英雄专属天赋信息  key为专属天赋id
    EXCLUSIVE_TALENT_SCHEMEINFOMAP      m_mapHeroExclusiveTalentInfo;

    // 英雄对应的最高星级
    std::map<int, int>                  m_mapHeroStarLvLimit;
};