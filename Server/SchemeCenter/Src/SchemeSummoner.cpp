/*******************************************************************
** 文件名:	SchemeSummoner.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	马传飞
** 日  期:	2016/8/3
** 版  本:	1.0
** 描  述:	

********************************************************************/
#include "StdAfx.h"
#include "SchemeSummoner.h"
#include "IServerGlobal.h"

bool CSchemeSummonerSkill::OnSchemeLoad(SCRIPT_READER reader,const char* szFileName)
{
    ICSVReader* pCSVReader = reader.pCSVReader;
    if ( pCSVReader == NULL || szFileName == NULL || reader.type != READER_CSV)
    {
        return false;
    }
    m_mapSummonerSkillInfo.clear();
    int nLen = 0;
    int nCount = pCSVReader->GetRecordCount();
    for ( int i = 0; i < nCount; ++i )
    {
        // 记录序列号
        int nColumnIndex = 0;
        SSummonerSkillData item;
        nLen = sizeof(item.stName);
        pCSVReader->GetString( i, nColumnIndex++, item.stName, nLen);
        item.nId = pCSVReader->GetInt(i, nColumnIndex++, 0);
        item.nLv = pCSVReader->GetInt(i, nColumnIndex++, 0);
        item.nIconId = pCSVReader->GetInt(i, nColumnIndex++, 0);
        nLen = sizeof(item.stDesc);
        pCSVReader->GetString(i, nColumnIndex++, item.stDesc, nLen );

        m_mapSummonerSkillInfo[item.nId] = item;
    }
    return true;
}

bool CSchemeSummonerSkill::OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName)
{

    return true;
}


// 根据技能id获得iconId
int CSchemeSummonerSkill::getIconId( int nSkillId )
{
    std::map<int, SSummonerSkillData>::iterator it = m_mapSummonerSkillInfo.find(nSkillId);
    if ( it == m_mapSummonerSkillInfo.end() )
    {
        return -1;
    }
    SSummonerSkillData sSkillInfo = it->second;
    return sSkillInfo.nIconId;
}

bool CSchemeSummonerSkill::LoadScheme(void)
{
    ISchemeEngine *pSchemeEngine = gServerGlobal->getSchemeEngine();
    if(pSchemeEngine == NULL)
    {
        return false;
    }

    // 加载脚本
    string stringPath = SCP_PATH_FORMAT( SUMMONER_SKILL_INFO );
    bool bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink *)this);
    if(!bResult)
    {
        Error("config file load failed! filename=" << stringPath.c_str() << endl);
        return false;
    }
    AfterSchemeLoaded();
    return true;
}

void CSchemeSummonerSkill::Close(void)
{
    m_mapSummonerSkillInfo.clear();
}