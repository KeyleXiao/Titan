/*******************************************************************
** 文件名:	CSchemeEconomyControl.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2015 - All Rights Reserved
** 创建人:	李有红
** 日  期:	2017/2/23
** 版  本:	1.0
** 描  述:	
            经济控制脚本
********************************************************************/


#include "Stdafx.h"
#include <string>
#include "IServerGlobal.h"
#include "SchemeEconomyControl.h"

CSchemeEconomyControl::CSchemeEconomyControl(void)
{

}

CSchemeEconomyControl::~CSchemeEconomyControl(void)
{

}

// 载入脚本
bool CSchemeEconomyControl::LoadScheme(void)
{
    ISchemeEngine *pSchemeEngine = gServerGlobal->getSchemeEngine();
    if (pSchemeEngine == NULL)
    {
        return false;
    }

    // 加载脚本
    string stringPath = SCP_PATH_FORMAT(ECONOMYCONTROL_SCHEME_FILENAME);
    bool bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink *)this);
    if (!bResult)
    {
        ErrorLn("config file load failed! filename=" << stringPath.c_str());
        return false;
    }

    return true;
}

// 关闭
void CSchemeEconomyControl::Close(void)
{
    m_mapScheme.clear();
}


/////////////////////////////////ISchemeUpdateSink/////////////////////////////////////////
// CSV配置载入时通知
bool CSchemeEconomyControl::OnSchemeLoad(SCRIPT_READER reader,const char* szFileName)
{
    ICSVReader *pCSVReader = reader.pCSVReader;
    if ( pCSVReader == NULL || reader.type != READER_CSV)
        return false;

    m_mapScheme.clear();

    // 读取
    int nRecordCount = pCSVReader->GetRecordCount();
    for ( int nRow = 0; nRow < nRecordCount; ++nRow )
    {
		SEconomyControlConfig_S node;

		node.configWarID = pCSVReader->GetInt(nRow, COLNUM_CONFIG_WAR_ID, -1);
		node.id = pCSVReader->GetInt(nRow, COLNUM_ID, 0);
		node.aRatio = pCSVReader->GetInt(nRow, COLNUM_A_RATIO, 0);
		node.bRatio = pCSVReader->GetInt(nRow, COLNUM_B_RATIO, 0);
		node.nRatioType[EWEcmExpCtrlRatioType_Sodlier] = pCSVReader->GetFloat(nRow, COLNUM_SOLDIER, 0);
		node.nRatioType[EWEcmExpCtrlRatioType_Monster] = pCSVReader->GetFloat(nRow, COLNUM_MONSTER, 0);
		node.nRatioType[EWEcmExpCtrlRatioType_Salary] = pCSVReader->GetFloat(nRow, COLNUM_SALARY, 0);
		node.nRatioType[EWEcmExpCtrlRatioType_Hero] = pCSVReader->GetFloat(nRow, COLNUM_HERO, 0);

		m_mapScheme[node.configWarID][node.id - 1] = node;
    }

	AfterSchemeLoaded();

    return true;
}

	
// 配置动态更新时通知
bool CSchemeEconomyControl::OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName)
{
    CSchemeEconomyControl* pNewInstance = NewInstance();
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

SEconomyControlConfig_S* CSchemeEconomyControl::getEconomyCtrl(int nConfigWarID, int nLineID )
{
	ECO_CTRL_MAP_ITER iter = m_mapScheme.find(nConfigWarID);
	if (iter == m_mapScheme.end())
	{
		return NULL;
	}

	map<int, SEconomyControlConfig_S>::iterator iterConfig = iter->second.find(nLineID);
	if (iterConfig == iter->second.end())
	{
		return NULL;
	}


	return &iterConfig->second;
}