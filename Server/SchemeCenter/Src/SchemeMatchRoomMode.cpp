#include "StdAfx.h"
#include "SchemeMatchRoomMode.h"
#include "IServerGlobal.h"
#include <string>

bool C_SchemeMatchRoomMode::LoadScheme( void )
{
	ISchemeEngine * pSchemeEngine = gServerGlobal->getSchemeEngine();
	if(pSchemeEngine == NULL )
	{
		return false;
	}

	char szPath[MAX_PATH];
	sprintf(szPath,"%s\\%s",getWorkDir(), DMatchRoomMode_SCHEME_FILENAME);
	bool bResult = pSchemeEngine->LoadScheme(szPath, (ISchemeUpdateSink *)this, false);
	if(!bResult)
	{
		Error("C_SchemeMatchRoomMap::LoadScheme szPath=" << szPath << endl);
		return false;
	}

	return true;
}

void C_SchemeMatchRoomMode::Close( void )
{
	m_SchemeInfo.clear();
}

SSchemeMatchRoomMode * C_SchemeMatchRoomMode::getSchemeInfo( T_MDModeID ModeID )
{
	T_SchemeInfo::iterator itM = m_SchemeInfo.find( ModeID );
	if(itM != m_SchemeInfo.end())
	{
		return &(itM->second);
	}

	return nullptr;
}

bool C_SchemeMatchRoomMode::OnSchemeUpdate( SCRIPT_READER reader, const char* szFileName )
{
	return false;
}

bool C_SchemeMatchRoomMode::OnSchemeLoad( SCRIPT_READER reader, const char* szFileName )
{
	TiXmlDocument * pTiXmlDocument = reader.pXMLReader;
	if(pTiXmlDocument == NULL || szFileName == NULL || reader.type != READER_XML)
	{
		return false;
	}
	m_SchemeInfo.clear();

	TiXmlElement * pRootElement = pTiXmlDocument->FirstChildElement("Modes");
	if(pRootElement == NULL)
	{
		return false;
	}
	TiXmlElement * pSubElement = pRootElement->FirstChildElement("Mode");
	if(pSubElement == NULL)
	{
		return false;
	}

	SSchemeMatchRoomMode SchemeInfo;
	LPCSTR pszAttrProp = nullptr;
	TiXmlElement * pChildElement = nullptr;
	while(pSubElement)
	{
		pszAttrProp = pSubElement->Attribute("id", (int *)&SchemeInfo.m_ModeID);
		if(pszAttrProp == NULL || 0 == SchemeInfo.m_ModeID)		
		{
			ErrorLn("m_ModeID不能等于0");
			return false;
		}
		if(nullptr != getSchemeInfo(SchemeInfo.m_ModeID))
		{
			ErrorLn("重复的m_ModeID=" << SchemeInfo.m_ModeID);
			return false;
		}
		
		// todo:pt 读取数据

		m_SchemeInfo[ SchemeInfo.m_ModeID ] = SchemeInfo;
		pSubElement = pSubElement->NextSiblingElement("Mode");
	}
	{// todo:pt 添加测试数据
		m_SchemeInfo.clear();
		for (int i = 0; i < MatchRoomModes; ++i)
		{
			SSchemeMatchRoomMode DataForTest;
			DataForTest.m_ModeID			= i;
			DataForTest.m_CampRoleMax		= i + 3;
			DataForTest.m_SelectHeroTime	= 5 + (i + 1) ;
			DataForTest.m_ConfirmHeroTime	= 3;

			m_SchemeInfo[i] = DataForTest;
		}
	}
	AfterSchemeLoaded();

	return true;
}
