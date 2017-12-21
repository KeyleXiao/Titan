/*******************************************************************
** 文件名:	SchemeWarOftenUseEquip.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2015 - All Rights Reserved
** 创建人:	李有红
** 日  期:	2017/4/17
** 版  本:	1.0
** 描  述:
常用装备配置脚本
********************************************************************/

#include "Stdafx.h"
#include <string>
#include "IServerGlobal.h"
#include "SchemeWarOftenUseEquip.h"
#include "SchemeWarDef.h"

CSchemeWarOftenUseEquip::CSchemeWarOftenUseEquip(void)
{

}

CSchemeWarOftenUseEquip::~CSchemeWarOftenUseEquip(void)
{

}

// 载入脚本
bool CSchemeWarOftenUseEquip::LoadScheme(void)
{
    ISchemeEngine *pSchemeEngine = gServerGlobal->getSchemeEngine();
    if (pSchemeEngine == NULL)
    {
        return false;
    }

    // 加载脚本
    string stringPath = SCP_PATH_FORMAT(WAR_SCHEME_COMMON_EQUIP);
    bool bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink *)this);
    if (!bResult)
    {
        ErrorLn("config file load failed! filename=" << stringPath.c_str());
        return false;
    }

    return true;
}

// 关闭
void CSchemeWarOftenUseEquip::Close(void)
{
	memset(&m_OftenUseEquipConfig, 0, sizeof(m_OftenUseEquipConfig));
}


/////////////////////////////////ISchemeUpdateSink/////////////////////////////////////////
// CSV配置载入时通知
bool CSchemeWarOftenUseEquip::OnSchemeLoad(SCRIPT_READER reader,const char* szFileName)
{
    ICSVReader *pCSVReader = reader.pCSVReader;
    if ( pCSVReader == NULL || reader.type != READER_CSV)
        return false;

    // 读取
    int nRecordCount = pCSVReader->GetRecordCount();

	// 该脚本只有一行数据
	if (nRecordCount != 1)
	{
		ErrorLn(__FUNCTION__ " The record count of " << WAR_SCHEME_COMMON_EQUIP << " isn't 1.nRecordCount=" << nRecordCount);

		return false;
	}

	m_OftenUseEquipConfig.nBaseWeight = pCSVReader->GetInt(0, COLNUM_BaseWeight, 0);
	m_OftenUseEquipConfig.nDampRatio = pCSVReader->GetInt(0, COLNUM_DampRatio, 0);

	AfterSchemeLoaded();

    return true;
}

	
// 配置动态更新时通知
bool CSchemeWarOftenUseEquip::OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName)
{
    CSchemeWarOftenUseEquip* pNewInstance = NewInstance();
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

int CSchemeWarOftenUseEquip::getBaseWeight()
{
	return m_OftenUseEquipConfig.nBaseWeight;
}

float CSchemeWarOftenUseEquip::getDampRatio()
{
	return (float)m_OftenUseEquipConfig.nDampRatio / 100.f;
}
