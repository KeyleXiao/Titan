/*******************************************************************
** 文件名:	SchemeWarPersonConfig.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	马传飞
** 日  期:	3/10/2016
** 版  本:	1.0
** 描  述:	战场相关配置信息
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

	// 加载战场配置
	if (!loadWarData(pXMLReader))
		return false;

	// 加载其他配置数据
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

    // 加载脚本
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

		// 不能有重复的nWarID
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

		// 连杀间隔
		pChildElement->Attribute("TimeCountKillInterval", (int*)&WarConfigInfo.nTimeCountKillInterval);

		// 压入map
		m_mapSchemeWarConfig[WarConfigInfo.nWarID] = WarConfigInfo;

		// 读取下一个
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

	// 怪物的监视距离  ， 人进入这个距离之后才能打到前
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
