#include "Stdafx.h"
#include "SchemePreloadMonsterRes.h"


bool CSchemePreloadMonsterRes::OnSchemeLoad(SCRIPT_READER reader,const char* szFileName)
{
    //ICSVReader * pCSVReader = reader.pCSVReader;
    //if ( pCSVReader==0 || reader.type!=READER_CSV)
    //    return false;
    //m_mapMonster.clear();
    //int nLen = 0;

    //// ¶ÁÈ¡
    //int nRecordCount = pCSVReader->GetRecordCount();
    //for ( int nRow = 0; nRow < nRecordCount; ++nRow )
    //{
    //    int i = 0;
    //    MonsterIdVec item;
    //    int nMapId  = pCSVReader->GetInt(nRow, i++, 0);
    //    int nMonsterId = pCSVReader->GetInt(nRow, i++, 0);
    //    AllMonsterMap::iterator itMonster = m_mapMonster.find(nMapId);
    //    if ( itMonster == m_mapMonster.end() )
    //    {
    //        MonsterIdVec itemVec;
    //        itemVec.push_back(nMonsterId);
    //        m_mapMonster[nMapId] = itemVec;
    //    }else
    //    {
    //        itMonster->second.push_back(nMonsterId);
    //    }
    //}
    return true;
}

bool CSchemePreloadMonsterRes::OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName)
{
    return OnSchemeLoad(reader, szFileName);
}

void CSchemePreloadMonsterRes::getMapAllMonster(int nMapId, std::vector<int>& allMonster)
{
    AllMonsterMap::iterator it = m_mapMonster.find(nMapId);
    if ( it != m_mapMonster.end() )
    {
        allMonster = it->second;
    }
    
}

bool CSchemePreloadMonsterRes::LoadScheme()
{
    /*ISchemeEngine* pSchemeEngine = gClientGlobal->getSchemeEngine();
    if ( pSchemeEngine == NULL)
    {
    return false;
    }

    string stringPath = SCP_PATH_FORMAT( PRELOAD_MONSTER_RES );
    bool bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink*) this );
    if ( !bResult )
    {
    ErrorLn("config file load failed filename = " << stringPath.c_str());;
    return false;
    }*/

    return true;
}

void CSchemePreloadMonsterRes::Close()
{
    m_mapMonster.clear();
}