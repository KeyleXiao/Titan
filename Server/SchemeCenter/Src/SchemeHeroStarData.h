/*******************************************************************
** �ļ���:	SchemeHeroStarData.h
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
    // ��������ר���츳������
    void parseConditionInfo(char* pValue, std::map<int, int>& mapCondition);

private:
	typedef std::map<int, std::map<int, SHeroStarSchemeInfo>> HERO_STAR_SCHEMEINFOMAP;
    // Ӣ���Ǽ���Ϣ keyΪӢ��id�� �μ�keyΪ�Ǽ�
    HERO_STAR_SCHEMEINFOMAP             m_mapHeroStarSchemeInfo;

    // Ӣ�۳ɳ�������Ϣ   keyΪ�ɳ�id
    GROW_PROPERTY_SCHEMEINFOMAP         m_mapGrowSchemeInfo;

    // Ӣ��ר���츳��Ϣ  keyΪר���츳id
    EXCLUSIVE_TALENT_SCHEMEINFOMAP      m_mapHeroExclusiveTalentInfo;

    // Ӣ�۶�Ӧ������Ǽ�
    std::map<int, int>                  m_mapHeroStarLvLimit;
};