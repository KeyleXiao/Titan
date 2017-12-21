/*******************************************************************
** 文件名:	CSchemeWarSubFuncSetting.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2015 - All Rights Reserved
** 创建人:	李有红
** 日  期:	2017/4/13
** 版  本:	1.0
** 描  述:	
            战场子功能设置脚本
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

// 载入脚本
bool CSchemeWarSubFunc::LoadScheme(void)
{
    ISchemeEngine *pSchemeEngine = gServerGlobal->getSchemeEngine();
    if (pSchemeEngine == NULL)
    {
        return false;
    }

    // 加载脚本
    string stringPath = SCP_PATH_FORMAT(WAR_SCHEME_SUB_FUNC);
    bool bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink *)this);
    if (!bResult)
    {
        ErrorLn("config file load failed! filename=" << stringPath.c_str());
        return false;
    }

    return true;
}

// 关闭
void CSchemeWarSubFunc::Close(void)
{
    m_mapScheme.clear();
}


/////////////////////////////////ISchemeUpdateSink/////////////////////////////////////////
// CSV配置载入时通知
bool CSchemeWarSubFunc::OnSchemeLoad(SCRIPT_READER reader,const char* szFileName)
{
    ICSVReader *pCSVReader = reader.pCSVReader;
    if ( pCSVReader == NULL || reader.type != READER_CSV)
        return false;

    m_mapScheme.clear();

    // 读取
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

	
// 配置动态更新时通知
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
