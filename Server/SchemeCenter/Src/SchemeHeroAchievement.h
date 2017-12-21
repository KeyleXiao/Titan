/*******************************************************************
** 文件名:	SchemeHeroAchievement.h
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

using namespace rkt;
class CSchemeHeroAchievement : public ISchemeHeroAchievementConfig, public ISchemeUpdateSink, public CSchemeCache<CSchemeHeroAchievement>
{
public:
    /////////////////////////////////ISchemeUpdateSink/////////////////////////////////////////
    virtual bool			OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);

    virtual bool			OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);

    /////////////////////////////////ISchemeHeroAchievementConfig/////////////////////////////////////////
    virtual const SHeroAchievementScoreConfig* getHeroAchievementConfig( int nMatchTypeID, int nRankLv);

public:
    /////////////////////////////////CSchemeHeroCardCreat/////////////////////////////////////////
    bool                    LoadScheme();

    void                    Close();

protected:
    bool                    loadHeroAchievementScore(SCRIPT_READER reader);

private:
    // 成就得分配置信息 key为战场类型id
    typedef std::map<int, SHeroAchievementScoreConfig> HEROACHIEVEMENT;
    std::map<int, HEROACHIEVEMENT>      m_mapHeroAchievementScore;
};