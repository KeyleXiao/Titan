#include "stdafx.h"
#include "SchemeRunePage.h"
#include "IServerGlobal.h"
#include <string>

#ifdef FUNCTION_RUNE_OPEN

CSchemeRunePage::CSchemeRunePage(void)
{
    m_SchemeInfo.clear();
}

CSchemeRunePage::~CSchemeRunePage(void)
{
    m_SchemeInfo.clear();
}

bool CSchemeRunePage::LoadScheme(void)
{
    ISchemeEngine * pSchemeEngine = gServerGlobal->getSchemeEngine();
    if(pSchemeEngine == NULL )
    {
        return false;
    }

    string stringPath = SCP_PATH_FORMAT( RUNE_PAGE_FILENAME );
    bool bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink *)this, false);
    if(!bResult)
    {
        ErrorLn("config file load failed! filename=" << stringPath.c_str());
        return false;
    }

    return true;
}

void CSchemeRunePage::Close(void)
{	
    m_SchemeInfo.clear();
}

// CSchemeRunePage
// TODO
void CSchemeRunePage::GetRunePageByID(DWORD dPlayerID, 
    std::map<int, Entity_Rune_Page_Info> &p_MapRunePage)
{
    for (int i = 1; i < 3; ++i)
    {
        Entity_Rune_Page_Info pRunePage;
        pRunePage.nPageIdx = i;
        T_MapRunePage::iterator iterRuneInfo = m_SchemeInfo.begin();
        for (; iterRuneInfo != m_SchemeInfo.end(); ++iterRuneInfo)
        {
            pRunePage.nRuneGroove[iterRuneInfo->second.nPostion - 1] = iterRuneInfo->second.nID;
            if (pRunePage.nRuneGroove[iterRuneInfo->second.nPostion - 1] != 0)
            {
                pRunePage.nRuneNum++;
            }
        }
        p_MapRunePage.insert(make_pair(pRunePage.nPageIdx, pRunePage));
    }
}

bool CSchemeRunePage::OnSchemeLoad(SCRIPT_READER reader, LPCSTR szFileName)
{
    ICSVReader * pCSVReader = reader.pCSVReader;
    if(pCSVReader == NULL || szFileName == NULL || reader.type!=READER_CSV)
    {
        return false;
    }

    // Çå¿Õ
    m_SchemeInfo.clear();
    // ¶ÁÈ¡
    for (size_t nRow = 0; nRow < pCSVReader->GetRecordCount(); ++nRow)
    {
        int nColumnIndex = 0;
        RUNE_INFO pRuneInfo;
        pRuneInfo.nItemID = pCSVReader->GetInt(nRow, nColumnIndex, 0); nColumnIndex++;
        pRuneInfo.nID = pCSVReader->GetInt(nRow, nColumnIndex, 0); nColumnIndex++;
        pRuneInfo.nPostion = pCSVReader->GetInt(nRow, nColumnIndex, 0);
        m_SchemeInfo.insert(make_pair<int, RUNE_INFO>(pRuneInfo.nItemID, pRuneInfo));
    }

    AfterSchemeLoaded();

    return true;
}

bool CSchemeRunePage::OnSchemeUpdate(SCRIPT_READER reader, LPCSTR szFileName)
{
    return false;
}

#endif