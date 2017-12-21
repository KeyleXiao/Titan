/*******************************************************************
** 文件名:	CSchemeChatCD.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2015 - All Rights Reserved
** 创建人:	李有红
** 日  期:	2017/1/3
** 版  本:	1.0
** 描  述:	
            聊听CD时间
********************************************************************/


#include "Stdafx.h"
#include <string>
#include "IServerGlobal.h"
#include "SchemeChatCD.h"

CSchemeChatCD::CSchemeChatCD(void)
{

}

CSchemeChatCD::~CSchemeChatCD(void)
{

}

// 载入脚本
bool CSchemeChatCD::LoadScheme(void)
{
    ISchemeEngine *pSchemeEngine = gServerGlobal->getSchemeEngine();
    if (pSchemeEngine == NULL)
    {
        return false;
    }

    // 加载脚本
    string stringPath = SCP_PATH_FORMAT(CHATCD_SCHEME_FILENAME);
    bool bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink *)this);
    if (!bResult)
    {
        ErrorLn("config file load failed! filename=" << stringPath.c_str());
        return false;
    }

    return true;
}

// 关闭
void CSchemeChatCD::Close(void)
{
    m_mapScheme.clear();
}


/////////////////////////////////ISchemeUpdateSink/////////////////////////////////////////
// CSV配置载入时通知
bool CSchemeChatCD::OnSchemeLoad(SCRIPT_READER reader,const char* szFileName)
{
    ICSVReader *pCSVReader = reader.pCSVReader;
    if ( pCSVReader == NULL || reader.type != READER_CSV)
        return false;

    m_mapScheme.clear();

    // 读取
    int nRecordCount = pCSVReader->GetRecordCount();
    for ( int nRow = 0; nRow < nRecordCount; ++nRow )
    {
		SSchemeChatCD node;

		// 频道ID
		node.nChannel = pCSVReader->GetInt(nRow, COLNUM_CHANNEL, -1);
		if (node.nChannel < 0)
		{
			continue;
		}

		// 等级限制
		node.nLimitLevel = pCSVReader->GetInt(nRow, COLNUM_LEVEL, 1);

		// 冷却时间
		node.nCDTime = pCSVReader->GetInt(nRow, COLNUM_TIME, 0);

        m_mapScheme[node.nChannel] = node;
    }

	AfterSchemeLoaded();

    return true;
}

	
// 配置动态更新时通知
bool CSchemeChatCD::OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName)
{
    CSchemeChatCD* pNewInstance = NewInstance();
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


int CSchemeChatCD::getLimitLevel( int nChannel )
{
	std::map<int, SSchemeChatCD>::iterator iter = m_mapScheme.find(nChannel);
	if (iter == m_mapScheme.end())
	{
		WarningLn(__FUNCTION__": not found item, check scheme file config! channel="<<nChannel);
		return 0;
	}

	return iter->second.nLimitLevel;
}

int CSchemeChatCD::getCDTime( int nChannel )
{
	std::map<int, SSchemeChatCD>::iterator iter = m_mapScheme.find(nChannel);
	if (iter == m_mapScheme.end())
	{
		WarningLn(__FUNCTION__": not found item, check scheme file config! channel="<<nChannel);
		return 0;
	}

	return iter->second.nCDTime;
}

