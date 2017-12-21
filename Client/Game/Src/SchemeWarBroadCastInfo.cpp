#include "StdAfx.h"
#include "IClientGlobal.h"
#include "SchemeWarBroadCastInfo.h"
#include <string>
#include "XlsValue.h"

bool CSchemeWarBroadCastInfo::LoadScheme( void )
{
	ISchemeEngine *pSchemeEngine = gClientGlobal->getSchemeEngine();
	if(pSchemeEngine == NULL)
	{
		return false;
	}

    string stringPath = SCP_PATH_FORMAT( WARBROADCASTINFO_SCHEME_FILENAME );
	bool bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink *)this);
	if(!bResult)
	{
		ErrorLn("config file load failed! filename=" << stringPath.c_str());
		return false;
	}

	return true;
}

void CSchemeWarBroadCastInfo::Close( void )
{
	m_vctSchemeInfo.clear();
}


bool CSchemeWarBroadCastInfo::OnSchemeUpdate( SCRIPT_READER reader, const char* szFileName )
{
	return OnSchemeLoad(reader, szFileName);
}

bool CSchemeWarBroadCastInfo::OnSchemeLoad( SCRIPT_READER reader, const char* szFileName )
{

	ICSVReader * pCSVReader = reader.pCSVReader;
	if(pCSVReader == NULL || szFileName == NULL || reader.type!=READER_CSV)
	{
		return false;
	}

	Close(); 

	int nLen = 0;
	// 读取
	int nRecordCount = pCSVReader->GetRecordCount();
	for (int nRow = 0; nRow < nRecordCount; ++nRow)
	{
		int nIndex = 0;
		int nLen = 0;
		SWarBroadCastInfoScheme item;
		// ID
		item.nIndex = pCSVReader->GetInt(nRow, nIndex++, 0);
		// 我方音效ID	
		item.nSelfCampSoundID = pCSVReader->GetInt(nRow, nIndex++, 0);;

		// 本阵营广播描述	
		nLen = sizeof(item.szSelfCampDes);
		pCSVReader->GetString(nRow, nIndex++, item.szSelfCampDes, nLen);

		// 敌方音效ID	
		item.nEnimyCampSoundID = pCSVReader->GetInt(nRow, nIndex++, 0);;

		// 敌方阵营广播描述
		nLen = sizeof(item.szEnimyCampDes);
		pCSVReader->GetString(nRow, nIndex++, item.szEnimyCampDes, nLen);

		// 插入到列表中
		m_vctSchemeInfo[item.nIndex] = item;
	}
	return true;
}

SWarBroadCastInfoScheme* CSchemeWarBroadCastInfo::getWarBroadCastInfoByIndex(int nIndex)
{
	VCT_SchemeInfo::iterator it = m_vctSchemeInfo.find(nIndex);
	if(it == m_vctSchemeInfo.end())
	{
		Error(__FUNCTION__": failed! nIndex=" << nIndex);
		return NULL;
	}

	return &(it->second);
}