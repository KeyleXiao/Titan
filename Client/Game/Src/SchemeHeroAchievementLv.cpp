/*******************************************************************
** 文件名:	SchemeHeroAchievementLv.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	马传飞
** 日  期:	2016-3-31
** 版  本:	1.0
** 描  述:	英雄成就分数及表现
********************************************************************/
#include "StdAfx.h"
#include "SchemeHeroAchievementLv.h"
#include "SchemeDef.h"
#include "IClientGlobal.h"

bool CSchemeHeroAchievementLv::OnSchemeLoad(SCRIPT_READER reader,const char* szFileName)
{
    ICSVReader * pCSVReader = reader.pCSVReader;
    if ( pCSVReader==0 || reader.type!=READER_CSV)
        return false;

    // 清空
    m_vecpHeroAchievementLv.clear();

    int nLen = 0;
    // 读取
    int nRecordCount = pCSVReader->GetRecordCount();
    for ( int nRow = 0; nRow < nRecordCount; ++nRow )
    {
        int nColumnIndex = 0;
        SHeroAchievementLvConfig item;
        item.nID = pCSVReader->GetInt(nRow, nColumnIndex++, 0);
        nLen = sizeof(item.szAchievementDesc);
        pCSVReader->GetString( nRow, nColumnIndex++, item.szAchievementDesc, nLen);
        nLen = sizeof(item.szIconID);
        pCSVReader->GetString(nRow, nColumnIndex++, item.szIconID, nLen);
        item.nAchievementScore = pCSVReader->GetInt(nRow, nColumnIndex++, 0);
        m_vecpHeroAchievementLv.push_back(item);
    }
    return true;
}

bool CSchemeHeroAchievementLv::OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName)
{
    return OnSchemeLoad(reader, szFileName);
}

int CSchemeHeroAchievementLv::getIconIDAccordScore( int nScore, int* pArrIcon, int nCount)
{
    string stIcon;
    std::vector<SHeroAchievementLvConfig>::iterator it = m_vecpHeroAchievementLv.begin();
    for (; it != m_vecpHeroAchievementLv.end(); ++it )
    {
        if (it->nAchievementScore < nScore )
        {
            stIcon = it->szIconID;
            continue;
        }
        break;
    }
    return parseIntArray( stIcon.c_str(), pArrIcon, nCount);;
}


bool CSchemeHeroAchievementLv::LoadScheme()
{
    ISchemeEngine* pSchemeEngine = gClientGlobal->getSchemeEngine();
    if ( pSchemeEngine == NULL)
    {
        return false;
    }

    // 加载SummonerSkill.csv脚本
    string stringPath = SCP_PATH_FORMAT( HEROACHIEVEMENTLV_SCHEME_FILENAME );
    bool bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink*) this );
    if ( !bResult )
    {
        ErrorLn("config file load failed filename = " << stringPath.c_str());;
        return false;
    }
    return true;
}

// 关闭
void CSchemeHeroAchievementLv::Close()
{
    // 清空
    m_vecpHeroAchievementLv.clear();
}