/*******************************************************************
** �ļ���:	SchemeSummoner.cpp
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	����
** ��  ��:	2016-3-31
** ��  ��:	1.0
** ��  ��:	�ٻ�ʦ�������ñ�
********************************************************************/
#include "StdAfx.h"
#include "SchemeSummoner.h"
#include "IClientGlobal.h"

bool CSchemeSummonerSkill::OnSchemeLoad(SCRIPT_READER reader,const char* szFileName)
{
    ICSVReader * pCSVReader = reader.pCSVReader;
    if ( pCSVReader==0 || reader.type!=READER_CSV)
        return false;

    // ���
    m_mapSummonerSkillInfo.clear();

    int nLen = 0;
    // ��ȡ
    int nRecordCount = pCSVReader->GetRecordCount();
    for ( int nRow = 0; nRow < nRecordCount; ++nRow )
    {
        SSummonerSkillInfo item;
        ZeroMemory(&item, sizeof(SSummonerSkillInfo) );
        nLen = sizeof(item.stName);
        pCSVReader->GetString(nRow, SUMMONER_NAME, item.stName, nLen);
        item.nId = pCSVReader->GetInt(nRow, SUMMONER_ID, 0);
        item.nLv = pCSVReader->GetInt(nRow, SUMMONER_LV, 0);
        item.nIconId = pCSVReader->GetInt(nRow, SUMMONER_ICONID, 0);
        nLen = sizeof(item.stDesc);
        pCSVReader->GetString(nRow, SUMMONER_DESC, item.stDesc, nLen);

		// id->item
        m_mapSummonerSkillInfo[item.nId] = item;

		// warmode->item
		int nWarMode = pCSVReader->GetInt(nRow, SUMMONER_WARMODE, 0);
		if (m_mapWarType2Summon.find(nWarMode) == m_mapWarType2Summon.end())
		{
			std::map<int, SSummonerSkillInfo> tempMap;
			m_mapWarType2Summon[nWarMode] = tempMap;
		}
		m_mapWarType2Summon[nWarMode].insert(std::make_pair(item.nId, item));
    }
    return true;
}

bool CSchemeSummonerSkill::OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName)
{
    return OnSchemeLoad(reader, szFileName);
}

bool CSchemeSummonerSkill::LoadScheme()
{
    ISchemeEngine* pSchemeEngine = gClientGlobal->getSchemeEngine();
    if ( pSchemeEngine == NULL)
    {
        return false;
    }

    // ����SummonerSkill.csv�ű�
    string stringPath = SCP_PATH_FORMAT( SUMMONER_SKILL_INFO );
    bool bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink*) this );
    if ( !bResult )
    {
        ErrorLn("config file load failed filename = " << stringPath.c_str());;
        return false;
    }
    return true;
}

// �ر�
void CSchemeSummonerSkill::Close()
{
    // ���
    m_mapSummonerSkillInfo.clear();
}

std::map<int, SSummonerSkillInfo>* CSchemeSummonerSkill::getAllSummonerSkillInfo()
{
    return &m_mapSummonerSkillInfo;
}

std::map<int, SSummonerSkillInfo>* CSchemeSummonerSkill::getSummonerSkillByWarMode(int nWarMode)
{
	if (m_mapWarType2Summon.find(nWarMode) != m_mapWarType2Summon.end())
	{
		return &m_mapWarType2Summon[nWarMode];
	}

	return NULL;
}

int CSchemeSummonerSkill::getIconId( int nSkillId )
{
    std::map<int, SSummonerSkillInfo>::iterator it = m_mapSummonerSkillInfo.find(nSkillId);
    if ( it == m_mapSummonerSkillInfo.end() )
    {
        return -1;
    }
    SSummonerSkillInfo sSkillInfo = it->second;
    return sSkillInfo.nIconId;
}
SSummonerSkillInfo * CSchemeSummonerSkill::getSummonerSkillInfo( int nSkillId )
{
    std::map<int, SSummonerSkillInfo>::iterator it = m_mapSummonerSkillInfo.find(nSkillId);
    if ( it == m_mapSummonerSkillInfo.end() )
    {
        return NULL;
    }
    return &it->second;;
}