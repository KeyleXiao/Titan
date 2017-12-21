/*******************************************************************
** �ļ���:	SchemeWarOftenUseEquip.cpp
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2015 - All Rights Reserved
** ������:	���к�
** ��  ��:	2017/4/17
** ��  ��:	1.0
** ��  ��:
����װ�����ýű�
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

// ����ű�
bool CSchemeWarOftenUseEquip::LoadScheme(void)
{
    ISchemeEngine *pSchemeEngine = gServerGlobal->getSchemeEngine();
    if (pSchemeEngine == NULL)
    {
        return false;
    }

    // ���ؽű�
    string stringPath = SCP_PATH_FORMAT(WAR_SCHEME_COMMON_EQUIP);
    bool bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink *)this);
    if (!bResult)
    {
        ErrorLn("config file load failed! filename=" << stringPath.c_str());
        return false;
    }

    return true;
}

// �ر�
void CSchemeWarOftenUseEquip::Close(void)
{
	memset(&m_OftenUseEquipConfig, 0, sizeof(m_OftenUseEquipConfig));
}


/////////////////////////////////ISchemeUpdateSink/////////////////////////////////////////
// CSV��������ʱ֪ͨ
bool CSchemeWarOftenUseEquip::OnSchemeLoad(SCRIPT_READER reader,const char* szFileName)
{
    ICSVReader *pCSVReader = reader.pCSVReader;
    if ( pCSVReader == NULL || reader.type != READER_CSV)
        return false;

    // ��ȡ
    int nRecordCount = pCSVReader->GetRecordCount();

	// �ýű�ֻ��һ������
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

	
// ���ö�̬����ʱ֪ͨ
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
