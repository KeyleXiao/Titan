#include "stdafx.h"
#include "SchemeAIEcmAdjust.h"
#include "IServerGlobal.h"

#define SCHME_AI_ECM_ADJUST "Scp\\AIEcmAdjust.csv" 

CSchemeAIEcmAdjust::CSchemeAIEcmAdjust()
{
    m_nSplitCount = 0;
}

bool CSchemeAIEcmAdjust::LoadScheme()
{
	ISchemeEngine *pSchemeEngine = gServerGlobal->getSchemeEngine();
	if (pSchemeEngine == NULL)
	{
		return false;
	}


	string stringPath = SCP_PATH_FORMAT(SCHME_AI_ECM_ADJUST);
	if (!pSchemeEngine->LoadScheme(stringPath.c_str(), this, true))
	{
		ErrorLn("load " << SCHME_AI_ECM_ADJUST << " failed");
		return false;
	}

	return true;
}

float CSchemeAIEcmAdjust::getAdjustFactor(int nMatchCount, int nEcmDiff)
{
	for (std::vector<EcmAdjustNode>::iterator it = m_vecNode.begin(); it != m_vecNode.end(); ++it)
	{
		if (nEcmDiff > it->nDiffMin && nEcmDiff < it->nDiffMax)
		{
            if(nMatchCount > m_nSplitCount)
            {
                return it->fFactorNormal;
            }
			return it->fFactorEarly;
		}
	}

	return 1;
}

bool CSchemeAIEcmAdjust::OnSchemeLoad(SCRIPT_READER reader,const char* szFileName)
{
	m_vecNode.clear();

	char szData[256] = { 0 };
	std::vector<std::string> vecDataList;

	ICSVReader* pCsvReader = reader.pCSVReader;

	// 读取
	int nRecordCount = pCsvReader->GetRecordCount();
	for (int nRow = 0; nRow < nRecordCount; ++nRow)
	{
		int nCol = 0;

		// 经济差
		memset(&szData, 0, sizeof(szData));
		int nDataLen = sizeof(szData);
		pCsvReader->GetString(nRow, nCol++, szData, nDataLen);
		vecDataList.clear();
		StringHelper::split(vecDataList, szData, ';');
		if(vecDataList.size() != 2)
			continue;

		EcmAdjustNode node;
		node.nDiffMin = StringHelper::toInt(vecDataList[0]);
		node.nDiffMax = StringHelper::toInt(vecDataList[1]);

		// 前期补偿系数
		node.fFactorEarly = pCsvReader->GetFloat(nRow, nCol++);

        // 正常补偿系数
        node.fFactorNormal = pCsvReader->GetFloat(nRow, nCol++);

        // 分界场次
        m_nSplitCount = pCsvReader->GetInt(nRow, nCol++);

		m_vecNode.push_back(node);
	}

    AfterSchemeLoaded();

    return true;
}

bool CSchemeAIEcmAdjust::OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName)
{
    CSchemeAIEcmAdjust* pNew = NewInstance();
    if(pNew == NULL)
    {
        return false;
    }

    if(pNew->OnSchemeLoad(reader, szFileName))
    {
        return true;
    }
    else
    {
        safeDelete(pNew);
        return false;
    }
}

void CSchemeAIEcmAdjust::Close()
{
	m_vecNode.clear();
}