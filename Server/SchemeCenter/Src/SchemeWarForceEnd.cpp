/*******************************************************************
** 文件名:	CSchemeWarForceEnd.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2015 - All Rights Reserved
** 创建人:	李有红
** 日  期:	2017/3/28
** 版  本:	1.0
** 描  述:	
            战场强制脚本
********************************************************************/


#include "Stdafx.h"
#include <string>
#include "IServerGlobal.h"
#include "SchemeWarForceEnd.h"
#include "SchemeWarDef.h"

CSchemeWarForceEnd::CSchemeWarForceEnd(void)
{

}

CSchemeWarForceEnd::~CSchemeWarForceEnd(void)
{

}

// 载入脚本
bool CSchemeWarForceEnd::LoadScheme(void)
{
    ISchemeEngine *pSchemeEngine = gServerGlobal->getSchemeEngine();
    if (pSchemeEngine == NULL)
    {
        return false;
    }

    // 加载脚本
    string stringPath = SCP_PATH_FORMAT(WAR_SCHEME_FORCE_END);
    bool bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink *)this);
    if (!bResult)
    {
        ErrorLn("config file load failed! filename=" << stringPath.c_str());
        return false;
    }

    return true;
}

// 关闭
void CSchemeWarForceEnd::Close(void)
{
    m_mapScheme.clear();
}


/////////////////////////////////ISchemeUpdateSink/////////////////////////////////////////
// CSV配置载入时通知
bool CSchemeWarForceEnd::OnSchemeLoad(SCRIPT_READER reader,const char* szFileName)
{
    ICSVReader *pCSVReader = reader.pCSVReader;
    if ( pCSVReader == NULL || reader.type != READER_CSV)
        return false;

    m_mapScheme.clear();

    // 读取
    int nRecordCount = pCSVReader->GetRecordCount();
    for ( int nRow = 0; nRow < nRecordCount; ++nRow )
    {
		SWarForceEndInfo node;

		node.nMatchType = pCSVReader->GetInt(nRow, COLNUM_MatchType, 0);
		node.bEnable = pCSVReader->GetBool(nRow, COLNUM_IsEnable, false);
		node.dwTriggerTime = pCSVReader->GetInt(nRow, COLNUM_Time, 0);
		node.nKillDiffWeight = pCSVReader->GetInt(nRow, COLNUM_KillDiffWeight, 0);
		node.nEcmDiffWeight = pCSVReader->GetInt(nRow, COLNUM_EcmDiffWeight, 0);
		node.nTowerDiffWeight = pCSVReader->GetInt(nRow, COLNUM_TowerDiffWeight, 0);

		m_mapScheme[node.nMatchType] = node;
    }

	AfterSchemeLoaded();

    return true;
}

	
// 配置动态更新时通知
bool CSchemeWarForceEnd::OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName)
{
    CSchemeWarForceEnd* pNewInstance = NewInstance();
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

SWarForceEndInfo* CSchemeWarForceEnd::getWarForceEndInfo(int nMatchType)
{
	WAR_FORCEEND_MAP_ITER iter = m_mapScheme.find(nMatchType);
	if (iter == m_mapScheme.end())
	{
		return NULL;
	}

	return &(iter->second);
}

bool CSchemeWarForceEnd::isEnableForceEnd(int nMatchType)
{
	WAR_FORCEEND_MAP_ITER iter = m_mapScheme.find(nMatchType);
	if (iter == m_mapScheme.end())
	{
		return false;
	}

	return iter->second.bEnable;
}
