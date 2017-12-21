#include "stdafx.h"
#include "SchemeAIIDTable.h"
#include "IClientGlobal.h"
#include "AIDef.h"

bool CSchemeAIIDTable::LoadScheme()
{
    ISchemeEngine *pSchemeEngine = gClientGlobal->getSchemeEngine();
    if(pSchemeEngine == NULL)
    {
        return false;
    }


    /*string stringPath = SCP_PATH_FORMAT( SCHEME_MONSTER_AI_CONFIG );
    if(!pSchemeEngine->LoadScheme(stringPath.c_str(),this,true))
    {
        ErrorLn("load " << SCHEME_MONSTER_AI_CONFIG << " failed");
        return false;
    }*/

    // 加载ai配置相关
    string stringPath = SCP_PATH_FORMAT( SCHEME_HERO_AI_CONFIG );
    if(!pSchemeEngine->LoadScheme(stringPath.c_str(),this,true))
    {
        ErrorLn("load " << SCHEME_HERO_AI_CONFIG << " failed");
        return false;
    }

    return true;	
}

bool CSchemeAIIDTable::OnSchemeLoad(SCRIPT_READER reader,const char* szFileName)
{
    bool bRet = false;
    string strMonsterPath = SCP_PATH_FORMAT( SCHEME_MONSTER_AI_CONFIG );
    string strHeroPath = SCP_PATH_FORMAT( SCHEME_HERO_AI_CONFIG );

    m_mapHeroAISchemeTable.clear();

    if(strcmp(szFileName, strMonsterPath.c_str()) == 0)
    {
        bRet = LoadMonsterAIScheme(reader.pCSVReader);
    }
    else if(strcmp(szFileName, strHeroPath.c_str()) == 0)
    {
        bRet = LoadHeroAIScheme(reader.pCSVReader);
    }

    if(!bRet)
    {
        return false;
    }

    return true;
}

void CSchemeAIIDTable::Close(void)
{
    m_mapHeroAISchemeTable.clear();
    m_mapNpcAISchemeTable.clear();
}



bool CSchemeAIIDTable::isHeroAIValid(int nBattleFieldID, int nVocation, int nAIID)
{
    if(nAIID == -1)
    {
        return false;
    }

    std::map<int, std::map<int, HeroAISchemeNode>>::iterator itBfScheme = m_mapHeroAISchemeTable.find(nBattleFieldID);
    if(itBfScheme == m_mapHeroAISchemeTable.end())
    {
        return false;
    }

    
    std::map<int, HeroAISchemeNode>::iterator it = itBfScheme->second.find(nVocation);
    if(it == itBfScheme->second.end())
    {
        return false;
    }

    for(int n = 0; n < MAX_AICONFIG_COUNT; ++n)
    {
        if(it->second.AIInfoList[n].nAIID == nAIID)
        {
            return true;
        }
    }

    return false; 
}

// 获取NPC AI配置ID
int CSchemeAIIDTable::getNpcAISchemeID(int nBattleFieldID, int nNpcID)
{
    std::map<int, std::map<int, int>>::iterator itBfScheme = m_mapNpcAISchemeTable.find(nBattleFieldID);
    if(itBfScheme == m_mapNpcAISchemeTable.end())
    {
        return -1;
    }

    std::map<int, int>::iterator it = itBfScheme->second.find(nNpcID);
    if(it == itBfScheme->second.end())
    {
        return -1;
    }

    return it->second;
}



bool CSchemeAIIDTable::OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName)
{
    return OnSchemeLoad(reader,szFileName);
}

bool CSchemeAIIDTable::LoadMonsterAIScheme(ICSVReader* pCSVReader)
{
    // 读取
    int nRecordCount = pCSVReader->GetRecordCount();
    for(int nRow=0; nRow<nRecordCount; ++nRow)
    {
        int nCol = 0;

        // 战场ID
        int nBattleFiledID = pCSVReader->GetInt(nRow, nCol++, -1);

        // MonsterID(假如是NPC 此列为怪物ID)
        int nMonsterID = pCSVReader->GetInt(nRow, nCol++, -1);

        // AI配置ID
        int nAISchemeID = pCSVReader->GetInt(nRow, nCol++, -1);

        // 描述
        nCol++;

        // npc
        m_mapNpcAISchemeTable[nBattleFiledID][nMonsterID] = nAISchemeID;
    }

    return true;

}

bool CSchemeAIIDTable::LoadHeroAIScheme(ICSVReader* pCSVReader)
{
    // 读取
    int nRecordCount = pCSVReader->GetRecordCount();
    for(int nRow=0; nRow<nRecordCount; ++nRow)
    {
        int nCol = 0;

        // 战场ID
        int nBattleFiledID = pCSVReader->GetInt(nRow, nCol++, -1);

        // MonsterID(假如是NPC 此列为怪物ID)
        int nVocation = pCSVReader->GetInt(nRow, nCol++, -1);

        // AI配置ID列表
        char strAIList[128] = {0};
        int nLen = sizeof(strAIList);
        int nAIList[MAX_AICONFIG_COUNT] = {0};
        pCSVReader->GetString(nRow, nCol++, strAIList, nLen);
        int nAICount = parseIntArray(strAIList, nAIList, MAX_AICONFIG_COUNT, ';');

        // AI配置Rank分列表
        char strAIRankList[128] = {0};
        int nRankLen = sizeof(strAIRankList);
        int nAIRankList[MAX_AIRANK_COUNT] = {0};
        pCSVReader->GetString(nRow, nCol++, strAIRankList, nRankLen);
        int nAIRankCount = parseIntArray(strAIRankList, nAIRankList, MAX_AIRANK_COUNT, ';');

        char strAIDesList[128] = {0};
        int nDesLen = sizeof(strAIDesList);
        pCSVReader->GetString(nRow, nCol++, strAIDesList, nDesLen);
        vector<string> vecDes;
        char *p = strtok(strAIDesList,";");
        while(p)
        {
            string strDes = p;
            vecDes.push_back(strDes);
            p=strtok(NULL,";");
        }

        if (vecDes.size() != nAICount)
        {
            ErrorLn(__FUNCTION__": error count nAICount != nDesCount nRow = "<<nRow<< " nCol ="<< nCol -1);
            return false;
        }

        HeroAISchemeNode sNode;
        for (int i = 0; i < nAICount; ++i)
        {
            sNode.AIInfoList[i].nAIID	= nAIList[i];
            sstrcpyn(sNode.AIInfoList[i].szAIDes, vecDes[i].data(), sizeof(sNode.AIInfoList[i].szAIDes));
        }

        sNode.nAIRankCount = nAIRankCount;
        for (int i = 0; i < nAICount; ++i)
        {
            sNode.nRankList[i]= nAIRankList[i];
        }

        // 开启标志
        sNode.bOpen = pCSVReader->GetInt(nRow, nCol++, 0) == 0? false: true;
        if(!sNode.bOpen )
        {
            continue;
        }
        m_mapHeroAISchemeTable[nBattleFiledID][nVocation] = sNode;
    }
    return true;
}

int CSchemeAIIDTable::getHeroAIDefault(int nBattleFieldID, int nVocation)
{

    int defaultID = -1;
    std::map<int, std::map<int, HeroAISchemeNode>>::iterator itBfScheme = m_mapHeroAISchemeTable.find(nBattleFieldID);
    if(itBfScheme == m_mapHeroAISchemeTable.end())
    {
        return defaultID;
    }


    std::map<int, HeroAISchemeNode>::iterator it = itBfScheme->second.find(nVocation);
    if(it == itBfScheme->second.end())
    {
        return defaultID;
    }

    for(int n = 0; n < MAX_AICONFIG_COUNT; ++n)
    {
        if(it->second.AIInfoList[n].nAIID != -1)
        {
            defaultID = it->second.AIInfoList[n].nAIID;
            break;
        }
    }
    return defaultID;
}

bool CSchemeAIIDTable::isHaveAIConfig(int nBattleFieldID)
{
    std::map<int, std::map<int, HeroAISchemeNode>>::iterator itBfScheme = m_mapHeroAISchemeTable.find(nBattleFieldID);
    if(itBfScheme == m_mapHeroAISchemeTable.end())
    {
        return false;
    }

    return true;
}

// 获取NPC AI配置ID
const char* CSchemeAIIDTable::getHeroAIDes(int nBattleFieldID, int nVocation, int nAIID)
{
    if(nAIID == -1)
    {
        return "";
    }

    std::map<int, std::map<int, HeroAISchemeNode>>::iterator itBfScheme = m_mapHeroAISchemeTable.find(nBattleFieldID);
    if(itBfScheme == m_mapHeroAISchemeTable.end())
    {
        return "";
    }

    std::map<int, HeroAISchemeNode>::iterator it = itBfScheme->second.find(nVocation);
    if(it == itBfScheme->second.end())
    {
        return "";
    }

    for (int n = 0; n < MAX_AICONFIG_COUNT; ++n)
    {
        if (nAIID == it->second.AIInfoList[n].nAIID)
        {
            return it->second.AIInfoList[n].szAIDes;
        }
    }
    return ""; 
}
