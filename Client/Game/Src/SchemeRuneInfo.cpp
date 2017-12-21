#include "stdafx.h"
#include "SchemeRuneInfo.h"
#include "IClientGlobal.h"
#include <string>

CSchemeRuneInfo::CSchemeRuneInfo(void)
{
    m_runeSchemeInfo.clear();
}

CSchemeRuneInfo::~CSchemeRuneInfo(void)
{
    m_runeSchemeInfo.clear();
}

bool CSchemeRuneInfo::LoadScheme(void)
{
    ISchemeEngine * pSchemeEngine = gClientGlobal->getSchemeEngine();
    if(pSchemeEngine == NULL )
    {
        return false;
    }

    string stringPath = SCP_PATH_FORMAT( RUNE_SCHMEME_FILENAME );
    bool bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink *)this, false);
    if(!bResult)
    {
        ErrorLn("config file load failed! filename=" << stringPath.c_str());
        return false;
    }

    return true;
}

void CSchemeRuneInfo::Close(void)
{	
    m_runeSchemeInfo.clear();
}

RuneInfoScheme* CSchemeRuneInfo::GetRuneSchemeInfo(int nRuneID)
{
    T_SchemeInfo::iterator itM = m_runeSchemeInfo.find( nRuneID );
    if(itM != m_runeSchemeInfo.end())
    {
        return &((*itM).second);
    }

    return NULL;
}

bool CSchemeRuneInfo::OnSchemeLoad(SCRIPT_READER reader, LPCSTR szFileName)
{
    ICSVReader * pCSVReader = reader.pCSVReader;
    if(pCSVReader == NULL || szFileName == NULL || reader.type != READER_CSV)
    {
        return false;
    }

    // Çå¿Õ
    m_runeSchemeInfo.clear();

    int nLen = 0;
    // ¶ÁÈ¡
    for (size_t nRow = 0; nRow < pCSVReader->GetRecordCount(); ++nRow)
    {
        RuneInfoScheme item;
        ZeroMemory(&item,sizeof(RuneInfoScheme));

        item.nID = pCSVReader->GetInt(nRow, RUNE_INFO_COL_ID, 0);
        item.nType = pCSVReader->GetInt(nRow, RUNE_INFO_COL_TYPE, 0);
        item.nSpecial = pCSVReader->GetFloat(nRow, RUNE_INFO_COL_SPC, 0);
        item.nLevel = pCSVReader->GetInt(nRow, RUNE_INFO_COL_LEVEL, 0);

        nLen = sizeof(item.stEffectArray);
        pCSVReader->GetString(nRow,  RUNE_INFO_COL_EffectArray, item.stEffectArray, nLen);			
        nLen = sizeof(item.stEffectValueArray);
        pCSVReader->GetString(nRow,  RUNE_INFO_COL_EffectValueArray, item.stEffectValueArray, nLen);	
        m_runeSchemeInfo.insert(make_pair<unsigned int, RuneInfoScheme>(item.nID, item));
    }

    return true;
}

bool CSchemeRuneInfo::OnSchemeUpdate(SCRIPT_READER reader, LPCSTR szFileName)
{
    return false;
}

