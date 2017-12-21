/*******************************************************************
** �ļ���:	SchemeExpOrGoldCreate.cpp
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	����
** ��  ��:	2016/8/3
** ��  ��:	1.0
** ��  ��:	

********************************************************************/
#include "StdAfx.h"
#include "SchemeExpOrGoldCreate.h"
#include "IServerGlobal.h"

bool CSchemeExpOrGoldCreate::OnSchemeLoad(SCRIPT_READER reader,const char* szFileName)
{
    ICSVReader * pCSVReader = reader.pCSVReader;
    if ( pCSVReader==0 || reader.type!=READER_CSV)
        return false;

    // ���
    m_mapExpOrGoldConfig.clear();

    int nLen = 0;
    // ��ȡ
    int nRecordCount = pCSVReader->GetRecordCount();

    for ( int nRow = 0; nRow < nRecordCount; ++nRow )
    {
        int nColumnIndex = 0;
        SExpOrGoldCreateConfig item;

        item.nWarTypeID = pCSVReader->GetInt(nRow, nColumnIndex++, 0);
        item.nExpType = pCSVReader->GetInt( nRow, nColumnIndex++, 0);
        item.fGoldAddition = pCSVReader->GetFloat( nRow, nColumnIndex++, 0);
        for (int i = 0; i < ENECPORGOLDCOEFF_MAX; ++i )
        {
            item.fWinCoeff[i] = pCSVReader->GetFloat( nRow, nColumnIndex++, 0);
            item.fFailCoeff[i] = pCSVReader->GetFloat( nRow, nColumnIndex++, 0);
        }

        item.nPersonLimit = pCSVReader->GetInt( nRow, nColumnIndex++, 0);

        std::map<int, std::map<int, SExpOrGoldCreateConfig>>::iterator it = m_mapExpOrGoldConfig.find(item.nWarTypeID);
        if ( it == m_mapExpOrGoldConfig.end() )
        {
            std::map<int, SExpOrGoldCreateConfig> mapItem;
            mapItem[item.nExpType] = item;
            m_mapExpOrGoldConfig[item.nWarTypeID] = mapItem;
        }else
        {
            std::map<int, SExpOrGoldCreateConfig>& mapItem = it->second;
            mapItem[item.nExpType] = item;
        }
    }

    AfterSchemeLoaded();

    return true;
}

bool CSchemeExpOrGoldCreate::OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName)
{
    return OnSchemeLoad(reader, szFileName);
}

SExpOrGoldCreateConfig* CSchemeExpOrGoldCreate::getExpOrGoldCreateConfig( int nWarTypeID , int ExpType)
{
    std::map<int, std::map<int, SExpOrGoldCreateConfig>>::iterator it = m_mapExpOrGoldConfig.find( nWarTypeID );
    if ( it == m_mapExpOrGoldConfig.end() )
    {
        return NULL;
    }

    std::map<int, SExpOrGoldCreateConfig>::iterator itConfig = it->second.find(ExpType);
    if ( itConfig == it->second.end() )
    {
        return NULL;
    }
    return &itConfig->second;
}

bool CSchemeExpOrGoldCreate::LoadScheme()
{
    ISchemeEngine* pSchemeEngine = gServerGlobal->getSchemeEngine();
    if ( pSchemeEngine == NULL)
    {
        return false;
    }

    // ����SummonerSkill.csv�ű�
    string stringPath = SCP_PATH_FORMAT( EXPORGOLDCREATE_SCHEME_FILENAME );
    bool bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink*) this );
    if ( !bResult )
    {
        ErrorLn("config file load failed filename = " << stringPath.c_str());;
        return false;
    }
    return true;
}

// �ر�
void CSchemeExpOrGoldCreate::Close()
{
    // ���
    m_mapExpOrGoldConfig.clear();
}