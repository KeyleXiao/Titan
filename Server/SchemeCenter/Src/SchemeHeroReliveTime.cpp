/*******************************************************************
** 文件名:	SchemeHeroReliveTime.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2015 - All Rights Reserved
** 创建人:	李有红
** 日  期:	2017/5/10
** 版  本:	1.0
** 描  述:
英雄复活时间
********************************************************************/


#include "Stdafx.h"
#include <string>
#include "IServerGlobal.h"
#include "SchemeHeroReliveTime.h"

CSchemeHeroReliveTime::CSchemeHeroReliveTime(void)
{

}

CSchemeHeroReliveTime::~CSchemeHeroReliveTime(void)
{

}

// 载入脚本
bool CSchemeHeroReliveTime::LoadScheme(void)
{
    ISchemeEngine *pSchemeEngine = gServerGlobal->getSchemeEngine();
    if (pSchemeEngine == NULL)
    {
        return false;
    }

    // 加载脚本
    string stringPath = SCP_PATH_FORMAT(HERORELIVETIME_SCHEME_FILENAME);
    bool bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink *)this);
    if (!bResult)
    {
        ErrorLn("config file load failed! filename=" << stringPath.c_str());
        return false;
    }

    return true;
}

// 关闭
void CSchemeHeroReliveTime::Close(void)
{
    m_mapScheme.clear();
}


/////////////////////////////////ISchemeUpdateSink/////////////////////////////////////////
// CSV配置载入时通知
bool CSchemeHeroReliveTime::OnSchemeLoad(SCRIPT_READER reader,const char* szFileName)
{
    ICSVReader *pCSVReader = reader.pCSVReader;
    if ( pCSVReader == NULL || reader.type != READER_CSV)
        return false;

    m_mapScheme.clear();

    // 读取
    int nRecordCount = pCSVReader->GetRecordCount();
    for ( int nRow = 0; nRow < nRecordCount; ++nRow )
    {
		SHeroReliveTime node;
		
		node.nConfigWarID = pCSVReader->GetInt(nRow, COLNUM_ConfigWarID, 0);
		node.nLevel = pCSVReader->GetInt(nRow, COLNUM_Level, 0);
		node.nReliveTime = pCSVReader->GetInt(nRow, COLNUM_ReliveTime, 0) * 1000;		// 转换为毫秒

		m_mapScheme[node.nConfigWarID][node.nLevel] = node.nReliveTime;
    }

	AfterSchemeLoaded();

    return true;
}

	
// 配置动态更新时通知
bool CSchemeHeroReliveTime::OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName)
{
    CSchemeHeroReliveTime* pNewInstance = NewInstance();
    if (pNewInstance == NULL)
    {
        return false;
    }

    if (pNewInstance->OnSchemeLoad(reader, szFileName))
    {
        return true;
    }
    else
    {
        safeDelete(pNewInstance);
        return false;
    }
}

int CSchemeHeroReliveTime::getHeroReliveTime(int nConfigWarID, int nLevel)
{
	HERO_RELIVETIME_MAP_ITER it = m_mapScheme.find(nConfigWarID);
	if (it == m_mapScheme.end())
	{
		return 0;
	}

	map<int, int>::iterator it2 = it->second.find(nLevel);
	if (it2 == it->second.end())
	{
		return 0;
	}

	return it2->second;
}