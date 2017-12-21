/*******************************************************************
** 文件名:	SchemePlayerGloryInfo.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	马传飞
** 日  期:	2016/8/3
** 版  本:	1.0
** 描  述:	

********************************************************************/
#include "StdAfx.h"
#include "SchemePlayerGloryInfo.h"
#include "IServerGlobal.h"

bool CSchemePlayerGloryInfo::OnSchemeLoad(SCRIPT_READER reader,const char* szFileName)
{
    ICSVReader* pCSVReader = reader.pCSVReader;
    if ( pCSVReader == NULL || szFileName == NULL || reader.type != READER_CSV)
        return false;
    
    m_mapGloryInfo.clear();
    int nCount = pCSVReader->GetRecordCount();
    char chValue[512] = {0};
    int nLen = sizeof(chValue);
    for ( int i = 0; i < nCount; ++i )
    {
        memset(chValue, 0, nLen);
        int nColumnIndex = 0;
        SGlorySchemeInfo item;
        item.nId = pCSVReader->GetInt(i, nColumnIndex++, 0);
        pCSVReader->GetString(i, nColumnIndex++, chValue, nLen);
        parseConditionInfo(chValue, item.mapCondition);
        memset(chValue, 0, nLen);
        pCSVReader->GetString(i, nColumnIndex++, chValue,nLen);
        item.stDesc = chValue;
        item.nDifficulty = pCSVReader->GetInt(i, nColumnIndex++, 0);
        m_mapGloryInfo[item.nId] = item;
    }
    return true;
}
bool CSchemePlayerGloryInfo::OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName)
{
    return true;
}

/////////////////////////////////CSchemePlayerGloryInfo/////////////////////////////////////////
bool CSchemePlayerGloryInfo::LoadScheme(void)
{
    ISchemeEngine *pSchemeEngine = gServerGlobal->getSchemeEngine();
    if(pSchemeEngine == NULL)
    {
        return false;
    }

    // 加载脚本
    string stringPath = SCP_PATH_FORMAT( GLORY_SCHEME_FILENAME );
    bool bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink *)this);
    if(!bResult)
    {
        Error("config file load failed! filename=" << stringPath.c_str() << endl);
        return false;
    }
    AfterSchemeLoaded();
    return true;
}
void CSchemePlayerGloryInfo::Close(void)
{

}

void CSchemePlayerGloryInfo::parseConditionInfo(char* pValue, std::map<int, long>& mapCondition)
{
    int nCondition[GLORY_CONDITION_NUM];
    char* pCondition = strtok( pValue, ";");
    int nCount = 0;
    while( pCondition )
    {
        memset(nCondition, 0, sizeof(nCondition));
        nCount = parseIntArray( pCondition, nCondition, GLORY_CONDITION_NUM, '_');
        if ( nCount > GLORY_CONDITION_NUM )
        {
            WarningLn(__FUNCTION__":nCount >= CONDITION_NUM");
            continue;
        }
        int nTypeID = nCondition[GLORY_CONDITION_TYPE];
        std::map<int,long>::iterator it = mapCondition.find(nTypeID);
        if( it != mapCondition.end())
        {
            WarningLn(__FUNCTION__": map is have id = " << nTypeID);
            continue;
        }

        mapCondition[nTypeID] = nCondition[GLORY_CONDITION_VALUE];
        pCondition = strtok(NULL,";");
    }
}

std::map<int, SGlorySchemeInfo>* CSchemePlayerGloryInfo::getGlorySchemeInfo()
{
    return &m_mapGloryInfo;
}