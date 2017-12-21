/*******************************************************************
** �ļ���:	CSchemeWarSubFuncSetting.h
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
#include "SchemeWarSubFunc.h"
#include "SchemeWarDef.h"

CSchemeWarSubFunc::CSchemeWarSubFunc(void)
{

}

CSchemeWarSubFunc::~CSchemeWarSubFunc(void)
{

}

// ����ű�
bool CSchemeWarSubFunc::LoadScheme(void)
{
    ISchemeEngine *pSchemeEngine = gServerGlobal->getSchemeEngine();
    if (pSchemeEngine == NULL)
    {
        return false;
    }

    // ���ؽű�
    string stringPath = SCP_PATH_FORMAT(WAR_SCHEME_SUB_FUNC);
    bool bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink *)this);
    if (!bResult)
    {
        ErrorLn("config file load failed! filename=" << stringPath.c_str());
        return false;
    }

    return true;
}

// �ر�
void CSchemeWarSubFunc::Close(void)
{
    m_mapScheme.clear();
}


/////////////////////////////////ISchemeUpdateSink/////////////////////////////////////////
// CSV��������ʱ֪ͨ
bool CSchemeWarSubFunc::OnSchemeLoad(SCRIPT_READER reader,const char* szFileName)
{
    ICSVReader *pCSVReader = reader.pCSVReader;
    if ( pCSVReader == NULL || reader.type != READER_CSV)
        return false;

    m_mapScheme.clear();

    // ��ȡ
    int nRecordCount = pCSVReader->GetRecordCount();
    for ( int nRow = 0; nRow < nRecordCount; ++nRow )
    {
		SWarSubFunc node;

		node.nMatchType = pCSVReader->GetInt(nRow, COLNUM_MatchType, 0);

		node.bEnableList[WarSubFuncType_XPSkill]= (bool)pCSVReader->GetInt(nRow, COLNUM_XPSkill_OnOff, 0);
		node.bEnableList[WarSubFuncType_Support] = (bool)pCSVReader->GetInt(nRow, COLNUM_Support_OnOff, 0);
		node.bEnableList[WarSubFuncType_SaveOftenUseEquip] = (bool)pCSVReader->GetInt(nRow, COLNUM_SaveOftenEuip_OnOff, 0);
		node.bEnableList[WarSubFuncType_AIEcmCtrl] = (bool)pCSVReader->GetInt(nRow, COLNUM_AIEcmCtrl_OnOff, 0);
		node.bEnableList[WarSubFuncType_AIExpCtrl] = (bool)pCSVReader->GetInt(nRow, COLNUM_AIExpCtrl_OnOff, 0);

		m_mapScheme[node.nMatchType] = node;
    }

	AfterSchemeLoaded();

    return true;
}

	
// ���ö�̬����ʱ֪ͨ
bool CSchemeWarSubFunc::OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName)
{
    CSchemeWarSubFunc* pNewInstance = NewInstance();
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

bool CSchemeWarSubFunc::isEnableSubFunc(int nMatchType, int nSubFuncType)
{
	WAR_SUBFUNC_MAP_ITER iter = m_mapScheme.find(nMatchType);
	if (iter == m_mapScheme.end())
	{
		return false;
	}

	if (nSubFuncType < 0 && nSubFuncType >= WarSubFuncType_Max)
	{
		return false;
	}

	return iter->second.bEnableList[nSubFuncType];
}
