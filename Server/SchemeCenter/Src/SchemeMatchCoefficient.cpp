#include "StdAfx.h"
#include "SchemeMatchCoefficient.h"
#include "IServerGlobal.h"

bool CSchemeMatchCoefficient::OnSchemeLoad(SCRIPT_READER reader,const char* szFileName)
{
    ICSVReader * pCSVReader = reader.pCSVReader;
    if ( pCSVReader==0 || reader.type!=READER_CSV)
        return false;

    // 清空
    m_mapMatchCoeff.clear();

    int nLen = 0;
    // 读取
    int nRecordCount = pCSVReader->GetRecordCount();

	for ( int nRow = 0; nRow < nRecordCount; ++nRow )
	{
		int nColumnIndex = 0;
		SMatchCoefficient item;
		item.nWarID = pCSVReader->GetInt(nRow, nColumnIndex++, 0);
		item.nAG = pCSVReader->GetInt(nRow, nColumnIndex++, 0);
		item.nBG = pCSVReader->GetInt(nRow, nColumnIndex++, 0);
		item.nCG = pCSVReader->GetInt(nRow, nColumnIndex++, 0);
		item.nAE = pCSVReader->GetInt(nRow, nColumnIndex++, 0);
		item.nBE = pCSVReader->GetInt(nRow, nColumnIndex++, 0);
		item.nCE = pCSVReader->GetInt(nRow, nColumnIndex++, 0);
		item.nW11 = pCSVReader->GetInt(nRow, nColumnIndex++, 0);
		item.fW12 = pCSVReader->GetFloat(nRow, nColumnIndex++, 0.0f);
		item.nW13 = pCSVReader->GetInt(nRow, nColumnIndex++, 0);
		item.nW14 = pCSVReader->GetInt(nRow, nColumnIndex++, 0);
		item.nW15 = pCSVReader->GetInt(nRow, nColumnIndex++, 0);
		item.nW21 = pCSVReader->GetInt(nRow, nColumnIndex++, 0);
		item.fW22 = pCSVReader->GetFloat(nRow, nColumnIndex++, 0.0f);
		item.fW23 = pCSVReader->GetFloat(nRow, nColumnIndex++, 0.0f);
		item.nW24 = pCSVReader->GetInt(nRow, nColumnIndex++, 0);
		item.nW25 = pCSVReader->GetInt(nRow, nColumnIndex++, 0);
		m_mapMatchCoeff[item.nWarID] = item;
    }

    return true;
}

SMatchCoefficient* CSchemeMatchCoefficient::getMatchCoeff( int nWarID )
{
    std::map<int, SMatchCoefficient>::iterator it = m_mapMatchCoeff.find(nWarID);
    if ( it == m_mapMatchCoeff.end() )
    {
        return NULL;
    }
    return &it->second;
}

bool CSchemeMatchCoefficient::OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName)
{
    return OnSchemeLoad(reader, szFileName);
}

bool CSchemeMatchCoefficient::LoadScheme()
{
    ISchemeEngine* pSchemeEngine = gServerGlobal->getSchemeEngine();
    if ( pSchemeEngine == NULL)
    {
        return false;
    }

    // 加载SummonerSkill.csv脚本
    string stringPath = SCP_PATH_FORMAT( MATCHCOEFFICIENT_SCHEME_FILENAME );
    bool bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink*) this );
    if ( !bResult )
    {
        ErrorLn("config file load failed filename = " << stringPath.c_str());;
        return false;
    }

    AfterSchemeLoaded();
    return true;
}

// 关闭
void CSchemeMatchCoefficient::Close()
{
    // 清空
    m_mapMatchCoeff.clear();
}