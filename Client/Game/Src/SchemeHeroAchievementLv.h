/*******************************************************************
** 文件名:	SchemeHeroAchievementLv.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	马传飞
** 日  期:	2016-3-31
** 版  本:	1.0
** 描  述:	英雄成就分数及表现
********************************************************************/
#pragma once
#include "Singleton.h"
#include "ISchemeCenter.h"
#include "SchemeDef.h"

using namespace rkt;
class CSchemeHeroAchievementLv : public ISchemeHeroAchievementLvConfig, public ISchemeUpdateSink
{
public:
    /////////////////////////////////ISchemeUpdateSink/////////////////////////////////////////
    virtual bool			OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);

    virtual bool			OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);

    /////////////////////////////////ISchemeHeroAchievementLvConfig/////////////////////////////////////////
    virtual int             getIconIDAccordScore( int nScore, int* pArrIcon, int nCount);

public:
    /////////////////////////////////CSchemeHeroCardCreat/////////////////////////////////////////
    bool                    LoadScheme();

    void                    Close();
private:
    // 成就等级信息 key为成就id
    std::vector<SHeroAchievementLvConfig>         m_vecpHeroAchievementLv;
};