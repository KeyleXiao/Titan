/*******************************************************************
** �ļ���:	SchemeSummoner.h
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


using namespace std;
class CSchemeSummonerSkill: public ISchemeSummonerSkill, public ISchemeUpdateSink, public CSchemeCache<CSchemeSummonerSkill>
{
public:
    virtual bool            OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);

    virtual bool            OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);

public:
    /////////////////////////////ISchemeSummonerSkill/////////////////////////////////////////////

    // ���ݼ���id���iconId
    virtual int getIconId( int nSkillId );

    //////////////////////////////////////////////////////////////////////////
    bool											LoadScheme(void);

    void											Close(void);

private:
    // �ٻ�ʦ�������ñ�
    std::map<int, SSummonerSkillData>        m_mapSummonerSkillInfo;
};