/*******************************************************************
** 文件名:	SchemeHeroAchievement.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	马传飞
** 日  期:	2016/8/3
** 版  本:	1.0
** 描  述:	

********************************************************************/
#include "StdAfx.h"
#include "SchemeHeroAchievement.h"
#include "IServerGlobal.h"
#include "SchemeDef.h"

bool CSchemeHeroAchievement::OnSchemeLoad(SCRIPT_READER reader,const char* szFileName)
{
    ICSVReader * pCSVReader = reader.pCSVReader;
    if ( pCSVReader==0 || reader.type!=READER_CSV)
        return false;

    // 清空
    m_mapHeroAchievementScore.clear();

    int nLen = 0;
    // 读取
    int nRecordCount = pCSVReader->GetRecordCount();

    for ( int nRow = 0; nRow < nRecordCount; ++nRow )
    {
        int nColumnIndex = 0;
        SHeroAchievementScoreConfig item;
        item.nID = pCSVReader->GetInt(nRow, nColumnIndex++, 0);
        item.nRankLV = pCSVReader->GetInt(nRow, nColumnIndex++, 0);
        item.nExpectValue = pCSVReader->GetInt(nRow, nColumnIndex++, 0);
        item.nWinRateValue = pCSVReader->GetInt(nRow, nColumnIndex++, 0);
        item.nScoreValue = pCSVReader->GetInt(nRow, nColumnIndex++, 0);
        item.nScoreMinLimit = pCSVReader->GetInt(nRow, nColumnIndex++, 0);
        item.nScoringRatio = pCSVReader->GetInt(nRow, nColumnIndex++, 0);
        item.nScoringRewards = pCSVReader->GetInt(nRow, nColumnIndex++, 0);

        std::map<int, HEROACHIEVEMENT>::iterator it = m_mapHeroAchievementScore.find(item.nID);
        if ( it == m_mapHeroAchievementScore.end() )
        {
            HEROACHIEVEMENT mapHeroAchievement;
            mapHeroAchievement[item.nRankLV] = item;
            m_mapHeroAchievementScore[item.nID] = mapHeroAchievement;
        }else
        {
            HEROACHIEVEMENT& pHeroAchievement = it->second;
            pHeroAchievement[item.nRankLV] = item;
        }
    }
    return true;
}

bool CSchemeHeroAchievement::OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName)
{
    return OnSchemeLoad(reader, szFileName);
}

const SHeroAchievementScoreConfig* CSchemeHeroAchievement::getHeroAchievementConfig( int nMatchTypeID, int nRankLv)
{
    std::map<int, HEROACHIEVEMENT>::iterator it = m_mapHeroAchievementScore.find(nMatchTypeID);
    if ( it == m_mapHeroAchievementScore.end() )
    {
        ErrorLn(__FUNCTION__": at m_mapHeroAchievementScore can't find nMatchTypeID = " << nMatchTypeID );
        return NULL;
    }
    HEROACHIEVEMENT& pmapHeroAchievement = it->second;
    
    HEROACHIEVEMENT::iterator itResult = pmapHeroAchievement.find(nRankLv);
    if ( itResult == pmapHeroAchievement.end() )
    {
        ErrorLn(__FUNCTION__": at m_mapHeroAchievementScore can't find nMatTypeID = " << nMatchTypeID << ", nRankLv = " << nRankLv);
        return NULL;
    }
    return &itResult->second;
}

bool CSchemeHeroAchievement::LoadScheme()
{
    ISchemeEngine* pSchemeEngine = gServerGlobal->getSchemeEngine();
    if ( pSchemeEngine == NULL)
    {
        return false;
    }

    // 加载HeroAchievementScore.csv脚本
    string stringPath = SCP_PATH_FORMAT( HEROACHIEVEMENTSCORE_SCHEME_FILENAME );
    bool bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink*) this );
    if ( !bResult )
    {
        ErrorLn("config file load failed filename = " << stringPath.c_str());
        return false;
    }
    AfterSchemeLoaded();
    return true;
}

// 关闭
void CSchemeHeroAchievement::Close()
{
    // 清空
    m_mapHeroAchievementScore.clear();
}

bool CSchemeHeroAchievement::loadHeroAchievementScore(SCRIPT_READER reader)
{
    ICSVReader * pCSVReader = reader.pCSVReader;
    if ( pCSVReader==0 || reader.type!=READER_CSV)
        return false;

    // 清空
    m_mapHeroAchievementScore.clear();

    int nLen = 0;
    // 读取
    int nRecordCount = pCSVReader->GetRecordCount();

    for ( int nRow = 0; nRow < nRecordCount; ++nRow )
    {
        int nColumnIndex = 0;
        SHeroAchievementScoreConfig item;
        item.nID = pCSVReader->GetInt(nRow, nColumnIndex++, 0);
        item.nRankLV = pCSVReader->GetInt(nRow, nColumnIndex++, 0);
        item.nExpectValue = pCSVReader->GetInt(nRow, nColumnIndex++, 0);
        item.nWinRateValue = pCSVReader->GetInt(nRow, nColumnIndex++, 0);
        item.nScoreValue = pCSVReader->GetInt(nRow, nColumnIndex++, 0);
        item.nScoreMinLimit = pCSVReader->GetInt(nRow, nColumnIndex++, 0);
        item.nScoringRatio = pCSVReader->GetInt(nRow, nColumnIndex++, 0);
        item.nScoringRewards = pCSVReader->GetInt(nRow, nColumnIndex++, 0);

        std::map<int, HEROACHIEVEMENT>::iterator it = m_mapHeroAchievementScore.find(item.nID);
        if ( it == m_mapHeroAchievementScore.end() )
        {
            HEROACHIEVEMENT mapHeroAchievement;
            mapHeroAchievement[item.nRankLV] = item;
            m_mapHeroAchievementScore[item.nID] = mapHeroAchievement;
        }else
        {
            HEROACHIEVEMENT& pHeroAchievement = it->second;
            pHeroAchievement[item.nRankLV] = item;
        }
    }
    return true;
}