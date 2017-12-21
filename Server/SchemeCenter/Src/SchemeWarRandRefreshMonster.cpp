/*******************************************************************
** 文件名:	SchemeWarRandRefreshMonster.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2015 - All Rights Reserved
** 创建人:	李有红
** 日  期:	2017/11/25
** 版  本:	1.0
** 描  述:	
            战场随机刷怪
********************************************************************/


#include "Stdafx.h"
#include <string>
#include "IServerGlobal.h"
#include "SchemeWarRandRefreshMonster.h"
#include "SchemeWarDef.h"

CSchemeWarRandRefreshMons::CSchemeWarRandRefreshMons(void)
{

}

CSchemeWarRandRefreshMons::~CSchemeWarRandRefreshMons(void)
{

}

// 载入脚本
bool CSchemeWarRandRefreshMons::LoadScheme(void)
{
    ISchemeEngine *pSchemeEngine = gServerGlobal->getSchemeEngine();
    if (pSchemeEngine == NULL)
    {
        return false;
    }

    // 加载脚本
    string stringPath = SCP_PATH_FORMAT(WAR_SCHEME_RANDREFRESHMONS);
    bool bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink *)this);
    if (!bResult)
    {
        ErrorLn("config file load failed! filename=" << stringPath.c_str());
        return false;
    }

    return true;
}

// 关闭
void CSchemeWarRandRefreshMons::Close(void)
{
	m_mapScheme.clear();
}


/////////////////////////////////ISchemeUpdateSink/////////////////////////////////////////
// CSV配置载入时通知
bool CSchemeWarRandRefreshMons::OnSchemeLoad(SCRIPT_READER reader,const char* szFileName)
{
    ICSVReader *pCSVReader = reader.pCSVReader;
    if ( pCSVReader == NULL || reader.type != READER_CSV)
        return false;

	m_mapScheme.clear();

	float floatArr[3] = { 0 };
	char szBuf[128] = { 0 };
	int nLen = 0;

    // 读取
    int nRecordCount = pCSVReader->GetRecordCount();
    for ( int nRow = 0; nRow < nRecordCount; ++nRow )
    {
		SWarRandRefreshMons node;
		int nCol = 0;

		node.nConfigWarID = pCSVReader->GetInt(nRow, nCol++, 0);
		node.nRefreshTime = pCSVReader->GetInt(nRow, nCol++, 0) * 1000;  // 转换成毫秒
		node.nMonsterIndexID = pCSVReader->GetInt(nRow, nCol++, 0);
		node.nMonsterCamp = pCSVReader->GetInt(nRow, nCol++, 0);
		//node.nMonsterNum = pCSVReader->GetInt(nRow, nCol++, 0);
		node.nAliveTime = pCSVReader->GetInt(nRow, nCol++, 0) * 1000;  // 转换成毫秒
		node.isInSafetyZone = pCSVReader->GetBool(nRow, nCol++, false);
		node.fMonsDistance = pCSVReader->GetFloat(nRow, nCol++, 0);

		// 刷新圆心坐标
		nLen = sizeof(szBuf);
		fill_n(floatArr, 3, 0.0f);
		pCSVReader->GetString(nRow, nCol++, szBuf, nLen);
		int nCount = parseFloatArray(szBuf, floatArr, 3);
		if (nCount != 3)
		{
			ErrorLn(__FUNCTION__":The center of refresh is invalid," << nCount);
			return false;
		}
		node.centerPos.x = floatArr[0];
		node.centerPos.y = floatArr[1];
		node.centerPos.z = floatArr[2];

		node.radiusRange = pCSVReader->GetFloat(nRow, nCol++, 0);

		m_mapScheme[node.nMonsterIndexID] = node;
    }

	AfterSchemeLoaded();

    return true;
}

	
// 配置动态更新时通知
bool CSchemeWarRandRefreshMons::OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName)
{
    CSchemeWarRandRefreshMons* pNewInstance = NewInstance();
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

vector<SWarRandRefreshMons> CSchemeWarRandRefreshMons::getRandRefreshMonsList(int nConfigWarID)
{
	vector<SWarRandRefreshMons> result;
	RANDREFRESHMONS_MAP_ITER iter = m_mapScheme.begin();
	for (; iter != m_mapScheme.end(); ++iter)
	{
		if (iter->second.nConfigWarID == nConfigWarID)
		{
			result.push_back(iter->second);
		}
	}

	return result;
}

SWarRandRefreshMons * CSchemeWarRandRefreshMons::getRandRefreshMons(int nMonsterIndex)
{
	RANDREFRESHMONS_MAP_ITER iter = m_mapScheme.find(nMonsterIndex);
	if (iter == m_mapScheme.end())
		return NULL;

	return &iter->second;
}
