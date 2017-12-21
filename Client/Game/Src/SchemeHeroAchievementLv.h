/*******************************************************************
** �ļ���:	SchemeHeroAchievementLv.cpp
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	����
** ��  ��:	2016-3-31
** ��  ��:	1.0
** ��  ��:	Ӣ�۳ɾͷ���������
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
    // �ɾ͵ȼ���Ϣ keyΪ�ɾ�id
    std::vector<SHeroAchievementLvConfig>         m_vecpHeroAchievementLv;
};