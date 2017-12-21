/*******************************************************************
** �ļ���:	SchemeOutputRestriction.cpp
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	����
** ��  ��:	2016/8/3
** ��  ��:	1.0
** ��  ��:	

********************************************************************/
#include "StdAfx.h"
#include "SchemeOutputRestriction.h"
#include "IServerGlobal.h"

bool CSchemeOutputRestriction::OnSchemeLoad(SCRIPT_READER reader,const char* szFileName)
{
    ICSVReader * pCSVReader = reader.pCSVReader;
    if ( pCSVReader==0 || reader.type!=READER_CSV)
        return false;

    // ���
    m_mapOutputRestriction.clear();

    int nLen = 0;
    // ��ȡ
    int nRecordCount = pCSVReader->GetRecordCount();

    for ( int nRow = 0; nRow < nRecordCount; ++nRow )
    {
        int nColumnIndex = 0;
        SOutputRestrictionConfig item;
        item.nMatchType = pCSVReader->GetInt( nRow, nColumnIndex++, 0);
        char    szPersonNum[GAME_MSGSTR_MAXSIZE] = {0};
        nLen = sizeof(szPersonNum);
        pCSVReader->GetString(nRow, nColumnIndex++, szPersonNum, nLen);
        
        parseConditionInfo(szPersonNum,item.mapPersonNum);
        item.nPersonMin = pCSVReader->GetInt( nRow,nColumnIndex++, 0);
        item.nMatchTime = pCSVReader->GetInt( nRow,nColumnIndex++, 0);
        m_mapOutputRestriction[item.nMatchType] = item;
    }
    AfterSchemeLoaded();
    return true;
}

bool CSchemeOutputRestriction::OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName)
{
    return OnSchemeLoad(reader, szFileName);
}

const SOutputRestrictionConfig* CSchemeOutputRestriction::getOutputRestrictionConfig( int nMatchTypeID )
{
    std::map<int, SOutputRestrictionConfig>::iterator it = m_mapOutputRestriction.find(nMatchTypeID);
    if ( it == m_mapOutputRestriction .end() )
    {
        ErrorLn(__FUNCTION__": in m_mapOutputRestriction can't find nMatchTypeID = " << nMatchTypeID );
        return NULL;
    }

    return &it->second;
}

bool CSchemeOutputRestriction::LoadScheme()
{
    ISchemeEngine* pSchemeEngine = gServerGlobal->getSchemeEngine();
    if ( pSchemeEngine == NULL)
    {
        return false;
    }

    // ����OutputRestriction.csv�ű�
    string stringPath = SCP_PATH_FORMAT( OUTPUTRESTRICTION_SCHEME_FILENAME );
    bool bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink*) this );
    if ( !bResult )
    {
        ErrorLn("config file load failed filename = " << stringPath.c_str());;
        return false;
    }
    return true;
}

// �ر�
void CSchemeOutputRestriction::Close()
{
    // ���
    m_mapOutputRestriction.clear();
}

void CSchemeOutputRestriction::parseConditionInfo(char* pValue, std::map<int, int>& mapCondition)
{
    int nCondition[GLORY_CONDITION_NUM];
    char* pCondition = strtok( pValue, ";");
    int nCount = 0;
    while( pCondition )
    {
        memset(nCondition, 0, sizeof(nCondition));
        nCount = parseIntArray( pCondition, nCondition, GLORY_CONDITION_NUM, '-');
        if ( nCount > GLORY_CONDITION_NUM )
        {
            WarningLn(__FUNCTION__":nCount >= CONDITION_NUM");
            continue;
        }
        int nTypeID = nCondition[GLORY_CONDITION_TYPE];
        std::map<int,int>::iterator it = mapCondition.find(nTypeID);
        if( it != mapCondition.end())
        {
            WarningLn(__FUNCTION__": map is have id = " << nTypeID);
            continue;
        }

        mapCondition[nTypeID] = nCondition[GLORY_CONDITION_VALUE];
        pCondition = strtok(NULL,";");
    }
}