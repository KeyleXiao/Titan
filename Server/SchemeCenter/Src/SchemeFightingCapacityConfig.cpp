/*******************************************************************
** �ļ���:	SchemeFightingCapacityConfig.cpp
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	����
** ��  ��:	2016/8/3
** ��  ��:	1.0
** ��  ��:	

********************************************************************/
#include "StdAfx.h"
#include "SchemeFightingCapacityConfig.h"
#include "IServerGlobal.h"

bool CSchemeFightingCapacityConfig::OnSchemeLoad(SCRIPT_READER reader,const char* szFileName)
{
    ICSVReader * pCSVReader = reader.pCSVReader;
    if ( pCSVReader==0 || reader.type!=READER_CSV)
        return false;

    // ���
    m_mapFightingCapacity.clear();

    // ��ȡ
    int nRecordCount = pCSVReader->GetRecordCount();

    for ( int nRow = 0; nRow < nRecordCount; ++nRow )
    {
        int nColumnIndex = 0;
        SFightingCapacityConfig item;
        item.nMatchType = pCSVReader->GetInt(nRow, nColumnIndex++, 0);
        item.fFightCoeff = pCSVReader->GetFloat(nRow, nColumnIndex++, 0);
        m_mapFightingCapacity[item.nMatchType] = item;
    }
    AfterSchemeLoaded();
    return true;
}

bool CSchemeFightingCapacityConfig::OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName)
{
    return OnSchemeLoad(reader, szFileName);
}

const SFightingCapacityConfig* CSchemeFightingCapacityConfig::getFightConfig( int nWarTypeID )
{
    std::map<int, SFightingCapacityConfig>::iterator it = m_mapFightingCapacity.find(nWarTypeID);
    if ( it == m_mapFightingCapacity.end() )
    {
        //ErrorLn(__FUNCTION__": can't find nWarTypeID == " << nWarTypeID << ", fight Capacity Config!!!");
        return NULL;
    }
    return &it->second;
}

bool CSchemeFightingCapacityConfig::LoadScheme()
{
    ISchemeEngine* pSchemeEngine = gServerGlobal->getSchemeEngine();
    if ( pSchemeEngine == NULL)
    {
        return false;
    }

    // ����SummonerSkill.csv�ű�
    string stringPath = SCP_PATH_FORMAT( FIGHTCONFIG_SCHEME_FILENAME );
    bool bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink*) this );
    if ( !bResult )
    {
        ErrorLn("config file load failed filename = " << stringPath.c_str());;
        return false;
    }
    return true;
}

// �ر�
void CSchemeFightingCapacityConfig::Close()
{
    // ���
    m_mapFightingCapacity.clear();
}