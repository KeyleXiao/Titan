#include "stdafx.h"
#include "SchemeHeroRecommendPath.h"
#include "IServerGlobal.h"

#define HERO_RECOMMEND_PATH_FILE "Scp\\HeroRecommendPath.csv"


bool CHeroRecommendPathScheme::Load()
{
    ISchemeEngine *pSchemeEngine = gServerGlobal->getSchemeEngine();
    if(pSchemeEngine == NULL)
    {
        return false;
    }

    // 载入英雄推荐线路脚本
    std::string strPath = SCP_PATH_FORMAT( HERO_RECOMMEND_PATH_FILE );
    if(!pSchemeEngine->LoadScheme(strPath.c_str(),this,true))
    {
        ErrorLn("load " << HERO_RECOMMEND_PATH_FILE << " failed");
        return false;
    }

    return true;
}

// 获取英雄的推荐线路
int CHeroRecommendPathScheme::getRecommendPath(int nHeroID, int nBattleField)
{
    std::map<int, std::map<int, int>>::iterator itTable =  m_mapRecommendPath.find(nBattleField);
    if(itTable == m_mapRecommendPath.end())
        return -1;

    std::map<int, int>& refTable = itTable->second;
    std::map<int, int>::iterator it = refTable.find(nHeroID);
    if(it == refTable.end())
        return -1;

    return it->second;
}

// 获得某个战场的pathID列表
bool CHeroRecommendPathScheme::getPathList(int nBattleField, PathRecommendHeroNum* pResultBuf, int& nRefCount)
{
    if(pResultBuf == NULL || nRefCount <= 0)
    {
        ErrorLn(__FUNCTION__ << ", error param..");
        return false;
    }

    std::map<int, std::map<int, int>>::iterator itTable =  m_mapBattleFieldPath.find(nBattleField);
    if(itTable == m_mapBattleFieldPath.end())
    {
        nRefCount = 0;
        return false;
    }

    std::map<int, int>& refTable = itTable->second;
    int nIndex = 0;
    for(std::map<int, int>::iterator it = refTable.begin(); it != refTable.end(); ++it)
    {
        pResultBuf[nIndex].nPathID = it->first;
        pResultBuf[nIndex++].nRecommendHeroNum = it->second;

        if(nIndex >= nRefCount)
        {
            break;
        }
    }

    nRefCount = nIndex;

    return true;
}

bool CHeroRecommendPathScheme::OnSchemeLoad(SCRIPT_READER reader,const char* szFileName)
{
    m_mapRecommendPath.clear();
    m_mapBattleFieldPath.clear();

    ICSVReader* pCSVReader = reader.pCSVReader;
    int nRecordCount = pCSVReader->GetRecordCount();
    for(int nRow = 0; nRow < nRecordCount; ++nRow)
    {
        int nCol = 0;

        // 职业ID
        int nHeroID = pCSVReader->GetInt(nRow, nCol++, 0);

        // 战场ID
        int nBattleFieldID = pCSVReader->GetInt(nRow, nCol++, 0);

        // 推荐线路ID
        int nRecommendPathID = pCSVReader->GetInt(nRow, nCol++, -1);

        // 推荐人数
        int nRecommendHeroNum = pCSVReader->GetInt(nRow, nCol++, 0); 

        m_mapRecommendPath[nBattleFieldID][nHeroID] = nRecommendPathID;

        m_mapBattleFieldPath[nBattleFieldID][nRecommendPathID] = nRecommendHeroNum; 
    }

    AfterSchemeLoaded();

    return true;
}

bool CHeroRecommendPathScheme::OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName)
{
    CHeroRecommendPathScheme* pNew = NewInstance();
    if(pNew == NULL)
    {
        return false;
    }

    if(pNew->OnSchemeLoad(reader, szFileName))
    {
        return true;
    }
    else
    {
        safeDelete(pNew);
        return false;
    }
}

void CHeroRecommendPathScheme::Close()
{

}