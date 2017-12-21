/*******************************************************************
** 文件名:	CSchemeWarSafetyZone.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2015 - All Rights Reserved
** 创建人:	李有红
** 日  期:	2017/11/1
** 版  本:	1.0
** 描  述:
大逃杀-安全区/毒气区配置
********************************************************************/

#include "Stdafx.h"
#include "SchemeWarSafetyZone.h"
#include "IServerGlobal.h"

CSchemeWarSafetyZone::CSchemeWarSafetyZone(void)
{

}

CSchemeWarSafetyZone::~CSchemeWarSafetyZone(void)
{

}

// 载入脚本
bool CSchemeWarSafetyZone::LoadScheme(void)
{
    ISchemeEngine *pSchemeEngine = gServerGlobal->getSchemeEngine();
    if (pSchemeEngine == NULL)
    {
        return false;
    }

    // 加载脚本
    string stringPath = SCP_PATH_FORMAT(WAR_SCHEME_SAFETYZONE);
    bool bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink *)this);
    if (!bResult)
    {
        ErrorLn("config file load failed! filename=" << stringPath.c_str());
        return false;
    }

    return true;
}

// 关闭
void CSchemeWarSafetyZone::Close(void)
{
    m_mapScheme.clear();
}


/////////////////////////////////ISchemeUpdateSink/////////////////////////////////////////
// CSV配置载入时通知
bool CSchemeWarSafetyZone::OnSchemeLoad(SCRIPT_READER reader,const char* szFileName)
{
    ICSVReader *pCSVReader = reader.pCSVReader;
    if ( pCSVReader == NULL || reader.type != READER_CSV)
        return false;

    m_mapScheme.clear();

	vector<string> strList;
	char szBuf[128] = { 0 };
	int nLen = sizeof(szBuf);

    // 读取
    int nRecordCount = pCSVReader->GetRecordCount();
    for ( int nRow = 0; nRow < nRecordCount; ++nRow )
    {
		SWarSafetyZone node;
		int nCol = 0;
		
		// 轮数
		node.nRound = pCSVReader->GetInt(nRow, nCol++, 0);

		// 名称
		nCol++;

		// 毒区圆心(只需要取配置中的第一次的毒区圆心)
		nLen = sizeof(szBuf);
		strList.clear();
		if (nRow == 0)
		{
			if (pCSVReader->GetString(nRow, nCol, szBuf, nLen))
			{
				StringHelper::split(strList, szBuf, ';');
				if (strList.size() != 3)
				{
					ErrorLn(__FUNCTION__":centerOfNoxiousZone(x,y,z), The number of params isn't 3, num=" << strList.size() << ",nRound" << node.nRound);
					return false;
				}

				node.centerOfNoxiousZone.x = StringHelper::toFloat(strList[0]);
				node.centerOfNoxiousZone.y = StringHelper::toFloat(strList[1]);
				node.centerOfNoxiousZone.z = StringHelper::toFloat(strList[2]);
			}
			else
			{
				ErrorLn(__FUNCTION__":centerOfNoxiousZone(x,y,z), Invalid string, nRound=" << node.nRound);
				return false;
			}
		}
		nCol++;

		// 毒区半径
		node.fRadiusOfNoxiousZone = pCSVReader->GetFloat(nRow, nCol++, 0);

		// 安全区半径
		node.fRadiusOfSafetyZone = pCSVReader->GetFloat(nRow, nCol++, 0);

		// 扩散倒计时
		node.nSpreadCountdown = pCSVReader->GetInt(nRow, nCol++, 0) * 1000;  // 转换成毫秒

		// 扩散时间
		node.nSpreadTime = pCSVReader->GetInt(nRow, nCol++, 0) * 1000;  // 转换成毫秒
		
		// 扩散提示时间
		nLen = sizeof(szBuf);
		strList.clear();
		if (pCSVReader->GetString(nRow, nCol++, szBuf, nLen))
		{
			StringHelper::split(strList, szBuf, ';');
			if (strList.empty())
			{
				ErrorLn(__FUNCTION__":nSpreadTipsTime, is empty, num=" << strList.size() << ",nRound" << node.nRound);
				return false;
			}

			for (int i = 0; i < strList.size(); ++i)
			{
				int nTime = StringHelper::toInt(strList[i]) * 1000; // 转换成毫秒

				node.nSpreadTipsTime.push_back(nTime);
			}
		}
		else
		{
			ErrorLn(__FUNCTION__":buffOfNoxiousZone(buffID,buffLevel), Invalid string, nRound=" << node.nRound);
			return false;
		}

		// 安全区广播文本
		nLen = sizeof(node.szCDBroadcast);
		pCSVReader->GetString(nRow, nCol++, node.szCDBroadcast, nLen);

		// 毒区扩散文本
		nLen = sizeof(node.szSpreadBeginBcast);
		pCSVReader->GetString(nRow, nCol++, node.szSpreadBeginBcast, nLen);

		// 毒区Buff
		nLen = sizeof(szBuf);
		strList.clear();
		if (pCSVReader->GetString(nRow, nCol++, szBuf, nLen))
		{
			StringHelper::split(strList, szBuf, ';');
			if (strList.size() != 2)
			{
				ErrorLn(__FUNCTION__":buffOfNoxiousZone(buffID,buffLevel), The number of params isn't 2, num=" << strList.size() << ",nRound" << node.nRound);
				return false;
			}

			node.nBuffID = StringHelper::toFloat(strList[0]);
			node.nBuffIDLevel = StringHelper::toFloat(strList[1]);
		}
		else
		{
			ErrorLn(__FUNCTION__":buffOfNoxiousZone(buffID,buffLevel), Invalid string, nRound=" << node.nRound);
			return false;
		}

		m_mapScheme[node.nRound] = node;
    }

	AfterSchemeLoaded();

    return true;
}

	
// 配置动态更新时通知
bool CSchemeWarSafetyZone::OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName)
{
	CSchemeWarSafetyZone* pNewInstance = NewInstance();
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

SWarSafetyZone * CSchemeWarSafetyZone::getWarSafetyZone(int nRound)
{
	WAR_SAFETYZONE_MAP_ITER iter = m_mapScheme.find(nRound);
	if (iter == m_mapScheme.end())
	{
		return NULL;
	}

	return &(iter->second);
}
