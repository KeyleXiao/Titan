/*******************************************************************
** �ļ���:	SchemeWarPersonConfig.cpp
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	����
** ��  ��:	3/10/2016
** ��  ��:	1.0
** ��  ��:	ս�����������Ϣ
********************************************************************/

#include "Stdafx.h"
#include "SchemeWarPersonConfig.h"
#include "IClientGlobal.h"
//#include "EntityDef.h"

/////////////////////////ISchemeUpdateSink/////////////////////////
bool CSchemeWarPersonConfig::OnSchemeLoad(SCRIPT_READER reader,const char* szFileName)
{
    TiXmlDocument *pXMLReader = reader.pXMLReader;
    if(pXMLReader == NULL || szFileName == NULL || reader.type!= READER_XML)
    {
        return false;
    }

	// ����ս������
	if (!loadWarData(pXMLReader))
		return false;

	// ����������������
	if (!loadOtherData(pXMLReader))
		return false;

    return true;
}
bool CSchemeWarPersonConfig::OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName)
{
    return OnSchemeLoad(reader, szFileName);
}
/////////////////////////ISchemeWarPersonConfig/////////////////////


/////////////////////////CSchemeWarPersonConfig////////////////////
bool CSchemeWarPersonConfig::LoadScheme(void)
{
    ISchemeEngine *pSchemeEngine = gClientGlobal->getSchemeEngine();
    if(pSchemeEngine == NULL)
    {
        return false;
    }

    // ���ؽű�
    string stringPath = SCP_PATH_FORMAT( WAR_SCHEME_PERSON_CONFIG );
    bool bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink *)this);
    if(!bResult)
    {
        Error("config file load failed! filename=" << stringPath.c_str() << endl);
        return false;
    }

    return true;	
}
void CSchemeWarPersonConfig::Close(void)
{

}

CSchemeWarPersonConfig::CSchemeWarPersonConfig(void)
{
}

CSchemeWarPersonConfig::~CSchemeWarPersonConfig(void)
{

}

bool CSchemeWarPersonConfig::loadWarData(TiXmlDocument * pXMLReader)
{
	TiXmlElement *pRootElement = pXMLReader->FirstChildElement("scheme");
	if (pRootElement == NULL)
	{
		return false;
	}

	TiXmlElement * pChildElement = NULL;
	TiXmlElement * pGrandchildElement = NULL;
	LPCSTR pszAttrProp = NULL;

	TiXmlElement * pSubElement = pRootElement->FirstChildElement("war");
	if (pSubElement == NULL)
	{
		return false;
	}

	while (pSubElement)
	{
		SchemeWarPersonConfigInfo WarConfigInfo;

		// WarID
		pszAttrProp = pSubElement->Attribute("war_id", (int *)&WarConfigInfo.nWarID);
		if (pszAttrProp == NULL || WarConfigInfo.nWarID <= 0)
		{
			ErrorLn("WarConfigInfo.nWarID < = 0! nWarID=" << WarConfigInfo.nWarID);
			return false;
		}

		// �������ظ���nWarID
		if (GetSchemeWarPersonConfigInfo(WarConfigInfo.nWarID) != NULL)
		{
			ErrorLn("repeated WarConfigInfo.nWarID=" << WarConfigInfo.nWarID);
			return false;
		}

		// [config]
		pChildElement = pSubElement->FirstChildElement("config");
		if (pChildElement == NULL)
		{
			return false;
		}

		// ��ɱ���
		pChildElement->Attribute("TimeCountKillInterval", (int*)&WarConfigInfo.nTimeCountKillInterval);

		// ѹ��map
		m_mapSchemeWarConfig[WarConfigInfo.nWarID] = WarConfigInfo;

		// ��ȡ��һ��
		pSubElement = pSubElement->NextSiblingElement("war");
	}

	return true;
}

bool CSchemeWarPersonConfig::loadOtherData(TiXmlDocument * pXMLReader)
{
	TiXmlElement *pRootElement = pXMLReader->FirstChildElement("DistanceConfig");
	if (pRootElement == NULL)
	{
		return false;
	}

	// [config]
	TiXmlElement * pChildElement = NULL;
	pChildElement = pRootElement->FirstChildElement("config");
	if (pChildElement == NULL)
	{
		return false;
	}

	// ����ļ��Ӿ���  �� �˽����������֮����ܴ�ǰ
	pChildElement->Attribute("MonsterDistance", &m_warPersonConfig.nMonsDistance);
	pChildElement->Attribute("TowerDistance", &m_warPersonConfig.nTowerDistance);
	pChildElement->Attribute("MoneyEffect", &m_warPersonConfig.nMoneyEffect);
	pChildElement->Attribute("MonsterDeadEffect", &m_warPersonConfig.nMonsterDeadEffect);

	return true;
}

SWarPersonConfig* CSchemeWarPersonConfig::GetSchemeWarPersonConfigInfo()
{
    return &m_warPersonConfig;
}

const SchemeWarPersonConfigInfo * CSchemeWarPersonConfig::GetSchemeWarPersonConfigInfo(int WarID)
{
	map<int, SchemeWarPersonConfigInfo>::iterator iter = m_mapSchemeWarConfig.find(WarID);
	if (iter != m_mapSchemeWarConfig.end())
	{
		return &(iter->second);
	}

	return NULL;
}
