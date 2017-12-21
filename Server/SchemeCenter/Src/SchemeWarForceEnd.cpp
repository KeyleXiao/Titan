/*******************************************************************
** �ļ���:	CSchemeWarForceEnd.cpp
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2015 - All Rights Reserved
** ������:	���к�
** ��  ��:	2017/3/28
** ��  ��:	1.0
** ��  ��:	
            ս��ǿ�ƽű�
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

// ����ű�
bool CSchemeWarForceEnd::LoadScheme(void)
{
    ISchemeEngine *pSchemeEngine = gServerGlobal->getSchemeEngine();
    if (pSchemeEngine == NULL)
    {
        return false;
    }

    // ���ؽű�
    string stringPath = SCP_PATH_FORMAT(WAR_SCHEME_FORCE_END);
    bool bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink *)this);
    if (!bResult)
    {
        ErrorLn("config file load failed! filename=" << stringPath.c_str());
        return false;
    }

    return true;
}

// �ر�
void CSchemeWarForceEnd::Close(void)
{
    m_mapScheme.clear();
}


/////////////////////////////////ISchemeUpdateSink/////////////////////////////////////////
// CSV��������ʱ֪ͨ
bool CSchemeWarForceEnd::OnSchemeLoad(SCRIPT_READER reader,const char* szFileName)
{
    ICSVReader *pCSVReader = reader.pCSVReader;
    if ( pCSVReader == NULL || reader.type != READER_CSV)
        return false;

    m_mapScheme.clear();

    // ��ȡ
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

	
// ���ö�̬����ʱ֪ͨ
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
