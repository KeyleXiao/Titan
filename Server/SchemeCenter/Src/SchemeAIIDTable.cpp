#include "stdafx.h"
#include "SchemeAIIDTable.h"
#include "IServerGlobal.h"
#include "AIDef.h"


bool CSchemeAIIDTable::LoadScheme()
{
    ISchemeEngine *pSchemeEngine = gServerGlobal->getSchemeEngine();
    if(pSchemeEngine == NULL)
    {
        return false;
    }


    string stringPath = SCP_PATH_FORMAT( SCHEME_MONSTER_AI_CONFIG );
    if(!pSchemeEngine->LoadScheme(stringPath.c_str(),this,true))
    {
        ErrorLn("load " << SCHEME_MONSTER_AI_CONFIG << " failed");
        return false;
    }

    
    stringPath = SCP_PATH_FORMAT( SCHEME_HERO_AI_CONFIG );
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

    
    if(rkt::StringHelper::casecmp(szFileName, strMonsterPath.c_str()) == 0)
    {
        bRet = LoadMonsterAIScheme(reader.pCSVReader);
    }
    else if(rkt::StringHelper::casecmp(szFileName, strHeroPath.c_str()) == 0)
    {
        bRet = LoadHeroAIScheme(reader.pCSVReader);
    }

    if(!bRet)
    {
        return false;
    }

    AfterSchemeLoaded();

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

// ��ȡNPC AI����ID
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

// ��ȡӢ�� AI����ID
int CSchemeAIIDTable::getHeroAISchemeID(int nBattleFieldID, int nVocation)
{
    std::map<int, std::map<int, HeroAISchemeNode>>::iterator itBfScheme = m_mapHeroAISchemeTable.find(nBattleFieldID);
    if(itBfScheme == m_mapHeroAISchemeTable.end())
    {
        return -1;
    }

    std::map<int, HeroAISchemeNode>::iterator it = itBfScheme->second.find(nVocation);
    if(it == itBfScheme->second.end())
    {
        return -1;
    }

    return it->second.AIInfoList[0].nAIID;
}

// ��ȡӢ�۵��߻س�AI nTime:����ʱ��
int CSchemeAIIDTable::getHeroDisconnectBackAI(int nBattleFieldID, int nVocation, int& nTime)
{
    std::map<int, std::map<int, HeroAISchemeNode>>::iterator itBfScheme = m_mapHeroAISchemeTable.find(nBattleFieldID);
    if(itBfScheme == m_mapHeroAISchemeTable.end())
    {
        return -1;
    }

    std::map<int, HeroAISchemeNode>::iterator it = itBfScheme->second.find(nVocation);
    if(it == itBfScheme->second.end())
    {
        return -1;
    }

    nTime = it->second.nDiscconnectBackAITime; 
    return it->second.nDisconnectBackAI;
}

bool CSchemeAIIDTable::OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName)
{
    CSchemeAIIDTable* pNewInstance = NewInstance();
    if(pNewInstance == NULL)
    {
        return false;
    }

    if(pNewInstance->OnSchemeLoad(reader, szFileName))
    {
        return true;
    }
    else
    {
        safeDelete(pNewInstance);
        return false;
    }
}

bool CSchemeAIIDTable::LoadMonsterAIScheme(ICSVReader* pCSVReader)
{
    // ��ȡ
    m_mapNpcAISchemeTable.clear();

    int nRecordCount = pCSVReader->GetRecordCount();
    for(int nRow=0; nRow<nRecordCount; ++nRow)
    {
        int nCol = 0;

        // ս��ID
        int nBattleFiledID = pCSVReader->GetInt(nRow, nCol++, -1);

        // MonsterID(������NPC ����Ϊ����ID)
        int nMonsterID = pCSVReader->GetInt(nRow, nCol++, -1);

        // AI����ID
        int nAISchemeID = pCSVReader->GetInt(nRow, nCol++, -1);

        // ����
        nCol++;

        // npc
        m_mapNpcAISchemeTable[nBattleFiledID][nMonsterID] = nAISchemeID;
    }

    return true;

}

bool CSchemeAIIDTable::LoadHeroAIScheme(ICSVReader* pCSVReader)
{
    // ��ȡ
    m_mapHeroAISchemeTable.clear();
    int nRecordCount = pCSVReader->GetRecordCount();
    for(int nRow=0; nRow<nRecordCount; ++nRow)
    {
        int nCol = 0;

        // ս��ID
        int nBattleFiledID = pCSVReader->GetInt(nRow, nCol++, -1);

        // Ӣ��ְҵID
        int nVocation = pCSVReader->GetInt(nRow, nCol++, -1);

        // AI����ID�б�
        char strAIList[128] = {0};
        int nLen = sizeof(strAIList);
        int nAIList[MAX_AICONFIG_COUNT] = {0};
        pCSVReader->GetString(nRow, nCol++, strAIList, nLen);
        int nAICount = parseIntArray(strAIList, nAIList, MAX_AICONFIG_COUNT, ';');

        HeroAISchemeNode sNode;
        for (int i = 0; i < nAICount; ++i)
        {
            sNode.AIInfoList[i].nAIID	= nAIList[i];
        }

        // �Ѷȷ�������
        nCol++;

        // �����б�
        nCol++;

        // ������־
        nCol++;

        // Ӣ�۱�ע
        nCol++;

        // ���߻س�AI
        sNode.nDisconnectBackAI = pCSVReader->GetInt(nRow, nCol++, -1); 

        // ���߻س�AIʱ��(��)
        sNode.nDiscconnectBackAITime = pCSVReader->GetInt(nRow, nCol++, -1);  

        // Ӣ��
        m_mapHeroAISchemeTable[nBattleFiledID][nVocation] = sNode;
    }
    return true;
}
