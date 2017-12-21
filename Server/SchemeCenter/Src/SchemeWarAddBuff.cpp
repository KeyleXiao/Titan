/*******************************************************************
** �ļ���:	CSchemeWarAddBuff.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2015 - All Rights Reserved
** ������:	���к�
** ��  ��:	2017/4/13
** ��  ��:	1.0
** ��  ��:	
            ս���ӹ������ýű�
********************************************************************/


#include "Stdafx.h"
#include <string>
#include "IServerGlobal.h"
#include "SchemeWarAddBuff.h"
#include "SchemeWarDef.h"

CSchemeWarAddBuff::CSchemeWarAddBuff(void)
{

}

CSchemeWarAddBuff::~CSchemeWarAddBuff(void)
{

}

// ����ű�
bool CSchemeWarAddBuff::LoadScheme(void)
{
    ISchemeEngine *pSchemeEngine = gServerGlobal->getSchemeEngine();
    if (pSchemeEngine == NULL)
    {
        return false;
    }

    // ���ؽű�
    string stringPath = SCP_PATH_FORMAT(WAR_SCHEME_ADD_BUFF);
    bool bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink *)this);
    if (!bResult)
    {
        ErrorLn("config file load failed! filename=" << stringPath.c_str());
        return false;
    }

    return true;
}

// �ر�
void CSchemeWarAddBuff::Close(void)
{
    m_mapScheme.clear();
}


/////////////////////////////////ISchemeUpdateSink/////////////////////////////////////////
// CSV��������ʱ֪ͨ
bool CSchemeWarAddBuff::OnSchemeLoad(SCRIPT_READER reader,const char* szFileName)
{
    ICSVReader *pCSVReader = reader.pCSVReader;
    if ( pCSVReader == NULL || reader.type != READER_CSV)
        return false;

    m_mapScheme.clear();

    // ��ȡ
    int nRecordCount = pCSVReader->GetRecordCount();
    for ( int nRow = 0; nRow < nRecordCount; ++nRow )
    {
		SWarAddBuff node;

		node.nMatchType = pCSVReader->GetInt(nRow, COLNUM_MatchType, 0);
		node.nBuffID = pCSVReader->GetInt(nRow, COLNUM_BuffID, 0);
		node.nLimitLv = pCSVReader->GetInt(nRow, COLNUM_LimitLv, 0);
		node.nTime = pCSVReader->GetInt(nRow, COLNUM_Time, 0) * 1000;
		node.nBuffLv = pCSVReader->GetInt(nRow, COLNUM_BuffLv, 0);
		node.bAddToAI = pCSVReader->GetBool(nRow, COLNUM_IsAddToAI, false);
		node.bAddToAIEx = pCSVReader->GetBool(nRow, COLNUM_IsAddToAIEx, false);

		m_mapScheme[node.nMatchType].push_back(node);
    }

	AfterSchemeLoaded();

    return true;
}

	
// ���ö�̬����ʱ֪ͨ
bool CSchemeWarAddBuff::OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName)
{
    CSchemeWarAddBuff* pNewInstance = NewInstance();
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

// ��ȡ���Buff��Ϣ
vector<SWarAddBuff>* CSchemeWarAddBuff::getAddBuffInfo(int nMatchID)
{
	WAR_ADDBUFF_MAP_ITER iter = m_mapScheme.find(nMatchID);
	if (iter == m_mapScheme.end())
		return NULL;

	return &iter->second;
}
