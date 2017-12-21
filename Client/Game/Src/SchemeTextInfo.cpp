#include "StdAfx.h"
#include "IClientGlobal.h"
#include "SchemeTextInfo.h"
#include <string>
#include "XlsValue.h"

bool CSchemeTextInfo::LoadScheme( void )
{
	ISchemeEngine *pSchemeEngine = gClientGlobal->getSchemeEngine();
	if(pSchemeEngine == NULL)
	{
		return false;
	}

    string stringPath = SCP_PATH_FORMAT( TEXTINFO_SCHEME_FILENAME );
	bool bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink *)this);
	if(!bResult)
	{
		ErrorLn("config file load failed! filename=" << stringPath.c_str());
		return false;
	}

	return true;
}

void CSchemeTextInfo::Close( void )
{
	m_vctSchemeInfo.clear();
}


bool CSchemeTextInfo::OnSchemeUpdate( SCRIPT_READER reader, const char* szFileName )
{
	return OnSchemeLoad(reader, szFileName);
}

bool CSchemeTextInfo::OnSchemeLoad( SCRIPT_READER reader, const char* szFileName )
{

	ICSVReader * pCSVReader = reader.pCSVReader;
	if(pCSVReader == NULL || szFileName == NULL || reader.type!=READER_CSV)
	{
		return false;
	}

	Close(); 

	char szText[1024];
	// 读取
	int nRecordCount = pCSVReader->GetRecordCount();
	for (int nRow = 0; nRow < nRecordCount; ++nRow)
	{
		// 跳过第一列描述
		int nIndex = 1;
		
		int nID = pCSVReader->GetInt(nRow, nIndex++, 0);
		int nLen = 1024;
		if ( !pCSVReader->GetString(nRow, nIndex++, szText, nLen) )
		{
			return false;
		}

		m_vctSchemeInfo.insert( make_pair(nID, szText) );
	}
	return true;
}

string& CSchemeTextInfo::getText( int nID )
{
	VCT_SchemeInfo::iterator itor = m_vctSchemeInfo.find( nID );
	if (itor != m_vctSchemeInfo.end())
	{
		return itor->second;
	}
	return ms_NullString;
}

std::string CSchemeTextInfo::ms_NullString;
