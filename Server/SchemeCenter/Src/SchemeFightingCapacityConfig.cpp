/*******************************************************************
** 文件名:	SchemeFightingCapacityConfig.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	马传飞
** 日  期:	2016/8/3
** 版  本:	1.0
** 描  述:	

********************************************************************/
#include "StdAfx.h"
#include "SchemeFightingCapacityConfig.h"
#include "IServerGlobal.h"

bool CSchemeFightingCapacityConfig::OnSchemeLoad(SCRIPT_READER reader,const char* szFileName)
{
    ICSVReader * pCSVReader = reader.pCSVReader;
    if ( pCSVReader==0 || reader.type!=READER_CSV)
        return false;

    // 清空
    m_mapFightingCapacity.clear();

    // 读取
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

    // 加载SummonerSkill.csv脚本
    string stringPath = SCP_PATH_FORMAT( FIGHTCONFIG_SCHEME_FILENAME );
    bool bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink*) this );
    if ( !bResult )
    {
        ErrorLn("config file load failed filename = " << stringPath.c_str());;
        return false;
    }
    return true;
}

// 关闭
void CSchemeFightingCapacityConfig::Close()
{
    // 清空
    m_mapFightingCapacity.clear();
}