/*******************************************************************
** �ļ���:	SchemeHeroAchievement.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	����
** ��  ��:	2016/8/3
** ��  ��:	1.0
** ��  ��:	

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
    // �ɾ͵÷�������Ϣ keyΪս������id
    typedef std::map<int, SHeroAchievementScoreConfig> HEROACHIEVEMENT;
    std::map<int, HEROACHIEVEMENT>      m_mapHeroAchievementScore;
};