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

    // ��ȡӢ��AI����ID(ָ������)
    virtual bool isHeroAIValid(int nBattleFieldID, int nVocation, int nAIID);

    // ��ȡNPC AI����ID
    virtual int getNpcAISchemeID(int nBattleFieldID, int nNpcID);

    // ��ȡӢ�� AI����ID
    virtual int getHeroAISchemeID(int nBattleFieldID, int nVocation);

    // ��ȡӢ�۵��߻س�AI nTime:����ʱ��
    virtual int getHeroDisconnectBackAI(int nBattleFieldID, int nVocation, int& nTime);

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