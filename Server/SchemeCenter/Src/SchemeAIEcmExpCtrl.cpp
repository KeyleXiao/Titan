/*******************************************************************
** 文件名:	CSchemeAIEcmExpCtrl.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2015 - All Rights Reserved
** 创建人:	李有红
** 日  期:	2017/10/28
** 版  本:	1.0
** 描  述:
			经济控制脚本
********************************************************************/

#include "Stdafx.h"
#include <string>
#include "IServerGlobal.h"
#include "SchemeAIEcmExpCtrl.h"

CSchemeAIEcmExpCtrl::CSchemeAIEcmExpCtrl(void)
{

}

CSchemeAIEcmExpCtrl::~CSchemeAIEcmExpCtrl(void)
{

}

// 载入脚本
bool CSchemeAIEcmExpCtrl::LoadScheme(void)
{
    ISchemeEngine *pSchemeEngine = gServerGlobal->getSchemeEngine();
    if (pSchemeEngine == NULL)
    {
        return false;
    }

    // 加载AI经济控制脚本
    string stringPath = SCP_PATH_FORMAT(AIECM_CTR_SCHEME_FILENAME);
    bool bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink *)this);
    if (!bResult)
    {
        ErrorLn("config file load failed! filename=" << stringPath.c_str());
        return false;
    }

	// 加载AI经验控制脚本
	stringPath = SCP_PATH_FORMAT(AIEXP_CTR_SCHEME_FILENAME);
	bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink *)this);
	if (!bResult)
	{
		ErrorLn("config file load failed! filename=" << stringPath.c_str());
		return false;
	}
    return true;
}

// 关闭
void CSchemeAIEcmExpCtrl::Close(void)
{
	m_mapEcmCtrlScheme.clear();
	m_mapExpCtrlScheme.clear();
}


/////////////////////////////////ISchemeUpdateSink/////////////////////////////////////////
// CSV配置载入时通知
bool CSchemeAIEcmExpCtrl::OnSchemeLoad(SCRIPT_READER reader,const char* szFileName)
{

	bool bRet = false;
	string strAIEcmCtrPath = SCP_PATH_FORMAT(AIECM_CTR_SCHEME_FILENAME);
	string strAIExpCtrPath = SCP_PATH_FORMAT(AIEXP_CTR_SCHEME_FILENAME);

	if (rkt::StringHelper::casecmp(szFileName, strAIEcmCtrPath.c_str()) == 0)
	{
		bRet = LoadAIEcmCtrlScheme(reader);
	}

	if (rkt::StringHelper::casecmp(szFileName, strAIExpCtrPath.c_str()) == 0)
	{
		bRet = LoadAIExpCtrlScheme(reader);
	}

	if (!bRet)
	{
		return false;
	}

  
	AfterSchemeLoaded();

    return true;
}

	
// 配置动态更新时通知
bool CSchemeAIEcmExpCtrl::OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName)
{
	CSchemeAIEcmExpCtrl* pNewInstance = NewInstance();
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

bool CSchemeAIEcmExpCtrl::LoadAIEcmCtrlScheme(SCRIPT_READER reader)
{
	ICSVReader *pCSVReader = reader.pCSVReader;
	if (pCSVReader == NULL || reader.type != READER_CSV)
		return false;

	m_mapEcmCtrlScheme.clear();
	std::vector<std::string> vecDataList;

	// 读取
	int nRecordCount = pCSVReader->GetRecordCount();
	for (int nRow = 0; nRow < nRecordCount; ++nRow)
	{
		SAIEcmCtrlConfig_S node;

		node.configWarID = pCSVReader->GetInt(nRow, COLNUM_CONFIG_WAR_ID, -1);

		char buf[64] = { 0 };
		int nLen = sizeof(buf);
		if (pCSVReader->GetString(nRow, COLNUM_MATCHSCORE_RANGE, buf, nLen))
		{
			vecDataList.clear();
			StringHelper::split(vecDataList, buf, ';');
			if (vecDataList.size() != 2)
			{
				ErrorLn(__FUNCTION__":Parsing script failed.MatchScore has more than 2." << AIECM_CTR_SCHEME_FILENAME);
				return false;
			}
			node.matchScoreMin = StringHelper::toInt(vecDataList[0]);
			node.matchScoreMax = StringHelper::toInt(vecDataList[1]);
			
		}
		else
		{
			ErrorLn(__FUNCTION__":Parsing script failed.MatchScore not right." << AIECM_CTR_SCHEME_FILENAME);
			return false;
		}

		node.id = pCSVReader->GetInt(nRow, COLNUM_ID, 0);
		node.aRatio = pCSVReader->GetInt(nRow, COLNUM_A_RATIO, 0);
		node.bRatio = pCSVReader->GetInt(nRow, COLNUM_B_RATIO, 0);
		node.nRatioType[EWEcmExpCtrlRatioType_Sodlier] = pCSVReader->GetFloat(nRow, COLNUM_SOLDIER, 0);
		node.nRatioType[EWEcmExpCtrlRatioType_Monster] = pCSVReader->GetFloat(nRow, COLNUM_MONSTER, 0);
		node.nRatioType[EWEcmExpCtrlRatioType_Salary] = pCSVReader->GetFloat(nRow, COLNUM_SALARY, 0);
		node.nRatioType[EWEcmExpCtrlRatioType_Hero] = pCSVReader->GetFloat(nRow, COLNUM_HERO, 0);

		m_mapEcmCtrlScheme[node.configWarID][node.id - 1].push_back(node);
	}

	return true;
}

bool CSchemeAIEcmExpCtrl::LoadAIExpCtrlScheme(SCRIPT_READER reader)
{
	ICSVReader *pCSVReader = reader.pCSVReader;
	if (pCSVReader == NULL || reader.type != READER_CSV)
		return false;

	m_mapExpCtrlScheme.clear();
	std::vector<std::string> vecDataList;

	// 读取
	int nRecordCount = pCSVReader->GetRecordCount();
	for (int nRow = 0; nRow < nRecordCount; ++nRow)
	{
		SAIExpCtrlConfig_S node;

		node.configWarID = pCSVReader->GetInt(nRow, COLNUM_CONFIG_WAR_ID, -1);

		char buf[64] = { 0 };
		int nLen = sizeof(buf);
		if (pCSVReader->GetString(nRow, COLNUM_MATCHSCORE_RANGE, buf, nLen))
		{
			vecDataList.clear();
			StringHelper::split(vecDataList, buf, ';');
			if (vecDataList.size() != 2)
			{
				ErrorLn(__FUNCTION__":Parsing script failed.MatchScore has more than 2." << AIEXP_CTR_SCHEME_FILENAME);
				return false;
			}
			node.matchScoreMin = StringHelper::toInt(vecDataList[0]);
			node.matchScoreMax = StringHelper::toInt(vecDataList[1]);

		}
		else
		{
			ErrorLn(__FUNCTION__":Parsing script failed.MatchScore not right." << AIEXP_CTR_SCHEME_FILENAME);
			return false;
		}

		node.id = pCSVReader->GetInt(nRow, COLNUM_ID, 0);
		node.aRatio = pCSVReader->GetInt(nRow, COLNUM_A_RATIO, 0);
		node.bRatio = pCSVReader->GetInt(nRow, COLNUM_B_RATIO, 0);
		node.nRatioType[EWEcmExpCtrlRatioType_Sodlier] = pCSVReader->GetFloat(nRow, COLNUM_SOLDIER, 0);
		node.nRatioType[EWEcmExpCtrlRatioType_Monster] = pCSVReader->GetFloat(nRow, COLNUM_MONSTER, 0);
		node.nRatioType[EWEcmExpCtrlRatioType_Salary] = pCSVReader->GetFloat(nRow, COLNUM_SALARY, 0);
		node.nRatioType[EWEcmExpCtrlRatioType_Hero] = pCSVReader->GetFloat(nRow, COLNUM_HERO, 0);

		m_mapExpCtrlScheme[node.configWarID][node.id - 1].push_back(node);
	}

	return true;
}

SAIEcmCtrlConfig_S* CSchemeAIEcmExpCtrl::getAIEcmCtrl(int nConfigWarID, int nMatchScore, int nLineID )
{
	ECM_CTRL_MAP_ITER iter = m_mapEcmCtrlScheme.find(nConfigWarID);
	if (iter == m_mapEcmCtrlScheme.end())
	{
		return NULL;
	}

	map<int, list<SAIEcmCtrlConfig_S>>::iterator iterListConfig = iter->second.find(nLineID);
	if (iterListConfig == iter->second.end())
	{
		return NULL;
	}

	list<SAIEcmCtrlConfig_S>::iterator iterConfig = iterListConfig->second.begin();
	for (; iterConfig != iterListConfig->second.end(); ++iterConfig)
	{
		if (iterConfig->matchScoreMin < nMatchScore && nMatchScore < iterConfig->matchScoreMax)
		{
			return &(*iterConfig);

		}
	}

	return NULL;
}

SAIExpCtrlConfig_S * CSchemeAIEcmExpCtrl::getAIExpCtrl(int nConfigWarID, int nMatchScore, int nLineID)
{
	EXP_CTRL_MAP_ITER iter = m_mapExpCtrlScheme.find(nConfigWarID);
	if (iter == m_mapExpCtrlScheme.end())
	{
		return NULL;
	}

	map<int, list<SAIExpCtrlConfig_S>>::iterator iterListConfig = iter->second.find(nLineID);
	if (iterListConfig == iter->second.end())
	{
		return NULL;
	}

	list<SAIExpCtrlConfig_S>::iterator iterConfig = iterListConfig->second.begin();
	for (; iterConfig != iterListConfig->second.end(); ++iterConfig)
	{
		if (iterConfig->matchScoreMin < nMatchScore && nMatchScore < iterConfig->matchScoreMax)
		{
			return &(*iterConfig);

		}
	}

	return NULL;
}
