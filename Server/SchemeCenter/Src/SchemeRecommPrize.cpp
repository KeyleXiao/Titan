/*******************************************************************
** 文件名:	SchemeHeroCard.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	王全申
** 日  期:	2017-12-22
** 版  本:	1.0
** 描  述:	推广员奖励配置 RecommendPrize.csv

			
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#include <string>
#include "StdAfx.h"
#include "SchemeRecommPrize.h"
#include "IServerGlobal.h"
#include "SchemeDef.h"

using namespace std;

CSchemeRecommPrize::CSchemeRecommPrize(void)
{	

}

bool CSchemeRecommPrize::LoadScheme(void)
{
	ISchemeEngine *pSchemeEngine = gServerGlobal->getSchemeEngine();
	if(pSchemeEngine == NULL)
	{
		return false;
	}

	// 加载csv脚本
    string stringPath = SCP_PATH_FORMAT(RECOMMPRIZE_SCHEME_FILENAME);
	bool bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink *)this);
	if(!bResult)
	{
		ErrorLn("config file load failed! filename=" << stringPath.c_str());
		return false;
	}

	return true;	
}

void CSchemeRecommPrize::Close(void)
{	
	// 清空
	m_mapConfig.clear();
}

bool CSchemeRecommPrize::OnSchemeLoad(SCRIPT_READER reader,const char* szFileName)
{
	ICSVReader * pCSVReader = reader.pCSVReader;

	if(pCSVReader == NULL || szFileName == NULL || reader.type!=READER_CSV)
	{
		return false;
	}

	// 清空
    m_mapConfig.clear();

    int nLen = 0;
    char szPrizeID[128] = {0};
    int prizeIDArray[3];
	// 读取
	int nRecordCount = pCSVReader->GetRecordCount();
	for (int nRow=0; nRow<nRecordCount; ++nRow)
	{
        SSchemeRecommPrize node;
		node.nTargetID = pCSVReader->GetInt(nRow, COL_TARGET_ID, 0);
        node.nMatchNum = pCSVReader->GetInt(nRow, COL_MATCH_NUM);
        szPrizeID[0] = '\0';
        nLen = sizeof(szPrizeID) / sizeof(char);
        pCSVReader->GetString(nRow, COL_PRIZE_ID, szPrizeID, nLen);

        int count = parseIntArray(szPrizeID, prizeIDArray, sizeof(prizeIDArray)/sizeof(int));
        for (int i = 0; i < count; ++i)
            node.vecPrizeID.push_back(prizeIDArray[i]);

        m_mapConfig[node.nTargetID] = node;
	}

	AfterSchemeLoaded();

	return true;
}

SSchemeRecommPrize* CSchemeRecommPrize::getByTargetID(int nTargetID)
{
    if (m_mapConfig.find(nTargetID) == m_mapConfig.end())
        return NULL;

    return &m_mapConfig[nTargetID];
}

bool CSchemeRecommPrize::OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName)
{
	CSchemeRecommPrize* pNewInstance = NewInstance();
	if(pNewInstance == NULL)
	{
		return false;
	}
	
	if(pNewInstance->OnSchemeLoad(reader, szFileName))
	{
		return true;
	}
	else
	{
		safeDelete(pNewInstance);
		return false;
	}
}