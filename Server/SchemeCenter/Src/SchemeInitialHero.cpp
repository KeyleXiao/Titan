#include "StdAfx.h"
#include "SchemeInitialHero.h"
#include "IServerGlobal.h"

bool CSchemeInitialHero::OnSchemeLoad(SCRIPT_READER reader,const char* szFileName)
{
    ICSVReader* pCSVReader = reader.pCSVReader;
    if ( pCSVReader == NULL || szFileName == NULL ||  reader.type != READER_CSV )
    {
        return false;
    }
    m_initialHeroMap.clear();

    int nLen = 0;
    int nCount = pCSVReader->GetRecordCount();
    for ( int i = 0; i < nCount; ++i )
    {
        // 记录序列号
        int nCol = 0;
        SInitialHeroSchemeInfo item;
        item.nHeroID = pCSVReader->GetInt(i, nCol++, 0);
        item.nIntialLv = pCSVReader->GetInt(i, nCol++, 0);
        item.nSkinID = pCSVReader->GetInt(i, nCol++, 0);
        item.nHeroStar = pCSVReader->GetInt(i, nCol++, 0);
        item.dwDeadLine = pCSVReader->GetInt64(i, nCol++, 0);
        item.nFlag = pCSVReader->GetInt(i, nCol++, 0);

        char szTmp[512] = {0};
        nLen = sizeof(szTmp);
        pCSVReader->GetString(i, nCol++, szTmp, nLen);
        int nArrID[32] = {0};
        int nSize = parseIntArray(szTmp, nArrID, 32, ';');
        for (int j = 0; j < nSize; ++j )
        {
            item.vecExclusiveID.push_back(nArrID[j]);
        }

        item.nIsActivate = pCSVReader->GetInt(i, nCol++, 0);
        m_initialHeroMap[item.nHeroID] = item;
    }

    return true;
}

bool CSchemeInitialHero::OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName)
{
    return true;
}

bool CSchemeInitialHero::LoadScheme(void)
{
    ISchemeEngine *pSchemeEngine = gServerGlobal->getSchemeEngine();
    if(pSchemeEngine == NULL)
    {
        return false;
    }

    // 加载脚本
    string stringPath = SCP_PATH_FORMAT( INITIALHERO_SCHEME_FILENAME );
    bool bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink *)this);
    if(!bResult)
    {
        Error("config file load failed! filename=" << stringPath.c_str() << endl);
        return false;
    }
    AfterSchemeLoaded();
    return true;
}

void CSchemeInitialHero::Close(void)
{
    m_initialHeroMap.clear();
}