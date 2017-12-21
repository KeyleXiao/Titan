/*******************************************************************
** �ļ���:	SchemeWarRandRefreshMonster.cpp
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2015 - All Rights Reserved
** ������:	���к�
** ��  ��:	2017/11/25
** ��  ��:	1.0
** ��  ��:	
            ս�����ˢ��
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

// ����ű�
bool CSchemeWarRandRefreshMons::LoadScheme(void)
{
    ISchemeEngine *pSchemeEngine = gServerGlobal->getSchemeEngine();
    if (pSchemeEngine == NULL)
    {
        return false;
    }

    // ���ؽű�
    string stringPath = SCP_PATH_FORMAT(WAR_SCHEME_RANDREFRESHMONS);
    bool bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink *)this);
    if (!bResult)
    {
        ErrorLn("config file load failed! filename=" << stringPath.c_str());
        return false;
    }

    return true;
}

// �ر�
void CSchemeWarRandRefreshMons::Close(void)
{
	m_mapScheme.clear();
}


/////////////////////////////////ISchemeUpdateSink/////////////////////////////////////////
// CSV��������ʱ֪ͨ
bool CSchemeWarRandRefreshMons::OnSchemeLoad(SCRIPT_READER reader,const char* szFileName)
{
    ICSVReader *pCSVReader = reader.pCSVReader;
    if ( pCSVReader == NULL || reader.type != READER_CSV)
        return false;

	m_mapScheme.clear();

	float floatArr[3] = { 0 };
	char szBuf[128] = { 0 };
	int nLen = 0;

    // ��ȡ
    int nRecordCount = pCSVReader->GetRecordCount();
    for ( int nRow = 0; nRow < nRecordCount; ++nRow )
    {
		SWarRandRefreshMons node;
		int nCol = 0;

		node.nConfigWarID = pCSVReader->GetInt(nRow, nCol++, 0);
		node.nRefreshTime = pCSVReader->GetInt(nRow, nCol++, 0) * 1000;  // ת���ɺ���
		node.nMonsterIndexID = pCSVReader->GetInt(nRow, nCol++, 0);
		node.nMonsterCamp = pCSVReader->GetInt(nRow, nCol++, 0);
		//node.nMonsterNum = pCSVReader->GetInt(nRow, nCol++, 0);
		node.nAliveTime = pCSVReader->GetInt(nRow, nCol++, 0) * 1000;  // ת���ɺ���
		node.isInSafetyZone = pCSVReader->GetBool(nRow, nCol++, false);
		node.fMonsDistance = pCSVReader->GetFloat(nRow, nCol++, 0);

		// ˢ��Բ������
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

	
// ���ö�̬����ʱ֪ͨ
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
