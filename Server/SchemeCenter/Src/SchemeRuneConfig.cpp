#include "stdafx.h"
#include "SchemeRuneConfig.h"
#include "IServerGlobal.h"
#include <string>

#ifdef FUNCTION_RUNE_OPEN

CSchemeRuneConfig::CSchemeRuneConfig(void)
{
    m_SchemeInfo.clear();
}

CSchemeRuneConfig::~CSchemeRuneConfig(void)
{
    m_SchemeInfo.clear();
}

bool CSchemeRuneConfig::LoadScheme(void)
{
    ISchemeEngine * pSchemeEngine = gServerGlobal->getSchemeEngine();
    if(pSchemeEngine == NULL )
    {
        return false;
    }

    string stringPath = SCP_PATH_FORMAT( RUNE_CONFIG_FILENAME );
    bool bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink *)this, false);
    if(!bResult)
    {
        ErrorLn("config file load failed! filename=" << stringPath.c_str());
        return false;
    }

    return true;
}

void CSchemeRuneConfig::Close(void)
{	
    m_SchemeInfo.clear();
}

// CSchemeRuneConfig
// TODO
void CSchemeRuneConfig::GetRuneInfoByID(DWORD dPlayerID, 
    std::map<int, EntityHeroRuneStore> &m_mRune)
{
    m_mRune = m_SchemeInfo;
}

bool CSchemeRuneConfig::OnSchemeLoad(SCRIPT_READER reader, LPCSTR szFileName)
{
    ICSVReader * pCSVReader = reader.pCSVReader;
    if(pCSVReader == NULL || szFileName == NULL || reader.type!=READER_CSV)
    {
        return false;
    }

    // Çå¿Õ
    m_SchemeInfo.clear();
    int nLen = 0;
    // ¶ÁÈ¡
    for (size_t nRow = 0; nRow < pCSVReader->GetRecordCount(); ++nRow)
    {
        int nColumnIndex = 0;
        EntityHeroRuneStore pRune;
        pRune.nRuneId = pCSVReader->GetInt(nRow, nColumnIndex, 0); nColumnIndex++;
        pRune.byActiveRuneCount = pCSVReader->GetInt(nRow, nColumnIndex, 0);
        pRune.byUnActiveRuneCount = 0;
        m_SchemeInfo.insert(make_pair<int, EntityHeroRuneStore>(pRune.nRuneId, pRune));
    }
    AfterSchemeLoaded();

    return true;
}

bool CSchemeRuneConfig::OnSchemeUpdate(SCRIPT_READER reader, LPCSTR szFileName)
{
    return false;
}
#endif