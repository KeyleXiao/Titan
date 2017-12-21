/*******************************************************************
** �ļ���:	SchemeSummoner.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	����
** ��  ��:	2016-3-31
** ��  ��:	1.0
** ��  ��:	�ٻ�ʦ�������ñ�
********************************************************************/
#pragma once
#include "Singleton.h"
#include "ISchemeCenter.h"
#include "EntityDef.h"

using namespace rkt;
class CSchemeSummonerSkill : public ISchemeSummonerSkill, public ISchemeUpdateSink
{
public:
    /////////////////////////////////ISchemeUpdateSink/////////////////////////////////////////
	virtual bool			OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);

	virtual bool			OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);

    /////////////////////////////////ISchemeSummonerSkill/////////////////////////////////////////
    virtual std::map<int, SSummonerSkillInfo>* getAllSummonerSkillInfo();

	virtual std::map<int, SSummonerSkillInfo>* getSummonerSkillByWarMode(int nWarMode);

    virtual int getIconId( int nSkillId );

    virtual SSummonerSkillInfo* getSummonerSkillInfo(int nSkillId );

public:
    /////////////////////////////////CSchemeSummonerSkill/////////////////////////////////////////
    bool                    LoadScheme();

    void                    Close();

private:
    // ����idΪ����
    std::map<int, SSummonerSkillInfo>		m_mapSummonerSkillInfo;

	// ս��ģʽΪ����
	typedef std::map<int, std::map<int, SSummonerSkillInfo> >		TMap_WarMode2Summon;
	TMap_WarMode2Summon						m_mapWarType2Summon;
};