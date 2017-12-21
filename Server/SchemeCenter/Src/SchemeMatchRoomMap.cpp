#include "StdAfx.h"
#include "SchemeMatchRoomMap.h"
#include "IServerGlobal.h"
#include <string>
#include "XlsValue.h"

bool C_SchemeMatchRoomMap::LoadScheme( void )
{
	ISchemeEngine * pSchemeEngine = gServerGlobal->getSchemeEngine();
	if(pSchemeEngine == NULL )
	{
		return false;
	}

	//char szPath[MAX_PATH];
	//sprintf(szPath,"%s\\%s",getWorkDir(), DMatchRoomMap_SCHEME_FILENAME);
	//bool bResult = pSchemeEngine->LoadScheme(szPath, (ISchemeUpdateSink *)this, false);
	//if(!bResult)
	//{
	//	Error("C_SchemeMatchRoomMap::LoadScheme szPath=" << szPath << endl);
	//	return false;
	//}

	string stringPath = DMatchRoomMap_SCHEME_FILENAME;
	bool bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink *)this);
	if(!bResult)
	{
		Error("C_SchemeMatchRoomMap::LoadScheme failed! filename=" << stringPath.c_str() << endl);
		return false;
	}

	return true;
}

void C_SchemeMatchRoomMap::Close( void )
{
		m_SchemeInfo.clear();
}

SSchemeMatchRoomMap * C_SchemeMatchRoomMap::getSchemeInfo( T_MDMapID MapID )
{
	T_SchemeInfo::iterator itM = m_SchemeInfo.find( MapID );
	if(itM != m_SchemeInfo.end())
	{
		return &((*itM).second);
	}

	return nullptr;
}

bool C_SchemeMatchRoomMap::OnSchemeUpdate( SCRIPT_READER reader, const char* szFileName )
{
	return false;
}

bool C_SchemeMatchRoomMap::OnSchemeLoad( SCRIPT_READER reader, const char* szFileName )
{

	ICSVReader * pCSVReader = reader.pCSVReader;
	if(pCSVReader == NULL || szFileName == NULL || reader.type!=READER_CSV)
	{
		return false;
	}

	// 清空
	m_SchemeInfo.clear();

	// 读取
	int nRecordCount = pCSVReader->GetRecordCount();
	for (int nRow = 0; nRow < nRecordCount; ++nRow)
	{
		int nIndex = 0;
		SSchemeMatchRoomMap data;
		memset(&data, 0, sizeof(data));

		data.m_MapID	= static_cast<T_MDMapID>( pCSVReader->GetInt(nRow, nIndex++, 0) );
		char szModes[1024];
		int nLen = sizeof(szModes);
		pCSVReader->GetString(nRow, nIndex++, szModes, nLen);
		int nModes[ MatchRoomModes + 1 ] = {0};
		int nCount = parseIntArray(szModes, nModes, MatchRoomModes + 1, ';');
		if (nCount > MatchRoomModes)
		{
			ErrorLn("C_SchemeMatchRoomMap::OnSchemeLoad nCount > MatchRoomModes nCount= " << nCount);
			return false;
		}
		for (int i = 0; i < nCount; ++i)
		{
			int nMode = nModes[i];
			if (nMode >= MatchRoomModes)
			{
				ErrorLn("C_SchemeMatchRoomMap::OnSchemeLoad nMode >= MatchRoomModes nMode= " << nMode);
				return false;
			}
			if (data.m_Modes[ nMode ])
			{
				ErrorLn("C_SchemeMatchRoomMap::OnSchemeLoad data.m_Modes[ nMode ] nMode= " << nMode);

				return false;
			}
			data.m_Modes[ nMode ] = true;
		}

		m_SchemeInfo[ data.m_MapID ] = data;
	}

	{// todo:pt 添加测试数据 等有地图数据了 就不用了
		m_SchemeInfo.clear();
		for (int i = 0; i < MatchRoomModes; ++i)
		{
			SSchemeMatchRoomMap DataForTest;
			DataForTest.m_MapID = i + 1;			
			sprintf_s(DataForTest.m_szName, sizeof(DataForTest.m_szName), a2utf8("地图名%d"), i + 1);
			sprintf_s(DataForTest.m_szDesc, sizeof(DataForTest.m_szDesc), a2utf8("地图名%d的描述"), i + 1);
			sprintf_s(DataForTest.m_szPicPath, sizeof(DataForTest.m_szPicPath), a2utf8("Cest01"), i + 1);
			DataForTest.m_u8CampCount = 2;
	
			vector<Vector3> vecPos;
			vecPos.push_back( Vector3( 15.0f, 22.0f, 134.0f) );
			DataForTest.m_CampBornPos.push_back(vecPos);
			vecPos.clear();
			vecPos.push_back( Vector3( 245.0f, 22.0f, 135.0f) );
			DataForTest.m_CampBornPos.push_back(vecPos);
	
			memset(DataForTest.m_Modes, 1, sizeof(DataForTest.m_Modes));
			DataForTest.m_Modes[i] = false;
	
			m_SchemeInfo[i] = DataForTest;
		}
	}

	AfterSchemeLoaded();
	return true;
}

//bool C_SchemeMatchRoomMap::OnSchemeLoad( SCRIPT_READER reader, const char* szFileName )
//{
//	TiXmlDocument * pTiXmlDocument = reader.pXMLReader;
//	if(pTiXmlDocument == NULL || szFileName == NULL || reader.type != READER_XML)
//	{
//		return false;
//	}
//	m_SchemeInfo.clear();
//
//	TiXmlElement * pRootElement = pTiXmlDocument->FirstChildElement("Maps");
//	if(pRootElement == NULL)
//	{
//		return false;
//	}
//	TiXmlElement * pSubElement = pRootElement->FirstChildElement("Map");
//	if(pSubElement == NULL)
//	{
//		return false;
//	}
//
//	SSchemeMatchRoomMap SchemeInfo;
//	LPCSTR pszAttrProp = nullptr;
//	TiXmlElement * pChildElement = nullptr;
//	while(pSubElement)
//	{
//		pszAttrProp = pSubElement->Attribute("id", (int *)&SchemeInfo.m_MapID);
//		if(pszAttrProp == NULL || 0 == SchemeInfo.m_MapID)		
//		{
//			ErrorLn("m_MapID不能等于0");
//			return false;
//		}
//		if(nullptr != getSchemeInfo(SchemeInfo.m_MapID))
//		{
//			ErrorLn("重复的m_MapID=" << SchemeInfo.m_MapID);
//			return false;
//		}
//
//		// todo:pt 读取数据
//
//		m_SchemeInfo[ SchemeInfo.m_MapID ] = SchemeInfo;
//		pSubElement = pSubElement->NextSiblingElement("Map");
//	}
//
//	{// todo:pt 添加测试数据
//		for (int i = 0; i < MatchRoomModes; ++i)
//		{
//			SSchemeMatchRoomMap DataForTest;
//			DataForTest.m_MapID = i + 1;			
//			sprintf_s(DataForTest.m_szName, sizeof(DataForTest.m_szName), a2utf8("地图名%d"), i + 1);
//			sprintf_s(DataForTest.m_szDesc, sizeof(DataForTest.m_szDesc), a2utf8("地图名%d的描述"), i + 1);
//			sprintf_s(DataForTest.m_szPicPath, sizeof(DataForTest.m_szPicPath), a2utf8("Cest01"), i + 1);
//			DataForTest.m_u8CampCount = 2;
//
//			vector<Vector3> vecPos;
//			vecPos.push_back( Vector3( 245.0f, 22.0f, 135.0f) );
//			DataForTest.m_CampBornPos.push_back(vecPos);
//			vecPos.clear();
//			vecPos.push_back( Vector3( 15.0f, 22.0f, 134.0f) );
//			DataForTest.m_CampBornPos.push_back(vecPos);
//
//			memset(DataForTest.m_Modes, 1, sizeof(DataForTest.m_Modes));
//			DataForTest.m_Modes[i] = false;
//
//			m_SchemeInfo[i] = DataForTest;
//		}
//	}
//
//	AfterSchemeLoaded();
//
//	return true;
//}
