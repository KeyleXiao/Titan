#pragma once
#include "ISchemeCenter.h"
#include "ISchemeEngine.h"
#include "SchemeCache.h"
#include "AIDef.h"

// AI ID���ñ�
class CSchemeAIIDTable : public ISchemeAIIDTable, public ISchemeUpdateSink, public CSchemeCache<CSchemeAIIDTable>
{
public:
    bool LoadScheme();

    void Close(void);

    // ��ȡĬ��Ӣ��AI����
    virtual int getHeroAIDefault(int nBattleFieldID, int nVocation, int nRank = 0);

    // ��ȡӢ��AI����ID(ָ������)
    virtual bool isHeroAIValid(int nBattleFieldID, int nVocation, int nAIID);

    // ��ȡNPC AI����ID
    virtual int getNpcAISchemeID(int nBattleFieldID, int nNpcID);

    // ��ȡNPC AI����ID
    virtual const char* getHeroAIDes(int nBattleFieldID, int nVocation, int nAIID);

    // �Ƿ���AI����
    virtual bool isHaveAIConfig(int nBattleFieldID);


    virtual bool OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);

	virtual bool OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);

private:

    bool LoadMonsterAIScheme(ICSVReader* pCSVReader);
    bool LoadHeroAIScheme(ICSVReader* pCSVReader);

    // <ս��, <ְҵ,AI����>>
    std::map<int, std::map<int, HeroAISchemeNode>> m_mapHeroAISchemeTable;

    // <ս��, <npcid,AI����>>
    std::map<int, std::map<int, int>> m_mapNpcAISchemeTable;

};